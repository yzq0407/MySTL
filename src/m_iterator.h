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


    //reverse iterator, added in C++11
    //note that reverse iterator is just the adapter for iterator
    //mostly copied from llvm implementation
    template <typename Iterator>
    class reverse_iterator: public iterator<typename iterator_traits<Iterator>::iterator_category,
                                            typename iterator_traits<Iterator>::value_type,
                                            typename iterator_traits<Iterator>::difference_type,
                                            typename iterator_traits<Iterator>::pointer,
                                            typename iterator_traits<Iterator>::reference>
    {
    protected:
        Iterator current;
    public:
        using iterator_type = Iterator;
        using difference_type = typename iterator_traits<Iterator>::difference_type;
        using reference = typename iterator_traits<Iterator>::reference;
        using pointer = typename iterator_traits<Iterator>::pointer;
        
        //interface, since it is added from c++11, we use constexpr function as much as possible
        //under c++ standard, this means all the iterator interface has to declared to be constexpr
        //this is no longer the case under c++14
        constexpr reverse_iterator():current() {}
        constexpr explicit reverse_iterator(Iterator iterator): current(iterator) {}
        
        //copy
        template <class U>
        constexpr reverse_iterator(const reverse_iterator<U>& other): current(other.current) {}

        template <class U>
        reverse_iterator& operator=(const reverse_iterator<U>& rhs) {
            current = rhs.current;
            return *this;
        }

        constexpr Iterator base() const{return current;}

        reference operator*() const {
            Iterator tmp = current;
            return *--tmp;
        }

        constexpr pointer operator->() const {
            return &operator*();
        }
        //pre increment
        reverse_iterator& operator++() {
            --current;
            return *this;
        }
        //post increment
        reverse_iterator operator++(int) {
            reverse_iterator tmp(current);
            --current;
            return tmp; 
        }
        //pre decrement
        reverse_iterator& operator--() {
            ++current;
            return *this;
        }
        //post decrement
        reverse_iterator operator--(int) {
            reverse_iterator tmp(current);
            ++current;
            return tmp;
        }


        //only for random access iterator
        constexpr reverse_iterator operator+ (difference_type n) const {
            return reverse_iterator(current - n);
        }

        constexpr reverse_iterator operator- (difference_type n) const {
            return reverse_iterator(current + n);
        }

        reverse_iterator& operator+=(difference_type n) const {
            current -= n;
            return *this;
        }

        reverse_iterator& operator-=(difference_type n) {
            current += n;
            return *this;
        }

        constexpr reference operator[](difference_type n) const {
            return *(this + n);
        }
    };

    //non member function for reverse iterator
    template <typename _Iter1, typename _Iter2>
    inline bool operator==(const reverse_iterator<_Iter1>& r_it1, const reverse_iterator<_Iter2>& r_it2) {
        return r_it1.base() == r_it2.base();
    }

    template <typename _Iter1, typename _Iter2>
    inline bool operator!=(const reverse_iterator<_Iter1>& r_it1, const reverse_iterator<_Iter2>& r_it2) {
        return r_it1.base() != r_it2.base();
    }

    template <typename _Iter1, typename _Iter2>
    inline bool operator>(const reverse_iterator<_Iter1>& r_it1, const reverse_iterator<_Iter2>& r_it2) {
        return r_it1.base() > r_it2.base();
    }

    template <typename _Iter1, typename _Iter2>
    inline bool operator>=(const reverse_iterator<_Iter1>& r_it1, const reverse_iterator<_Iter2>& r_it2) {
        return r_it1.base() >= r_it2.base();
    }


    template <typename _Iter1, typename _Iter2>
    inline bool operator<(const reverse_iterator<_Iter1>& r_it1, const reverse_iterator<_Iter2>& r_it2) {
        return r_it1.base() < r_it2.base();
    }
    
    //trailing return type deduction
    template <typename _Iter1, typename _Iter2>
    inline auto operator-(const reverse_iterator<_Iter1>& r_it1, const reverse_iterator<_Iter2>& r_it2)
    -> decltype(r_it1.base - r_it2.base()){
        return r_it1.base() - r_it2.base();
    }

    template <typename _Iter>
    inline reverse_iterator<_Iter> operator+(typename reverse_iterator<_Iter>::difference_type _n, const reverse_iterator<_Iter>& _r_it) {
        return _r_it + _n;
    }
    
    //this function is for C++14 and above, but added anyway
    template <typename _Iter>
    inline reverse_iterator<_Iter> make_reverse_iterator(_Iter _i) {
        return reverse_iterator<_Iter> (_i);
    }

}

#endif
