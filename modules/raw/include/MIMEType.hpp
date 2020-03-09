/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** raw MIMEType.hpp
*/

#pragma once

#include <string>
#include <unordered_map>
#include <stdexcept>

namespace Zia::Raw {

    class MIMEType {
    public:
        static const std::string &getFromFile(const std::string &path)
        {
            auto dot = path.rfind(".");

            if (dot == std::string::npos)
                throw std::runtime_error("Unknown MIME type");
            
            std::string ext = path.substr(dot);
            return MAPPINGS.find(ext)->second;
        }

        static bool isKnowingExtension(const std::string &path)
        {
            auto dot = path.rfind(".");

            if (dot == std::string::npos)
                return false;
            
            std::string ext = path.substr(dot);

            return MAPPINGS.find(ext) != MAPPINGS.end();
        }
    
    protected:
    private:
        static const std::unordered_map<std::string, std::string> MAPPINGS;
    };

}
