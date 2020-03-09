/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** zia server Client.hpp
*/

#pragma once

#include <iostream>
#include <deque>
#include <vector>
#include "zia/common/Network.hpp"
#include "openZia/Endpoint.hpp"
#include "openZia/OperatingSystem.hpp"

namespace Zia::Server {

    class Client {
    public:
        Client(socket_t fd, struct sockaddr addr) : _fd(fd), _addr(addr) {
            this->updateIp();
            this->updatePort();
        }

        ~Client() {
            this->close();
        }

        void close() {
            if (this->_opened) {
                #if defined(SYSTEM_LINUX) || defined(SYSTEM_DARWIN)
                    ::close(this->_fd);
                #elif defined(SYSTEM_WINDOWS)
                    closesocket(this->_fd);
                #endif
            }
            this->_opened = false;
        }

        socket_t getFd() const { return this->_fd; }

        const std::string &getIp() const { return this->_ip; }

        unsigned short getPort() const { return this->_port; }

        std::vector<std::int8_t> &getInBuffer() { return this->_inBuffer; }

        oZ::Endpoint toEndpoint() const {
            return oZ::Endpoint(this->_ip, this->_port);
        }

    protected:
    private:
        socket_t _fd;
        struct sockaddr _addr;
        std::string _ip;
        unsigned short _port;
        bool _opened = true;

        std::vector<std::int8_t> _inBuffer = std::vector<std::int8_t>();

        void updateIp() {
            std::string ip;

            switch (this->_addr.sa_family) {
                case AF_INET: {
                    struct sockaddr_in *addr_in = (struct sockaddr_in *) &this->_addr;
                    char buf[INET_ADDRSTRLEN + 1] = {0};
                    inet_ntop(AF_INET, &addr_in->sin_addr, buf, INET_ADDRSTRLEN);
                    ip = std::string(buf);
                    break;
                }
                case AF_INET6: {
                    struct sockaddr_in6 *addr_in6 = (struct sockaddr_in6 *) &this->_addr;
                    char buf[INET6_ADDRSTRLEN + 1] = {0};
                    inet_ntop(AF_INET6, &addr_in6->sin6_addr, buf, INET6_ADDRSTRLEN);
                    ip = std::string(buf);
                    break;
                }
            }

            this->_ip = ip;
        }

        void updatePort() {
            unsigned short port = 0;

            switch (this->_addr.sa_family) {
                case AF_INET: {
                    struct sockaddr_in *addr_in = (struct sockaddr_in *) &this->_addr;
                    port = ntohs(addr_in->sin_port);
                    break;
                }
                case AF_INET6: {
                    struct sockaddr_in6 *addr_in6 = (struct sockaddr_in6 *) &this->_addr;
                    port = ntohs(addr_in6->sin6_port);
                    break;
                }
            }

            this->_port = port;
        }
    };

    std::ostream &operator<<(std::ostream &stream, const Client &client);
}