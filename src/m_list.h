//An implementation of the stl list 
//Created by Theodore on 11/29/2016
#ifndef __MY_STL_LIST_H
#define __MY_STL_LIST_H

#include "m_memory.h"         //for allocator
#include "m_iterator.h"       //for iterator type traits
#include <cstddef>            //for std::ptrdiff_t


namespace my_stl {

    //the list_node struct
    //the list is a doubly linked list under the hood
    //other than this, stl also has a slist class which support single linked list
    template <typename _Tp>
    struct __list_node {
        __list_node<_Tp> *prev;
        __list_node<_Tp> *next;
        _Tp val;
        explicit __list_node(_Tp _val): prev(nullptr), next(nullptr), val(_val){}
        __list_node(): prev(nullptr), next(nullptr) {};
    };


    //list iterator type, sgi used the following template declaration:
    //template<typename T, typename Ptr, typename Ref> class __list_iterator;
    //in order to differentiate const_iterator and iterator
    //const_iterator:     __list_iterator<T, const T*, const T&>
    //iterator:           __list_iterator<T, T*, T&>
    //iterator itsself is never const, so for operation like ++, --, it will always return the corresponding ref type
    //
    //
    //but apparently, having three type parameters is scary, here I define two types of iterator
    //const_iterator and iterator, which allow implicit conversion from second to the first

    //both const iterator and iterator will derive from the base
    template <typename _Tp>
    struct __list_iterator_base {
        using value_type = _Tp;
        using iterator_category = bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
    };
        

    //in this case, const_iterator would be __list_iterator<const _Tp> and normal is without const
    template <typename _Tp>
    struct __list_iterator: public __list_iterator_base<_Tp> {
        using pointer = _Tp*;
        using reference = _Tp&;
        
        using __node_ptr_type = __list_node<_Tp>*;

        __node_ptr_type node_ptr;

        explicit __list_iterator(__node_ptr_type __node_ptr): node_ptr(__node_ptr){}
        __list_iterator(): node_ptr(nullptr){};
        __list_iterator(const __list_iterator& other): node_ptr(other.node_ptr){}

        __list_iterator& operator=(const __list_iterator& rhs) {
            if (this != &rhs) {
                node_ptr = rhs.node_ptr;
            }
            return *this;
        }

        bool operator==(const __list_iterator& rhs) {
            return node_ptr == rhs.node_ptr;
        }
        

        bool operator!=(const __list_iterator& rhs) {
            return !operator=(rhs);
        }
        
        reference operator*() {
            return node_ptr -> val;
        }

        pointer operator->() {
            return &(node_ptr -> val);
        }

        __list_iterator& operator++() {
            node_ptr = node_ptr -> next;
            return *this;
        }

        __list_iterator operator++(int) {
            __list_iterator _temp(this);
            this -> operator++();
            return _temp;
        }


        __list_iterator& operator--() {
            node_ptr = node_ptr -> prev;
            return *this;
        }

        __list_iterator operator--(int) {
            __list_iterator _temp(this);
            this -> operator--();
            return _temp;
        }
    };


    template <typename _Tp>
    struct __list_const_iterator: public __list_iterator_base<_Tp>{
        using pointer = const _Tp*;
        using reference = const _Tp&;

        using __node_ptr_type = const __list_node<_Tp>*;
        
        __node_ptr_type node_ptr;

        explicit __list_const_iterator(__node_ptr_type __node_ptr): node_ptr(__node_ptr){};
         __list_const_iterator(): node_ptr(nullptr){};
        __list_const_iterator(const __list_const_iterator& other): node_ptr(other.node_ptr){}

        //implicit conversion from plain iterator
        __list_const_iterator(const __list_iterator<_Tp>& other): node_ptr(other.node_ptr) {}

        __list_const_iterator& operator=(const __list_const_iterator& rhs) {
            if (this != &rhs) {
                node_ptr = rhs.node_ptr;
            }
            return *this;
        }


        bool operator==(const __list_const_iterator& rhs) {
            return node_ptr == rhs.node_ptr;
        }
        

        bool operator!=(const __list_const_iterator& rhs) {
            return !operator==(rhs);
        }
        
        reference operator*() {
            return node_ptr -> val;
        }

        pointer operator->() {
            return &(node_ptr -> val);
        }

        __list_const_iterator& operator++() {
            node_ptr = node_ptr -> next;
            return *this;
        }

        __list_const_iterator operator++(int) {
            __list_const_iterator _temp(this);
            this -> operator++();
            return _temp;
        }


        __list_const_iterator& operator--() {
            node_ptr = node_ptr -> prev;
            return *this;
        }

        __list_const_iterator operator--(int) {
            __list_const_iterator _temp(this);
            this -> operator--();
            return _temp;
        }
    };



    template <typename _Tp, typename Alloc = alloc>
    class list {
        protected:
            //the following are required by stl standard;
            using size_type = std::size_t;
            using difference_type = std::ptrdiff_t;
            using value_type = _Tp;
            using pointer = _Tp*;
            using const_pointer = const _Tp*;
            using reference = _Tp&;
            using const_reference = const _Tp&;
            using iterator = __list_iterator<_Tp>;
            using const_iterator = __list_const_iterator<_Tp>;

            //these are node required by stl standard;
            using __node = __list_node<_Tp>;
            using __node_ptr = __node*;
            

            //the list is a doubly linked list, we only need to keep a node as end
            __node_ptr __end;
            using data_allocator = my_simple_alloc<__node, Alloc>;


            __node* __get_node(size_type n) {
                return data_allocator::allocate(n);
            }

        public:

            list(): __end(__get_node(1)) {
                __end -> next = __end -> prev = __end;
            }

            iterator begin() {
                return iterator(__end -> next);
            }

            const_iterator begin() const{
                return const_iterator(__end -> next);
            }

            iterator end() {
                return iterator(__end);
            }
            
            const_iterator end() const{
                return const_iterator(__end);
            }

            const_iterator cbegin() const{
                return const_iterator(__end -> next);
            };

            const_iterator cend() const {
                return const_iterator(__end);
            }

            bool empty() const {
                return __end -> next == __end;
            }

            size_type size() {
                return distance(cbegin(), cend());
            }

            void push_back(const value_type& value) {
                __node_ptr __tail = new __node(value);
                __tail -> next = __end;
                __tail -> prev = __end -> prev;
                __end -> prev -> next = __tail;
                __end -> prev = __tail;
            }
    };
}

#endif
