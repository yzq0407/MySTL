#include "../src/m_list.h"
#include <list>
#include <gtest/gtest.h>
#include <iostream>
#include "test_objects.h"


template<typename T>
inline void assertListSize(const std::list<T>& std_list, const my_stl::list<T>& my_list) {
    ASSERT_EQ(std_list.size(), my_list.size());
}


template <typename T>
inline void assertListEqual(const std::list<T>& std_list, const my_stl::list<T>& my_list) {
    ASSERT_EQ(std_list.size(), my_list.size()) << "given two different list to be tested";
    if (my_list.empty())    return;
    ASSERT_EQ(std_list.front() == my_list.front(), true);
    ASSERT_EQ(std_list.back() == my_list.back(), true);
    auto m_begin = my_list.cbegin();
    for (auto s_begin = std_list.cbegin();
           s_begin != std_list.cend(); ++s_begin, ++m_begin) {
        ASSERT_EQ(*s_begin == *m_begin, true);
    }
    ASSERT_EQ(m_begin == my_list.cend(), true);
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

//fixture
class ListTestFixture: public ::testing::Test {
    protected:
        std::list<int> sl_i;
        my_stl::list<int> ml_i;
        std::list<Test_FOO_Simple> sl_s;
        my_stl::list<Test_FOO_Simple> ml_s;

        std::list<Test_FOO_Heap> sl_h;
        my_stl::list<Test_FOO_Heap> ml_h;

        std::list<Test_FOO_Array> sl_a;
        my_stl::list<Test_FOO_Array> ml_a;

        std::list<std::string> sl_str;
        my_stl::list<std::string> ml_str;
};

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



template<typename T>
inline void testPushInsertTemplate(std::list<T>& std_list, my_stl::list<T>& my_list) {
    assertListEqual(std_list, my_list);
    for (int i = 0; i < 100; ++i) {
        my_list.push_back(T(i));
        std_list.push_back(T(i));
        assertListEqual(std_list, my_list);
        //obtain a iterator and increment it random times
        auto s_it = std_list.cbegin();
        auto m_it = my_list.cbegin();
        size_t incre = rand() % (i + 1);
        for (; incre > 0; --incre, ++s_it, ++m_it);
        auto my_insertion = my_list.insert(m_it, T(i * 2));
        auto std_insertion = std_list.insert(s_it, T(i * 2));
        if (std_insertion != std_list.cend()) {
            ASSERT_EQ(*std_insertion == *my_insertion, true);
        }
        assertListEqual(std_list, my_list);
    }
}



TEST_F(ListTestFixture, TestPushAndInsert) {
    testPushInsertTemplate(sl_i, ml_i);
    testPushInsertTemplate(sl_a, ml_a);
    testPushInsertTemplate(sl_s, ml_s);
    testPushInsertTemplate(sl_h, ml_h);
}


template<typename T>
inline void testListInsertTemplate(std::list<T>& std_list, my_stl::list<T>& my_list) {
    ASSERT_EQ(std_list.size(), 0);
    ASSERT_EQ(my_list.size(), 0);
    std_list.insert(std_list.begin(), {T(4), T(-1), T(13), T(12)});
    my_list.insert(my_list.end(), {T(4), T(-1), T(13), T(12)});
    assertListEqual(std_list, my_list);
    auto pos_std = std_list.end();
    auto pos_my = my_list.end();
    --pos_std;
    --pos_my;
    --pos_std;
    --pos_my;
    std_list.insert(pos_std, 12, T(100));
    my_list.insert(pos_my, 12, T(100));
    assertListEqual(std_list, my_list);

    std_list.insert(std_list.end(), 0, T(1));
    my_list.insert(my_list.end(), 0, T(1));
    assertListEqual(std_list, my_list);

    std_list.insert(++std_list.begin(), {T(2), T(3)});
    my_list.insert(++my_list.begin(), {T(2), T(3)});
    assertListEqual(std_list, my_list);

    std::vector<T> ua = {};
    std_list.insert(std_list.end(), ua.begin(), ua.end());
    my_list.insert(my_list.end(), ua.begin(), ua.end());
    assertListEqual(std_list, my_list);

    ua = {T(1), T(2), T(3)};
    std_list.insert(std_list.end(), ua.begin(), ua.end());
    my_list.insert(my_list.end(), ua.begin(), ua.end());
    assertListEqual(std_list, my_list);

    std_list.clear();
    my_list.clear();
    assertListEqual(std_list, my_list);
}

TEST_F(ListTestFixture, TestListInsert) {
    testListInsertTemplate(sl_i, ml_i);
    testListInsertTemplate(sl_a, ml_a);
    testListInsertTemplate(sl_s, ml_s);
    testListInsertTemplate(sl_h, ml_h);
}

template<typename T>
inline void testMoreConstructions() {
    using my_stl::list;
    //range fill
    list<T> m_list (12, T(3));
    std::list<T> s_list(12, T(3));
    assertListEqual(s_list, m_list);

    list<T> m_list2(100);
    std::list<T> s_list2(100);
    assertListEqual(s_list2, m_list2);

    list<T> m_list3(m_list2);
    assertListEqual(s_list2, m_list3);

    list<T> m_list4(list<T>{});
    assertListEqual(std::list<T>{}, m_list4);

    list<T> m_list5(s_list.cbegin(), s_list.cend());
    assertListEqual(s_list, m_list5);


    list<T> m_list_move {T(1), T(2), T(3), T(4), T(5), T(6)};
    std::list<T> s_list_move {T(1), T(2), T(3), T(4), T(5), T(6)};
    list<T> m_list6(std::move(m_list_move));
    assertListEqual(s_list_move, m_list6);
}

/* TEST(ListTest, ListMoreConstructions) { */
/*     testMoreConstructions<int>(); */
/*     testMoreConstructions<Test_FOO_Simple>(); */
/*     testMoreConstructions<Test_FOO_Array>(); */
/*     testMoreConstructions<Test_FOO_Heap>(); */
/* } */


template<typename T>
inline void testListSpliceTemplate(my_stl::list<T>& m_list) {
    m_list.insert(m_list.cend(), {T(1), T(2), T(3), T(4)});
    my_stl::list<T> splice_list1 = {T(10), T(20), T(30)};
    auto it = m_list.begin();
    ++it;
    m_list.splice(it, splice_list1);
    ASSERT_EQ(m_list == my_stl::list<T> ({T(1), T(10), T(20), T(30), T(2), T(3), T(4)}), true);
    ASSERT_EQ(splice_list1.empty(), true) << "splice failed";
    splice_list1.splice(splice_list1.begin(), m_list, it);
    ASSERT_EQ(m_list == my_stl::list<T> ({T(1), T(10), T(20), T(30), T(3), T(4)}), true);
    ASSERT_EQ(splice_list1 == my_stl::list<T> ({T(2)}), true);

    it = m_list.begin();
    my_stl::advance(it, 3);

    m_list.splice(m_list.begin(), m_list, it, m_list.end());
    ASSERT_EQ(m_list == my_stl::list<T> ({T(30), T(3), T(4), T(1), T(10), T(20)}), true);
}

TEST_F(ListTestFixture, TestListSplice) {
    testListSpliceTemplate(ml_i);
    testListSpliceTemplate(ml_a);
    testListSpliceTemplate(ml_s);
    testListSpliceTemplate(ml_h);
}




    

