//this is a simple implementation of the C++ STL allocator class
//Created by Theodore Yang on 10/22/2016
#ifndef __MY_STL_VECTOR_H
#define __MY_STL_VECTOR_H

#include "m_memory.h"  //for allocator
#include "m_algobase.h"  //for copy function
//#include <stdio.h>

namespace my_stl {
    template <typename _Tp, typename Alloc = __malloc_alloc<0>>
    class vector {
        private:
            //three pointers pointed to the [start, last) and the end of the allocated memory
            // |-------elements-------| ######|
            // ^                      ^       ^
            // |                      |       |
            // start                  last    end of storage
            _Tp* start;
            _Tp* last;
            _Tp* end_of_storage;
            //allocator for memory management
            new_allocator<_Tp> data_allocator;
            
        public:
            //all the nested data type
            typedef _Tp value_type;
            typedef _Tp* pointer;
            typedef _Tp* iterator;
            typedef const _Tp* const_pointer;
            typedef const _Tp* const_iterator;
            typedef _Tp& reference;
            typedef const _Tp& const_reference;
            typedef size_t size_type;
            typedef ptrdiff_t difference_type;
            //followings are interfaces required by STL standard
        private:
            // helper function to allocate n object and initialize it by default value
            iterator allocate_and_fill(size_type n, const _Tp& value) {
                iterator res = data_allocator.allocate(n);
                my_stl::uninitialized_fill (res, res + n, value);
                return res;
            }

            void deallocate() noexcept {
                if (!start) return;
                destroy(start, last);
                data_allocator.deallocate(start, end_of_storage - start);
            }
           
            //initialize all the data member field
            void fill_initialize (size_type n, const _Tp& value) {
                start = allocate_and_fill(n, value);
                end_of_storage = last = start + n;
            }

            template <typename RandomAccessIterator>
            inline void __construct_from_iterator(RandomAccessIterator _first,
                    RandomAccessIterator _last, random_access_iterator_tag)
            {
                auto n = _last - _first;
                start = data_allocator.allocate(n);
                last = end_of_storage = start + n;
                for (iterator it = start;  n != 0; --n, ++it, ++_first) {
                    construct(it, *_first);
                }
            }

            template <typename InputIterator>
            inline void __construct_from_iterator(InputIterator _first, 
                    InputIterator _last, input_iterator_tag) {
                for (; _first != _last; ++_first) {
                    push_back(*_first);
                }
            }

        public:
            iterator begin() {return start;}
            iterator end() {return last;}
            const_iterator cbegin() const {return start;};
            const_iterator cend() const {return last;};
            size_type size() const {return last - start;}
            size_type capacity() const {return end_of_storage - start;}
            bool empty() const {return start == last;}

            reference operator[](size_type n) {
                //check boundary and return
                if (n >= size()) {
                    std::cerr << "out of vector boundary" << std::endl;
                    exit(1);
                }
                return *(start + n);
            }
            
            const_reference operator[](size_type n) const {
                if (n >= size()) {
                    std::cerr << "out of vector boundary" << std::endl;
                    exit(1);
                }
                return *(start + n);
            }

            //ctors
            vector() noexcept: start(nullptr), last(nullptr), end_of_storage(nullptr){}

            vector(size_type n, const _Tp& value) {
                fill_initialize(n, value);
            }

            vector(int n, const _Tp& value) {
                fill_initialize(n, value);
            }
            vector(long n, const _Tp& value) {
                fill_initialize(n, value);
            }

            explicit vector(size_type n) {
                //note that this will require the default ctor of the type
                fill_initialize(n, _Tp());
            }

            vector(const vector<_Tp>& rhs) {
                //copy constructor, need to allocate and initialize all the elements
                start = data_allocator.allocate(rhs.size());
                end_of_storage = last = my_stl::uninitialized_copy(rhs.start, rhs.last, start);
            }

            //c++11
            vector(std::initializer_list<_Tp> _il) {
                if (_il.size() > 0) {
                    start = data_allocator.allocate(_il.size());
                    end_of_storage = last = my_stl::uninitialized_copy(_il.begin(), _il.end(), start);
                }
            }

            vector<_Tp>& operator=(const vector<_Tp>& rhs) {
                //note this is the only exception safe implementation which also also
                //overloading with move assignment operator
                vector<_Tp> temp(rhs);
                swap(temp);
                return *this;
            }

            //move constructor
            vector(vector<_Tp>&& rhs) noexcept: start(rhs.start), last(rhs.last), end_of_storage(rhs.end_of_storage) {
                rhs.start = rhs.last = rhs.end_of_storage = nullptr;
            }

            //move assignment
            vector<_Tp>& operator=(vector<_Tp> &&rhs) noexcept {
                if (this != &rhs) {
                    deallocate();
                    start = rhs.start;
                    last = rhs.last;
                    end_of_storage = rhs.end_of_storage;
                    rhs.start = rhs.last = rhs.end_of_storage = nullptr;
                }
                return *this;
            }


            template<typename InputIterator>
            vector(InputIterator _first, InputIterator _last) {
                //construct vector based on the iterator type, if it is random iterator, we get the distance before iterate it
                __construct_from_iterator(_first, _last, typename iterator_traits<InputIterator>::iterator_category());
            }
                

            bool operator==(const vector<_Tp>& rhs) const {
                if (size() != rhs.size())   return false;
                for (auto _i1 = cbegin(), _i2 = rhs.cbegin(); _i1 != cend(); ++_i1, ++_i2) {
                    if (*_i1 != *_i2)   return false;
                }
                return true;
            }

            bool operator!=(const vector<_Tp>& rhs) const {
                return !operator==(rhs);
            }

            //swap function, should implement std::swap but let's keep it as it is 
            void swap(vector<_Tp> &rhs) noexcept{
                iterator start_temp = start;
                iterator last_temp = last;
                iterator end_temp = end_of_storage;
                start = rhs.start;
                last = rhs.last;
                end_of_storage = rhs.end_of_storage;
                rhs.start = start_temp;
                rhs.last = last_temp;
                rhs.end_of_storage = end_temp;
            }
                
            //dtor
            ~vector() {
                deallocate();
            }

            reference front() {
                return *start;
            }

            reference back() {
                return *(last - 1);
            }

            void push_back (const _Tp& x) {
                if (last != end_of_storage) {
                    construct(last++, x);
                }
                else if (start) {
                    //allocate twice the size
                    int temp= size();
                    iterator new_first = data_allocator.allocate(temp * 2);
                    iterator new_last = uninitialized_copy(start, last, new_first);
                    deallocate();
                    start = new_first;
                    last  = new_last;
                    end_of_storage = start + temp * 2;
                    construct(last++, x);
                }
                else {
                    //its empty vetor, allocate exact one element
                    start = data_allocator.allocate(1);
                    end_of_storage = last = start + 1;
                    construct(start, x);
                }
            }

            void pop_back() {
                destroy(--last);
            }

            void resize (size_type new_size, const _Tp& x) {
                //if the current size is greater than count, the countainer is resuced to its 
                // count elements
                //if the current size is less than count, additional elements are appended and 
                //initialized with copies of value
                if (size() > new_size) {
                    destroy(start + new_size, last);
                    last = start + new_size;
                }
                else if (size() < new_size) {
                    if (end_of_storage - start >= new_size) {
                        uninitialized_fill(last, start + new_size, x);
                        last = start + new_size;
                    }
                    else {
                        iterator new_start = data_allocator.allocate(new_size);
                        iterator new_end = uninitialized_copy(start, last, new_start);
                        uninitialized_fill(new_end, new_start + new_size, x);
                        deallocate();
                        start = new_start;
                        last = end_of_storage = start + new_size;
                    }
                }
            }

            void resize (size_type new_size) {
                //this will call the default 
                resize(new_size, _Tp());
            }

            void clear () {
                if (start) {
                    destroy(start, last);
                    last = start;
                }
            }

            //remove elements in the range [head, tail)
            iterator erase(iterator head, iterator tail) {
                //destroy the range first
                if (head >= tail)    return head;
                iterator new_last = copy(tail, last, head);
                destroy(new_last, last);
                last = new_last;
                return head;
            }

            //remove one single elements in the index
            iterator erase(iterator position) {
                if (position + 1 != end()) {
                    copy(position + 1, last, position);
                }
                --last;
                destroy(last);
                return position;
            }

            //the insert function
            iterator insert(const iterator pos, const _Tp& value);

            iterator insert(const iterator pos, size_type count, const _Tp& value);

            //the reserve function
            void reserve(size_type size) {
                if (capacity() < size) {
                    iterator new_start = data_allocator.allocate(size);
                    iterator new_last = uninitialized_copy(start, last, new_start);
                    deallocate();
                    start = new_start;
                    last = new_last;
                    end_of_storage = start + size;
                }
            }
    };


    template <typename _Tp, typename Alloc>
    typename vector<_Tp, Alloc>::iterator vector<_Tp, Alloc>::insert(const typename vector<_Tp, Alloc>::iterator pos,
            const _Tp& value){
        return insert(pos, 1, value);
    }

    template <typename _Tp, typename Alloc>
    typename vector<_Tp, Alloc>::iterator vector<_Tp, Alloc>::insert(
            const typename vector<_Tp, Alloc>::iterator pos,
            typename vector<_Tp, Alloc>::size_type count, const _Tp& value){
        if (count) {         //only insert if n is not 0
            if (end_of_storage - last >= count) {     //if there is enough space
                //if the end already passed the pos + n, we need to copy [end - n, end)
                //to [end, end + n)
                if (last - pos >= count) {
                    my_stl::uninitialized_copy(last - count, last, last);
                    //then copy [pos, end - n) to [pos + n, end)
                    //note we should use copy_backward, otherwise, there will be corrupted element
                    my_stl::copy_backward(pos, last - count, last);
                    //fill element in the range [pos, pos + n)
                    my_stl::fill(pos, pos + count, value); 
                }
                //in this case, end is less than pos + n
                else {
                    //copy [pos, last) into [last + n - (last - pos), last + n)
                    my_stl::uninitialized_copy(pos, last, count + pos);
                    //fill [post, last) with value
                    my_stl::fill(pos, last, value);
                    //initialize [last, pos + n) with value
                    my_stl::uninitialized_fill_n(last, pos + count - last, value);
                }
                last += count;
            }
            else {                  //there is not enough space need to allocate enough space
                //printf ("start inserting----alocate new memory\n");
                //we need to determine how much space is allocated 2 times or right amount
                const size_type old_size = size();
                //printf ("old size is %d\n", (int)old_size);
                const size_type new_size = old_size + (count > old_size? count: old_size);
                //printf ("new size is %d\n", (int)new_size);
                iterator new_first = data_allocator.allocate(new_size);
                //copy the first part
                iterator new_last = uninitialized_copy(start, pos, new_first);
                //fill the value
                new_last = uninitialized_fill_n(new_last, count, value);
                //copy the last part
                new_last = uninitialized_copy(pos, last, new_last);
                //deallocate the memmoty
                deallocate();
                start = new_first;
                last = new_last;
                end_of_storage = start + new_size;
            }
        }
        return start;
    }
}
#endif
