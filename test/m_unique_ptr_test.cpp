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
            delete pointer;
        }
    };

    template <typename T>
    struct empty_deleter_2 {
        //add a counter
        static int counter;
        void operator()(T* pointer) {
            delete pointer;
            ++counter;
        }

        static int getCount() {
            return counter;
        }
    };


    template <typename T>
    int empty_deleter_2<T>::counter = 0;
    
    template <typename T>
    struct nonempty_deleter_1{
        int count = 0;
        void operator()(T* pointer) {
            delete pointer;
            ++count;
        }

        int getCount() {
            return count;
        }
    };

    struct nonempty_deleter_2 {
        bool is_last_int = false;

        template <typename T>
        void operator()(T* pointer) {
            delete pointer;
            if (my_stl::is_integral_v<T>) {
                is_last_int = true;
            }
            else {
                is_last_int = false;
            }
        }
    };
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

        //test one of the argument is lambda
        const std::string capture = "now, please capture me if you can";
        auto lambda_1 = [] (const std::string& a, const std::string& b) -> std::string {
            return a + b;};
        auto lambda_2 = [&] (int idx) -> char {return capture[idx];};
        static_assert(sizeof lambda_1 == 1, "lambda object size failure");
        static_assert(sizeof lambda_2 == sizeof(void *), "lambda object size failure");
        
        const compressed_pair<decltype(lambda_1), std::string> pair_5(lambda_1, "hello world");
        const compressed_pair<std::string, decltype(lambda_1)> pair_6("hello world", lambda_1);
        ASSERT_EQ(pair_5.first()("easy", "hard"), "easyhard");
        ASSERT_EQ(pair_6.second()("easy", "hard"), "easyhard");

        const compressed_pair<decltype(lambda_1), decltype(lambda_2)>
            pair_7(lambda_1, lambda_2);
        ASSERT_EQ(pair_7.first()("easy", "hard"), "easyhard");
        ASSERT_EQ(pair_7.second()(10), 'e');
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

        //lambda
        auto lambda_1 = [] (const std::string& a, const std::string& b) -> std::string {
            return a + b;};
        auto lambda_2 = [] (int a) -> int {return 10 * a % 2;};
        static_assert(sizeof lambda_1 == 1, "failed on empty lambda object");
        static_assert(sizeof lambda_2 == 1, "failed on empty lambda object");
        compressed_pair<std::less<int>, decltype(lambda_1)> pair_4(std::less<int>(), lambda_1);
        compressed_pair<decltype(lambda_1), decltype(lambda_2)> pair_5(lambda_1, lambda_2);
        static_assert(sizeof pair_4 == 1, "failed on both empty compressed pair");
        static_assert(sizeof pair_5 == 1, "failed on both empty compressed pair");
        ASSERT_EQ(pair_4.first()(5, 3), false);
        ASSERT_EQ(pair_4.second()("hello", "world"), "helloworld");
        ASSERT_EQ(pair_5.first()("hello", "world"), "helloworld");
        ASSERT_EQ(pair_5.second()(10), 0); 
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

    template <typename T>
    void testPlainTypeDeleterConstructionTemplate(const T& value) {
        //test construction using all the empty deleters
        std::unique_ptr<T, empty_deleter_1<T>> s_up(new T(value), empty_deleter_1<T>());
        my_stl::unique_ptr<T, empty_deleter_1<T>> m_up(new T(value), empty_deleter_1<T>());
        static_assert(sizeof s_up == sizeof m_up, "plain deleter construction failed");

        for (int count = 0; count <= 100; ++count) {
            std::unique_ptr<T, empty_deleter_2<T>> s_up(new T(value), 
                    empty_deleter_2<T>());
            my_stl::unique_ptr<T, empty_deleter_2<T>> m_up(new T(value),
                    empty_deleter_2<T>());
            ASSERT_EQ(*s_up, *m_up);
            ASSERT_EQ(2 * count, empty_deleter_2<T>::counter);
        }

        //test with a lambda function deleter
        auto lambda_deleter = [](T* ptr) {delete ptr;};
        std::unique_ptr<T, decltype(lambda_deleter)> s_up_lamb_del(new T(value), lambda_deleter);
        my_stl::unique_ptr<T, decltype(lambda_deleter)> m_up_lamb_del(new T(value), lambda_deleter);
        static_assert(sizeof s_up_lamb_del == sizeof m_up_lamb_del, 
                "plain deleter construction failed"); 
        ASSERT_EQ(*s_up_lamb_del == *m_up_lamb_del, true);
        
        //test with nonempty deleter
        nonempty_deleter_1<T> nemp_del;
        for (int count =0; count <= 10; ++count) {
            std::unique_ptr<T, nonempty_deleter_1<T>> s_up_nonemp1(new T(value), nemp_del);
            my_stl::unique_ptr<T, nonempty_deleter_1<T>> m_up_nonemp1(new T(value), nemp_del);
            ASSERT_EQ(s_up_nonemp1.get_deleter().getCount(), 0);
            ASSERT_EQ(m_up_nonemp1.get_deleter().getCount(), 0);
        }
        ASSERT_EQ(nemp_del.getCount(), 0);
    }

    TEST(UniquePtrTest, TestPlainTypeDeleterConstruction) {
        testPlainTypeDeleterConstructionTemplate<int>(15);
        testPlainTypeDeleterConstructionTemplate<Test_FOO_Simple>(Test_FOO_Simple(-10000));
        testPlainTypeDeleterConstructionTemplate<Test_FOO_Array>(Test_FOO_Array(210793));
        testPlainTypeDeleterConstructionTemplate<Test_FOO_Heap>(Test_FOO_Heap(0));
        testPlainTypeDeleterConstructionTemplate<std::string>("test constructions!");
    }

    template <typename T>
    void testReferenceTypeDeleterConstructionTemplate(const T& value) {
        empty_deleter_1<T> deleter1;
        empty_deleter_2<T> deleter2;
        nonempty_deleter_1<T> deleter3;
        std::unique_ptr<T, empty_deleter_1<T>&> s_up_1(new T(value), deleter1);
        my_stl::unique_ptr<T, empty_deleter_1<T>&> m_up_1(new T(value), deleter1);
        static_assert(sizeof s_up_1 == sizeof m_up_1, 
                "failed on reference type deleter construction");
        ASSERT_EQ(*s_up_1 == *m_up_1, true);

        const int base_counter = deleter2.counter;
        for (int i = 0; i <= 100; ++i) {
            std::unique_ptr<T, empty_deleter_2<T>&> s_up_2(new T(value), deleter2);
            my_stl::unique_ptr<T, empty_deleter_2<T>&> m_up_2(new T(value), deleter2);
            ASSERT_EQ(2 * i + base_counter, deleter2.counter);
        }

        for (int i = 0; i <= 100; ++i) {
            std::unique_ptr<T, nonempty_deleter_1<T>&> s_up_3(new T(value), deleter3);
            my_stl::unique_ptr<T, nonempty_deleter_1<T>&> m_up_3(new T(value), deleter3);
            ASSERT_EQ(2 * i, deleter3.getCount());
            ASSERT_EQ(2 * i, s_up_3.get_deleter().getCount());
            ASSERT_EQ(2 * i, m_up_3.get_deleter().getCount());
        }
        nonempty_deleter_2 deleter4;
        {
            my_stl::unique_ptr<T, nonempty_deleter_2&> m_up_4(new T(value), deleter4);
        }
        ASSERT_EQ(deleter4.is_last_int, my_stl::is_integral_v<T>);
    }

    TEST(UniquePtrTest, TestReferenceTypeDeleterConstruction) {
        testReferenceTypeDeleterConstructionTemplate<int>(15);
        testReferenceTypeDeleterConstructionTemplate<char>('a');
        testReferenceTypeDeleterConstructionTemplate<Test_FOO_Simple>(Test_FOO_Simple(-10000));
        testReferenceTypeDeleterConstructionTemplate<Test_FOO_Array>(Test_FOO_Array(210793));
        testReferenceTypeDeleterConstructionTemplate<Test_FOO_Heap>(Test_FOO_Heap(0));
        testReferenceTypeDeleterConstructionTemplate<std::string>("test constructions!");
    }

    template <typename T>
    void testResetAndReleaseTemplate(const T& value1, const T& value2) {
        using up_emp1 = my_stl::unique_ptr<T, empty_deleter_1<T>>;
        using up_emp2 = my_stl::unique_ptr<T, empty_deleter_2<T>>;
        using up_nemp1 = my_stl::unique_ptr<T, nonempty_deleter_1<T>&>;
        using up_nemp2 = my_stl::unique_ptr<T, nonempty_deleter_1<T>>;

        T* pointer_value = nullptr;
        {
            up_emp1 uptr(new T(value1));
            ASSERT_EQ(*uptr == value1, true);
            uptr.reset(new T(value2));
            ASSERT_EQ(*uptr == value2, true);
            pointer_value = uptr.release();
        }
        ASSERT_EQ(value2 == *pointer_value, true);
        delete pointer_value;

        int base = empty_deleter_2<T>::getCount();
        {
            up_emp2 uptr(new T(value1));
            ASSERT_EQ(*uptr == value1, true);
            uptr.reset(new T(value2));
            ASSERT_EQ(*uptr == value2, true);
            pointer_value = uptr.release();
        }
        ASSERT_EQ(value2 == *pointer_value, true);
        ASSERT_EQ(empty_deleter_2<T>::getCount(), base + 2);
        delete pointer_value;

        nonempty_deleter_1<T> deleter_nonempty;
        {
            up_nemp1 uptr(new T(value2), deleter_nonempty);
            ASSERT_EQ(*uptr == value2, true);
            uptr.reset(new T(value1));
            ASSERT_EQ(*uptr == value1, true);
            pointer_value = uptr.release();
        }
        ASSERT_EQ(value1 == *pointer_value, true);
        ASSERT_EQ(deleter_nonempty.getCount(), 2);
        delete pointer_value;

        {
            up_nemp2 uptr(new T(value2), nonempty_deleter_1<T>());
            ASSERT_EQ(*uptr == value2, true);
            uptr.reset(new T(value1));
            ASSERT_EQ(*uptr == value1, true);
            pointer_value = uptr.release();
            ASSERT_EQ(uptr.get_deleter().getCount(), 1);
        }
        ASSERT_EQ(value1 == *pointer_value, true);
        delete pointer_value;
    }

    TEST(UniquePtrTest, TestResetAndRelease) {
        testResetAndReleaseTemplate<int>(5, -10);
        testResetAndReleaseTemplate<char>('a', 'b');
        testResetAndReleaseTemplate<std::string>("hello", "world world world");
        testResetAndReleaseTemplate<Test_FOO_Simple>(Test_FOO_Simple(18), Test_FOO_Simple(11));
        testResetAndReleaseTemplate<Test_FOO_Array>(Test_FOO_Array(10), Test_FOO_Array(12));
        testResetAndReleaseTemplate<Test_FOO_Heap>(Test_FOO_Heap(3), Test_FOO_Heap(9));
    }


}// unit test
