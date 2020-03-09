/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** AuthorizationModule.cpp
*/

#include <algorithm>
#include <sstream>
#include "zia/common/PathUtils.hpp"
#include "zia/common/Base64.hpp"
#include "AuthorizationModule.hpp"

namespace Zia::Authorization {

    AuthorizationModule::AuthorizationModule() : _logger("AuthorizationModule") {}

    void AuthorizationModule::onLoadConfigurationFile(const std::string &directory)
    {
        try {
            Config config(PathUtils::join(directory, "authorization.toml").string());

            if (!config.root().contains("rules"))
                return;

            for (const auto &rule : toml::find<toml::array>(config.root(), "rules")) {
                if (rule.count("username") == 0)
                    throw std::runtime_error("Missing username in rule");
                if (rule.count("password") == 0)
                    throw std::runtime_error("Missing password in rule");
                if (rule.count("resources") == 0)
                    throw std::runtime_error("Missing resources in rule");

                AuthorizationNode node;
                node.username = toml::find<std::string>(rule, "username");
                node.password = toml::find<std::string>(rule, "password");
                node.resources = toml::find<std::vector<std::string>>(rule, "resources");

                this->_nodes.push_back(node);
            }
        } catch (const std::runtime_error &e) {
            std::stringstream ss;
            ss << "Failed to load the configuration, the module will not operate ("
                << e.what() << ").";
            this->_logger.error(ss.str());
        }
    }

    void AuthorizationModule::onRegisterCallbacks(oZ::Pipeline &pipeline)
    {
        pipeline.registerCallback(oZ::State::Interpret, oZ::Priority::ASAP, this,
            &AuthorizationModule::onPipelineInterpret);
    }

    bool AuthorizationModule::onPipelineInterpret(oZ::Context &context)
    {
        if (context.hasError())
            return true;

        for (auto &node : this->_nodes) {
            auto found = std::find(node.resources.begin(), node.resources.end(), context.getRequest().getURI());

            if (found == node.resources.end())
                continue;

            this->_logger.debug("Requested resource is protected.");

            if (!context.getRequest().getHeader().exists("Authorization")) {
                this->_logger.warning(
                    "Client tried to access a protected resource without any credentials."
                );

                context.getResponse().setCode(oZ::HTTP::Code::Unauthorized);
                context.getResponse().getReason() = "Unauthorized";
                context.getResponse().getHeader().set("WWW-Authenticate", "Basic realm=\"Zia\"");
                return false;
            }

            auto &credentials = context.getRequest().getHeader().get("Authorization");
            size_t firstSpace = credentials.find(" ");

            if (firstSpace == std::string::npos) {
                context.getResponse().setCode(oZ::HTTP::Code::Unauthorized);
                context.getResponse().getReason() = "Unauthorized";
                return false;
            }

            std::string credentialsKind = credentials.substr(0, firstSpace);
            std::string credentialsValue = credentials.substr(firstSpace + 1);

            if (!this->validateCredentials(node, credentialsKind, credentialsValue)) {
                this->_logger.warning(
                    "Client tried to access a protected resource with bad credentials."
                );

                context.getResponse().setCode(oZ::HTTP::Code::Unauthorized);
                context.getResponse().getReason() = "Unauthorized";
                return false;
            }

            this->_logger.debug("Provided credentials are valid.");
        }
        return true;
    }

    bool AuthorizationModule::validateCredentials(AuthorizationNode &node,
        const std::string &kind, const std::string &value) const
    {
        if (kind != "Basic")
            return false;

        std::string decoded = Base64::decode(value);
        size_t sep = decoded.find(":");

        if (sep == std::string::npos)
            return false;

        std::string username = decoded.substr(0, sep);
        std::string password = decoded.substr(sep + 1);

        return username == node.username && password == node.password;
    }

}

OPEN_ZIA_MAKE_ENTRY_POINT(Zia::Authorization::AuthorizationModule)
