#include "../src/m_type_traits.h"
#include <gtest/gtest.h>
#include <iostream>
#include "test_objects.h"

using namespace my_stl;
TEST(TypeTraitsTest, TestAreSame) {
    bool areSame = __are_same<const int, const int>::value;
    ASSERT_EQ(areSame, 1);
    areSame = __are_same<int, int>::value;
    ASSERT_EQ(areSame, 1);

    areSame = __are_same<int&, int&>::value;
    ASSERT_EQ(areSame, 1);
    areSame = __are_same<int&&, int&&>::value;
    ASSERT_EQ(areSame, 1);
    areSame = __are_same<int*, int*>::value;
    ASSERT_EQ(areSame, 1);
    areSame = __are_same<const int, const int>::value;
    ASSERT_EQ(areSame, 1);
    areSame = __are_same<const int*, const int*>::value;
    ASSERT_EQ(areSame, 1);
    areSame = __are_same<const int*, int>::value;
    ASSERT_EQ(areSame, 0);
    areSame = __are_same<const int*, int * const>::value;
    ASSERT_EQ(areSame, 0);
    areSame = __are_same<const int*, int* volatile>::value;
    ASSERT_EQ(areSame, 0);
    areSame = __are_same<int&, int>::value;
    ASSERT_EQ(areSame, 0);
    areSame = __are_same<const int&, const int>::value;
    ASSERT_EQ(areSame, 0);
    areSame = __are_same<const int&, const int&&>::value;
    ASSERT_EQ(areSame, 0);
    areSame = __are_same<int, int* const>::value;
    ASSERT_EQ(areSame, 0);

    areSame = __are_same<void, void*>::value;
    ASSERT_EQ(areSame, 0);
    areSame = __are_same<void*, void*>::value;
    ASSERT_EQ(areSame, 1);
}

TEST(TypeTraitsTest, TestRemoveReference) {
    bool flag = __are_same<remove_reference_t<const int>, const int>::value;
    ASSERT_EQ(flag, 1);
    flag = __are_same<remove_reference_t<int>, int>::value;
    ASSERT_EQ(flag, 1);
    flag = __are_same<remove_reference_t<const int&>, const int>::value;
    ASSERT_EQ(flag, 1);
    flag = __are_same<remove_reference_t<int&>, int>::value;
    ASSERT_EQ(flag, 1);
    flag = __are_same<remove_reference_t<int*>, int*>::value;
    ASSERT_EQ(flag, 1);
    flag = __are_same<remove_reference_t<int&>, int&>::value;
    ASSERT_EQ(flag, 0);
    flag = __are_same<remove_reference_t<const int&>, const int>::value;
    ASSERT_EQ(flag, 1);
    flag = __are_same<remove_reference_t<const volatile int&>, const volatile int>::value;
    ASSERT_EQ(flag, 1);
}
