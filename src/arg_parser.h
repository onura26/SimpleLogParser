// src/arg_parser.h

#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include <string>
#include <vector>
#include <optional>
#include <chrono>
#include "utils.h"
#include "date.h"
#include <stdexcept>
#include <regex>

constexpr int MIN_REQUIRED_ARGS {2};
constexpr int FIRST_PATTERN_ARG_INDEX {2};

/**
 * Command-line program options structure (parsed from argv).
 * Supports pattern matching, date range filtering, and grep-style context lines.
 */
struct ProgramOptions
{
    std::string inputFilePath;               // target log file
    std::vector<std::string> searchPatterns; // patterns to match (literal or regex)
    bool caseInsensitive {false};            // -i flag
    bool useRegex {false};                   // -r flag
    
    // date range filtering (-from, -to flags)
    std::optional<std::chrono::system_clock::time_point> fromTime;
    std::optional<std::chrono::system_clock::time_point> toTime;

    // log format config (-f, --log-format flag)
    LogLevelConfig logFormat {DEFAULT_LOG_LEVEL_CONFIG};

    // cache detected date format to avoid re-detection (optimizes regex)
    LogDateFormat detectedDateFormat {LogDateFormat::UNKNOWN};

    // context lines (grep-style): -B (before), -A (after), -C (both)
    int beforeContext {0}; 
    int afterContext {0};  
};

/**
 * Parses command-line arguments into ProgramOptions structure.
 * Validates required arguments and options.
 * 
 * @param argc Argument count from main().
 * @param argv Argument vector from main().
 * @return Parsed ProgramOptions structure.
 * @throws std::runtime_error on invalid arguments or malformed regex.
 */
ProgramOptions parse_arguments(int argc, char* argv[]);

#endif // ARG_PARSER_H