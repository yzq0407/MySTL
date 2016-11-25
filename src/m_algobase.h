//Created by Theodore Yang on 11/24/2016
//implement several important functions
//this implementation is based on my understanding of the sgi stl implementation of m_algobase.h

#include "m_type_traits.h"
#include <string.h>    //for memmove
#include <cstddef>    //for ptrdiff_t  size_t
#include "m_iterator.h"


namespace my_stl {
    //copy function, optimized in the following way
    //1. is input is char* or wchar_t*, use memmove()
    //2. based on the input iterator type
    //      I. T* and const T*, if have trivial =, use memmove()
    //      II. RandomAccessIterator      use n < last - first to carry for-loop
    //      III, other input iterator, use first != last to carry for-loop

    //copy [first, last) into [result, result + (last - first))
    //generally, should use operation *result = *first, *(result + 1) = *(first + 1) ....
    //return iterator pointing to result + (last - first)
    //remember that if there is overlap, this can result in wrong result depend on whether it is
    //using memmove (memmove is copying all the data first and then assign later, so won't cause
    //any problem)
    //
    //forward declarations:
    template<typename InputIterator, typename OutputIterator>
    struct __copy_dispatch;

    template<typename InputIterator, typename OutputIterator>
    OutputIterator __copy(InputIterator first, InputIterator last,
            OutputIterator result, input_iterator_tag);
    //-------------------------------------------------------------------------
    //                  the most generalized function
    template <typename InputIterator, typename OutputIterator>
    inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result) {
        return __copy_dispatch<InputIterator, OutputIterator>() (first, last, result);
    }

    //-------------------------------------------------------------------------
    //              two specialized function, char* and wchar_t version
    //              not a template because c++ does not allow template partial specialization
    inline char* copy (const char* first, const char* last, char* result) {
        memmove(result, first, last - first);
        return result + (last - first);
    }

    inline wchar_t* copy (const wchar_t* first, const wchar_t* last,
            wchar_t* result) {
        memmove(result, first, sizeof(wchar_t)* (last - first));
        return result + (last - first);
    }

    //-----------------------------------------------------------------------------
    //                Dispatch functor to determine the iterator type
    //                remember C++ does not allow function template partial specialization
    //                but we still want to separate pointer like iterator with other iterator
    //                (for the reason that we can possibly use memmove() on pointer like iterator
    template<typename InputIterator, typename OutputIterator>
    struct __copy_dispatch {
        OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result) {
            return __copy(first, last, result, 
                    typename iterator_traits<InputIterator>::iterator_categoty());
        }
    };

    //--------------------------------------------------------------------------
    //              const pointer and pointer type iterator
    //              both these two functions need to determine whether the type they
    //              are pointing to has trivial operator=
    //              if so, we can also use memmove
    template<typename _TYPE>
    struct __copy_dispatch<_TYPE*, _TYPE*> {
        _TYPE* operator()(_TYPE* first, _TYPE* last, _TYPE* result) {
            return __copy_t(first, last, result, 
                    typename __type_traits<_TYPE>::has_trivial_assignment_operator());
        }
    };

    template<typename _TYPE>
    struct __copy_dispatch<const _TYPE*, _TYPE*> {
        _TYPE* operator()(const _TYPE* first, const _TYPE* last, _TYPE* result) {
            return __copy_t(first, last, result, 
                    typename __type_traits<_TYPE>::has_trivial_assignment_operator());
        }
    };

    //generalized form
    template<typename InputIterator, typename OutputIterator>
    inline OutputIterator __copy(InputIterator first, InputIterator last, OutputIterator result, 
            input_iterator_tag) {
        for (; first != last; ++first, ++last) {
            *result = *first;
        }
        return result;
    }

    //random access iterator form
    template<typename InputIterator, typename OutputIterator>
    inline OutputIterator __copy(InputIterator first, InputIterator last, OutputIterator result, 
            random_access_iterator_tag) {
        //use distance to determine the for-loop times
        for (typename iterator_traits<InputIterator>::difference_type distance = last - first;
                distance > 0; --distance, ++first, ++result) {
            *result = *first;
        }
        return result;
    }

    template<typename _TYPE> 
    inline _TYPE* __copy_t(_TYPE* first, _TYPE* last, _TYPE* result, __true_type) {
        //so the pointer is pointing to the type with trivial operator=
        //we only need to call memmove()
        memmove(result, first, sizeof(_TYPE) * (last - first));
        return result + (last - first);
    }

    template<typename _TYPE>
    inline _TYPE* __copy_t(_TYPE* first, _TYPE* last, _TYPE* result, __false_type) {
        //the operator= is not trivial, we need to call it one by one
        //but at least, it is a random access iterator
        for (ptrdiff_t distance = last - first; distance > 0; --distance, ++first, ++result) {
            *result = *first;
        }
        return result;
    }
}
