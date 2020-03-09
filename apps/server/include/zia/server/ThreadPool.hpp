/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** zia server ThreadPool.hpp
*/

#pragma once

#include <atomic>
#include <functional>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <vector>
#include "zia/common/Logger.hpp"

namespace Zia::Server {

    using Consumer = std::function<void()>;

    class ThreadPool {
    public:
        ThreadPool(size_t nb) : _nb(nb), _logger("ThreadPool")
        {
            for (size_t i = 0; i < nb; i += 1) {
                this->_threads.emplace_back([&] {
                    while (this->_work.load()) {
                        Consumer task;

                        {
                            std::unique_lock<std::mutex> lock(this->_tasksMutex);

                            if (this->_tasks.empty())
                                this->_cond.wait(lock);
                            if (!this->_work.load())
                                return;
                            task = std::move(this->_tasks.front());
                            this->_tasks.pop();
                        }

                        this->_logger.debug("Executing task.");
                        task();
                    }
                });
            }
        }

        ~ThreadPool()
        {
            this->_work = false;
            this->_cond.notify_all();

            for (std::thread &t : this->_threads)
                t.join();
        }

        bool pushTask(Consumer &&task)
        {
            std::unique_lock<std::mutex> lock(this->_tasksMutex);
            if (this->_tasks.size() > this->_nb * 2) {
                this->_logger.warning("Rejecting task, too many are already queued.");
                return false;
            }
            this->_tasks.push(std::forward<Consumer>(task));
            this->_cond.notify_one();
            return true;
        }

    protected:
    private:
        const size_t _nb;
        Logger _logger;
        std::vector<std::thread> _threads;
        std::queue<Consumer> _tasks;
        std::mutex _tasksMutex;
        std::condition_variable _cond;
        std::atomic_bool _work = true;
    };

}