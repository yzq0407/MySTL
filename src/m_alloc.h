//this is a simple implementation of the C++ STL allocator class
//Created by Theodore Yang on 10/17/2016
#ifndef __MY_STL_ALLOC_H
#define __MY_STL_ALLOC_H

#include <cstddef>
#include <new>   //for placement new
#include <cstdlib> //for exit
#include <iostream> //for cerr

namespace my_stl {
    template <class _Tp>
    class allocator {
        //by STL standard, these are the necessary interface for allocator
        public:
            //typename can be accessed using scope operator, but when used, need to 
            //specifiy "typename"
            typedef _Tp value_type;
            typedef _Tp* pointer;
            typedef const _Tp* const_pointer;
            typedef _Tp& reference;
            typedef const _Tp& const_reference;
            typedef size_t size_type;
            typedef ptrdiff_t diference_type;

            //nested class template with only one data member
            template <class _Tp1> struct rebind{
                typedef allocator<_Tp1> other;
            };
            // auto generated 
            /*                 //constructor */
            /*                 allocator(); */

            /*                 //copy constructor */
            /*                 allocator(const allocator&); */

            /*                 //generic copy constructor */
            /*                 template<class _Tp1> allocator(const allocator<_Tp1>&); */

            /*                 //destructor */
            /*                 ~allocator(); */

            //get back the address of one item
            inline pointer address(reference tp) {return &tp;}

            //get back the const address of one item
            inline const pointer address(const_reference tp) const{return &tp;}

            //allocator, second argument is just to enhance locality
            inline pointer allocate(size_type n, const void* = 0) {
                //set no handler for allocation
                std::set_new_handler(nullptr);
                pointer attempt_alloc = (pointer) (::operator new (n * (sizeof(_Tp))));
                if (!attempt_alloc) {
                    std::cerr << "out of memory" << std::endl;
                }
                return attempt_alloc;
            }

            //return all the memory allocated, n has to be the same as we run allocate
            inline void deallocate(pointer p, size_type n) {
                //use placement delete
                ::operator delete(p);
            }

            //the max volumn
            size_type max_size() const {
                return UINT_MAX / sizeof (_Tp);
            }

            //construct x for the pointer, use placement new
            inline void construct(pointer p, const_reference x){new (p) _Tp(x);};

            //destroy this object
            inline void destroy(pointer p) {p -> ~_Tp();};
    };
}
#endif
