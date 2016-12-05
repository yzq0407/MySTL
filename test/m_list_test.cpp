#include "../src/m_list.h"
#include <list>
#include <gtest/gtest.h>
#include <iostream>

TEST(ListTest, TestListConstruction) {
    my_stl::list<int> list;
    std::list<int> list2;
    ASSERT_EQ(list.empty(), true) << "failed to create empty list";
    ASSERT_EQ(list.size(), 0) << "empty list size is not 0";
    list.push_back(1);
    ASSERT_EQ(list.empty(), false) << "push back failed on empty test";
    ASSERT_EQ(list.size(), 1) << "push back failed on size test";
    list.push_back(2);
    ASSERT_EQ(list.empty(), false);
    ASSERT_EQ(list.size(), 2);
}
