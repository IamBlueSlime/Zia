/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** zia common StringUtils.hpp
*/

#pragma once

#include <string>

namespace Zia {

    class StringUtils {
        public:
            static bool endsWith(const std::string &str, const std::string &ending) {
                if (str.length() < ending.length())
                    return false;

                return str.compare(str.length() - ending.length(), ending.length(), ending) == 0;
            }

        protected:
        private:
    };

}