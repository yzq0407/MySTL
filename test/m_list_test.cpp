#include "../src/m_list.h"
#include <list>
#include <gtest/gtest.h>
#include <iostream>

TEST(ListTest, TestListConstruction) {
    my_stl::list<int> list;
    std::list<int> list2;

    std::cout << *list.node << std::endl;

}
