#include "../src/m_list.h"
#include <list>
#include <gtest/gtest.h>
#include <iostream>
#include <algorithm>   //for std::sort
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

TEST(ListTest, ListMoreConstructions) {
    testMoreConstructions<int>();
    testMoreConstructions<Test_FOO_Simple>();
    testMoreConstructions<Test_FOO_Array>();
    testMoreConstructions<Test_FOO_Heap>();
}


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

template <typename T>
void testListReverseIteratorTemplate(std::list<T>& s_list, my_stl::list<T>& m_list) {
    s_list = {T(5), T(2), T(3), T(12), T(-1)};
    m_list = {T(5), T(2), T(3), T(12), T(-1)};
    auto m_it = m_list.crbegin();
    auto s_it = s_list.crbegin();
    for (;m_it != m_list.crend() && s_it != s_list.crend();
            ++m_it, ++s_it){
        ASSERT_EQ(*m_it == *s_it, true);
    }
}


TEST_F(ListTestFixture, TestListReverseIterator) {
    testListReverseIteratorTemplate(sl_i, ml_i);
    testListReverseIteratorTemplate(sl_a, ml_a);
    testListReverseIteratorTemplate(sl_h, ml_h);
    testListReverseIteratorTemplate(sl_s, ml_s);
}

template<typename T>
inline void testListAssignmentTemplate(std::list<T>& s_list, my_stl::list<T>& m_list) {
    assertListEqual(s_list, m_list);
    //test preconstructed list
    {
        std::list<T> s_rhs = {T(1), T(2), T(-1), T(100)};
        my_stl::list<T> m_rhs = {T(1), T(2), T(-1), T(100)};
        s_list = s_rhs;
        m_list = m_rhs;
        assertListEqual(s_list, m_list);
        //assert self assignment
        s_list = s_list;
        m_list = m_list;
        assertListEqual(s_list, m_list);
    }
    {
        //move assignment
        std::list<T> s_rhs = {T(-1), T(12), T(-114), T(0), T(500), T(213)};
        my_stl::list<T> m_rhs = {T(-1), T(12), T(-114), T(0), T(500), T(213)};
        s_list = std::move(s_rhs);
        m_list = std::move(m_rhs);
        assertListEqual(s_list, m_list);
        //rvalue move
        s_list = std::list<T> {T(-3), T(-300)};
        m_list = my_stl::list<T> {T(-3), T(-300)};
        //self move
        s_list = std::move(s_rhs);
        m_list = std::move(m_rhs);
        assertListEqual(s_list, m_list);
    }
    {
        //large assignment
        std::list<T> s_rhs(100000, T(-11));
        my_stl::list<T> m_rhs(100000, T(-11));
        s_list = s_rhs;
        m_list = m_rhs;
        assertListEqual(s_list, m_list);
    }
}

TEST_F(ListTestFixture, TestListAssignment) {
    testListAssignmentTemplate(sl_i, ml_i);
    testListAssignmentTemplate(sl_a, ml_a);
    testListAssignmentTemplate(sl_h, ml_h);
    testListAssignmentTemplate(sl_s, ml_s);
}

//test merge function
TEST(ListTest, TestListMerge) {
    auto less = [] (const int a, const int b) {return a < b;};
    {
        //empty merge
        std::list<int> l1 = {-3, 9, 10, 14, 21};
        std::list<int> l2 = {};
        l1.merge(l2);
        my_stl::list<int> m1 = {-3, 9, 10, 14, 21};
        my_stl::list<int> m2 = {};
        m1.merge(m2, less);
        assertListEqual(l1, m1);
        assertListEqual(l2, m2);
        l2.merge(l1);
        m2.merge(m1, less);
        assertListEqual(l1, m1);
        assertListEqual(l2, m2);
    }
    {
        std::list<int> l1;
        std::list<int> l2;
        l1.merge(l2);
        my_stl::list<int> m1;
        my_stl::list<int> m2;
        m1.merge(m2, less);
        assertListEqual(l1, m1);
    }
    {
        std::list<int> l1 = {-3, 9, 10, 14, 21};
        std::list<int> l2 = {-100, -99, -98, -97};
        l1.merge(l2);
        my_stl::list<int> m1 = {-3, 9, 10, 14, 21};
        my_stl::list<int> m2 = {-100, -99, -98, -97};
        m1.merge(m2, less);
        assertListEqual(l1, m1);
    }
    {
        std::list<int> l1 = {-3, 9, 10, 14, 21};
        std::list<int> l2 = {-2, 2, 8, 20, 100};
        l1.merge(l2);
        my_stl::list<int> m1 = {-3, 9, 10, 14, 21};
        my_stl::list<int> m2 = {-2, 2, 8, 20, 100};
        m1.merge(m2);
        assertListEqual(l1, m1);
    }
    {
        std::list<int> l2 = {-3, 9, 10, 14, 21};
        std::list<int> l1 = {-2, 2, 8, 20, 100};
        l1.merge(l2);
        my_stl::list<int> m2 = {-3, 9, 10, 14, 21};
        my_stl::list<int> m1 = {-2, 2, 8, 20, 100};
        m1.merge(m2);
        assertListEqual(l1, m1);
    }
    {
        std::list<int> l2(100, -1);
        std::list<int> l1(10000, -2);
        l1.merge(l2);
        my_stl::list<int> m2(100, -1);
        my_stl::list<int> m1(10000, -2);
        m1.merge(m2);
        assertListEqual(l1, m1);
    }
    {
        //large dataset tests with random numbers
        using std::clock;
        constexpr size_t data_size = 10000;
        std::vector<int> rand_nums1(data_size);
        std::vector<int> rand_nums2(data_size);
        for (int i = 0; i < data_size; ++i) {
            rand_nums1[i] = rand() % data_size;
            rand_nums2[i] = rand() % data_size;
        }
        std::sort(rand_nums1.begin(), rand_nums1.end());
        std::sort(rand_nums2.begin(), rand_nums2.end());
        clock_t tic, toc;
        {
            std::list<int> list1(rand_nums1.begin(), rand_nums1.end());
            std::list<int> list2(rand_nums2.begin(), rand_nums2.end());
            tic = clock();
            list1.merge(list2);
            toc = clock();
            std::cout << "It take std::list " << (float(toc - tic) / CLOCKS_PER_SEC) 
                << " secs to merge two sorted lists with size: " << data_size << std::endl;

            //my list
            my_stl::list<int> m_list1(rand_nums1.begin(), rand_nums1.end());
            my_stl::list<int> m_list2(rand_nums2.begin(), rand_nums2.end());
            tic = clock();
            m_list1.merge(m_list2);
            toc = clock();
            std::cout << "It take my_stl::list " << (float(toc - tic) / CLOCKS_PER_SEC) 
                << " secs to merge two sorted lists with size: " << data_size << std::endl;

            //assert equal
            assertListEqual(list1, m_list1);
        }
    }

}

template<typename T, typename Comp = std::less<T>>
void testListSortTemplate(std::list<T>& s_l, my_stl::list<T>& m_l, Comp comp = Comp()) {
    using std::clock;
    assertListEqual(s_l, m_l);
    {
        //empty test
        s_l.sort(comp);
        m_l.sort(comp);
        assertListEqual(s_l, m_l);
        //one elements sort
        s_l.push_back(T(-4));
        m_l.push_back(T(-4));
        s_l.sort(comp);
        m_l.sort(comp);
        assertListEqual(s_l, m_l);
        s_l.push_back(T(0));
        m_l.push_back(T(0));
        s_l.sort(comp);
        m_l.sort(comp);
        assertListEqual(s_l, m_l);
        s_l.clear();
        m_l.clear();
    }
    {
        //hundreds element
        for (int i = 0; i < 100; ++i) {
            int val = rand() % 200;
            s_l.push_back(T(val));
            m_l.push_back(T(val));
        }
        assertListEqual(s_l, m_l);
        s_l.sort(comp);
        m_l.sort(comp);
        assertListEqual(s_l, m_l);
    }
    {
        //large number test, start from totally random
        size_t testSize = 1000;
        clock_t tic, toc;
        for (int i = 0; i < testSize; ++i) {
            T value = T(rand() % testSize);
            s_l.push_back(value);
            m_l.push_back(value);
        }
        //sort random value
        tic = clock();
        s_l.sort(comp);
        toc = clock();
        std::cout << "sort size: " << testSize << " std::list with random values takes " 
            << (float(toc - tic) / CLOCKS_PER_SEC) << " secs" << std::endl;

        tic = clock();
        m_l.sort(comp);
        toc = clock();
        std::cout << "sort size: " << testSize << " my_stl::list with random values takes " 
            << (float(toc - tic) / CLOCKS_PER_SEC) << " secs" << std::endl;
        assertListEqual(s_l, m_l);

        //sort in-order value
        tic = clock();
        s_l.sort(comp);
        toc = clock();
        std::cout << "sort size: " << testSize << " std::list with in-order values takes " 
            << (float(toc - tic) / CLOCKS_PER_SEC) << " secs" << std::endl;

        tic = clock();
        m_l.sort(comp);
        toc = clock();
        std::cout << "sort size: " << testSize << " my_stl::list with in-order values takes " 
            << (float(toc - tic) / CLOCKS_PER_SEC) << " secs" << std::endl;
        assertListEqual(s_l, m_l);
    }


}

TEST_F(ListTestFixture, ListSortTest) {
    testListSortTemplate(sl_i, ml_i);
    testListSortTemplate(sl_i, ml_i, std::greater<int>());

    //custom sort for heap object
    auto heap_less = [] (const Test_FOO_Heap& a, const Test_FOO_Heap& b) -> bool {
        if (a.getSimpleObjectMember() -> getCharMember() != 
                b.getSimpleObjectMember() -> getCharMember()) {
            return a.getSimpleObjectMember() -> getCharMember() <
                b.getSimpleObjectMember() -> getCharMember();
        }
        else return *a.getIntMember() > *b.getIntMember();
    };
    testListSortTemplate(sl_h, ml_h, heap_less);
}

TEST(ListTest, ListEmplaceTest) {
    std::string str = "emplace is a great addition intrduced in C++11 stl!";
    std::list<std::string> s_ls;
    my_stl::list<std::string> m_ls;
    for (int i = 0; i < 15; ++i) {
        s_ls.emplace_back(i, 'a');
        m_ls.emplace_back(i, 'a');
    }
    assertListEqual(s_ls, m_ls);
    for (int i = 0; i < 10; ++i) {
        s_ls.emplace_front(str, i, i + 5); 
        m_ls.emplace_front(str, i, i + 5); 
    }
    assertListEqual(s_ls, m_ls);
}
