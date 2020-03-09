/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** http HTTPModule.hpp
*/

#pragma once

#include <regex>
#include "openZia/IModule.hpp"
#include "openZia/HTTP.hpp"
#include "zia/common/Logger.hpp"
#include "openZia/Pipeline.hpp"

namespace Zia::Headers {

    class HeadersModule: public oZ::IModule {
    public:
        HeadersModule() : _logger("HeadersModule") {};
        void onRegisterCallbacks(oZ::Pipeline &pipeline) override;
        const char *getName() const override { return "Headers"; }
        bool onPipelineBeforeParse(oZ::Context &context);

    protected:
    private:
        Logger _logger;
        static const std::regex REQUEST_LINE_REGEX;
        bool onParseASAP(oZ::Context &context);
    };
} 