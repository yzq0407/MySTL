//unit tests for the unique_ptr class 
//Created by Theodore Yang on 12/24/2016

#include <iostream>
#include <memory>

#include "../src/m_unique_ptr.h"
#include "test_objects.h"

#include <gtest/gtest.h>
#include "boost/compressed_pair.hpp"


namespace {
    //empty class and structs
    struct pure_empty {
    };

    struct base {
        constexpr int operator()(int x, int y) {
            return x * 10 + y;
        }
    };

    struct derive: base {
        //this overhide the base
        constexpr int operator()(int x, int y) {
            return x * 5 + y;
        }
    };

    //deleters
    template <typename T>
    struct empty_deleter_1 {
        //same as the default deleter
        void operator()(T* pointer) {
            delete(pointer);
        }
    };

    template <typename T>
    struct empty_deleter_2 {
        //add a counter
        static int counter;
        void operator()(T* pointer) {
            delete(pointer);
            ++counter;
        }

        static int getCount() {
            return counter;
        }
    };

    template <typename T>
    int empty_deleter_2<T>::counter = 0;
}

namespace unit_tests {
    TEST(UniquePtrTest, TestCompressedPair1) {
        //non empty compressed pair
        using namespace my_stl;
        compressed_pair<int, int> pair(1, 2);
        static_assert(sizeof (boost::compressed_pair<int, int>)
                == sizeof (pair), "failed on compressed_pair<int, int>");
        ASSERT_EQ(pair.first(), 1);
        ASSERT_EQ(pair.second(), 2);
        pair.first() = 5;
        pair.second() = -2;
        ASSERT_EQ(pair.first(), 5);
        ASSERT_EQ(pair.second(), -2);

        int x = 10;
        const std::string str= "hello, world";
        compressed_pair<int&, std::string> pair_2(x, str);
        static_assert(sizeof (boost::compressed_pair<int, std::string>)
                == sizeof (pair_2), "failed on compressed_pair");
        ASSERT_EQ(pair_2.first(), x);
        ASSERT_EQ(pair_2.second(), str);

        x = 15;
        ASSERT_EQ(pair_2.first(), x);

        compressed_pair<int, const std::string&> pair_3(x, str);
        static_assert(sizeof (boost::compressed_pair<int, const std::string&>)
                == sizeof (pair_3), "failed on compressed_pair");
        x = 20;
        ASSERT_EQ(pair_3.first(), 15);
        ASSERT_EQ(pair_2.second(), str);

        compressed_pair<const Test_FOO_Heap, Test_FOO_Array> pair_4
            (Test_FOO_Heap(1), Test_FOO_Array(10));
        static_assert(sizeof (boost::compressed_pair<const Test_FOO_Heap, Test_FOO_Array>)
                == sizeof (pair_4), "failed on compressed_pair");

        pair_4.second() = Test_FOO_Array(15);
        ASSERT_EQ(pair_4.second() == Test_FOO_Array(15), true);
    }

    TEST(UniquePtrTest, TestCompressedPair2) {
        //test one of the element is empty
        using namespace my_stl;
        compressed_pair<pure_empty, int> pair_1(2);
        static_assert(sizeof(pair_1) == sizeof (boost::compressed_pair<pure_empty, int>),
            "failed on empty compressed_pair");
        static_assert(sizeof(decltype(pair_1.first())) == sizeof(pure_empty&), 
                "failed on empty compressed_pair");
        ASSERT_EQ(pair_1.second(), 2);

        compressed_pair<std::less<int>, std::string> pair_2("abc");
        static_assert(sizeof(pair_2) 
                == sizeof (boost::compressed_pair<std::less<int>, std::string>), 
            "failed on empty compressed_pair");
        //test this functor
        ASSERT_EQ(pair_2.first()(5, 3), false);
        ASSERT_EQ(pair_2.first()(5, 5), false);
        ASSERT_EQ(pair_2.first()(-3, 3), true);

        int x = 5;
        const compressed_pair<const int, is_pointer<int>> pair_3(x);
        static_assert(sizeof(pair_3) 
                == sizeof (boost::compressed_pair<const int, is_pointer<int>>),
            "failed on empty compressed_pair");
        static_assert(is_same<typename decltype(pair_3)::second_reference, 
            is_pointer<int>&>::value, "");
        static_assert(!remove_reference_t<decltype(pair_3.second())>::value, 
           "failed on empty compressed pair");

        using less_str = std::less<std::string>;
        less_str less_func;
        //note that this the reference of a empty class doesn't count as empty class
        const compressed_pair<less_str&, const std::string&> pair_4(less_func, "hello world");
        static_assert(sizeof(pair_4) 
                == sizeof (boost::compressed_pair<less_str&, const std::string&>),
            "failed on empty compressed pair");
        /* std::cout << "the size of compressed_pair<less<string>&, const string&> is: " << */
        /*     sizeof pair_4 << std::endl; */
        
        ASSERT_EQ(pair_4.first()("easy", "hard"), true);
        ASSERT_EQ(pair_4.first()("hard", ""), false);
        ASSERT_EQ(pair_4.first()("easy", "easy"), false);
    }

    TEST(UniquePtrTest, TestCompressedPair3) {
        //test both empty classes components
        using namespace my_stl;
        compressed_pair<std::less<int>, std::greater<std::string>> pair_1;
        static_assert(sizeof(pair_1) == 
                sizeof (boost::compressed_pair<std::less<int>, std::greater<std::string>>), 
                "failed on both empty compressed pair");
        static_assert(pair_1.first()(3, 5), "failed on both empty compressed pair");
        ASSERT_EQ(pair_1.second()("hard", "easy"), true);

        //two same classes
        compressed_pair<true_type, true_type> pair_2;
        static_assert(sizeof(pair_2) == 
                sizeof (boost::compressed_pair<true_type, true_type>),
                "failed on both empty compressed pair");
        static_assert(pair_2.first().value == 1, "failed on both empty compressed pair");
        static_assert(pair_2.second().value == 1, "failed on both empty compressed pair");

        //inheritance
        compressed_pair<base, derive> pair_3;
        static_assert(pair_3.first()(10, 1) == 101, "failed on both empty compressed pair");
        static_assert(pair_3.second()(10, 1) == 51, "failed on both empty compressed pair");
    }

    TEST(UniquePtrTest, TestBasicConstruction) {
        std::unique_ptr<int> s_upi(new int(5));
        my_stl::unique_ptr<int> m_upi(new int(5));
        static_assert(sizeof (s_upi) == sizeof (m_upi), "basic construction failed");
        ASSERT_EQ(*s_upi,  *m_upi);

        std::unique_ptr<std::string> s_upstr(new std::string("hello world"));
        my_stl::unique_ptr<std::string> m_upstr(new std::string("hello world"));
        static_assert(sizeof (s_upstr) == sizeof (m_upstr), "basic construction failed");
        ASSERT_EQ(*s_upstr == *m_upstr, true);

        std::unique_ptr<Test_FOO_Simple> s_up_simple(new Test_FOO_Simple(100));
        my_stl::unique_ptr<Test_FOO_Simple> m_up_simple(new Test_FOO_Simple(100));
        static_assert(sizeof (s_up_simple) == sizeof (m_up_simple), "basic construction failed");
        ASSERT_EQ(*s_up_simple == *m_up_simple, true);

        std::unique_ptr<Test_FOO_Heap> s_up_heap(new Test_FOO_Heap(50));
        my_stl::unique_ptr<Test_FOO_Heap> m_up_heap(new Test_FOO_Heap(50));
        static_assert(sizeof (s_up_heap) == sizeof (m_up_heap), "basic construction failed");
        ASSERT_EQ(*s_up_heap == *m_up_heap, true);
    }

    TEST(UniquePtrTest, TestPlainTypeDeleterConstruction) {
        //construct a unique_ptr<_Tp, _Dp> with custome _Dp with plain type (not reference)
        using std::string;
        std::unique_ptr<int, empty_deleter_1<int>> s_upi(new int(5), empty_deleter_1<int>());
        my_stl::unique_ptr<int, empty_deleter_1<int>> m_upi(new int(5), empty_deleter_1<int>());
        static_assert(sizeof s_upi == sizeof m_upi, "plain deleter construction failed");

        std::unique_ptr<string, empty_deleter_1<string>> s_up_str(new string("basic string!"), empty_deleter_1<string>());
        my_stl::unique_ptr<string, empty_deleter_1<string>> m_up_str(new string("basic string!"), empty_deleter_1<string>());
        static_assert(sizeof s_up_str == sizeof m_up_str, "plain deleter construction failed");

        for (int count = 0; count <= 100; ++count) {
            std::unique_ptr<int, empty_deleter_2<int>> s_upi(new int(count), 
                    empty_deleter_2<int>());
            my_stl::unique_ptr<int, empty_deleter_2<int>> m_upi(new int(count),
                    empty_deleter_2<int>());
            ASSERT_EQ(*s_upi, *m_upi);
            ASSERT_EQ(2 * count, empty_deleter_2<int>::counter);
        }
    }
}// unit test
