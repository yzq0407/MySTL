#include "test_objects.h"

Test_FOO_Simple::Test_FOO_Simple(): mi(0), mll(-1), mc('a') {
}

Test_FOO_Simple::Test_FOO_Simple(int i): mi(i), mll(2 * i), mc('b') {}

Test_FOO_Simple::Test_FOO_Simple(char c): mi(0), mll(-2), mc(c) {}

Test_FOO_Simple::Test_FOO_Simple(int i, long long ll, char c):
    mi(i),
    mll(ll),
    mc(c)
{
}

bool Test_FOO_Simple::operator==(const Test_FOO_Simple& rhs) const {
    return mi == rhs.mi && mll == rhs.mll && mc == rhs.mc;
}


//FOO_Array
//
Test_FOO_Array::Test_FOO_Array() {
    for (int i = 0; i < size; ++i) {
        array[i] = 0;
    }
}

Test_FOO_Array::Test_FOO_Array(int val) {
    for (int i = 0; i < size; ++i) {
        array[i] = val;
    }
}

bool Test_FOO_Array::operator==(const Test_FOO_Array& rhs) const {
    for (int i = 0; i < size; ++i) {
        if (array[i] != rhs.array[i])   return false;
    }
    return true;
}


//FOO_Heap
Test_FOO_Heap::Test_FOO_Heap() {
    m1 = new int(-101);
    m2 = new Test_FOO_Simple(12);
}

Test_FOO_Heap::Test_FOO_Heap(int val) {
    m1 = new int(val);
    m2 = new Test_FOO_Simple(val);
}

Test_FOO_Heap::Test_FOO_Heap(const Test_FOO_Simple& foo) {
    m1 = new int(-101);
    m2 = new Test_FOO_Simple(foo);
}

Test_FOO_Heap::Test_FOO_Heap(const Test_FOO_Heap& foo) {
    m1 = new int(*foo.m1);
    m2 = new Test_FOO_Simple(*foo.m2);
}

const Test_FOO_Heap& Test_FOO_Heap::operator=(Test_FOO_Heap rhs) {
    swap(rhs);
    return *this;
}

Test_FOO_Heap::~Test_FOO_Heap() {
    delete m1;
    delete m2;
}



