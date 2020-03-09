/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** zia common Base64.hpp
*/

#pragma once

#include <string>

namespace Zia {

    class Base64 {
    public:
        static std::string encode(const unsigned char *bytes, unsigned int len);
        static std::string decode(const std::string &s);

    protected:
    private:
    };

}