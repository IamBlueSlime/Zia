/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** Client.cpp
*/

#include <iostream>
#include "zia/server/Client.hpp"

namespace Zia::Server {

    std::ostream &operator <<(std::ostream &stream, const Client &client)
    {
        stream << "Client[ip=\"" << client.getIp() << "\"; port=" << client.getPort() << "]";
        return stream;
    }

}