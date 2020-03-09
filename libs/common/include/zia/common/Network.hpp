/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** zia common Network.hpp
*/

#pragma once

#include "openZia/OperatingSystem.hpp"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_DARWIN)
    #include <arpa/inet.h>
    #include <unistd.h>

    typedef int socket_t;
#elif defined(SYSTEM_WINDOWS)
    #define _WINSOCKAPI_

    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #include <BaseTsd.h>
    #include <io.h>

    typedef SOCKET socket_t;
#endif