// main.cpp

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <array>
#include "src/arg_parser.h"
#include "src/file_processor.h"

/**
 * High-performance log parser with grep-style pattern matching,
 * date range filtering, and context lines support.
 * 
 * Usage: logparser <file> <pattern1> [pattern2 ...] [options]
 * 
 * Optimizations:
 * - Memory-mapped file access for large log files.
 * - Pre-compiled regex patterns (if -r flag used).
 * - Cached date format detection to speed up timestamp parsing.
 * - Efficient context line handling with ring buffers.
 * - Zero-copy string views for substring operations.
 * - Deque for O(1) ring buffer management.
 * - Deduplication of printed lines to avoid repeats.
 * 
 * Supported Features:
 * - Multiple search patterns (literal or regex).
 * - Case-insensitive search (-i flag).
 * - Date range filtering (-from, -to flags).
 * - Log format configuration (-f, --log-format flag).
 * - Grep-style context lines (-A, -B, -C flags).
 * - ANSI color-coded output based on log severity levels.
 * 
 * @author Onur Aydoğan
 * @version 1.5
 */
int main(int argc, char* argv[])
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    try
    {
        ProgramOptions options = parse_arguments(argc, argv);
        return search_in_file(options);
    }

    catch (const std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
}