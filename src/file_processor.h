// src/file_processor.h

#ifndef FILE_PROCESSOR_H
#define FILE_PROCESSOR_H

#include "arg_parser.h"
#include "utils.h"
#include "date.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <cstdlib>
#include <cstdio>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string_view>
#include <cstring>

constexpr int PRE_ALLOCATION_SIZE {512};

/**
 * Memory-mapped log file search with pattern matching and context lines.
 * 
 * ALGORITHM OVERVIEW:
 * 1. Memory-map the target log file for efficient access.
 * 2. Compile search patterns into regex if -r flag is set.
 * 3. Implement grep-style context lines (-A, -B, -C flags)
 *   - Ring buffer (deque) for before-context lines.
 *   - Countdown timer for after-context lines.
 * 4. Deduplication to avoid printing the same line multiple times.
 * 5. Print separators ("--") between close match groups.
 * 
 * @param options Parsed program options.
 * @return EXIT_SUCCESS on completion, EXIT_FAILURE on error.
 * @throws std::runtime_error on file or memory mapping errors.
 */
int search_in_file(const ProgramOptions& options);

#endif // FILE_PROCESSOR_H;