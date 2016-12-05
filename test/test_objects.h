#ifndef __M_TEST_OBJECTS_H
#define __M_TEST_OBJECTS_H
//bunch of object for testing, including several cases

class Test_FOO_Simple {
    private:
        int mi;
        long long mll;
        char mc;

    public:
        Test_FOO_Simple();
        explicit Test_FOO_Simple(int i);
        //a implicit conversion
        explicit Test_FOO_Simple(char c);
        Test_FOO_Simple(int, long long, char);

        bool operator==(const Test_FOO_Simple& rhs) const;
        bool operator!=(const Test_FOO_Simple& rhs) const;
};


class Test_FOO_Array {
    private:
        static constexpr int size = 100;
        int array[size];

    public:
        Test_FOO_Array();

        //explicitly initialize all the array member to be val
        explicit Test_FOO_Array(int val);
        bool operator==(const Test_FOO_Array&) const;
        bool operator!=(const Test_FOO_Array&) const;
};

class Test_FOO_Heap {
    private:
        int* m1;
        Test_FOO_Simple* m2;

    public:
        Test_FOO_Heap();
        explicit Test_FOO_Heap(int);
        explicit Test_FOO_Heap(const Test_FOO_Simple& foo);
        Test_FOO_Heap(const Test_FOO_Heap& foo);
        Test_FOO_Heap& operator=(const Test_FOO_Heap& foo);

        Test_FOO_Heap(Test_FOO_Heap&& foo);
        Test_FOO_Heap& operator=(Test_FOO_Heap&& rhs) noexcept;

        bool operator==(const Test_FOO_Heap& rhs) const;
        bool operator!=(const Test_FOO_Heap& rhs) const;

        void swap(Test_FOO_Heap& foo) noexcept{
            int* temp1 = m1;
            Test_FOO_Simple* temp2 = m2;
            m1 = foo.m1;
            m2 = foo.m2;
            foo.m1 = temp1;
            foo.m2 = temp2;
        }
        
        ~Test_FOO_Heap();
};
#endif
