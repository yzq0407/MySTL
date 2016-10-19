//a test cpp file to see my implemented allocator works for other STL containers
#include <iostream>
#include <vector>
#include <cassert>
#include <chrono>
#include "../src/m_alloc.h"

#define TEST_EPOCH 1000000
namespace my_stl {
    void testNaiveAllocWithVector () {
        //create a size 0 vector
        std::vector<int, naive_allocator<int>> test_vec;
        auto tic = std::chrono::duration_cast<std::chrono::milliseconds> 
            (std::chrono::system_clock::now().time_since_epoch()).count();
        //test push_back
        for (size_t i = 0; i < TEST_EPOCH; ++i) {
            assert(test_vec.size() == i);
            test_vec.push_back(i);
        }
        size_t size = TEST_EPOCH;
        while (!test_vec.empty()) {
            assert(test_vec.size() == size);
            --size;
            test_vec.pop_back();
        }
        auto toc = std::chrono::duration_cast<std::chrono::milliseconds> 
            (std::chrono::system_clock::now().time_since_epoch()).count();
        std::cout << "my simple implementation takes " << (toc - tic) << "ms " << std::endl;


        //now test the sgi alloc
        std::vector<int> sgi_vec;
        tic = std::chrono::duration_cast<std::chrono::milliseconds> 
            (std::chrono::system_clock::now().time_since_epoch()).count();
        //test push_back
        for (size_t i = 0; i < TEST_EPOCH; ++i) {
            assert(sgi_vec.size() == i);
            sgi_vec.push_back(i);
        }
        size = TEST_EPOCH;
        while (!test_vec.empty()) {
            assert(sgi_vec.size() == size);
            --size;
            sgi_vec.pop_back();
        }
        toc = std::chrono::duration_cast<std::chrono::milliseconds> 
            (std::chrono::system_clock::now().time_since_epoch()).count();
        std::cout << "sgi alloc implementation takes " << (toc - tic) << "ms " << std::endl;
    }
}


int main() {
    //test if SGI STL use the malloc allocator
    #ifdef __USE_MALLOC
    std::cout << "use malloc not defined" << std::endl;
    #else
    std::cout << "not using malloc" << std::endl;
    #endif
    //test result, sgi does not define use malloc
    my_stl::testNaiveAllocWithVector();
    //test on 10/18/16, pushing and poping 1M times
    //simple implementation -- 126ms
    //sgi alloc implementation -- 28ms
}

