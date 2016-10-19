//this is a simple implementation of the C++ STL allocator class
//Created by Theodore Yang on 10/17/2016
#ifndef __MY_STL_ALLOC_H
#define __MY_STL_ALLOC_H

#include <cstddef>
#include <new>   //for placement new
#include <cstdlib> //for exit
#include <iostream> //for cerr
#include <climits>  //for UINT_MAX

namespace my_stl {
    class __malloc_alloc;

    //this is a naive implementation of the allocator function
    //just wrap the new and delete under the allocate and deallocate method
    //which is quite similar to the simple_alloc class in SGI allocator
    template <class _Tp>
    class naive_allocator {
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
                typedef naive_allocator<_Tp1> other;
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
                    exit(1);
                }
                return attempt_alloc;
            }

            //return all the memory allocated, n has to be the same as we run allocate
            inline void deallocate(pointer p, size_type n) {
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



    //testing the naive allocator we can see that it is not doing very well, in SGI implementation
    //the allocator is divided into two level: first level is using malloc and free to allocate 
    //and manage memory, the second level(sub-allocator) is using complex memory pool
    //--------------------------simple allocator, generalized class--------------------------
    template <typename _Tp, typename Alloc = __malloc_alloc>
    class my_simple_alloc {
        //this allocator provide the STL standrad interface of allocator, in the SGI implementation
        //simple_alloc is used to combine the first level and second level allocator
        //implemented 10/18 using only __malloc_alloc to test the performace
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
            template <class _Tp1> 
            struct rebind{
                typedef my_simple_alloc<_Tp1> other;
            };
            /*                 //generic copy constructor */
            /*                 template<class _Tp1> allocator(const allocator<_Tp1>&); */

            //get back the address of one item
            inline pointer address(reference tp) {return &tp;}

            //get back the const address of one item
            inline const pointer address(const_reference tp) const{return &tp;}

            //allocator, second argument is just to enhance locality
            inline pointer allocate(size_type n, const void* = 0) {
                return (pointer) Alloc::allocate(n * sizeof(_Tp));
            }

            //return all the memory allocated, n has to be the same as we run allocate
            inline void deallocate(pointer p, size_type n) {
                Alloc::deallocate(p, n * sizeof(_Tp));
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
    //
    //--------------------------first level allocator: malloc_alloc---------------------------
    class __malloc_alloc {
        private:
            //these are the handlers for out of memory
            static void *oom_malloc(size_t);
            static void *oom_realloc(void *, size_t);
            //passed in to handle oom 
            static void(* __malloc_alloc_oom_handler) ();

        public: 
            static void* allocate(size_t n) {
                void* res = malloc(n);
                if (!res) res = oom_malloc(n);
                return res;
            }

            static void deallocate(void *p, size_t n) {
                //use free directly to freeup memory
                free(p);
            }

            static void* reallocate(void *p, size_t old_size, size_t new_size) {
                void* res = realloc(p, new_size);
                if (!res)   res = oom_malloc(new_size);
                return res;
            }

            static void (*set_malloc_alloc_oom_handler(void (*handler)())) (){
                void (* old) () = __malloc_alloc_oom_handler;
                __malloc_alloc_oom_handler = handler;
                return old;
            }
    };
    
    //default is null, means there are no handler for oom
    void (*__malloc_alloc::__malloc_alloc_oom_handler) () = nullptr;

    //implementation of all the handlers
    void* __malloc_alloc::oom_malloc(size_t size) {
        //just keep on looping to can the handler 
        while (true) {
            if (!__malloc_alloc_oom_handler) {
                std::cerr << "out of memory" << std::endl;
                exit(1);
            }
            (*__malloc_alloc_oom_handler)();
            void* res = malloc(size);
            if (!res)   return res;
        }
    }

    void* __malloc_alloc::oom_realloc(void * p, size_t n) {
        while (true) {
            if (!__malloc_alloc_oom_handler) {
                std::cerr << "out of memory" << std::endl;
                exit(1);
            }
            (*__malloc_alloc_oom_handler)();
            void* res = realloc(p, n);
            if (!res)   return res;
        }
    }
}
#endif
