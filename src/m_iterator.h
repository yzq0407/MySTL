//this is a simple implementation of the C++ standard library iterator
//include traits, iterator types and utility functions
//Created by Theodore Yang on 11/14/2016


#ifndef __MY_STL_ITERATOR_H
#define __MY_STL_ITERATOR_H

#include <cstddef>   //for std::ptrdiff_t
namespace my_stl {

    //tag definition
    struct input_iterator_tag {};

    struct output_iterator_tag {};

    struct forward_iterator_tag: public input_iterator_tag {};

    struct bidirectional_iterator_tag: public forward_iterator_tag {};

    struct random_access_iterator_tag: public bidirectional_iterator_tag {};


    //the general traits, using nested type to exact the iterator information
    template <typename Iterator>
    struct iterator_traits{
        //these type informations are required by C++ standard
        typedef typename Iterator::difference_type difference_type;
        typedef typename Iterator::value_type value_type;
        typedef typename Iterator::pointer pointer;
        typedef typename Iterator::reference reference;
        typedef typename Iterator::iterator_category iterator_category;
    };

    //template partial specialization in order to deal with the pointer type iterator
    template <typename T>
    struct iterator_traits<T*> {
        typedef std::ptrdiff_t difference_type;
        typedef T value_type;
        typedef T* pointer;
        typedef T& reference;
        //for pointer type, it is always a random access iterator
        typedef random_access_iterator_tag iterator_category;
    };

    //partial specialized template for const pointer
    template <typename T>
    struct iterator_traits<const T*> {
        typedef std::ptrdiff_t difference_type;
        //this is the only difference we want to add, it is not const T
        typedef T value_type;
        typedef const T* pointer;
        typedef const T& reference;
        typedef random_access_iterator_tag iterator_category;
    };

    //functions
    //
    template <typename ForwardIterator, typename Distance>
    void advance(ForwardIterator &it, Distance n);

    //c++11 functions
    template <typename ForwardIterator>
    void next(ForwardIterator &it, typename iterator_traits<ForwardIterator>::difference_type n = 1);

}

#endif
