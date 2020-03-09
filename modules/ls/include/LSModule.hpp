/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** http HttpModule.hpp
*/

#pragma once

#include <string>
#include "openZia/IModule.hpp"
#include "zia/common/Logger.hpp"

namespace Zia::LS {

    class LSModule: public oZ::IModule {
    public:
        void onRegisterCallbacks(oZ::Pipeline &pipeline) override;
        const char *getName() const override { return "LS"; }
        bool onInterpret(oZ::Context &ctx);
        void onLoadConfigurationFile(const std::string &directory) override;

    protected:
    private:
        static std::string toLower(const std::string &str);
        static bool sortOrder(const fs::directory_entry &first, const fs::directory_entry &second);
        Logger _logger = Logger("LSModule");
        std::string fill(const std::string &totalPath, std::string &askedPath);
        std::string _root = "";
        std::string _theme = "";
    };

}