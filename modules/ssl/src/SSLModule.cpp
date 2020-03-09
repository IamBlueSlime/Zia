/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** SSLModule.cpp
*/

#include <sstream>
#include "zia/common/PathUtils.hpp"
#include "SSLModule.hpp"

static const std::string SSLCTX_METADATA = "sslctx";

namespace Zia::SSL {

    SSLModule::SSLModule() : _logger("SSLModule")
    {
        SSL_load_error_strings();	
        OpenSSL_add_ssl_algorithms();
    }

    SSLModule::~SSLModule()
    {
        this->destroySSLContext();
        EVP_cleanup();
    }

    void SSLModule::onLoadConfigurationFile(const std::string &directory)
    {
        try {
            Config config(PathUtils::join(directory, "ssl.toml").string());

            if (!config.root().contains("cert") || !config.root().contains("private_key")) {
                throw std::runtime_error(
                    "Certificate or Private Key path not found in the configuration"
                );
            }

            initSSLContext(
                toml::find<std::string>(config.root(), "cert"),
                toml::find<std::string>(config.root(), "private_key")
            );
        } catch (const std::runtime_error &e) {
            std::stringstream ss;
            ss << "Failed to load the configuration, the module will not operate ("
                << e.what() << ").";
            this->_logger.error(ss.str());
        }
    }

    void SSLModule::onRegisterCallbacks(oZ::Pipeline &pipeline)
    {
        pipeline.registerCallback(oZ::State::BeforeParse, oZ::Priority::ASAP, this,
            &SSLModule::onPipelineBeforeParse);
        pipeline.registerCallback(oZ::State::Completed, oZ::Priority::Last, this,
            &SSLModule::onPipelineCompleted);
    }

    void SSLModule::onConnection(oZ::Context &context)
    {
        if (this->sslCtx == nullptr || !context.getPacket().hasEncryption())
            return;

        this->_logger.debug("Creating client's SSL context.");

        ::SSL *ssl = SSL_new(this->sslCtx);
        SSL_set_fd(ssl, context.getPacket().getFileDescriptor());

        int err;
        while ((err = SSL_accept(ssl)) <= 0) {
            this->_logger.warning("Failed to accept SSL connection.");
            ERR_print_errors_fp(stderr);
            SSL_shutdown(ssl);
            SSL_free(ssl);

            context.getResponse().setCode(oZ::HTTP::Code::BadRequest);
            context.getResponse().getReason() = "Bad Request";
            context.setErrorState();
            return;
        }

        this->_logger.debug("Accepted client's SSL connection.");
        context.setMetadata(SSLCTX_METADATA, ssl);
    }

    bool SSLModule::onPipelineBeforeParse(oZ::Context &context)
    {
        if (this->sslCtx == nullptr || !context.getPacket().hasEncryption()) 
            return true;
    
        if (!context.hasMetadata(SSLCTX_METADATA))
            return true;
        
        this->_logger.debug("Reading SSL request.");

        std::vector<char> buffer;
        buffer.reserve(1024);

        auto ssl = context.getMetadata<::SSL *>(SSLCTX_METADATA);
        auto &body = context.getPacket().getByteArray();
        int bytes;

        do {
            bytes = SSL_read(ssl, buffer.data(), 1024);

            if (bytes <= 0) {
                this->_logger.warning("Failed to read the SSL request.");
                ERR_print_errors_fp(stderr);

                context.getResponse().setCode(oZ::HTTP::Code::BadRequest);
                context.getResponse().getReason() = "Bad Request";
                context.setErrorState();
                return false;
            }

            body.reserve(body.size() + bytes); 
            body.insert(body.end(), buffer.begin(), buffer.begin() + bytes);
        } while (bytes == 1024);

        return true;
    }

    bool SSLModule::onPipelineCompleted(oZ::Context &context)
    {
        if (this->sslCtx == nullptr || !context.getPacket().hasEncryption())
            return true;
        
        if (!context.hasMetadata(SSLCTX_METADATA))
            return true;
    
        auto ssl = context.getMetadata<::SSL *>(SSLCTX_METADATA);
        this->_logger.debug("Writing SSL response.");

        std::stringstream ss;
        ss << "HTTP/1.1 " << static_cast<int>(context.getResponse().getCode())
            << " " << context.getResponse().getReason() << "\r\n";
        
        for (auto &header : context.getResponse().getHeader().getStringMultimap())
            ss << header.first << ": " << header.second << "\r\n";

        ss << "\r\n";

        std::string headStr = ss.str();
        SSL_write(ssl, headStr.data(), headStr.size());
        SSL_write(ssl, context.getResponse().getBody().data(),
            context.getResponse().getBody().size());

        return true;
    }

    void SSLModule::onDisconnection(oZ::Context &context)
    {
        if (this->sslCtx == nullptr || !context.hasMetadata(SSLCTX_METADATA))
            return;

        auto ssl = context.getMetadata<::SSL *>(SSLCTX_METADATA);
        this->_logger.debug("Terminating client's SSL connection.");

        SSL_shutdown(ssl);
        SSL_free(ssl);
        context.removeMetadata(SSLCTX_METADATA);
    }

    void SSLModule::initSSLContext(const std::string &certPath, const std::string &privKeyPath)
    {
        if (this->sslCtx != nullptr)
            this->destroySSLContext();
        
        this->sslCtx = SSL_CTX_new(SSLv23_server_method());
        if (!this->sslCtx) {
            ERR_print_errors_fp(stderr);
            throw std::runtime_error("Failed to create SSL context");
        }

        SSL_CTX_set_ecdh_auto(this->sslCtx, 1);

        if (SSL_CTX_use_certificate_file(this->sslCtx, certPath.c_str(), SSL_FILETYPE_PEM) <= 0) {
            ERR_print_errors_fp(stderr);
            throw std::runtime_error("Failed to load SSL certificate");
        }

        if (SSL_CTX_use_PrivateKey_file(this->sslCtx, privKeyPath.c_str(), SSL_FILETYPE_PEM) <= 0 ) {
            ERR_print_errors_fp(stderr);
            throw std::runtime_error("Failed to create SSL private key");
        }
    }

    void SSLModule::destroySSLContext()
    {
        if (this->sslCtx != nullptr) {
            SSL_CTX_free(this->sslCtx);
            this->sslCtx = nullptr;
        }
    }
}

OPEN_ZIA_MAKE_ENTRY_POINT(Zia::SSL::SSLModule)
