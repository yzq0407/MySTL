#include "../src/m_type_traits.h"
#include <gtest/gtest.h>
#include <iostream>
#include "test_objects.h"

using namespace my_stl;
TEST(TypeTraitsTest, TestAreSame) {
    bool areSame = is_same<const int, const int>::value;
    ASSERT_EQ(areSame, 1);
    areSame = is_same<int, int>::value;
    ASSERT_EQ(areSame, 1);

    areSame = is_same<int&, int&>::value;
    ASSERT_EQ(areSame, 1);
    areSame = is_same<int&&, int&&>::value;
    ASSERT_EQ(areSame, 1);
    areSame = is_same<int*, int*>::value;
    ASSERT_EQ(areSame, 1);
    areSame = is_same<const int, const int>::value;
    ASSERT_EQ(areSame, 1);
    areSame = is_same<const int*, const int*>::value;
    ASSERT_EQ(areSame, 1);
    areSame = is_same<const int*, int>::value;
    ASSERT_EQ(areSame, 0);
    areSame = is_same<const int*, int * const>::value;
    ASSERT_EQ(areSame, 0);
    areSame = is_same<const int*, int* volatile>::value;
    ASSERT_EQ(areSame, 0);
    areSame = is_same<int&, int>::value;
    ASSERT_EQ(areSame, 0);
    areSame = is_same<const int&, const int>::value;
    ASSERT_EQ(areSame, 0);
    areSame = is_same<const int&, const int&&>::value;
    ASSERT_EQ(areSame, 0);
    areSame = is_same<int, int* const>::value;
    ASSERT_EQ(areSame, 0);

    areSame = is_same<void, void*>::value;
    ASSERT_EQ(areSame, 0);
    areSame = is_same<void*, void*>::value;
    ASSERT_EQ(areSame, 1);
}

TEST(TypeTraitsTest, TestLogicalClauese) {
    static_assert(__and__<>::value, "and is not correct");
    static_assert(__and__<__true_type>::value, "and is not correct");
    static_assert(!__and__<__false_type>::value, "and is not correct");
    static_assert(!__and__<true_type, false_type>::value, "and is not correct");
    static_assert(__and__<__true_type, true_type>::value, "and is not correct");
    static_assert(!__and__<false_type, true_type>::value, "and is not correct");

    static_assert(!__and__<true_type, false_type, false_type, true_type>::value, "and is not correct");
    static_assert(__and__<true_type, true_type, true_type, true_type, true_type, true_type>::value, "and is not correct");
    static_assert(!__and__<false_type, true_type, true_type, true_type, true_type, true_type>::value, "and is not correct");
    static_assert(__and__<true_type, true_type, true_type>::value, "and is not correct");

    static_assert(!__or__<>::value, "or is not correct");
    static_assert(__or__<__true_type>::value, "or is not correct");
    static_assert(!__or__<__false_type>::value, "or is not correct");
    static_assert(__or__<true_type, false_type>::value, "or is not correct");
    static_assert(__or__<__true_type, true_type>::value, "or is not correct");
    static_assert(__or__<false_type, true_type>::value, "or is not correct");
    static_assert(!__or__<false_type, false_type>::value, "or is not correct");

    static_assert(__or__<true_type, false_type, false_type, true_type>::value, "or is not correct");
    static_assert(__or__<true_type, true_type, true_type, true_type, true_type, true_type>::value, "or is not correct");
    static_assert(__or__<false_type, true_type, true_type, true_type, true_type, true_type>::value, "or is not correct");
    static_assert(!__or__<false_type, false_type, false_type, false_type, false_type, false_type>::value, "or is not correct");

    //congrat assert
    ASSERT_EQ(true, true);
}

TEST(TypeTraitsTest, TestRemoveReference) {
    bool flag = is_same<remove_reference_t<const int>, const int>::value;
    ASSERT_EQ(flag, 1);
    flag = is_same<remove_reference_t<int>, int>::value;
    ASSERT_EQ(flag, 1);
    flag = is_same<remove_reference_t<const int&>, const int>::value;
    ASSERT_EQ(flag, 1);
    flag = is_same<remove_reference_t<int&>, int>::value;
    ASSERT_EQ(flag, 1);
    flag = is_same<remove_reference_t<int*>, int*>::value;
    ASSERT_EQ(flag, 1);
    flag = is_same<remove_reference_t<int&>, int&>::value;
    ASSERT_EQ(flag, 0);
    flag = is_same<remove_reference_t<const int&>, const int>::value;
    ASSERT_EQ(flag, 1);
    flag = is_same<remove_reference_t<const volatile int&>, const volatile int>::value;
    ASSERT_EQ(flag, 1);
}


TEST(TypeTraitsTest, TestIsArray) {
    static_assert(!is_array<int>::value, "is array failed");
    static_assert(!is_array<std::string>::value, "is array failed");
    static_assert(is_array<int[]>::value, "is array failed");
    static_assert(is_array<int[5]>::value, "is array failed");
    static_assert(is_array<std::string[]>::value, "is array failed");
    static_assert(is_array<std::string[5]>::value, "is array failed");
}

namespace __testEmptyStruct {
    struct empty1 {};
    struct empty2 {
        typedef int imp;
        typedef double donald;
    };
    struct empty3: empty2 {};

    struct empty4 {
        void print(int i, double d) {
            std::cout << i << d<< std::endl;
        }

        int getIncrement(int i) {
            return i + 1;
        }
    };

    struct solid1 {
        int x = 1;
    };

    struct solid2 {
        virtual void print(int i) {
            std::cout << "solid : " << i << std::endl;
        }
    };

    struct solid3: solid2 {};

    struct solid4 {
        char a = 'a';
    };

    struct solid5 {
        bool b = true;
    };
}

TEST(TypeTraitsTest, TestIsEmpty) {
    using namespace __testEmptyStruct;
    static_assert(is_empty_v<empty1>, "is_empty traits failed");
    static_assert(is_empty_v<empty2>, "is_empty traits failed");
    static_assert(is_empty_v<empty3>, "is_empty traits failed");
    static_assert(is_empty_v<empty4>, "is_empty traits failed");
    static_assert(!is_empty_v<solid1>, "is_empty traits failed");
    static_assert(!is_empty_v<solid2>, "is_empty traits failed");
    static_assert(!is_empty_v<solid3>, "is_empty traits failed");
    static_assert(!is_empty_v<solid4>, "is_empty traits failed");
    static_assert(!is_empty_v<solid5>, "is_empty traits failed");
}


