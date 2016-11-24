#include <gtest/gtest.h>
#include <memory>
#include "../src/m_alloc.h"
TEST(AllocatorTest, Allocation) {
    void* vp = nullptr;
    ASSERT_EQ(vp, nullptr) << "The pointer is not initialized empty";
    vp = my_stl::alloc::allocate(4);
    ASSERT_NE(vp, nullptr) << "Allocated pointer is still empty";
    my_stl::alloc::deallocate(vp, 4);
}
