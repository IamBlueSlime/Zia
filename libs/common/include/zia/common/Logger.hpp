/*
** EPITECH PROJECT, 2019
** CPP_zia_2019
** File description:
** zia common Logger.hpp
*/

#pragma once

#include <string>
#include <mutex>

namespace Zia {

    class Logger {
    public:
        Logger(const std::string &name);

        enum class LogLevel {
            LOG_DEBUG, LOG_INFO, LOG_WARNING, LOG_ERROR, LOG_CRITICAL
        };

        void log(LogLevel targetLevel, const std::string &message) const;
        void debug(const std::string &message) const { log(LogLevel::LOG_DEBUG, message); }
        void info(const std::string &message) const { log(LogLevel::LOG_INFO, message); }
        void warning(const std::string &message) const { log(LogLevel::LOG_WARNING, message); }
        void error(const std::string &message) const { log(LogLevel::LOG_ERROR, message); }
        void critical(const std::string &message) const { log(LogLevel::LOG_CRITICAL, message); }

        void setLevel(LogLevel level);

    protected:
    private:
        static std::size_t maxLength_;
        static std::mutex mutex_;
        const std::string name_;
        LogLevel level_ = LogLevel::LOG_INFO;

        static std::string getFormattedLevel(LogLevel level);
    };

}
