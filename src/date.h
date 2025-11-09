// src/date.h
#ifndef DATE_H
#define DATE_H

#include <string>
#include <optional>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <regex>
#include <ctime>
#include <fstream>

enum class LogDateFormat 
{
    YYYY_MM_DD_HH_MM_SS,
    DD_MM_YYYY_HH_MM_SS,
    MM_DD_YYYY_HH_MM_SS,
    UNKNOWN
};

// Constant(s)
constexpr off_t TIMESTAMP_PREFIX_LENGTH {19}; // Length of "YYYY-MM-DD HH:MM:SS"

/**
 * Detects the date format of a given date string.
 * Checks YYYY-MM-DD first (most common in logs), then falls back to DD-MM-YYYY.
 * Returns UNKNOWN if no known format is detected.
 * 
 * @param dateStr The date string to analyze (should be at least 19 chars for full timestamp).
 * @return Detected format enum, or UNKNOWN if not recognized.
 */
LogDateFormat detect_date_format(const std::string& dateStr);

/**
 * Parse timestamp string into system_clock::time_point.
 * Format must be pre-detected via detect_date_format().
 * 
 * @param dateStr Timestamp string to parse (e.g., "2025-11-09 14:23:45")
 * @param format Pre-detected format of the timestamp.
 * @return Parsed time_point, or std::nullopt if parsing fails.
 */
std::optional<std::chrono::system_clock::time_point> parse_log_timestamp(
    std::string_view dateStr, 
    LogDateFormat format);

/**
 * Extracts and parses timestamp from a log line.
 * Assumes timestamp is in first 19 characters of the line.
 * Returns std::nullopt if line too short, format unknown, or parsing fails.
 * 
 * @param line Full log line containing the timestamp.
 * @param format Pre-detected format of the timestamp (must not be UNKNOWN).
 * @return Parsed time_point, or std::nullopt on failure.
 */
std::optional<std::chrono::system_clock::time_point> extract_timestamp(const std::string& line, LogDateFormat format);

#endif // DATE_H