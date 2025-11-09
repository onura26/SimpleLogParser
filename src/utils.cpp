// src/utils.cpp

#include "utils.h"

LogLevel detect_log_level(const std::string& line, const LogLevelConfig& config)
{
    for (const auto& keyword : config.fatalKeywords) {
        if (contains_case_insensitive(line, keyword)) {
            return LogLevel::FATAL;
        }
    }

    for (const auto& keyword : config.errorKeywords) {
        if (contains_case_insensitive(line, keyword)) {
            return LogLevel::ERROR;
        }
    }

    for (const auto& keyword : config.warningKeywords) {
        if (contains_case_insensitive(line, keyword)) {
            return LogLevel::WARNING;
        }
    }

    for (const auto& keyword : config.infoKeywords) {
        if (contains_case_insensitive(line, keyword)) {
            return LogLevel::INFO;
        }
    }

    for (const auto& keyword : config.debugKeywords) {
        if (contains_case_insensitive(line, keyword)) {
            return LogLevel::DEBUG;
        }
    }

    return LogLevel::UNKNOWN;
}

const char* get_log_level_color(LogLevel level)
{
    switch (level) {
        case LogLevel::FATAL:
            return RED_COLOR;
        case LogLevel::ERROR:
            return RED_COLOR;
        case LogLevel::WARNING:
            return YELLOW_COLOR;
        case LogLevel::INFO:
            return GREEN_COLOR;
        case LogLevel::DEBUG:
            return BLUE_COLOR;
        default:
            return RESET_COLOR;
    }
}

std::string to_lower(const std::string& str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    
    return result;
}

bool contains_case_insensitive(std::string_view haystack, std::string_view needle)
{
    auto it = std::search(haystack.begin(), haystack.end(), 
                        needle.begin(), needle.end(), 
                        [](char ch1, char ch2) { return std::tolower(ch1) == std::tolower(ch2); });

    return it != haystack.end();
}