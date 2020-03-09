/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** HTTPModule.cpp
*/

#include <sstream>
#include <string>
#include "zia/common/Network.hpp"
#include "openZia/Pipeline.hpp"
#include "zia/common/BufferUtils.hpp"
#include "HTTPModule.hpp"

#include <iostream>

namespace Zia::HTTP {

    const std::regex HTTPModule::REQUEST_LINE_REGEX =
        std::regex("^([^\\s]+)\\s+([^\\s]+)(?:\\s+HTTP\\/([\\d.]+))?$");

    HTTPModule::HTTPModule() : _logger("HTTPModule") {}

    void HTTPModule::onRegisterCallbacks(oZ::Pipeline &pipeline)
    {
        pipeline.registerCallback(
			oZ::State::BeforeParse, oZ::Priority::ASAP, this, &HTTPModule::onPipelineBeforeParseASAP
		);
        pipeline.registerCallback(
			oZ::State::BeforeParse, oZ::Priority::High, this, &HTTPModule::onPipelineBeforeParse
		);
        pipeline.registerCallback(
			oZ::State::Completed, oZ::Priority::Last, this, &HTTPModule::onPipelineCompleted
		);
    }

    bool HTTPModule::onPipelineBeforeParseASAP(oZ::Context &context)
    {
        if (context.getPacket().hasEncryption())
            return true;
        
        std::vector<char> buffer;
        buffer.reserve(1024);

        auto &body = context.getPacket().getByteArray();
        int bytes;

        do {
            bytes = recv(context.getPacket().getFileDescriptor(), buffer.data(), 1024, 0);

            if (bytes <= 0) {
                this->_logger.warning("Failed to read request.");
                context.setErrorState();
                return false;
            }

            body.reserve(body.size() + bytes); 
            body.insert(body.end(), buffer.begin(), buffer.begin() + bytes); 
        } while (bytes == 1024);

        return true;
    }

    bool HTTPModule::onPipelineBeforeParse(oZ::Context &context)
    {
        std::string line = BufferUtils::readCRLFLine(context.getPacket().getByteArray());
        std::smatch matches;

        if (!std::regex_match(line, matches, REQUEST_LINE_REGEX)) {
            this->_logger.warning("Received an invalid request (invalid request line).");
            
            std::stringstream ss;
            ss << "Request line received: '" << line << "'.";
            this->_logger.debug(ss.str());

            context.getResponse().setCode(oZ::HTTP::Code::BadRequest);
            context.getResponse().getReason() = "Bad Request";
            context.setErrorState();
            return false;
        }

        auto method = HTTPModule::getHttpMethodFromStr(matches[1].str());
        if (method == oZ::HTTP::Method::NullMethod) {
            context.getResponse().setCode(oZ::HTTP::Code::BadRequest);
            context.getResponse().getReason() = "Bad Request";
            context.setErrorState();
            return false;
        }

        auto version = oZ::HTTP::Version(1, 1);
        if (matches.size() == 3)
            version = HTTPModule::getHttpVersionFromStr(matches[3].str());

        if (
            !(version.majorVersion == 0 && version.minorVersion == 9)
            && !(version.majorVersion == 1 && version.minorVersion == 0)
            && !(version.majorVersion == 1 && version.minorVersion == 1)
        ) {
            context.getResponse().setCode(oZ::HTTP::Code::HTTPVersionNotSupported);
            context.getResponse().getReason() = "HTTP Version not supported";
            context.setErrorState();
            return false;
        }

        context.getRequest().setMethod(method);
        context.getRequest().setVersion(version);
        context.getRequest().getURI() = matches[2].str();

        std::stringstream ss;
        ss << "Client requested " << matches[1].str() << " " << matches[2].str();
        ss << " (" << matches[3].str() << ").";
        this->_logger.debug(ss.str());
        return true;
    }

    bool HTTPModule::onPipelineCompleted(oZ::Context &context)
    {
        if (context.getPacket().hasEncryption())
            return true;
        
        this->_logger.debug("Writing response.");

        std::stringstream ss;
        ss << "HTTP/1.1 " << static_cast<int>(context.getResponse().getCode())
            << " " << context.getResponse().getReason() << "\r\n";
        
        for (auto &header : context.getResponse().getHeader().getStringMultimap())
            ss << header.first << ": " << header.second << "\r\n";

        ss << "\r\n";

        std::string headStr = ss.str();
        send(context.getPacket().getFileDescriptor(), headStr.data(), headStr.size(), 0);
        send(context.getPacket().getFileDescriptor(), context.getResponse().getBody().data(),
            context.getResponse().getBody().size(), 0);
        return true;
    }

    oZ::HTTP::Method HTTPModule::getHttpMethodFromStr(const std::string &str)
    {
        if (str == "OPTION") return oZ::HTTP::Method::Option;
        if (str == "GET") return oZ::HTTP::Method::Get;
        if (str == "HEAD") return oZ::HTTP::Method::Head;
        if (str == "POST") return oZ::HTTP::Method::Post;
        if (str == "PUT") return oZ::HTTP::Method::Put;
        if (str == "DELETE") return oZ::HTTP::Method::Delete;
        if (str == "TRACE") return oZ::HTTP::Method::Trace;
        if (str == "CONNECT") return oZ::HTTP::Method::Connect;
        return oZ::HTTP::Method::NullMethod;
    }

    oZ::HTTP::Version HTTPModule::getHttpVersionFromStr(const std::string &str)
    {
        auto index = str.find(".");

        #if defined(SYSTEM_WINDOWS)
            #pragma warning(disable:C4244)
        #endif
        if (index == std::string::npos)
            return oZ::HTTP::Version(std::stoul(str), 0);
        return oZ::HTTP::Version(std::stoul(str.substr(0, index)), std::stoul(str.substr(index + 1)));
        #if defined(SYSTEM_WINDOWS)
            #pragma warning(default:C4244)
        #endif
    }

}

OPEN_ZIA_MAKE_ENTRY_POINT(Zia::HTTP::HTTPModule)
