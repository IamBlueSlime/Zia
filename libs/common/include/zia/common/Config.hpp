/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** zia common ConfigUtils.hpp
*/

#pragma once

#include <fstream>
#include <string>

#if __has_include(<toml11/toml.hpp>)
    #include <toml11/toml.hpp>
#elif __has_include(<toml.hpp>)
    #include <toml.hpp>
#else
    #error Toml11 library required
#endif

namespace Zia {

    class Config {
    public:
        Config(const std::string &path) : _path(path)
        {
            this->reload();
        }

        void reload()
        {
            this->_root = toml::parse(this->_path);
        }

        void save()
        {
            std::ofstream out(this->_path);
            out << this->_root;
            out.close();
        }

        toml::value &root() { return this->_root; }
        const toml::value &root() const { return this->_root; }

    protected:
    private:
        const std::string _path;
        toml::value _root;
    };

}