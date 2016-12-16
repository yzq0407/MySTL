//implementation of the sgi stl version of stl_construct.h, mainly on two global functions: construct and destroy
//Here it is not global, for obvious reason
//Created by Theodore Yang on 11/18/2016

#ifndef __MY_STL_CONSTRUCT_H
#define __MY_STL_CONSTRUCT_H
#include <new>    //for placement new
#include <utility>            //for std::forward
#include "m_type_traits.h"   //for type traits
#include "m_iterator.h"      //for iterator traits

namespace my_stl {
    template <typename TYPE1, typename TYPE2>
    inline void construct (TYPE1* tp1, const TYPE2& t2) {
        //call placement new to construct type1 in place
        new (tp1) TYPE1(t2);
    }

    //variadic construct
    template <typename TYPE1, typename... Args>
    inline void construct (TYPE1* tp1, Args&&... args) {
        new (tp1) TYPE1(std::forward<Args>(args)...);
    }

    //destroy, we should have several versions
    //if it is just a single pointer, call destructor, shouldn't incur too much overhead
    //even it has a trivial dtor
    template <typename TYPE>
    inline void destroy (TYPE* tp) noexcept{
        tp -> ~TYPE();
    }

    //helper function to extract whether a type has a default dtor
    template <typename TYPE>
    inline typename __type_traits<TYPE>::has_trivial_dtor __has_trivial_dtor(const TYPE& type) noexcept{
        return typename __type_traits<TYPE>::has_trivial_dtor();
    }

    //if we want to destroy the object in the range of [start, end), it is important that the
    //dtor is not trivial, otherwise we are doing useless for-loop
    template <typename ForwardIterator>
    inline void destroy (ForwardIterator first, ForwardIterator last) noexcept{
        //depend on whether we have
        __destroy(first, last, __has_trivial_dtor(typename iterator_traits<ForwardIterator>::value_type()));
    }


    //overloaded function for destroy, for trivial dtor type
    template <typename ForwardIterator>
    inline void __destroy (ForwardIterator first, ForwardIterator last, __true_type) noexcept{
    }

    //if there is no trivial dtor, call this function
    template <typename ForwardIterator>
    inline void __destroy (ForwardIterator first, ForwardIterator last, __false_type) noexcept{
        for (; first != last; ++first) {
            destroy (&*first);
        }
    }

}

#endif
