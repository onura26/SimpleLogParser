// src/file_processor.cpp

#include "file_processor.h"

int search_in_file(const ProgramOptions& options)
{
    /*
    * CONTEXT LINES ALGORITHM 
    * 
    * This function implements grep-style context lines (-A, -B, -C flags).
    * 
    * 1. Ring Buffer (Before-Context):
    *   - Continously stores the last N lines in a deque (instead of vector for faster operations)
    *   - When match found -> dump buffer, then clear it
    *   - Deque has O(1) for push_back/pop_front 
    * 
    * 2. Countdown Timer (After-Context):
    *   - After a match, print next N lines regardless of pattern
    *   - Decrements counter each line until it reaches zero
    * 
    * 3. Deduplication:
    *   - Tracks last printed line number to avoid duplicates
    *   - Handles overlapping contexts when matches are close together
    * 
    * 4. Separators:
    *   - Prints "--" between close match groups (just like grep)
    * 
    * Ex:
    *   ./logparser log.txt "ERROR" -B 2 -A 1
    * 
    *   Output:
    *   [C:L18] line before match     ← before context (dim)
    *   [C:L19] line before match     ← before context (dim)
    *   [0:L20] ERROR: actual match   ← match (colored)
    *   [C:L21] line after match      ← after context (dim)
    *   --
    *   [C:L45] line before match     ← next match group
    *   [1:L46] ERROR: another match
    */

    // Open file 
    int fd = open(options.inputFilePath.c_str(), O_RDONLY);
    if (fd == -1)
    {
        throw std::runtime_error("Failed to open file: " + options.inputFilePath);
    }

    // Get file size
    struct stat sb;
    if (fstat(fd, &sb) == -1)
    {
        close(fd);
        throw std::runtime_error("Failed to get file size");
    }
    off_t fileSize {sb.st_size};

    // Empty file check
    if (fileSize == 0)
    {
        close(fd);
        std::cout << '\n' << "Total matches: 0" << std::endl;
        return EXIT_SUCCESS;
    }

    // Memory mapping process
    // MAP_PRIVATE = Changes won't affect the integrity of file
    // MAP_POPULATE = Prefetch pages into memory (for better performance)
    char* fileData {static_cast<char*>(mmap(nullptr, fileSize, PROT_READ, MAP_PRIVATE, fd, 0))};
    if (fileData == MAP_FAILED)
    {
        close(fd);
        throw std::runtime_error("Memory mapping failed");
    }
    madvise(fileData, fileSize, MADV_SEQUENTIAL | MADV_WILLNEED); // Tell OS to read sequentially
    close(fd);

    // Compile regex patterns if needed
    std::vector<std::regex> regexPatterns;
    if (options.useRegex)
    {
        regexPatterns.reserve(options.searchPatterns.size());

        for (const auto& pattern : options.searchPatterns)
        {
            regexPatterns.emplace_back(pattern, options.caseInsensitive ? std::regex::icase : std::regex::ECMAScript);
        }
    }

    // Context lines implementation
    // Ring buffer for before-context lines (-B flag)
    std::deque<std::pair<int, std::string>> beforeBuffer;

    int afterContextRemaining {0}; // Countdown timer for after-context lines (-A flag)

    int lastPrintedLine {-1}; // Deduplication tracker that prevents printing the same line twice

    bool needsSeparator {false}; // Separator flag 

    std::string line;
    int matchCount {0};
    int lineNumber {0};
    int linesWithTimestamps {0};
    LogDateFormat dateFormat {LogDateFormat::UNKNOWN};

    constexpr const char* CONTEXT_COLOR = "\033[2m"; // dim 

    // Line parser with memchr
    const char* lineStart {fileData};
    const char* fileEnd {fileData + fileSize};

    while (lineStart < fileEnd)
    {
        // Find new line
        const char* lineEnd {static_cast<const char*>(memchr(lineStart, '\n', fileEnd - lineStart))};

        if (lineEnd == nullptr)
        {
            lineEnd = fileEnd;
        }

        off_t lineLength {lineEnd - lineStart};

        // \r trimming
        if (lineLength > 0 && lineStart[lineLength - 1] == '\r')
        {
            --lineLength;
        }

        std::string_view lineView(lineStart, lineLength);
        ++lineNumber;
        
        // Detect format without double file open
        if (dateFormat == LogDateFormat::UNKNOWN && lineView.size() >= TIMESTAMP_PREFIX_LENGTH)
        {
            dateFormat = detect_date_format(std::string(lineView.substr(0, TIMESTAMP_PREFIX_LENGTH)));
        }

        // Date filtering (only create string if we need timestamp)
        bool skipLine = false;
        if (options.fromTime || options.toTime)
        {
            if (lineLength >= TIMESTAMP_PREFIX_LENGTH)
            {
                // Stack buffer to avoid heap allocations
                char tsBuf[TIMESTAMP_PREFIX_LENGTH + 1];
                memcpy(tsBuf, lineStart, TIMESTAMP_PREFIX_LENGTH);
                tsBuf[TIMESTAMP_PREFIX_LENGTH] = '\0';
                
                auto ts = parse_log_timestamp(tsBuf, dateFormat);
                if (ts)
                {
                    ++linesWithTimestamps;
                    if (options.fromTime && *ts < *(options.fromTime))
                        skipLine = true;
                    else if (options.toTime && *ts > *(options.toTime))
                        skipLine = true;
                }
            }
        }

        if (!skipLine)
        {
            bool found = false;

            if (options.useRegex)
            {
                // Regex needs string (unavoidable copy)
                std::string lineStr(lineView);
                for (const auto& regexPattern : regexPatterns)
                {
                    if (std::regex_search(lineStr, regexPattern))
                    {
                        found = true;
                        break;
                    }
                }
            }
            else
            {
                // Pure string_view matching - NO allocations!
                for (const auto& pattern : options.searchPatterns)
                {
                    bool match = options.caseInsensitive 
                        ? contains_case_insensitive(lineView, pattern) 
                        : lineView.find(pattern) != std::string_view::npos;

                    if (match)
                    {
                        found = true;
                        break;
                    }
                }
            }

            // Context handling
            if (found)
            {
                if (needsSeparator && lastPrintedLine != -1 && lineNumber - lastPrintedLine > 1)
                {
                    std::cout << "--\n";
                }

                for (const auto& [bufLineNum, bufLine] : beforeBuffer)
                {
                    if (bufLineNum > lastPrintedLine)
                    {
                        std::cout << CONTEXT_COLOR << "[C:L" << bufLineNum << "] " << bufLine << RESET_COLOR << '\n';
                        lastPrintedLine = bufLineNum;
                    }
                }

                LogLevel level = detect_log_level(std::string(lineView), options.logFormat);
                auto color = get_log_level_color(level);
                std::cout << color << "[" << static_cast<int>(level) << ":L" << lineNumber << "] " << lineView << RESET_COLOR << '\n';
                lastPrintedLine = lineNumber;
                ++matchCount;

                afterContextRemaining = options.afterContext;
                beforeBuffer.clear();
                needsSeparator = true;
            }

            else if (afterContextRemaining > 0)
            {
                if (lineNumber > lastPrintedLine)
                {
                    std::cout << CONTEXT_COLOR << "[C:L" << lineNumber << "] " << lineView << RESET_COLOR << '\n';
                    lastPrintedLine = lineNumber;
                }
                --afterContextRemaining;
            }

            else
            {
                if (options.beforeContext > 0)
                {
                    beforeBuffer.push_back({lineNumber, std::string(lineView)});
                    if (static_cast<int>(beforeBuffer.size()) > options.beforeContext)
                    {
                        beforeBuffer.pop_front();
                    }
                }
            }
        }

        // Move to next line
        lineStart = lineEnd + (lineEnd < fileEnd ? 1 : 0);
    }

    // Cleanup of mapped memory
    munmap(fileData, fileSize);

    // Warn user if date filtering was applied but no timestamps were found
    if ((options.fromTime || options.toTime) && linesWithTimestamps == 0)
    {
        std::cerr << '\n';
        std::cerr << "Warning: Date filtering was requested, but no valid timestamps were found in the log lines." << std::endl;
    }

    std::cout << '\n';
    std::cout << "Total Matches: " << matchCount << std::endl;

    return EXIT_SUCCESS;
}
