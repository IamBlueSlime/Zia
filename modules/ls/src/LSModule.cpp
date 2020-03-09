/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** HttpModule.cpp
*/

#include <iostream>

#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "openZia/Pipeline.hpp"
#include "zia/common/PathUtils.hpp"
#include "zia/common/Config.hpp"
#include "LSModule.hpp"

using Cmp = std::function<bool(fs::directory_entry &)>;

static const std::pair<Cmp, std::pair<std::string, std::string>> ICONS[] = {
    { [](fs::directory_entry &entry){return fs::is_regular_file(entry);}, { "file", "" } },
    { [](fs::directory_entry &entry){return fs::is_directory(entry);}, { "folder", "/" } }
};
static const std::string DEFAULT_ICON = "question";

namespace Zia::LS {

    void LSModule::onRegisterCallbacks(oZ::Pipeline &pipeline)
    {
        pipeline.registerCallback(
            oZ::State::Interpret, oZ::Priority::Independent, this, &LSModule::onInterpret
        );
    }

    std::string LSModule::toLower(const std::string &str) {
        std::string tmp(str);
        std::for_each(tmp.begin(), tmp.end(), [](char & c) {
            c = ::toupper(c);
        });
        return tmp;
    }

    bool LSModule::sortOrder(const fs::directory_entry &first, const fs::directory_entry &second) {
        std::string f = toLower(std::string(first.path().string()));
        std::string s = toLower(std::string(second.path().string()));

        for (std::size_t i = 0; i < f.length() && i < s.length(); ++i) {
            if (f[i] == s[i])
                continue;
            return f[i] < s[i]; 
        }
        return f[0] < s[0];
    }

    std::string LSModule::fill(const std::string &totalPath, std::string &askedPath)
    {
        std::vector<fs::directory_entry> lines;
        std::string res("    <h1>Path : " + askedPath + "</h1>\r\n");

        res += "    <ul>\r\n";
        if (totalPath.size() > _root.size()) {
            int lastSlash = askedPath.size() - 1;
            while (lastSlash > 0 && askedPath[lastSlash] != '/') --lastSlash;
            while (lastSlash > 0 && askedPath[lastSlash] == '/') --lastSlash;
            res += "        <div>\r\n            <i class=\"fas fa-folder\"></i>\r\n            <a href=\"" + askedPath.substr(0, lastSlash + 1) + "\">../</a>\r\n        </div>\r\n";
        }
        for (auto &tmp : fs::directory_iterator(totalPath))
            lines.push_back(tmp);
        std::sort(lines.begin(), lines.end(), sortOrder);

        for (auto &tmp : lines) {
            std::string type = DEFAULT_ICON;
            std::string suffix;
            std::string path = tmp.path().string();
            std::string name = path.substr(path.find_last_of(fs::path::preferred_separator) + 1);
            for (auto &tmp2 : ICONS) {
                if (tmp2.first(tmp)) {
                    type = tmp2.second.first;
                    suffix = tmp2.second.second;
                    break;
                }
            }
            if (askedPath == "/")
                askedPath = "";
            res += "        <div>\r\n            <i class=\"fas fa-" + type + "\"></i>\r\n            <a href=\"" + askedPath + '/' + name + "\">" + name + suffix + "</a>\r\n        </div>\r\n";
        }
        res += "    </ul>\r\n";
        return res;
    }

    bool LSModule::onInterpret(oZ::Context &ctx)
    {
        if (_root == "" && !ctx.hasMetadata("fsroot"))
            return true;

        const std::string replace = "ZIA_REPLACE";

        std::string askedPath = ctx.getRequest().getURI();
        int lastSlash = askedPath.size() - 1;
        while (lastSlash > 0 && askedPath[lastSlash] == '/') --lastSlash;
        askedPath = askedPath.substr(0, lastSlash + 1);

        std::string formattedPath = askedPath;
        PathUtils::updateSeparators(formattedPath);

        std::string rootToUse = ctx.hasMetadata("fsroot") ? ctx.getMetadata<std::string &>("fsroot") : _root;

        std::string innerIndex(rootToUse + (askedPath != "/" ? formattedPath : "") + "/index.html");
        PathUtils::updateSeparators(innerIndex);

        std::string totalPath(rootToUse + (askedPath != "/" ? formattedPath : ""));

        if (fs::is_directory(totalPath) && fs::exists(innerIndex) && fs::is_regular_file(fs::path(innerIndex))) {
            ctx.getRequest().getURI() += "index.html";
            return true;
        }

        if (!fs::exists(totalPath) || !fs::is_directory(totalPath))
            return true;

        this->_logger.debug("Requested resource is a directory.");

        fs::perms permissions = fs::status(totalPath).permissions();
        if ((permissions & fs::perms::owner_read) == fs::perms::none &&
        (permissions & fs::perms::group_read) == fs::perms::none) {
            ctx.getResponse().setCode(oZ::HTTP::Code::Forbidden);
            ctx.getResponse().getReason() = "Forbidden";
            ctx.setErrorState();
            return false;
        }

        std::string generated = fill(totalPath, askedPath);
        std::string realstr = _theme;
        realstr.replace(realstr.find(replace), replace.size(), generated);

        ctx.getResponse().setCode(oZ::HTTP::Code::OK);
        ctx.getResponse().getReason() = "OK";
        ctx.getResponse().getHeader().set("Content-Type", "text/html");
        ctx.getResponse().getBody().insert(ctx.getResponse().getBody().end(), realstr.begin(), realstr.end());
        return false;
    }

    void LSModule::onLoadConfigurationFile(const std::string &directory)
    {
        try {
            Config config(PathUtils::join(directory, "ls.toml").string());

            if (!config.root().contains("root")) {
                throw std::runtime_error(
                    "Root not found in the configuration"
                );
            } else if (!config.root().contains("theme")) {
                throw std::runtime_error(
                    "Theme not found in the configuration"
                );
            }

            _root = toml::find<std::string>(config.root(), "root");
            std::replace(_root.begin(), _root.end(), '/', (char) fs::path::preferred_separator);
            if (_root[_root.size() - 1] != fs::path::preferred_separator)
                _root += fs::path::preferred_separator;

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

OPEN_ZIA_MAKE_ENTRY_POINT(Zia::LS::LSModule)

// PEUX PAS OUVRIR : 403
// ctx.getResponse().setCode(403);