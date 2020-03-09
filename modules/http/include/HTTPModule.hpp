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

namespace Zia::HTTP {

    class HTTPModule: public oZ::IModule {
    public:
        HTTPModule();

        void onRegisterCallbacks(oZ::Pipeline &pipeline) override;
        const char *getName() const override { return "HTTP"; }

    protected:
    private:
        static const std::regex REQUEST_LINE_REGEX;
        Logger _logger;

        bool onPipelineBeforeParseASAP(oZ::Context &context);
        bool onPipelineBeforeParse(oZ::Context &context);
        bool onPipelineCompleted(oZ::Context &context);

        static oZ::HTTP::Method getHttpMethodFromStr(const std::string &str);
        static oZ::HTTP::Version getHttpVersionFromStr(const std::string &str);
    };
}