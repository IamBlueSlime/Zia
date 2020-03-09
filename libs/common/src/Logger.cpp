/*
** EPITECH PROJECT, 2019
** CPP_zia_2019
** File description:
** Logger.cpp
*/

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <ctime>
#include "zia/common/Logger.hpp"

namespace Zia {
    
    std::size_t Logger::maxLength_ = 25;
    std::mutex Logger::mutex_ = std::mutex();

    Logger::Logger(const std::string &name) : name_(name)
    {
        std::string envKey = "LOGGER_" + name;

        std::transform(
            envKey.begin(), envKey.end(), envKey.begin(), [](unsigned char c) {
                return std::toupper(c);
            }
        );

        if (name.size() > maxLength_)
            maxLength_ = name.size() + 3;

        const char *envValue = std::getenv(envKey.c_str());

        if (envValue == nullptr)
            envValue = std::getenv("LOGGER_ALL");

        if (envValue != nullptr) {
            std::string envValueStr(envValue);

            if (envValueStr == "DEBUG")
                this->setLevel(LogLevel::LOG_DEBUG);
            else if (envValueStr == "INFO")
                this->setLevel(LogLevel::LOG_INFO);
            else if (envValueStr == "WARNING")
                this->setLevel(LogLevel::LOG_WARNING);
            else if (envValueStr == "ERROR")
                this->setLevel(LogLevel::LOG_ERROR);
            else if (envValueStr == "CRITICAL")
                this->setLevel(LogLevel::LOG_CRITICAL);
        }
    }

    void Logger::log(LogLevel targetLevel, const std::string &message) const
    {
        if (targetLevel < level_)
            return;

        std::unique_lock lock(this->mutex_);
        
        std::time_t currentTime = std::time(nullptr);
        char buffer[100];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S",
            std::localtime(&currentTime));

        std::cerr <<
            "[" << buffer << "] " <<
            std::right << std::setw(maxLength_) << std::setfill(' ') << "[" + name_ + "] " <<
            std::left << std::setw(24) << std::setfill(' ') <<
            getFormattedLevel(targetLevel) << ": " <<
            message <<
        std::endl;
        std::cerr.flush();
    }

    void Logger::setLevel(LogLevel level)
    {
        level_ = level;
    }

    std::string Logger::getFormattedLevel(LogLevel level)
    {
        if (level == LogLevel::LOG_DEBUG)
            return "\e[34mdebug\e[39m";
        else if (level == LogLevel::LOG_INFO)
            return "\e[32minfo\e[39m";
        else if (level == LogLevel::LOG_WARNING)
            return "\e[93mwarning\e[39m";
        else if (level == LogLevel::LOG_ERROR)
            return "\e[31merror\e[39m";
        else if (level == LogLevel::LOG_CRITICAL)
            return "\e[91mcritical\e[39m";

        return "<unknown>";
    }

}
