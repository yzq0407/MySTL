//an implementation of the sgi stl verstion of uninitialized, mainly on the three global functions
//
//of course in this case it is not in global scope but the implementation is based on the type
//traits approach which seperate the plain old type (POD, primitive) with other object types
//to initialize and copy objects
//Created by Theodore Yang on 11/18/2016
#ifndef MY_STL_UNINITIALIZED_H
#define MY_STL_UNINITIALIZED_H

#include <string.h>   //for memmove
#include "m_type_traits.h"


namespace my_stl {
    //this function construct object in the allocated memory [result, result + (last - first))
    //by calling the copy ctor, note that the construction of these object has to be "commit or rollback"
    //which means if any exception is thrown, we have to destroy all the previous object
    template <typename InputIterator, typename ForwardIterator>
    ForwardIterator uninitialized_copy (InputIterator first, InputIterator last, ForwardIterator result) {
       for (; first != last; ++result, ++first) {
           construct (&*result, *first);
       }
       return result;
    }

    //specialized version, for char* and wchar_t*, use memmove, is much faster than constructing
    inline char* uninitialized_copy (const char* first, const char* last, char* result) {
        memmove (result, first, last - first);
        return result + (last - first);
    }

    inline wchar_t* uninitialized_copy (const wchar_t* first, wchar_t* last, wchar_t* result) {
        memmove (result, first, last - first);
        return result + (last - first);
    }
    

    //this function will initialize the memory in [first, last) by the given value T
    template <typename ForwardIterator, typename TYPE>
    void uninitialized_fill (ForwardIterator first, ForwardIterator last, const TYPE& value) {
        for (; first != last; ++first) {
            construct (&*first, value);
        }
    }

    //this function will contruct exactly n object in the range [first, first + n) by given value T
    template <typename ForwardIterator, typename TYPE>
    void uninitialize_fill_n (ForwardIterator first, size_t n, const TYPE& value) {
        for (; n > 0; --n, ++first) {
            construct(&*first, value);
        }
    }
}


#endif
