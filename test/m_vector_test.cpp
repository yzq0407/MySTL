#include "../src/m_vector.h"
#include <gtest/gtest.h>
#include "test_objects.h"


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
