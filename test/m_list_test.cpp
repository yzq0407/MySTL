#include "../src/m_list.h"
#include <list>
#include <gtest/gtest.h>
#include <iostream>
#include "test_objects.h"


template<typename T>
inline void assertListSize(const std::list<T>& std_list, const my_stl::list<T>& my_list) {
    ASSERT_EQ(std_list.size(), my_list.size());
}

template<typename T>
inline void testPushBackAndClearTemplate(my_stl::list<T>& my_list){
    ASSERT_EQ(my_list.empty(), true) << "not given empty list";
    ASSERT_EQ(my_list.size(), 0) << "not given empty list";

    for (size_t size = 0; size < 100; ++size) {
        ASSERT_EQ(my_list.size(), size) << "push back size error";
        my_list.push_back(T(size));
    }

    my_list.clear();
    ASSERT_EQ(my_list.empty(), true) << "cannot clear to get empty list";
    ASSERT_EQ(my_list.size(), 0) << "cannot clear to get size 0 list";
    
    //clear an empty list
    my_list.clear();
    ASSERT_EQ(my_list.empty(), true) << "cannot clear to get empty list";
    ASSERT_EQ(my_list.size(), 0) << "cannot clear to get size 0 list";


    //re insert to make sure no memory leak
    for (size_t size = 0; size < 1000; ++size) {
        ASSERT_EQ(my_list.size(), size);
        my_list.push_back(T(size));
    }
}

TEST(ListTest, TestBasicListConstruction) {
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


TEST(ListTest, TestListPushBackAndClear) {
    my_stl::list<int> list_int;
    testPushBackAndClearTemplate(list_int);
    my_stl::list<Test_FOO_Heap> list_heap;
    testPushBackAndClearTemplate(list_heap);
}
