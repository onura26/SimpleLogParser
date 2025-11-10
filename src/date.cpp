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
    std::tm tm = {};
    std::string dateString(dateStr);
    std::istringstream ss(dateString);

    switch (format)
    {
        case LogDateFormat::YYYY_MM_DD_HH_MM_SS:
            ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
            break;
        case LogDateFormat::DD_MM_YYYY_HH_MM_SS:
            ss >> std::get_time(&tm, "%d-%m-%Y %H:%M:%S");
            break;
        case LogDateFormat::MM_DD_YYYY_HH_MM_SS:
            ss >> std::get_time(&tm, "%m-%d-%Y %H:%M:%S");
            break;
        default:
            return std::nullopt;
    }

    if (ss.fail())
        return std::nullopt;

    std::time_t tt = std::mktime(&tm);
    if (tt == -1)
        return std::nullopt;

    return std::chrono::system_clock::from_time_t(tt);
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

