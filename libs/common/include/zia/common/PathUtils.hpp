/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** zia common PathUtils.hpp
*/

#pragma once

#include <algorithm>
#include <string>
#include <vector>
#include "openZia/OperatingSystem.hpp"

#if __has_include(<filesystem>)
    #include <filesystem>
    namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
    #include <experimental/filesystem>
    namespace fs = std::experimental::filesystem;
#else
    #error Filesystem library required
#endif

namespace Zia {

    class PathUtils {
    public:
        template<typename First, typename... Others>
        static fs::path join(const First &parent, const Others &... files)
        {
            return fs::path(parent) / join(files...);
        }

        static fs::path join(const std::string &parent, const std::vector<std::string> &files)
        {
            fs::path res = fs::path(parent);

            for (const std::string &file : files)
                res /= file;
            return res;
        }

        static void updateSeparators(std::string &str)
        {
            #if defined(SYSTEM_WINDOWS)
                std::replace(str.begin(), str.end(), '/', '\\');
            #else
                std::replace(str.begin(), str.end(), '/', '/');
            #endif
        }

    protected:
    private:
        template<typename First>
        static fs::path join(const First &file)
        {
            return fs::path(file);
        }
    };

}