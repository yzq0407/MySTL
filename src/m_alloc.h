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
            inline size_type max_size() const {
                return UINT_MAX / sizeof (_Tp);
            }

            //construct x for the pointer, use placement new
            inline void construct(pointer p, const_reference x){new (p) _Tp(x);};

            //destroy this object
            inline void destroy(pointer p) {p -> ~_Tp();};
    };

    //
    //--------------------------first level allocator: malloc_alloc---------------------------
    //modified 11/23/2016, add make it into a template such that the initialization of static 
    //variable wouldn't cause compiler error
    template <int inst>
    class __malloc_alloc {
        private:
            //these are the handlers for out of memory
            inline static void *oom_malloc(size_t);
            inline static void *oom_realloc(void *, size_t);
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
    template <int inst>
    void (*__malloc_alloc<inst>::__malloc_alloc_oom_handler) () = nullptr;

    //implementation of all the handlers
    template <int inst>
    void *__malloc_alloc<inst>::oom_malloc(size_t size) {
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

    template <int inst>
    void *__malloc_alloc<inst>::oom_realloc(void * p, size_t n) {
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
    // the second level allocator uses memory pool to allocate memory, this allocator
    // will keep a memory pool using a array of linkedlist contains memory chunk size from 
    // 8 bytes to 128 bytes, we will round up if the request memory is in between. Whenever a 
    // allocate request come in, it find the right list index and return it. When a deallocate
    // request come in, it will recycle the memory by putting it back to the memory pool
    //-----------------------------second level allocator----------------------------
    //modified it to be a template such that there is no dual initialization for static
    //members of memory pool
    template <int inst>
    class __default_alloc {
        private:
            static constexpr size_t __MAX_BYTES = 128;
            static constexpr size_t __ALIGN = 8;
            static constexpr size_t __NFREELISTS = __MAX_BYTES / __ALIGN;
            //default 20 chunks of memory at max each time
            static constexpr int __N_OBJS = 20;
            //we have a guarantee that char is gonna occupy 1 byte of memory, the union will take 4 or 8 (depend on platform)
            //bytes of memory
            
            //the start and end of memory pool, only change in chunk_alloc
            static char *start_free;
            static char *end_free;
            static size_t heap_size;

            union obj{
                obj* free_list_next;
                char data;
            };
            //a free list array of free memories
            static obj* volatile free_list[__NFREELISTS];
            
            //if the size if not the multiple of 8, we want to round up
            static size_t round_up(size_t n) {
                return (n + __ALIGN - 1) & ~(__ALIGN - 1);
            }

            //for a size below 128, get the index in the free list
            static size_t get_list_index(size_t n) {
                return (n + __ALIGN - 1)/ __ALIGN - 1;
            }


            //if the memory is not available, refill it
            //refill and chunk_alloc is implemented as state of art (also the core of the memory pool)
            //the job of refill is to maintain a array of linkedlist, while each of node is a chunk of memory
            //if by any chance that there is no memory chunck in the list to return, it will ask memory from memory pool


            static char *refill(size_t n) {
                //memory pool is defined to allocate 20 chunks of the given size at a time
                int n_objs = __N_OBJS;
                //n_objs is passed by reference, so that we get the exact number of chunks being allocated
                char *chunk = chunk_alloc(n, n_objs);
                //there are total n_objs chunks being allocated, so there are n_obj * n chars 
                if (n_objs == 1)    return chunk;
                char *ret = chunk;
                obj* volatile *my_free_list = free_list + get_list_index(n);
                obj* current_obj = *my_free_list = (obj *) (chunk + n);
                for (int i = 1; ;++i) {
                    obj* next_obj = (obj *)((char *)current_obj + n);
                    if (i == n_objs - 1) {
                        current_obj -> free_list_next = nullptr;
                        break;
                    }
                    current_obj = current_obj -> free_list_next = next_obj;
                }
                return ret;
            }

            //add n_objs chunk with n bytes into the list
            //memory pool is maintained by this funciton, while use start_free and end_free to represent its start and end
            // assume n is already a multiple of 8, we want to allocate n_objs * n bytes of memory int
            static char *chunk_alloc(size_t n, int& n_objs) {
                char *result;
                int total_bytes = n * n_objs;
                int bytes_left = end_free - start_free;
                //if there is already enough memory in the pool, we'll just return that
                if (bytes_left >= total_bytes) {
                    result = start_free;
                    start_free += total_bytes;
                    return result;
                }
                //if there are more than one chunk left, we still return that
                else if (bytes_left >= n) {
                    n_objs = bytes_left / n;
                    result = start_free;
                    start_free += (n_objs * n);
                    return result;
                }
                //literally no memory left to allocated to even one chunk
                else {
                    //we need to ask for more memory from heap to fill the memory, theoretically, it should grow up exponentially
                    //
                    int bytes_request = (total_bytes << 1) + round_up(heap_size >> 4);
                    start_free = (char *)malloc(bytes_request);
                    //if there is no memory enough for us to fill
                    if (!start_free) {
                        //start from the chunk size that is one large than the n, see if we can use that
                        //and find one to adjust into the memory pool
                        //we should never consider the chunk size less than n, cuz that will create disaster in multithread scenario
                        for (size_t chunk_size = n + __ALIGN; chunk_size <= __MAX_BYTES; chunk_size += __ALIGN) {
                            obj *volatile *my_free_list = free_list + get_list_index(chunk_size);
                            obj *chunk = *my_free_list;
                            if (chunk) {
                                *my_free_list = chunk -> free_list_next;
                                start_free = (char *) chunk;
                                end_free = start_free + chunk_size;
                                //recursively call chunk_alloc to allocate more memory
                                return chunk_alloc(n, n_objs);
                            }
                        }
                        //now we have real trouble, there is no memory left whatsoever in the pool, should call the malloc_alloc to
                        //trigger the handler and hopely that should return a valid memory address
                        start_free = (char *)__malloc_alloc<inst>::allocate(total_bytes);
                    }
                    //do not quite understand this mechanism, need to be tested
                    heap_size += bytes_request;
                    end_free = start_free + bytes_request;
                    //now we should have enough memory, recursively call to allocate memory
                    return chunk_alloc(n, n_objs);
                }
            }

        public:
            //allocate memory for given size
            static void *allocate(size_t n) {
                //if it is greater than 128 bytes, we use malloc to allocate memory
                if (n > __MAX_BYTES) {
                    return __malloc_alloc<inst>::allocate(n);
                }
                obj* volatile * my_free_list = free_list + get_list_index(n);
                obj* volatile res = *my_free_list;
                if (!res) {
                    return refill(round_up(n));
                }
                *my_free_list = res -> free_list_next;
                return res;
            }

            //deallocate memory for given size and address
            static void deallocate(void *p, size_t n) {
                if (n > __MAX_BYTES) {
                    __malloc_alloc<inst>::deallocate(p, n);
                    return;
                }
                obj* volatile *my_free_list = free_list + get_list_index(n);
                obj* volatile head = (obj *) p;
                head -> free_list_next = *my_free_list;
                *my_free_list = head;
            }
            static void *reallocate(void *p, size_t old_sz, size_t new_size);
    };

    template <int inst>
    char *__default_alloc<inst>::start_free = nullptr;

    template <int inst>
    char *__default_alloc<inst>::end_free = nullptr;

    template <int inst>
    size_t __default_alloc<inst>::heap_size = 0;

    //for 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120, 128
    template <int inst>
    typename __default_alloc<inst>::obj* volatile __default_alloc<inst>::free_list[__NFREELISTS] = 
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
    
    #ifdef __USE_MALLOC
        typedef malloc_alloc<0> alloc;
    #else
        typedef __default_alloc<0> alloc;
    #endif

    //testing the naive allocator we can see that it is not doing very well, in SGI implementation
    //the allocator is divided into two levels: first level is using malloc and free to allocate 
    //and manage memory, the second level(sub-allocator) is using complex memory pool
    //--------------------------simple allocator, generalized class--------------------------
    template <typename _Tp, typename Alloc = alloc>
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
   
}
#endif
