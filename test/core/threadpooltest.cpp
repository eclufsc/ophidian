#include "threadpooltest.h"

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

std::condition_variable condition;
std::mutex ready_m;


int main() {
    const auto kLutSize = 7;
    std::default_random_engine generator;
    auto pool = std::make_unique<ThreadPool<4>>();
    auto indices = std::make_shared<LutIndices<kLutSize>>();

    std::generate(indices->references.begin(), indices->references.end(), [&generator]{
        std::normal_distribution<double> distribution(std::pow(2.0, 6.0), std::pow(2.0, 5.0));
        return distribution(generator);
    });

    for(auto & ref : indices->references)
    {
        std::cout << ref << " ";
    }
    std::cout << std::endl;

    pool->post([&](){
        std::unique_lock<std::mutex> lock(ready_m);
        std::cout << "Thread " << std::this_thread::get_id() << " is waiting..." << std::endl;
        condition.wait(lock);
        std::cout << "Results:\n  \n";
        std::for_each(indices->indices.begin(), indices->indices.end(), [](const auto & el){
            std::cout << el << " ";
        });
        std::cout << std::endl;
    });

    pool->post([&](){
        std::unique_lock<std::mutex> lock(ready_m);
        std::cout << "Thread " << std::this_thread::get_id() << " is waiting..." << std::endl;
        condition.wait(lock);
        std::for_each(indices->indices.begin(), indices->indices.end(), [](const auto & el){
            if(el == 0) {
                // between 0 and 1
            } else if(el < kLutSize) {
                // between el-1 el
            }
        });
    });

    pool->post([&](){
        {
            std::unique_lock<std::mutex> lock(ready_m);
            std::cout << "Thread " << std::this_thread::get_id() << " is running the necessary task..." << std::endl;
        }
        std::array<std::array<double, 7>, 500>::const_iterator LUTIndexIt = indices->values.begin();
        std::array<double, 500>::const_iterator inputIt = indices->references.begin();
        std::array<std::array<double, 7>::size_type, 500>::iterator outputIt = indices->indices.begin();
        const auto LUTIndexEndIt = indices->values.end();
        for(; LUTIndexIt != LUTIndexEndIt; ++LUTIndexIt, ++outputIt, ++inputIt) {
            const double input = *inputIt;
            auto & output = *outputIt;
            output = std::distance(LUTIndexIt->begin(), std::lower_bound(LUTIndexIt->begin(), LUTIndexIt->end(), input));
        }
        condition.notify_all();
    });

    return 0;
}
