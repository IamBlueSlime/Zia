/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** error ErrorModule.cpp
*/

#include <sstream>
#include <string>
#include "zia/common/Network.hpp"
#include "openZia/Pipeline.hpp"
#include "zia/common/Config.hpp"
#include "zia/common/PathUtils.hpp"
#include "zia/common/BufferUtils.hpp"
#include "ErrorModule.hpp"

namespace Zia::Error {

    ErrorModule::ErrorModule() : _logger("ErrorModule") {}

    void ErrorModule::onRegisterCallbacks(oZ::Pipeline &pipeline) {
        pipeline.registerCallback(
            oZ::State::Error, oZ::Priority::Last, this, &ErrorModule::onPipelineError
        );
    }

    bool ErrorModule::onPipelineError(oZ::Context &context) {
        if (!context.hasError())
            return true;

        if (context.getResponse().getCode() == oZ::HTTP::Code::Undefined)
            return false;

        this->_logger.debug("Context marked as errored, injecting body.");
        
        std::string finalRes = _theme;
        size_t pos;

        while ((pos = finalRes.find("ZIA_ERROR_CODE")) != std::string::npos)
            finalRes.replace(pos, std::string("ZIA_ERROR_CODE").size(), std::to_string((int) context.getResponse().getCode()));
        while ((pos = finalRes.find("ZIA_ERROR_MESSAGE")) != std::string::npos)
            finalRes.replace(pos, std::string("ZIA_ERROR_MESSAGE").size(), context.getResponse().getReason());

        auto &body = context.getResponse().getBody();
        body.insert(body.end(), finalRes.begin(), finalRes.end());
        return true;
    }

    void ErrorModule::onLoadConfigurationFile(const std::string &directory)
    {
        try {
            Config config(PathUtils::join(directory, "error.toml").string());

            if (!config.root().contains("theme")) {
                throw std::runtime_error(
                    "Theme not found in the configuration"
                );
            }

            std::ifstream ifs(toml::find<std::string>(config.root(), "theme"));

            if (!ifs.good())
                throw std::runtime_error("Theme path not valid");

            _theme = std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        } catch (const std::runtime_error &e) {
            std::stringstream ss;
            ss << "Failed to load the configuration, the module will not operate ("
                << e.what() << ").";
            this->_logger.error(ss.str());
        }
    }

}

OPEN_ZIA_MAKE_ENTRY_POINT(Zia::Error::ErrorModule)
