/*
** EPITECH PROJECT, 2019
** CPP_zia_2019
** File description:
** server Server.hpp
*/

#pragma once

#include <atomic>
#include <string>
#include "zia/common/Logger.hpp"
#include "zia/common/Config.hpp"
#include "zia/server/ThreadPool.hpp"
#include "zia/server/ServerSocket.hpp"
#include "zia/server/FileWatcher.hpp"
#include "openZia/Pipeline.hpp"

namespace Zia::Server {

    class Server {
    public:
        Server();

        void start();
        void stop();

    protected:
    private:
        static Server *INSTANCE;

        Logger _logger;
        Config _config;
        ThreadPool _threadPool;
        oZ::Pipeline _pipeline;
        ServerSocket _httpSocket;
        ServerSocket _httpsSocket;
        FileWatcher _modulesWatcher;
        FileWatcher _configWatcher;
        std::atomic_bool _acceptClients;

        void reloadModules(bool initial = true);
        void setupCtrlC();
        void waitClients();

        static void onCtrlC(int signal)
        {
            (void) signal;
            INSTANCE->stop();
        }
    };

}