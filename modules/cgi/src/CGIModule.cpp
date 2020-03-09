/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** HttpModule.cpp
*/

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include "openZia/Pipeline.hpp"
#include "zia/common/BufferUtils.hpp"
#include "zia/common/Config.hpp"
#include "zia/common/PathUtils.hpp"
#include "CGIModule.hpp"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_DARWIN)
    #include <unistd.h>
    typedef int pid_t;
#elif defined(SYSTEM_WINDOWS)
    #include <io.h>
    #include <process.h>
    #include <sys/stat.h>

    extern "C" {
        typedef DWORD pid_t;
        pid_t my_fork(void);
        int my_pipe(int fds[2]);

        #define open _open
        #define close _close
        #define pipe my_pipe
        #define dup2 _dup2
        #define write _write
        #define execve _execve
        #define fork my_fork
    }
#endif


namespace Zia::CGI {

    CGIModule::CGIModule() : _logger("CGIModule") {}

    const std::regex CGIModule::REQUEST_LINE_REGEX =
            std::regex("([\\w-]+) *: *(.*)");
    
    void CGIModule::onLoadConfigurationFile(const std::string &directory)
    {
        try {
            Config config(PathUtils::join(directory, "cgi.toml").string());

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

    void CGIModule::onRegisterCallbacks(oZ::Pipeline &pipeline) {
        pipeline.registerCallback(
                oZ::State::Interpret, oZ::Priority::Independent, this, &CGIModule::onInterpretCgi
        );
    }

    std::vector<std::string> split(const std::string &strToSplit, char delimeter) {
        std::stringstream ss;
        ss << strToSplit;
        std::string item;
        std::vector<std::string> splittedStrings;
        while (std::getline(ss, item, delimeter)) {
            splittedStrings.push_back(item);
        }
        return splittedStrings;
    }


    bool CGIModule::onInterpretCgi(oZ::Context &context) {
        if (context.hasError())
            return true;

        try {
            auto uri = context.getRequest().getURI();

            std::string askedPath = context.getRequest().getURI();
            std::string formattedPath = askedPath;
            PathUtils::updateSeparators(formattedPath);

            std::string rootToUse = context.hasMetadata("fsroot") ? context.getMetadata<std::string &>("fsroot") : this->_root;
            std::string totalPath(rootToUse + (askedPath != "/" ? formattedPath : ""));

            if (!isExecutable(totalPath)) {
                return true;
            }

            int inFds[2] = {-1};
            pipe(inFds);

            int outFds[2] = {-1};
            pipe(outFds);

            pid_t pid = fork();
            if (pid != 0) {
                // Parent

                // close loop fd
                close(outFds[1]);
                close(inFds[0]);

                auto &body = context.getRequest().getBody();
                if (write(inFds[1], body.c_str(), body.length()) == -1) {
                    _logger.error("Write error");
                    return false;
                }
                close(inFds[1]);
                int readSize;
                std::string tempBody;
                do {
                    char buff[4096] = {};
                    readSize = read(outFds[0], &buff, 4096 - 1);
                    if (readSize < 0) {
                        _logger.error(strerror(errno));
                        break;
                    }
                    tempBody.append(buff);
                } while (readSize != 0);


                // split header and body
                int headerEnd = tempBody.find("\r\n\r\n"); // Find end of header
                std::string headerString = tempBody.substr(0, headerEnd);
                std::string bodyString = tempBody.substr(headerEnd + 4);

                // Parse header
                std::smatch matches;
                oZ::HTTP::Code code = oZ::HTTP::Code::OK;
                auto headerListTrash = split(headerString, '\r');
                for (auto &headerTrash: headerListTrash) {
                    if (std::regex_match(headerTrash, matches, REQUEST_LINE_REGEX)) {
                        if (matches[1] == "Status") {
                            code = static_cast<oZ::HTTP::Code>(std::stoi(matches[2]));
                            continue;
                        }
                        context.getResponse().getHeader().set(matches[1], matches[2]);
                    }
                }
                context.getResponse().setCode(code);

                // Feed body
                context.getResponse().getBody() = bodyString;


                close(outFds[0]);

            } else {
                // child
                dup2(inFds[0], 0);
                close(inFds[1]);
                dup2(outFds[1], 1);
                close(outFds[0]);

                std::vector<std::string> env;
                env.push_back("SERVER_SIGNATURE=Zia/1.0.0");
                env.push_back("SERVER_SOFTWARE=Zia/1.0.0");
                env.push_back("SERVER_NAME=localhost");
                env.push_back("SERVER_ADDR=127.0.0.1");
                // env.push_back("SERVER_PORT=80");
                env.push_back("REMOTE_ADDR=" + context.getPacket().getEndpoint().getAddress());
                env.push_back("DOCUMENT_ROOT=" + rootToUse);
                // env.push_back("REQUEST_SCHEME=http");
                // env.push_back("CONTEXT_PREFIX=");
                // env.push_back("CONTEXT_DOCUMENT_ROOT=");
                env.push_back("SERVER_ADMIN=webmaster@localhost");
                env.push_back("SCRIPT_FILENAME=" + totalPath);
                env.push_back("REMOTE_PORT=" + std::to_string(context.getPacket().getEndpoint().getPort()));
                env.push_back("GATEWAY_INTERFACE=CGI/1.1");
                env.push_back("SERVER_PROTOCOL=HTTP/1.1");
                env.push_back("REQUEST_METHOD=" + std::string(oZ::HTTP::MethodName(context.getRequest().getMethod())));
                env.push_back("QUERY_STRING=" + make_query_param(context.getRequest().getQueryParameters()));
                env.push_back("REQUEST_URI=" + context.getRequest().getURI());
                env.push_back("SCRIPT_NAME=" + context.getRequest().getURI());
                env.push_back("REDIRECT_STATUS=true");

                if (context.getRequest().getBody().length() != 0) {
                    env.push_back("CONTENT_LENGTH=" + std::to_string(context.getRequest().getBody().length()));
                    env.push_back("CONTENT_TYPE=" + context.getRequest().getHeader().get("Content-Type"));
                }
                
                for (const auto &hed : context.getRequest().getHeader().getStringMultimap())
                    env.push_back("HTTP_" + hed.first + "=" + hed.second);

                std::vector<std::string> envCpy(env.begin(), env.end());
                for (const std::string &envEntry : envCpy)
                    env.push_back("REDIRECT_" + envEntry);

                std::vector<char const *> envRaw;
                for (const std::string &envEntry : env)
                    envRaw.push_back(envEntry.c_str());
                envRaw.push_back(nullptr);

                char * const param[] = {const_cast<char * const>(totalPath.c_str()), nullptr};

                int ret = execve(totalPath.c_str(), param, const_cast<char * const *>(envRaw.data()));
                if (ret == -1) {
                    perror("execve");
                    write(1, FAILED_EXEC_RESPONCE_CGI.c_str(), FAILED_EXEC_RESPONCE_CGI.length());
                    exit(0);
                }
                // Never reached
            }
        } catch (const std::exception &e) {
            _logger.error(e.what());

            context.getResponse().setCode(oZ::HTTP::Code::InternalServerError);
            context.getResponse().getBody() = "Internal Server Error";
            context.setErrorState();
        }

        return false;
    }

    bool CGIModule::isExecutable(const std::string &path) {
        struct stat sb{};
        #if defined(SYSTEM_LINUX) || defined(SYSTEM_DARWIN)
        return stat(path.c_str(), &sb) == 0 && S_ISREG(sb.st_mode) && (S_IXUSR & sb.st_mode);
        #elif defined(SYSTEM_WINDOWS)
        return stat(path.c_str(), &sb) == 0;
        #endif
    }

    std::string CGIModule::make_query_param(const oZ::HTTP::QueryParameters &paramList) {
        std::string ret = "";
        for (auto &param: paramList) {
            ret.append(param.first + "=" + param.second + "&");
        }
        if (ret.length() != 0)
            ret.pop_back();
        return ret;
    }

}

OPEN_ZIA_MAKE_ENTRY_POINT(Zia::CGI::CGIModule)
