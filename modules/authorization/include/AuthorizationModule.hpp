/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** authorization AuthorizationModule.hpp
*/

#pragma once

#include "zia/common/Config.hpp"
#include "openZia/IModule.hpp"
#include "openZia/Pipeline.hpp"
#include "zia/common/Logger.hpp"

namespace Zia::Authorization {

    struct AuthorizationNode {
        std::string username;
        std::string password;
        std::vector<std::string> resources = std::vector<std::string>();
    };

    class AuthorizationModule: public oZ::IModule {
    public:
        AuthorizationModule();

        void onLoadConfigurationFile(const std::string &directory) override;

        void onRegisterCallbacks(oZ::Pipeline &pipeline) override;
        const char *getName() const override { return "Authorization"; }

    protected:
    private:
        Logger _logger;
        std::vector<AuthorizationNode> _nodes = std::vector<AuthorizationNode>();

        bool onPipelineInterpret(oZ::Context &context);
        bool validateCredentials(AuthorizationNode &node, const std::string &kind,
            const std::string &value) const;
    };

}