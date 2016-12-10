//this is a simple implementation of the C++ standard library iterator
//include traits, iterator types and utility functions
//Created by Theodore Yang on 11/14/2016


#ifndef __MY_STL_ITERATOR_H
#define __MY_STL_ITERATOR_H

#include <cstddef>   //for std::ptrdiff_t
namespace my_stl {

    //a general iterator base class, we want all iterator to inherit from this class
    template <typename Category, typename T, typename Distance = std::ptrdiff_t,
        typename Pointer = T*, typename Reference = T&>
    struct iterator {
        typedef Category iterator_category;
        typedef T value_type;
        typedef Distance difference_type;
        typedef Pointer pointer;
        typedef Reference reference;
    };

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
 
    //overloaded function1, for input iterator (basic type)
    template <typename InputIterator, typename Distance>
    inline void __advance(InputIterator &it, Distance n, input_iterator_tag) {
        while (n--) ++it;
    }

    //overloaded function2, for bidirectional iterator (the distance can be negative)
    template <typename BidirectionalIterator, typename Distance>
    inline void __advance(BidirectionalIterator &it, Distance n, bidirectional_iterator_tag) {
        if (n >= 0)
            while (n--) ++it;
        else
            while (n++) --it;
    }

    //overloaded function3, for random access iterator, directly add the distance into the iterator
    template <typename RandomAccessIterator, typename Distance>
    inline void __advance(RandomAccessIterator &it, Distance n, random_access_iterator_tag) {
        it += n;
    }

   //function to advance a iterator n steps
    template <typename InputIterator, typename Distance>
    void advance(InputIterator &it, Distance n) {
        //simply call the helper function to determine which algorithm should we use
        __advance(it, n, typename iterator_traits<InputIterator>::iterator_category());
    }

    //function to determine the distance between two iterators
    //overload function1, for input iterator
    template <typename InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type __distance(InputIterator first, 
            InputIterator last, input_iterator_tag) {
        typename iterator_traits<InputIterator>::difference_type n = 0;
        while (first != last) {
            ++first;
            ++n;
        }
        return n;
    }

    //overload function2, for random access iterator
    template <typename RandomAccessIterator>
    inline typename iterator_traits<RandomAccessIterator>::difference_type __distance(RandomAccessIterator first,
            RandomAccessIterator last, random_access_iterator_tag) {
        return last - first;
    }

    //combined distance function
    template <typename InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type distance(InputIterator first, InputIterator last) {
        return __distance(first, last, typename iterator_traits<InputIterator>::iterator_category());
    }

    //c++11 functions
    template <typename ForwardIterator>
    void next(ForwardIterator &it, typename iterator_traits<ForwardIterator>::difference_type n = 1);

}

#endif
