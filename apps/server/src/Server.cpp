/*
** EPITECH PROJECT, 2019
** CPP_zia_2019
** File description:
** Server.cpp
*/

#include <sstream>
#include <signal.h>
#include "zia/server/Server.hpp"
#include "openZia/Packet.hpp"
#include "openZia/Context.hpp"

namespace Zia::Server {

    Server *Server::INSTANCE = nullptr;

    Server::Server() : _logger("Server"), _config("config.toml"), _threadPool(8),
        _pipeline("modules", "config"),
        _httpSocket(toml::find<unsigned short>(_config.root(), "server", "http_port"), 10),
        _httpsSocket(toml::find<unsigned short>(_config.root(), "server", "https_port"), 10),
        _modulesWatcher("modules", [this]() { this->reloadModules(false); }),
        _configWatcher("config", [this]() { this->reloadModules(false); }),
        _acceptClients(false)
    {
        INSTANCE = this;
        this->reloadModules();
    }

    void Server::start()
    {
        this->_httpSocket.open([&](std::shared_ptr<Client> client) {
            if (!this->_acceptClients.load())
                return false;

            bool accepted = this->_threadPool.pushTask([client, this]() {
                auto endpoint = client->toEndpoint();
                auto context = oZ::Context(oZ::Packet(std::move(client->getInBuffer()), endpoint));
                context.getPacket().setFileDescriptor(client->getFd());
                // context.setMetadata("fsroot", toml::find<std::string>(_config.root(), "filesystem", "root"));

                this->_pipeline.onConnection(context);
                try {
                    this->_pipeline.runPipeline(context);

                    if (context.getState() == oZ::State::Error) {
                        context.setState(oZ::State::Completed);
                        this->_pipeline.runPipeline(context);
                    }
                } catch (...) {}
                this->_pipeline.onDisconnection(context);
                this->_httpSocket.removeClient(client);
            });

            if (!accepted)
                this->_httpSocket.removeClient(client);
            return accepted;
        });

        this->_httpsSocket.open([&](std::shared_ptr<Client> client) {
            if (!this->_acceptClients.load())
                return false;

            bool accepted = this->_threadPool.pushTask([client, this]() {
                auto endpoint = client->toEndpoint();
                auto context = oZ::Context(oZ::Packet(std::move(client->getInBuffer()), endpoint));
                context.getPacket().setFileDescriptor(client->getFd());
                context.getPacket().setEncryption(true);
                // context.setMetadata("fsroot", toml::find<std::string>(_config.root(), "filesystem", "root"));

                this->_pipeline.onConnection(context);
                try {
                    this->_pipeline.runPipeline(context);

                    if (context.getState() == oZ::State::Error) {
                        context.setState(oZ::State::Completed);
                        this->_pipeline.runPipeline(context);
                    }
                } catch (...) {}
                this->_pipeline.onDisconnection(context);
                this->_httpsSocket.removeClient(client);
            });

            if (!accepted)
                this->_httpsSocket.removeClient(client);
            return accepted;
        });

        this->_logger.info("Ready.");

        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

    void Server::stop()
    {
        this->_acceptClients = false;

        this->waitClients();

        this->_logger.info("Closing server sockets...");
        this->_httpSocket.close();
        this->_httpsSocket.close();

        this->_logger.info("Stopping file watchers...");
        this->_modulesWatcher.stop();
        this->_configWatcher.stop();
    }

    void Server::reloadModules(bool initial)
    {
        if (!initial)
            this->_logger.info("Preventing client from connecting.");
        this->_acceptClients = false;
        this->waitClients();

        this->_logger.info("Reloading modules...");
        this->_pipeline.loadModules();

        auto count = this->_pipeline.getModules().size();
        std::stringstream ss;
        ss << "Loaded " << count << " module";
        ss << (count > 1 ? "s" : "") << ":";
        this->_logger.info(ss.str());

        for (const auto &module : this->_pipeline.getModules()) {
            ss = std::stringstream();
            ss << "- " << module->getName();
            this->_logger.info(ss.str());
        }

        this->_acceptClients = true;

        if (!initial)
            this->_logger.info("Unlocked connections.");
    }

    void Server::setupCtrlC()
    {
        #if defined(SYSTEM_LINUX) || defined(SYSTEM_DARWIN)
            struct sigaction sigIntHandler;

            sigIntHandler.sa_handler = &Server::onCtrlC;
            sigemptyset(&sigIntHandler.sa_mask);
            sigIntHandler.sa_flags = 0;

            sigaction(SIGINT, &sigIntHandler, NULL);
        #elif defined(SYSTEM_WINDOWS)
            signal(SIGINT, &Server::onCtrlC);
        #endif
    }

    void Server::waitClients()
    {
        while (this->_httpSocket.clientsNb() > 0 || this->_httpsSocket.clientsNb() > 0) {
            this->_logger.info("Waiting for the clients to disconnect...");
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

}