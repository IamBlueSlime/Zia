/*
** EPITECH PROJECT, 2019
** CPP_zia_2019
** File description:
** Main.cpp
*/

#include "openZia/OperatingSystem.hpp"

#if defined(SYSTEM_WINDOWS)
    #include <winsock2.h>
#endif

#include "zia/server/Server.hpp"

int main()
{
    #if defined(SYSTEM_WINDOWS)
        WSADATA WSAData;
        WSAStartup(MAKEWORD(2,0), &WSAData);
    #endif
    Zia::Server::Server server;
    server.start();

    #if defined(SYSTEM_WINDOWS)
        WSACleanup();
    #endif

    return 0;
}
