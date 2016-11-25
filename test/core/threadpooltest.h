#ifndef OPHIDIAN_CORE_THREAD_POOL_TEST_H
#define OPHIDIAN_CORE_THREAD_POOL_TEST_H

#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>
#include <array>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include <condition_variable>

template <uint32_t kNumThreads>
class ThreadPool {
    using QueueOfTasks = std::queue< std::function<void()> >;
public:
    ThreadPool() :
    stop_(false) {
        workers_.reserve(kNumThreads);
        for(uint32_t i = 0; i < kNumThreads; ++i) {
            workers_.emplace_back([this]{
                std::function<void()> task;
                for(;;) {
                    {
                        std::unique_lock<std::mutex> lock(queueMutex_);
                        condition_.wait(lock, [this]()->bool{
                            return stop_ || !tasks_.empty();
                        });
                        if(stop_ && tasks_.empty())
                        {
                            return;
                        }
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    task();
                }
            });
        }
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            stop_ = true;
        }
        condition_.notify_all();
        for(auto & worker : workers_)
        {
            worker.join();
        }
    }

    template <class T>
    void post(T && task) {
        {
            {
                std::unique_lock<std::mutex> lock(queueMutex_);
                tasks_.emplace([task](){
                    task();
                });
            }
            condition_.notify_one();
        }
    }
private:
    std::vector<std::thread> workers_;
    QueueOfTasks tasks_;
    std::mutex queueMutex_;
    std::condition_variable condition_;
    bool stop_;
};


template <uint32_t kLutDimension>
struct LutIndices {
    std::array<double, 500> references;
    std::array<std::array<double, kLutDimension>, 500> values;
    typename std::array<typename std::array<double, kLutDimension>::size_type, 500> indices;

    LutIndices() {
        for(auto& bounds : values) {
            double curr = 0.0;
            for(auto & value : bounds) {
                value = std::pow(2.0, curr);
                curr += 1.0;
            }
        }
    }
};


#endif // OPHIDIAN_CORE_THREAD_POOL_TEST_H
