//this is a simple implementation of the C++ STL allocator class
//Created by Theodore Yang on 10/22/2016
#ifndef __MY_STL_VECTOR_H
#define __MY_STL_VECTOR_H

#include "m_memory.h"  //for allocator
#include <stdio.h>
    
namespace my_stl {
    
    template <typename _Tp, typename Alloc = __default_alloc>
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
            my_simple_alloc<_Tp, Alloc> data_allocator;
            
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
                uninitialized_fill (res, res + n, value);
                return start;
            }

            void deallocate() {
                if (!start) return;
                destroy(start, last);
                data_allocator.deallocate(start, end_of_storage - start);
            }
           
            //initialize all the data member field
            void fill_initialize (size_type n, const _Tp& value) {
                start = allocate_and_fill(n, value);
                end_of_storage = last = start + n;
            }

        public:
            inline iterator begin() {return start;}
            inline iterator end() {return last;}
            inline size_type size() const {return last - start;}
            inline size_type capacity() const {return end_of_storage - start;}
            inline bool empty() const {return start == last;}

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
            vector(): start(nullptr), last(nullptr), end_of_storage(nullptr){}

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
                //the semantics is that if the the vector is empty, do nothing
                if (start) {
                    destroy(--last);
                    if (start == last)  {
                        deallocate();
                        start = last = end_of_storage = nullptr;
                    }
                }
            }

            void resize (size_type new_size, const _Tp& x) {
                //if the current size is greater than count, the countainer is resuced to its 
                //first count elements
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
                    data_allocator.deallocate(start, end_of_storage - start);
                    start = last = end_of_storage = nullptr;
                }
            }
    };
}


#endif
