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
		using __node_ptr_type = __list_node<_Tp>*;

		//this is the actually node pointer
		__node_ptr_type node_ptr;
		explicit __list_iterator_base(__node_ptr_type __node_ptr) : node_ptr(__node_ptr) {}
		__list_iterator_base(): node_ptr(nullptr) {}

		bool operator==(const __list_iterator_base& __other) const {
			return node_ptr == __other.node_ptr;
		}

		bool operator!=(const __list_iterator_base& __other) const {
			return node_ptr != __other.node_ptr;
		}
    };
        

    //in this case, const_iterator would be __list_iterator<const _Tp> and normal is without const
    template <typename _Tp>
    struct __list_iterator: public __list_iterator_base<_Tp> {
        using pointer = _Tp*;
        using reference = _Tp&;
		using __iterator_base = __list_iterator_base<_Tp>;

        explicit __list_iterator(typename __iterator_base::__node_ptr_type __node_ptr): __iterator_base(__node_ptr){}
        __list_iterator(): __iterator_base(){};
        __list_iterator(const __list_iterator& other): __iterator_base(other.node_ptr){}

        reference operator*() {
            return this -> node_ptr -> val;
        }

        pointer operator->() {
            return &(this -> node_ptr -> val);
        }

        __list_iterator& operator++() {
            this -> node_ptr = this -> node_ptr -> next;
            return *this;
        }

        __list_iterator operator++(int) {
            __list_iterator _temp(this);
            this -> operator++();
            return _temp;
        }

        __list_iterator& operator--() {
            this -> node_ptr = this -> node_ptr -> prev;
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
		using __iterator_base = __list_iterator_base<_Tp>;

        explicit __list_const_iterator(typename __iterator_base::__node_ptr_type __node_ptr): __iterator_base(__node_ptr){}
        __list_const_iterator(): __iterator_base(){};
        __list_const_iterator(const __list_const_iterator& other): __iterator_base(other.node_ptr){}

        //implicit conversion from plain iterator
        __list_const_iterator(const __list_iterator<_Tp>& other): __iterator_base(other.node_ptr) {}

        reference operator*() const{
            return this -> node_ptr -> val;
        }

        pointer operator->() const{
            return &(this -> node_ptr -> val);
        }

        __list_const_iterator& operator++() {
            this -> node_ptr = this -> node_ptr -> next;
            return *this;
        }

        __list_const_iterator operator++(int) {
            __list_const_iterator _temp(this);
            this -> operator++();
            return _temp;
        }

        __list_const_iterator& operator--() {
            this -> node_ptr = this -> node_ptr -> prev;
            return *this;
        }

        __list_const_iterator operator--(int) {
            __list_const_iterator _temp(this);
            this -> operator--();
            return _temp;
        }
    };

    template <typename _Tp, typename Alloc = __malloc_alloc<0>>
    class list {
        public:
            //the following are required by stl standard;
            using iterator = __list_iterator<_Tp>;
            using const_iterator = __list_const_iterator<_Tp>;
            using size_type = std::size_t;
            using difference_type = std::ptrdiff_t;
            using value_type = _Tp;
            using pointer = _Tp*;
            using const_pointer = const _Tp*;
            using reference = _Tp&;
            using const_reference = const _Tp&;

        protected: 
            using __node = __list_node<_Tp>;
            using __node_ptr = __node*;
            

            //the list is a doubly linked list, we only need to keep a node as end
            __node_ptr __end;
            using data_allocator = my_simple_alloc<__node, Alloc>;

            __node* __get_node() const {
                return data_allocator::allocate(1);
            }

			void __destroy_node(__node *p) noexcept {
				data_allocator::deallocate(p, 1);
			}

			__node* __create_node(const value_type& val) const{
				__node* __a_node = __get_node();
				construct(&__a_node->val, val);
				return __a_node;
			}

            void __unlink_nodes(__node_ptr first, __node_ptr last) {
                //note we will unlink the nodes between [first, last]
                first -> prev -> next = last -> next;
                last -> next -> prev = first -> prev;
            }

            //insert dispatch declaration
            template <typename _Integer>
            iterator __insert_dispatch(const_iterator __position, _Integer i1, 
                    _Integer i2, __true_type);

            template<typename _InputIterator>
            iterator __insert_dispatch(const_iterator __position, _InputIterator first,
                    _InputIterator last, __false_type);

        public:
            
            //------------------------Constructors------------------------------
            list();

            list(std::initializer_list<value_type> il);

            template<typename InputIterator>
            list(InputIterator first, InputIterator last);
            
            //------------------------Destructors-------------------------------
			~list() {
				clear();
                //note we never initialize the value of __end -> val, so we never need to 
                //call the destructor of __end -> val
				__destroy_node(__end);
			}

            iterator begin() noexcept{
                return iterator(__end -> next);
            }

            const_iterator begin() const noexcept{
                return const_iterator(__end -> next);
            }

            iterator end() noexcept {
                return iterator(__end);
            }
            
            const_iterator end() const noexcept{
                return const_iterator(__end);
            }

            const_iterator cbegin() const noexcept{
                return const_iterator(__end -> next);
            };

            const_iterator cend() const noexcept{
                return const_iterator(__end);
            }

            reference front() {
                return *begin();
            }

            const_reference front() const{
                return *cbegin();
            }

            reference back() {
                return *(--end());
            }

            const_reference back() const {
                return *(--cend());
            }

            void swap(list& other) noexcept {
                __node_ptr temp = __end;
                this -> __end = other.__end;
                other.__end = temp;
            }

			void clear() noexcept {
				__node_ptr __ptr = __end->next;
				while (__ptr != __end) {
					__node_ptr temp = __ptr->next;
                    my_stl::destroy(&__ptr->val);
					__destroy_node(__ptr);
					__ptr = temp;
				}
                __end -> prev = __end -> next = __end;
			}
            
            //--------------------------------------------------------------------------------
            //-------------------------Insert operation --------------------------------------
            //
            //insert single value at a single position
            iterator insert(const_iterator __position, const value_type& value);

            //fill elements insert
            inline iterator insert(const_iterator __position, size_type n, const value_type& val);

            //range insert
            template <typename InputIterator>
            inline iterator insert(const_iterator __position, InputIterator first, InputIterator last);

            //move insert
            /* inline iterator insert(const_iterator __position, value_type&& val); */

            //initializer list
            iterator insert(const_iterator __position, std::initializer_list<value_type> il);
            //----------------------Insert operation end-------------------------------------

            //-------------------------------------------------------------------------------
            //------------------------Erase operation----------------------------------------
            //erase a single position
            iterator erase(const_iterator __position);

            //erase a range of values
            iterator erase(const_iterator __first, const_iterator __last);

            bool empty() const {
                return __end -> next == __end;
            }

            size_type size() const noexcept{
                return distance(cbegin(), cend());
            }

            inline void push_back(const value_type& value);

            inline void push_front(const value_type& value);

            inline void pop_back();
            
            inline void pop_front();

            //remove the same value
            inline void remove(const value_type& value);

            //unique will remove all the adjacent duplicate values 
            inline void unique();

    };

    //non member swap function, no throw
    template <typename _Tp, typename Alloc>
    void swap(list<_Tp, Alloc>& lhs, list<_Tp, Alloc>& rhs) noexcept{
        lhs.swap(rhs);
    }

    //------------------------------------------------------------------------
    //-----------------------------Constructors------------------------------
    //------------------------------------------------------------------------
    template<typename _Tp, typename Alloc>
    list<_Tp, Alloc>::list(): __end(__get_node()) {
        __end -> next = __end -> prev = __end;
    }

    template<typename _Tp, typename Alloc>
    template<typename InputIterator>
    list<_Tp, Alloc>::list(InputIterator first, InputIterator last): list() {
        insert(cend(), first, last);
    }

    template<typename _Tp, typename Alloc>
    list<_Tp, Alloc>::list(std::initializer_list<value_type> il): list() {
        insert(cend(), il.begin(), il.end());
    }

    //implementations
    template<typename _Tp, typename Alloc>
    typename list<_Tp, Alloc>::iterator 
    list<_Tp, Alloc>::insert(list::const_iterator __position, const _Tp& value) {
        __node_ptr __a_node = __create_node(value);
        __a_node -> next = __position.node_ptr;
        __a_node -> prev = __position.node_ptr -> prev;
        __position.node_ptr -> prev -> next = __a_node;
        __position.node_ptr -> prev = __a_node;
        return iterator(__a_node);
    }

    //fill elements insert
    template<typename _Tp, typename Alloc>
    typename list<_Tp, Alloc>::iterator
    list<_Tp, Alloc>::insert(const_iterator __position, size_type n, const value_type& val) {
        if (n == 0) return iterator(__position.node_ptr);
        for (; n > 0; --n, --__position) {
            insert(__position, val);
        }
        return iterator(__position.node_ptr -> prev);
    }

    //range insert
    template <typename _Tp, typename Alloc>
    template <typename InputIterator>
    typename list<_Tp, Alloc>::iterator 
    list<_Tp, Alloc>::insert(const_iterator __position, InputIterator first, InputIterator last) {
        return __insert_dispatch(__position, first, last, 
                typename _Is_integer<InputIterator>::integral());
    }

    //move insert
    /* template<typename _Tp, typename Alloc> */
    /* typename list<_Tp, Alloc>::iterator */
    /* list<_Tp, Alloc>::insert(list::const_iterator __position, _Tp&& val) { */
    /* } */

    //initializer list
    template<typename _Tp, typename Alloc>
    typename list<_Tp, Alloc>::iterator 
    list<_Tp, Alloc>::insert(const_iterator __position, std::initializer_list<value_type> il) {
        return insert(__position, il.begin(), il.end());
    }


    //dispatch functions, to resolve the overloading conflict when calling from list<int>
    //.insert(it, int, int). the iterator range insert and fill insert will collide
    template<typename _Tp, typename Alloc>
    template<typename _Integer>
    typename list<_Tp, Alloc>::iterator
    list<_Tp, Alloc>::__insert_dispatch(const_iterator __position, _Integer _i1, _Integer _i2,
                __true_type) {
        //this is the fill insert
        return insert(__position, (size_type) _i1, _i2);
    }

    template<typename _Tp, typename Alloc>
    template<typename InputIterator>
    typename list<_Tp, Alloc>::iterator
    list<_Tp, Alloc>::__insert_dispatch(const_iterator __position, InputIterator first,
            InputIterator last, __false_type) {
        //this is the range insert
        if (first == last)  return iterator(__position.node_ptr);
        for (; first != last; ++first) {
            insert(__position, *first);
        }
        return iterator(__position.node_ptr -> prev);
    }
    //--------------------------------end of insertion--------------------------------

    template<typename _Tp, typename Alloc>
    void list<_Tp, Alloc>::push_back(const _Tp& value) {
        insert(cend(), value);
    }

    template<typename _Tp, typename Alloc>
    void list<_Tp, Alloc>::push_front(const _Tp& value) {
        insert(cbegin(), value);
    }
    
    //erase implementations
    template<typename _Tp, typename Alloc>
    __list_iterator<_Tp> list<_Tp, Alloc>::erase(list::const_iterator __position) {
        iterator temp (__position.node_ptr -> next);
        __unlink_nodes(__position.node_ptr, __position.node_ptr);
        my_stl::destroy(&__position.node_ptr -> val);
        __destroy_node(__position.node_ptr);
        return temp;
    }
    
    //range based erase
    template<typename _Tp, typename Alloc>
    __list_iterator<_Tp> list<_Tp, Alloc>::erase(const_iterator __first, const_iterator __last) {
        if (__first != __last) {
            __unlink_nodes(__first.node_ptr, __last.node_ptr -> prev);
            while (__first != __last) {
                auto __node_ptr = __first.node_ptr;
                ++__first;
                my_stl::destroy (&__node_ptr -> val);
                __destroy_node(__node_ptr);
            }
        }
        return __list_iterator<_Tp>(__last.node_ptr);
    }

    template<typename _Tp, typename Alloc>
    void list<_Tp, Alloc>::pop_back() {
        erase(--cend());
    }

    template<typename _Tp, typename Alloc>
    void list<_Tp, Alloc>::pop_front() {
        erase(cbegin());
    }

    template<typename _Tp, typename Alloc>
    void list<_Tp, Alloc>::remove(const _Tp& value) {
        const_iterator head = begin();
        const_iterator tail = end();
        while (head != tail) {
            iterator next = head;
            ++next;
            if (*head == value)    erase(head);
            head = next;
        }
    }

    template<typename _Tp, typename Alloc>
    void list<_Tp, Alloc>::unique() {
        const_iterator head = begin();
        const_iterator tail = end();
        const_iterator next = head;
        while (++next != tail) {
            if (*head == *next) {
                erase(next);
            }
            else {
                head = next;
            }
            next = head;
        }
    }
}

#endif
