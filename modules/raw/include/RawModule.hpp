/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** raw RawModule.hpp
*/

#pragma once

#include <regex>
#include "openZia/IModule.hpp"
#include "openZia/HTTP.hpp"
#include "zia/common/Logger.hpp"

namespace Zia::Raw {

    class RawModule: public oZ::IModule {
    public:
        RawModule();

        void onLoadConfigurationFile(const std::string &directory) override;
        void onRegisterCallbacks(oZ::Pipeline &pipeline) override;
        const char *getName() const override { return "Raw"; }

    protected:
    private:
        Logger _logger;
        std::string _root;

        bool onPipelineInterpretLast(oZ::Context &context);
    };
}