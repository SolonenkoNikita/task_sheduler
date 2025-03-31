#include "Logger/Logger.hpp"

void Logger::log(const std::string & message)
{
    log_with_timestamp(message);
}

std::string Logger::format_with_timestamp(const std::string &message) 
{
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);

    std::ostringstream oss;
    oss << "[" << std::put_time(std::localtime(&time), "%d.%m.%Y %H:%M:%S") << "] " << message;
    return oss.str();
}

FileLogger::FileLogger(const std::string &log_dir, const std::string &log_file_name)
    : log_file_(create_log_file(log_dir, log_file_name)) {}

void FileLogger::log_with_timestamp(const std::string &message)  
{
    if (log_file_.is_open()) 
        log_file_ << format_with_timestamp(message) << "\n";
}
