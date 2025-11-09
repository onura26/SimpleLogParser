// src/date.cpp
#include "date.h"

LogDateFormat detect_date_format(const std::string& dateStr)
{
    // Regex patterns for different date formats defined in enum LogDateFormat
    // NOTE: Check YYYY-MM-DD first (most common for logs)
    static const std::regex ymd(R"(^\d{4}[-./]\d{2}[-./]\d{2})");

    if (std::regex_search(dateStr, ymd))
        return LogDateFormat::YYYY_MM_DD_HH_MM_SS;

    // For DD-MM-YYYY vs MM-DD-YYYY
    // We will default to DD-MM-YYYY (European/ISO standard)
    // If you need MM-DD-YYYY, change the order or add a config flag
    static const std::regex dmyOrMdy(R"(^\d{2}[-./]\d{2}[-./]\d{4})");

    if (std::regex_search(dateStr, dmyOrMdy))
    {
        // Try parsing as DD-MM-YYYY first
        // If your logs are US format, return MM-DD-YYYY instead
        return LogDateFormat::DD_MM_YYYY_HH_MM_SS;
    }

    return LogDateFormat::UNKNOWN; // Unknown or unsupported format
}

std::optional<std::chrono::system_clock::time_point> 
parse_log_timestamp(std::string_view dateStr, LogDateFormat format)
{
    std::string dateString(dateStr); 
    std::istringstream ss(dateString); // Input string stream for parsing
    std::chrono::sys_seconds tp;

    switch (format)
    {
        case LogDateFormat::YYYY_MM_DD_HH_MM_SS:
            ss >> std::chrono::parse("%Y-%m-%d %H:%M:%S", tp);
            break;
        case LogDateFormat::DD_MM_YYYY_HH_MM_SS:
            ss >> std::chrono::parse("%d-%m-%Y %H:%M:%S", tp);
            break;
        case LogDateFormat::MM_DD_YYYY_HH_MM_SS:
            ss >> std::chrono::parse("%m-%d-%Y %H:%M:%S", tp);
            break;
        default:
            return std::nullopt; // Unsupported format
    }

    // Check if parsing was successful
    if (ss.fail())
        return std::nullopt;

    return tp;
}

std::optional<std::chrono::system_clock::time_point> extract_timestamp(const std::string& line, LogDateFormat format)
{
    // Log timestamp length and format check
    if (line.size() < TIMESTAMP_PREFIX_LENGTH)
        return std::nullopt; 
    if (format == LogDateFormat::UNKNOWN)
        return std::nullopt;

    std::string_view prefix(line.data(), TIMESTAMP_PREFIX_LENGTH);
    
    // For performance, no more regex detection here - just parse with known format.

    return parse_log_timestamp(prefix, format);
}

