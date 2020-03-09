/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** HeadersModule.cpp
*/

#include <sstream>
#include "HeadersModule.hpp"
#include "zia/common/BufferUtils.hpp"
 
namespace Zia::Headers {

    const std::regex HeadersModule::REQUEST_LINE_REGEX =
        std::regex("([\\w-]+) *: *(.*)");

    void HeadersModule::onRegisterCallbacks(oZ::Pipeline &pipeline)
    {
        pipeline.registerCallback(
			oZ::State::Parse, oZ::Priority::High, this, &HeadersModule::onParseASAP
		);
    }

    bool HeadersModule::onParseASAP(oZ::Context &context)
    {
        if (context.hasError())
            return true;

        size_t queryPos = context.getRequest().getURI().find("?");

        if (queryPos != std::string::npos) {
            std::string query = context.getRequest().getURI().substr(queryPos + 1);
            context.getRequest().getURI() = context.getRequest().getURI().substr(0, queryPos);

            size_t found;
            while ((found = query.find("&")) != std::string::npos) {
                std::string param = query.substr(0, found);

                size_t delim = param.find("=");
                if (delim == std::string::npos) {
                    context.getRequest().getQueryParameters().insert(std::make_pair(param, ""));
                } else {
                    context.getRequest().getQueryParameters().insert(
                        std::make_pair(param.substr(0, delim), param.substr(delim + 1))
                    );
                }

                query.erase(0, found + 1);
            }
            
            size_t delim = query.find("=");
            if (delim == std::string::npos) {
                context.getRequest().getQueryParameters().insert(std::make_pair(query, ""));
            } else {
                context.getRequest().getQueryParameters().insert(
                    std::make_pair(query.substr(0, delim), query.substr(delim + 1))
                );
            }
        }
        
        this->_logger.debug("Parsing headers.");

        oZ::ByteArray data(context.getPacket().getByteArray());
        std::smatch matches;
        std::string line;

        while (!data.empty()) {
            line = BufferUtils::readCRLFLine(data);
            if (!std::regex_match(line, matches, REQUEST_LINE_REGEX)) {
                if (line == "") {
                    context.getRequest().getBody().append(std::string(data.begin(), data.end()));
                    break;
                } else {
                    this->_logger.warning("Received an invalid request (invalid header line).");

                    std::stringstream ss;
                    ss << "Header line received: '" << line << "'.";
                    this->_logger.debug(ss.str());

                    context.getResponse().setCode(oZ::HTTP::Code::BadRequest);
                    context.getResponse().getReason() = "Bad Request";
                    context.setErrorState();
                    return false;
                }
            } else {
                context.getRequest().getHeader().set(matches[1], matches[2]);
            }
        }
        return true;
    }
} 

OPEN_ZIA_MAKE_ENTRY_POINT(Zia::Headers::HeadersModule)