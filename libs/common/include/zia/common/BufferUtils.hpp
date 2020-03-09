/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** zia common ReadUtils.hpp
*/

#pragma once

#include <string>
#include "openZia/ByteArray.hpp"
#include "zia/common/StringUtils.hpp"

namespace Zia {

    class BufferUtils {
    public:
        static std::string readCRLFLine(oZ::ByteArray &buffer, bool removeSep = true)
        {
            return readUntil(buffer, "\r\n", removeSep);
        }

        static std::string readLFLine(oZ::ByteArray &buffer, bool removeSep = true)
        {
            return readUntil(buffer, "\n", removeSep);
        }

        static std::string readUntil(oZ::ByteArray &buffer, const std::string &sep, bool removeSep = true)
        {
            std::string line;

            while (!buffer.empty() && !StringUtils::endsWith(line, sep)) {
                line += buffer.front();
                buffer.erase(buffer.begin());
            }
            if (removeSep)
                line = line.substr(0, line.size() - sep.size());
            return line;
        }

    protected:
    private:
    };
}