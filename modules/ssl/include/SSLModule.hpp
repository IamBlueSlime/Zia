/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** ssl SSLModule.hpp
*/

#pragma once

#include <regex>
#include <unordered_map>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "zia/common/Network.hpp"
#include "zia/common/Config.hpp"
#include "openZia/IModule.hpp"
#include "openZia/Pipeline.hpp"
#include "zia/common/Logger.hpp"

namespace Zia::SSL {

    class SSLModule: public oZ::IModule {
    public:
        SSLModule();
        ~SSLModule();

        void onLoadConfigurationFile(const std::string &directory) override;

        void onRegisterCallbacks(oZ::Pipeline &pipeline) override;
        const char *getName() const override { return "SSL"; }

        void onConnection(oZ::Context &context) override;
        void onDisconnection(oZ::Context &context) override;

    protected:
    private:
        Logger _logger;
        SSL_CTX *sslCtx = nullptr;

        void initSSLContext(const std::string &certPath, const std::string &privKeyPath);
        void destroySSLContext();

        bool onPipelineBeforeParse(oZ::Context &context);
        bool onPipelineCompleted(oZ::Context &context);
    };

}