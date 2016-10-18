//a test cpp file to see my implemented allocator works for other STL containers
#include <iostream>
#include <vector>
#include "../src/m_alloc.h"

namespace my_stl {
    void testAllocWithVector () {
        //create a size 0 vector
        std::vector<int, allocator<int>> test_vec;
        for (size_t i = 0; i < 20; ++i) {
            test_vec.push_back(i);
            std::cout << "the vector size is: " << test_vec.size() << std::endl;
        }
    }
}


int main() {
    my_stl::testAllocWithVector();
}

