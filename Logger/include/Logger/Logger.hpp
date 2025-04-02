#pragma once

#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

/**
 * @class Logger
 * @brief Abstract base class for logging functionality.
 *
 * Provides a common interface for logging messages and utility methods for formatting messages with timestamps. 
 * Derived classes must implement the `log_with_timestamp` method.
 */
class Logger 
{
public:

    /**
     * @brief Virtual destructor for proper cleanup of derived classes.
     */
    virtual ~Logger() = default;

    /**
     * @brief Logs a message with a timestamp.
     *
     * Formats the message with a timestamp and delegates it to the `log_with_timestamp` method.
     * @param message The message to be logged.
     */
    void log(const std::string&);

protected:

    /**
     * @brief Formats a message with a timestamp.
     * 
     * Adds a timestamp in the format `[DD.MM.YYYY HH:MM:SS]` to the given message.
     * 
     * @param message The message to format.
     * @return std::string The formatted message with timestamp.
     */
    std::string format_with_timestamp(const std::string&);

    /**
     * @brief Pure virtual method for logging a message with a timestamp.
     *
     * Must be implemented by derived classes to define how the message is logged (e.g., to a file).
     *
     * @param message The message to log, already formatted with a timestamp.
     */
    virtual void log_with_timestamp(const std::string&) = 0;
};

/**
 * @class FileLogger
 * @brief A logger that writes messages to a file.
 *
 * Implements the `Logger` interface to log messages to a specified file in append mode.
 */
class FileLogger : public Logger 
{
public:

    /**
     * @brief Constructs a `FileLogger` instance.
     * 
     * Creates or opens a log file in the specified directory.
     * 
     * @param log_dir The directory where the log file will be stored.
     * @param log_file_name The name of the log file.
     */
    explicit FileLogger(const std::string&, const std::string&);

    /**
     * @brief Logs a message with a timestamp to the file.
     *
     * Writes the formatted message to the log file if it is open.
     *
     * @param message The message to log, already formatted with a timestamp.
     */
    void log_with_timestamp(const std::string&) override;

private:
    std::ofstream log_file_;

    /**
     * @brief Creates a log file in the specified directory.
     *
     * Ensures the directory exists and opens the file in append mode.
     *
     * @param log_dir The directory where the log file will be stored.
     * @param log_file_name The name of the log file.
     * @return std::ofstream The output file stream for the log file.
     */
    static std::ofstream create_log_file(const std::string& log_dir, const std::string& log_file_name) 
    {
        if (!std::filesystem::exists(log_dir)) 
            std::filesystem::create_directory(log_dir);

        std::string full_path = log_dir + "/" + log_file_name;
        
        return std::ofstream(full_path, std::ios::app);
    }
};

/**
 * @class ErrorLogger
 * @brief A logger specialized for error messages.
 *
 * Extends the `Logger` class to log error messages with an `[ERROR]` prefix to a specified file.
 */
class ErrorLogger : public Logger 
{
public:

    /**
     * @brief Constructs an `ErrorLogger` instance.
     * 
     * Creates or opens an error log file in the specified directory.
     * 
     * @param error_log_dir The directory where the error log file will be stored.
     * @param error_log_file_name The name of the error log file.
     */
    explicit ErrorLogger(const std::string&, const std::string&);

    /**
     * @brief Logs an error message with a timestamp to the file.
     *
     * Writes the formatted message with an `[ERROR]` prefix to the error log file if it is open.
     *
     * @param message The message to log, already formatted with a timestamp.
     */
    void log_with_timestamp(const std::string&) override;

private:
    std::ofstream error_log_file_;

    /**
     * @brief Creates an error log file in the specified directory.
     *
     * Ensures the directory exists and opens the file in append mode.
     *
     * @param error_log_dir The directory where the error log file will be
     * stored.
     * @param error_log_file_name The name of the error log file.
     * @return std::ofstream The output file stream for the error log file.
     */
    static std::ofstream create_error_log_file(const std::string& error_log_dir, const std::string& error_log_file_name) 
    {
        if (!std::filesystem::exists(error_log_dir)) 
            std::filesystem::create_directory(error_log_dir);

        std::string full_path = error_log_dir + "/" + error_log_file_name;
        return std::ofstream(full_path, std::ios::app);
    }
};