/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** zia server ServerSocket.hpp
*/

#pragma once

#include <functional>
#include <thread>
#include <mutex>
#include <vector>
#include <sstream>
#include "zia/common/Logger.hpp"
#include "zia/common/Network.hpp"
#include "zia/server/Client.hpp"

#include <iostream>
namespace Zia::Server {

    using ClientConsumer = std::function<bool(std::shared_ptr<Client>)>;

    class ServerSocket {
    public:
        ServerSocket(unsigned short port, unsigned int maxClients)
                : _port(port), _maxClients(maxClients),
                  _logger(std::string("ServerSocket(") + std::to_string(port) + std::string(")")),
                  _work(true), _ready(false) {
            this->_thread = std::thread([&] {
                this->internalRun();
            });
        }

        ~ServerSocket() {
            this->close();
            this->_thread.join();
        }

        void open(ClientConsumer &&callback) {
            this->_callback = std::forward<ClientConsumer>(callback);
            this->_sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

            if (this->_sd == -1)
                throw std::runtime_error("Failed to open server socket");

            int opt = 1;
            if (setsockopt(this->_sd, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) == -1)
                throw std::runtime_error("Failed to setup server socket");

            this->_serverAddr.sin_family = AF_INET;
            this->_serverAddr.sin_port = htons(this->_port);
            this->_serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

            if (bind(this->_sd, (struct sockaddr *) (&this->_serverAddr), sizeof(this->_serverAddr)) == -1)
                throw std::runtime_error("Failed to bind server socket");

            if (listen(this->_sd, this->_maxClients) == -1)
                throw std::runtime_error("Failed to listen to server socket");

            std::stringstream ss;
            ss << "Listening on port " << this->_port << ".";
            this->_logger.info(ss.str());

            this->_ready = true;
        }

        void close() {
            this->_work = false;
        }

        size_t clientsNb() {
            size_t nb;

            {
                std::unique_lock<std::mutex> lock(this->_clientsMutex);
                nb = this->_clients.size();
            }

            return nb;
        }

        void removeClient(std::shared_ptr<Client> client) {
            {
                std::unique_lock<std::mutex> lock(this->_clientsMutex);
                this->_clients.erase(std::remove_if(this->_clients.begin(), this->_clients.end(), [&](std::shared_ptr<Client> &c) {
                    return c.get() == client.get();
                }), this->_clients.end());
                this->_logger.debug("Disconnected client.");
            }
        }

    protected:
    private:
        const unsigned short _port;
        const unsigned int _maxClients;
        Logger _logger;
        ClientConsumer _callback;
        socket_t _sd;
        sockaddr_in _serverAddr = {};
        std::vector<std::shared_ptr<Client>> _clients {};
        std::mutex _clientsMutex;
        int _maxfdN = 0;
        std::atomic_bool _work;
        std::atomic_bool _ready;

        std::thread _thread;
        fd_set _fdset = {};

        void internalRun() {
            while (this->_work.load()) {
                if (!this->_ready.load()) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    continue;
                }

                struct timeval time{1, 0};
                int retPoll;

                {
                    std::unique_lock<std::mutex> lock(this->_clientsMutex);
                    this->updateFdSet();
                    retPoll = select(_maxfdN + 1, &this->_fdset, nullptr, nullptr, &time);
                }

                if (retPoll < 0) {
                    this->_logger.error("Failed to poll server socket.");
                    continue;
                } else if (retPoll == 0) {
                    std::unique_lock<std::mutex> lock(this->_clientsMutex);
                    this->updateFdSet();
                    continue;
                }

                if (FD_ISSET(_sd, &this->_fdset)) {
                    struct sockaddr clientAddr = {};
                    socklen_t socklen = sizeof(clientAddr);
                    socket_t clientFd = accept(this->_sd, &clientAddr, &socklen);

                    #if defined(SYSTEM_LINUX) || defined(SYSTEM_DARWIN)
                        if (clientFd == -1)
                            throw std::runtime_error("Failed to accept new client");
                    #elif defined(SYSTEM_WINDOWS)
                        if (clientFd == INVALID_SOCKET)
                            throw std::runtime_error("Failed to accept new client");
                    #endif

                    auto client = std::make_shared<Client>(clientFd, clientAddr);

                    {
                        std::unique_lock<std::mutex> lock(this->_clientsMutex);
                        this->_clients.push_back(client);
                    }

                    std::stringstream ss;
                    ss << "Accepted new client: " << *client << ".";
                    this->_logger.debug(ss.str());

                    if (!this->_callback(client))
                        this->removeClient(client);
                }
            }
        }

        void updateFdSet() {
            FD_ZERO(&this->_fdset);
            FD_SET(_sd, &this->_fdset);

            #if defined(SYSTEM_LINUX) || defined(SYSTEM_DARWIN)
                if (_sd > _maxfdN)
                    _maxfdN = _sd;
            #endif

            for (size_t i = 0; i < this->_clients.size(); i += 1) {
                socket_t nf = _clients[i]->getFd();
                #if defined(SYSTEM_LINUX) || defined(SYSTEM_DARWIN)
                    if (nf > _maxfdN)
                        _maxfdN = nf;
                #endif
                FD_SET(nf, &this->_fdset);
            }
        }
    };

}