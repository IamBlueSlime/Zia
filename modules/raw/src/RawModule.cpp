/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** RawModule.cpp
*/

#include <sstream>
#include <fstream>
#include <string>
#include "openZia/Pipeline.hpp"
#include "zia/common/Config.hpp"
#include "zia/common/PathUtils.hpp"
#include "RawModule.hpp"
#include "MIMEType.hpp"

namespace Zia::Raw {

    RawModule::RawModule() : _logger("RawModule") {}

    void RawModule::onLoadConfigurationFile(const std::string &directory)
    {
        try {
            Config config(PathUtils::join(directory, "raw.toml").string());

            if (!config.root().contains("root")) {
                throw std::runtime_error(
                    "Root not found in the configuration"
                );
            }
            _root = toml::find<std::string>(config.root(), "root");
            std::replace(_root.begin(), _root.end(), '/', (char) fs::path::preferred_separator);
            if (_root[_root.size() - 1] != fs::path::preferred_separator)
                _root += fs::path::preferred_separator;
        } catch (const std::runtime_error &e) {
            std::stringstream ss;
            ss << "Failed to load the configuration, the module will not operate ("
                << e.what() << ").";
            this->_logger.error(ss.str());
        }
    }

    void RawModule::onRegisterCallbacks(oZ::Pipeline &pipeline)
    {
        pipeline.registerCallback(
			oZ::State::Interpret, oZ::Priority::Last, this, &RawModule::onPipelineInterpretLast
		);
    }

    bool RawModule::onPipelineInterpretLast(oZ::Context &context)
    {
        if (context.hasError())
            return true;

        this->_logger.debug("Checking raw file.");

        std::string askedPath = context.getRequest().getURI();
        std::string formattedPath = askedPath;
        PathUtils::updateSeparators(formattedPath);

        std::string rootToUse = context.hasMetadata("fsroot") ? context.getMetadata<std::string &>("fsroot") : this->_root;
        std::string totalPath(rootToUse + (askedPath != "/" ? formattedPath : ""));

        if (!fs::exists(totalPath)) {
            context.getResponse().setCode(oZ::HTTP::Code::NotFound);
            context.getResponse().getReason() = "Not Found";
            context.setErrorState();

            this->_logger.debug("Requested resource don't exists.");
            return false;
        }

        fs::perms permissions = fs::status(totalPath).permissions();
        if (
            (permissions & fs::perms::owner_read) == fs::perms::none
            && (permissions & fs::perms::group_read) == fs::perms::none
        ) {
            context.getResponse().setCode(oZ::HTTP::Code::Forbidden);
            context.getResponse().getReason() = "Forbidden";
            context.setErrorState();

            this->_logger.debug("Can't access requested resource.");
            return false;
        }

        std::ifstream stream(totalPath, std::ios::binary);
        if (!stream.good()) {
            context.getResponse().setCode(oZ::HTTP::Code::InternalServerError);
            context.getResponse().getReason() = "Internal Server Error";
            context.setErrorState();

            this->_logger.error("Failed to read requested resource.");
            return false;
        }

        std::vector<char> bytes(std::istreambuf_iterator<char>{stream}, {});
        auto &body = context.getResponse().getBody();
        body.reserve(body.size() + bytes.size());
        body.insert(body.end(), bytes.begin(), bytes.end());

        context.getResponse().getHeader().set("Content-Length", std::to_string(bytes.size()));

        if (MIMEType::isKnowingExtension(totalPath))
            context.getResponse().getHeader().set("Content-Type", MIMEType::getFromFile(totalPath));

        context.getResponse().setCode(oZ::HTTP::Code::OK);
        context.getResponse().getReason() = "OK";
        return true;
    }

}

OPEN_ZIA_MAKE_ENTRY_POINT(Zia::Raw::RawModule)
