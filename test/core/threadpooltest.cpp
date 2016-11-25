#include <catch.hpp>
#include "threadpooltest.h"

std::condition_variable condition;
std::mutex ready_m;

TEST_CASE("Thread Pool Test", "[thread]") {
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
}
