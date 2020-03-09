/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** error ErrorModule.hpp
*/

#pragma once

#include <regex>
#include "openZia/IModule.hpp"
#include "openZia/HTTP.hpp"
#include "zia/common/Logger.hpp"

namespace Zia::Error {

    class ErrorModule: public oZ::IModule {
    public:
        ErrorModule();

        void onRegisterCallbacks(oZ::Pipeline &pipeline) override;
        const char *getName() const override { return "Error"; }
        void onLoadConfigurationFile(const std::string &directory) override;

    protected:
    private:
        Logger _logger;
        std::string _theme;

        bool onPipelineError(oZ::Context &context);
    };
}