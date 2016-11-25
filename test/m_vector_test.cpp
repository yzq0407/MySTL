#include "../src/m_vector.h"
#include <gtest/gtest.h>
#include "test_objects.h"
#include <ctime>   //for std::clock()


TEST(VectorTest, TestSimpleConstruction) {
    {   
        std::vector<int> s_vector;
        my_stl::vector<int> m_vector;
        ASSERT_EQ(s_vector.size(), m_vector.size()) << "Empty vector construction failed";
    }
    for (int size = 1; size < 10000; size += 10) {
        std::vector<int> s_vector(size);
        my_stl::vector<int> m_vector(size);
        ASSERT_EQ(s_vector.size(), m_vector.size()) << "Constructing vector with size " 
            << size << " failed";
        for (int i = 0; i < size; ++i) {
            ASSERT_EQ(s_vector[i], m_vector[i]) << "The " << i 
                << "th element with vector of size " << size << " failed";
        }
    }
    for (int size = 1; size < 10000; size += 10) {
        std::vector<int> s_vector(size, 1564);
        my_stl::vector<int> m_vector(size, 1564);
        ASSERT_EQ(s_vector.size(), m_vector.size());
        for (int i = 0; i < size; ++i) {
            ASSERT_EQ(s_vector[i], 1564);
            ASSERT_EQ(m_vector[i], 1564);
        }
    }
}

TEST(VectorTest, TestObjectConstruction) {
    {
        typedef std::vector<Test_FOO_Simple> SD_VS;
        typedef my_stl::vector<Test_FOO_Simple> M_VS;
        //empty construction
        {
            SD_VS sv;
            M_VS mv;
            ASSERT_EQ(sv.size(), mv.size());
        }
        //buld construction
        for (int size = 0; size < 1000; ++size) {
            SD_VS sv(size);
            M_VS mv(size);
            EXPECT_EQ(sv.size(), mv.size());
            EXPECT_EQ(mv == M_VS(size, Test_FOO_Simple()), 1);
        }
        
    }
    {
        //build construction2
        typedef std::vector<Test_FOO_Array> SD_VA;
        typedef my_stl::vector<Test_FOO_Array> M_VA;
        Test_FOO_Array tfa(14);
        for (int size = 0; size < 1000; ++size) {
            SD_VA sv(size, tfa);
            M_VA mv(size, tfa);
            for (int i = 0; i < size; ++i) {
                EXPECT_EQ(sv[i] == mv[i], true);
            }
        }
    }
    {
        //build construction3
        typedef std::vector<Test_FOO_Heap> SD_VH;
        typedef my_stl::vector<Test_FOO_Heap> M_VH;
        Test_FOO_Heap tfh(18);
        for (int size = 0; size < 1000; ++size) {
            SD_VH sv(size, tfh);
            M_VH mv(size, tfh);
            for (int i = 0; i < size; ++i) {
                EXPECT_EQ(sv[i] == mv[i], true);
            }
        } 
    }
}

//test fixture to be used in the following test
struct VectorTestFixture: public ::testing::Test{
    //int vector
    std::vector<int> svi;
    my_stl::vector<int> mvi;

    //simple objects
    std::vector<Test_FOO_Simple> sv_simple;
    my_stl::vector<Test_FOO_Simple> mv_simple;

    //array objects
    std::vector<Test_FOO_Array> sv_array;
    my_stl::vector<Test_FOO_Array> mv_array;

    //heap objects
    std::vector<Test_FOO_Heap> sv_heap;
    my_stl::vector<Test_FOO_Heap> mv_heap;
    
    void SetUp() {
        printf ("Setup the vector fixture\n");
    }

    void TearDown() {
        printf ("Tearing down the vector fixture\n");
    }
};

TEST_F(VectorTestFixture, TestPushBack) {
    //simple push
    const int size_limit = 10000;
    for (int i = 0; i < size_limit; ++i) {
        svi.push_back(i);
        mvi.push_back(i);
       
        sv_simple.push_back(Test_FOO_Simple(i, 2* i - 1, (char)(i % 256)));
        mv_simple.push_back(Test_FOO_Simple(i, 2* i - 1, (char)(i % 256)));

        sv_array.push_back(Test_FOO_Array(i));
        mv_array.push_back(Test_FOO_Array(i));

        sv_heap.push_back(Test_FOO_Heap(i));
        mv_heap.push_back(Test_FOO_Heap(i));

        EXPECT_EQ(svi.size(), mvi.size()) << "int vector size assertion failed";
        EXPECT_EQ(svi.capacity(), mvi.capacity()) << "int vector capacity assertion failed";
        
        EXPECT_EQ(sv_heap.size(), mv_heap.size()) << "heap vector capacity assertion failed";
        EXPECT_EQ(sv_heap.capacity(), mv_heap.capacity()) << "heap vector capacity assertion failed";
        
        EXPECT_EQ(svi.size(), mvi.size()) << "int vector size assertion failed";
        EXPECT_EQ(svi.capacity(), mvi.capacity()) << "int vector size assertion failed";
        
        EXPECT_EQ(svi.back(), mvi.back()) << "int vector back element assertion failed";
        EXPECT_EQ(sv_simple.front(), mv_simple.front()) << "simple vector front assertion failed";

        int medium = i / 2;
        EXPECT_EQ(sv_array[medium] == mv_array[medium], true) << "array vector medium element assertion failed"; 
    }
} 

TEST_F(VectorTestFixture, TestPopBack) {
    using std::clock;
    constexpr int size_limit = 10000;
    //also count time
    clock_t begin_time = clock();
    for (int i = 0; i < size_limit; ++i) {
        svi.push_back(i);
    }
    clock_t end_time = clock();
    printf("It takes std vector %f seconds to push 1000000 int\n", float(end_time - begin_time) / CLOCKS_PER_SEC);
    //my vector
    begin_time = clock();
    for (int i = 0; i < size_limit; ++i) {
        mvi.push_back(i);
    }
    end_time = clock();
    printf("It takes my vector %f seconds to push 1000000 int\n", float(end_time - begin_time) / CLOCKS_PER_SEC);
    //heap based object 
    begin_time = clock();
    for (int i = 0; i < size_limit; ++i) {
        sv_heap.push_back(Test_FOO_Heap(i));
    }
    end_time = clock();
    printf("It takes std vector %f seconds to push 1000000 heap objects\n", float(end_time - begin_time) / CLOCKS_PER_SEC);
    //my vector
    begin_time = clock();
    for (int i = 0; i < size_limit; ++i) {
        mv_heap.push_back(Test_FOO_Heap(i));
    }
    end_time = clock();
    printf("It takes my vector %f seconds to push 1000000 heap objects\n", float(end_time - begin_time) / CLOCKS_PER_SEC);

    while (!mvi.empty()) {
        EXPECT_EQ(svi.back(), mvi.back()) << "Back element assertion failed";
        EXPECT_EQ(svi.size(), mvi.size()) << "Size assertion after pop back failed";
        EXPECT_EQ(svi.capacity(), mvi.capacity()) << "Capacity assertion after pop back failed";
        svi.pop_back();
        mvi.pop_back();
    }

    //clear test
    sv_heap.clear();
    mv_heap.clear();
    ASSERT_EQ(sv_heap.size(), mv_heap.size()) << "Size assertion after clear failed";
    ASSERT_EQ(sv_heap.capacity(), mv_heap.capacity()) << "Capacity assertion after clear failed";
}

TEST_F(VectorTestFixture, TestAssignment) {
    using std::clock;
    clock_t begin, end;
    constexpr int large_size = 100000;
    constexpr int small_size = 500;
    //create two vector and assign one to the other
    begin = clock();
    mvi = my_stl::vector<int>(large_size, 20);
    end = clock();
    printf("It takes my vector %f seconds to be assigned from %d elements to %d elements\n", 
            float(end - begin) / CLOCKS_PER_SEC, 0, large_size);
    for (int num: mvi) {
        ASSERT_EQ(num, 20);
    }
    
    begin = clock();
    svi = std::vector<int>(large_size, 20);
    end = clock();
    printf("It takes std vector %f seconds to be assigned from %d elements to %d elements\n", 
            float(end - begin) / CLOCKS_PER_SEC, 0, large_size);
    //test vector contains heap objects assignment
    mv_heap = my_stl::vector<Test_FOO_Heap>(small_size, Test_FOO_Heap(20));
    sv_heap = std::vector<Test_FOO_Heap>(small_size, Test_FOO_Heap(20));

    begin = clock();
    mv_heap = my_stl::vector<Test_FOO_Heap>(large_size, Test_FOO_Heap(15));
    end = clock();
    printf("It takes my vector %f seconds to be assigned from %d heap objects to %d heap objects\n",
            float(end - begin) / CLOCKS_PER_SEC, small_size, large_size);
    
    begin = clock();
    sv_heap = std::vector<Test_FOO_Heap>(large_size, Test_FOO_Heap(15));
    end = clock();
    printf("It takes std vector %f seconds to be assigned from %d heap objects to %d heap objects\n",
            float(end - begin) / CLOCKS_PER_SEC, small_size, large_size);
    ASSERT_EQ(mv_heap.size(), sv_heap.size()) << "Size assertion failed after heap vector assignemtn";
    ASSERT_EQ(mv_heap.capacity(), mv_heap.capacity()) << "Capacity assertion failed after heap vector assignment";
    //also use range-for loop to loop through the vector
    for (Test_FOO_Heap object: mv_heap) {
        ASSERT_EQ(object == Test_FOO_Heap(15), 1) << "Element value assertion failed after heap vector assignment";
    }
}

TEST_F(VectorTestFixture, TestErasure) {
    constexpr int start = 1000, decrem = 2;
    for (int i = start; i >= 0; i -= decrem) {
        svi.push_back(i);
        mvi.push_back(i);

        sv_heap.push_back(Test_FOO_Heap(i));
        mv_heap.push_back(Test_FOO_Heap(i));
    }
    //generate random number to remove range of elements
    while (!svi.empty()) {
        int rand_start = rand() % svi.size();
        int rand_end = rand() % (svi.size() - rand_start) + rand_start + 1;
        //one third change to remove one element inplace
        bool erase_one = rand() % 3 == 0;
        if (erase_one) {
            svi.erase(svi.begin() + rand_start);
            mvi.erase(mvi.begin() + rand_start);
            sv_heap.erase(sv_heap.begin() + rand_start);
            mv_heap.erase(mv_heap.begin() + rand_start);
        }
        else {
            svi.erase(svi.begin() + rand_start, svi.begin() + rand_end);
            mvi.erase(mvi.begin() + rand_start, mvi.begin() + rand_end);
            sv_heap.erase(sv_heap.begin() + rand_start, sv_heap.begin() + rand_end);
            mv_heap.erase(mv_heap.begin() + rand_start, mv_heap.begin() + rand_end);
        }

        ASSERT_EQ(svi.size(), mvi.size()) << "int vector size assertion failed";
        ASSERT_EQ(svi.capacity(), mvi.capacity()) << "int vector capacity assertion failed";

        ASSERT_EQ(sv_heap.size(), mv_heap.size()) << "heap object vector size assertion failed";
        ASSERT_EQ(sv_heap.capacity(), mv_heap.capacity()) << "heap object capacity assertion failed";
        ASSERT_EQ(mvi.size(), mv_heap.size()) << "cross size validation failed"; 
        //size and capacity correct, iterator to match each element
        for (int i = 0; i < svi.size(); ++i) {
            ASSERT_EQ(svi[i], mvi[i]) << "Integer array at " << i <<"th element didn't match";
            ASSERT_EQ(sv_heap[i] == mv_heap[i], true) << "Heap object array at " << i <<"th element"
                << "didn't match";
        }
    }
}
