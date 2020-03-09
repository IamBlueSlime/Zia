/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** zia server FileWatcher.hpp
*/

#pragma once

#include <atomic>
#include <string>
#include <thread>
#include <unordered_map>
#include "zia/common/PathUtils.hpp"
#include "zia/common/Logger.hpp"

namespace Zia::Server {

    class FileWatcher {
    public:
        using Callback = std::function<void()>;

        FileWatcher(const std::string &path, Callback &&cb)
            : _path(path), _cb(std::forward<Callback>(cb)), _logger("FileWatcher")
        {
            this->check(true);

            this->_thread = std::thread([&] {
                while (this->_work) {
                    std::this_thread::sleep_for(std::chrono::seconds(1));

                    if (this->check())
                        this->_cb();
                }
            });
        }

        ~FileWatcher()
        {
            this->stop();
        }

        void stop()
        {
            this->_work = false;
            this->_thread.join();
        }

    protected:
    private:
        const std::string _path;
        const std::function<void()> _cb;
        Logger _logger;

        std::thread _thread;
        bool _work = true;
        std::unordered_map<std::string, fs::file_time_type> _paths {};

        bool check(bool silent = false)
        {
            bool notify = false;

            for (auto it = this->_paths.begin(); it != this->_paths.end();) {
                if (!fs::exists(it->first)) {
                    notify = true;
                    it = this->_paths.erase(it);

                    if (!silent)
                        this->_logger.info("Detected file deletion: " + it->first + ".");
                } else {
                    it++;
                }
            }

            for (auto &file : fs::recursive_directory_iterator(this->_path)) {
                auto current = fs::last_write_time(file);
                auto filePath = file.path().string();

                if (this->_paths.find(filePath) == this->_paths.end()) {
                    if (!silent)
                        this->_logger.info("Detected file creation: " + filePath + ".");
                    this->_paths[filePath] = current;
                    notify = true;
                } else if (this->_paths[filePath] != current) {
                    if (!silent)
                        this->_logger.info("Detected file motification: " + filePath + ".");
                    this->_paths[filePath] = current;
                    notify = true;
                }
            }

            return notify;
        }
    };

}
