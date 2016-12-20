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
