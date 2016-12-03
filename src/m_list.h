//An implementation of the stl list 
//Created by Theodore on 11/29/2016
#ifndef __MY_STL_LIST_H
#define __MY_STL_LIST_H

#include "m_memory.h"         //for allocator
#include "m_iterator.h"       //for iterator type traits
#include <cstddef>            //for std::ptrdiff_t


namespace my_stl {
    template <typename _Tp, typename Alloc = alloc>
    class list {
        protected:
            //the list_node struct
            //the list is a doubly linked list under the hood
            //other than this, stl also has a slist class which support single linked list
            template <typename _Tp_Node>
            struct __list_node {
                __list_node<_Tp_Node> *prev;
                __list_node<_Tp_Node> *next;
                _Tp_Node val;
                __list_node(_Tp_Node _val):prev(nullptr), next(nullptr), val(_val) {}
            };

            //the iterator 
            template <typename Type, typename Ptr, typename Ref> 
            struct __list_iterator {
                //for stl iterator, the following is required for iterator traits
                typedef Type value_type;
                typedef Ptr pointer;
                typedef Ref reference;
                typedef std::ptrdiff_t difference_type;
                typedef std::size_t size_type;


                typedef __list_iterator<Type, Ptr, Ref> self_type;
                typedef __list_iterator<Type, Type*, Type&> iterator;
                typedef __list_iterator<Type, const Type*, const Type&> const_iterator;
                typedef __list_node<Type>* node_ptr;
                node_ptr node;

                //contructor
                __list_iterator(node_ptr _node): node(_node) {}
                __list_iterator(): node(nullptr) {}

                bool operator==(const self_type& rhs) const {
                    return node == rhs.node;
                }

                bool operator!=(const self_type& rhs) const{
                    return node != rhs.node;
                }

                pointer operator->() const {
                    return &(node -> val);
                }

                reference operator*() const {
                    return node -> val;
                }

                const self_type& operator++() const{
                    node = node -> next;
                    return *this;
                }

                const self_type operator++(int) const {
                    const self_type temp = &this;
                    this -> operator++();
                    return temp;
                }
            };

            typedef __list_node<_Tp> list_node;

        public:
            //nested type definition
            typedef _Tp value_type;
            typedef _Tp* pointer;
            typedef const _Tp* const_pointer;
            typedef __list_iterator<_Tp, _Tp*, _Tp&> iterator;
            typedef const __list_iterator<_Tp, const _Tp*, const _Tp&> const_iterator;
            typedef _Tp& reference;
            typedef const _Tp& const_reference;
            typedef std::size_t size_type;
            typedef std::ptrdiff_t difference_type;

            iterator node;
    };
}

#endif
