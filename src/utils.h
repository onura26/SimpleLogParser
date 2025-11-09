// src/utils.h

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <algorithm>

// ANSI color codes for terminal text formatting
constexpr const char* RED_COLOR = "\033[31m";
constexpr const char* YELLOW_COLOR = "\033[33m";
constexpr const char* GREEN_COLOR = "\033[32m";
constexpr const char* BLUE_COLOR = "\033[34m";
constexpr const char* RESET_COLOR = "\033[0m";

constexpr std::string_view PROGRAM_VERSION {"v1.4"};

enum class LogLevel {
    FATAL,
    ERROR,
    WARNING,
    INFO,
    DEBUG,
    UNKNOWN
};

struct LogLevelConfig {
    std::vector<std::string> fatalKeywords;
    std::vector<std::string> errorKeywords;
    std::vector<std::string> warningKeywords;
    std::vector<std::string> infoKeywords;
    std::vector<std::string> debugKeywords;
};

namespace LogFormats {
    inline const LogLevelConfig GENERIC = {
        {"fatal", "critical", "emergency", "alert"},
        {"error", "err", "exception"},
        {"warning", "warn", "caution"},
        {"info", "information", "notice"},
        {"debug", "dbg", "trace", "verbose"}
    };

    inline const LogLevelConfig SYSLOG = {
        {"emergency", "alert", "critical"},
        {"error"},
        {"warning", "notice"},
        {"info"},
        {"debug"}
    };

    inline const LogLevelConfig JAVA = {
        {"fatal"},
        {"error"},
        {"warn"},
        {"info"},
        {"debug", "trace"}
    };

    inline const LogLevelConfig ANDROID = {
        {" F ", " F/"}, 
    {" E ", " E/"},
    {" W ", " W/"},
    {" I ", " I/"},
    {" D ", " V "}  // V = verbose = debug level
    };
}

inline const LogLevelConfig& DEFAULT_LOG_LEVEL_CONFIG = LogFormats::GENERIC;

/**
 * Detects the log severity level from a line content.
 * Scans line for keywords defined in config (case-insensitive).
 * Checks in this order: FATAL -> ERROR -> WARNING -> INFO -> DEBUG.
 * 
 * @param line Log line to analyze.
 * @param config Keyword patterns for each log level.
 * @return Detected log level, or UNKNOWN if none matched.
 */
LogLevel detect_log_level(const std::string& line, const LogLevelConfig& config);

/**
 * Map log level to corresponding ANSI color code (for terminal output).
 * FATAL/ERROR = Red, WARNING = Yellow, INFO = Green, DEBUG = Blue, UNKNOWN = Reset.
 * 
 * @param level Log level enum.
 * @return ANSI escape sequence string for the color (must not be freed).
 */
const char* get_log_level_color(LogLevel level);

/**
 * Converts a string to lowercase.
 * 
 * @param str Input string.
 * @return Lowercase version of the input string.
 */
std::string to_lower(const std::string& str);

/**
 * Lambda-style function to convert string to lowercase.
 * Seaches case-insensitive substring with zero-copy string_view.
 * 
 * @param haystack String to search within.
 * @param needle Substring to search for.
 * @return true if needle found in haystack (case-insensitive), false otherwise
 */
bool contains_case_insensitive(std::string_view haystack, std::string_view needle);

#endif // UTILS_H