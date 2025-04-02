#pragma once

#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

class Logger 
{
public:
    virtual ~Logger() = default;

    void log(const std::string&);

protected:
    std::string format_with_timestamp(const std::string&);

    virtual void log_with_timestamp(const std::string&) = 0;
};

class FileLogger : public Logger 
{
public:
    explicit FileLogger(const std::string&, const std::string&);

    void log_with_timestamp(const std::string&) override;

private:
    std::ofstream log_file_;

    static std::ofstream create_log_file(const std::string& log_dir, const std::string& log_file_name) 
    {
        if (!std::filesystem::exists(log_dir)) 
            std::filesystem::create_directory(log_dir);

        std::string full_path = log_dir + "/" + log_file_name;
        
        return std::ofstream(full_path, std::ios::app);
    }
};

class ErrorLogger : public Logger 
{
public:
    explicit ErrorLogger(const std::string&, const std::string&);

    void log_with_timestamp(const std::string&) override;

private:
    std::ofstream error_log_file_;

    static std::ofstream create_error_log_file(const std::string& error_log_dir, const std::string& error_log_file_name) 
    {
        if (!std::filesystem::exists(error_log_dir)) 
            std::filesystem::create_directory(error_log_dir);

        std::string full_path = error_log_dir + "/" + error_log_file_name;
        return std::ofstream(full_path, std::ios::app);
    }
};