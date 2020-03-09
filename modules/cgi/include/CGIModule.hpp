/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** http HttpModule.hpp
*/

#pragma once

#include <regex>
#include "openZia/IModule.hpp"
#include "openZia/HTTP.hpp"
#include "zia/common/Logger.hpp"

namespace Zia::CGI {

    class CGIModule: public oZ::IModule {
    public:
        CGIModule();

        void onRegisterCallbacks(oZ::Pipeline &pipeline) override;
        const char *getName() const override { return "CGI"; }
        void onLoadConfigurationFile(const std::string &directory) override;

    protected:
    private:
        static const std::regex REQUEST_LINE_REGEX;
        Logger _logger;
        std::string _root;

        bool onInterpretCgi(oZ::Context &context);

        static oZ::HTTP::Method getHttpMethodFromStr(const std::string &str);
        static oZ::HTTP::Version getHttpVersionFromStr(const std::string &str);

        const std::string FAILED_EXEC_RESPONCE_CGI = "Status: 500\r\n\r\n\nERROR";

        static bool isExecutable(const std::string& path);
        static std::string make_query_param(const oZ::HTTP::QueryParameters &paramList);
    };

}