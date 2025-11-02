#include <iostream>
#include <cstdlib>
#include <print>
#include <cstdio>
#include "src/arg_parser.h"
#include "src/file_processor.h"

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
        std::println(stderr, "Error: {}", ex.what());
        return EXIT_FAILURE;
    }
}