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
            
            //reverse iterator, added c++11
            using reverse_iterator = my_stl::reverse_iterator<iterator>;
            using const_reverse_iterator = my_stl::reverse_iterator<const_iterator>;

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

            //clang implementations
            //link the node [first, last] in front of position
            void __link_nodes(__node_ptr __position, __node_ptr first, __node_ptr last) {
                __position -> prev -> next = first;
                first -> prev = __position -> prev;
                __position -> prev = last;
                last -> next = __position;
            }

            void __unlink_nodes(__node_ptr first, __node_ptr last) {
                //note we will unlink the nodes between [first, last]
                first -> prev -> next = last -> next;
                last -> next -> prev = first -> prev;
            }

            //helper function in sgi stl (clang use __link_node, __link_node_at_front)
            void __transfer(iterator position, iterator first, iterator last) {
                last.node_ptr -> prev -> next = position.node_ptr;
                first.node_ptr -> prev -> next = last.node_ptr;
                position.node_ptr -> prev -> next = first.node_ptr;
                auto tmp = position.node_ptr -> prev;
                position.node_ptr -> prev = last.node_ptr -> prev;
                last.node_ptr -> prev = first.node_ptr -> prev;
                first.node_ptr -> prev = tmp;
            }

            //insert dispatch declaration
            template <typename _Integer>
            iterator __insert_dispatch(const_iterator __position, _Integer i1, 
                    _Integer i2, __true_type);

            template<typename _InputIterator>
            iterator __insert_dispatch(const_iterator __position, _InputIterator first,
                    _InputIterator last, __false_type);

            //assign dispatch declaration
            template<typename _Integer>
            void __assign_dispatch(_Integer n, _Integer val, __true_type);

            template<typename _InputIterator>
            void __assign_dispatch(_InputIterator first, _InputIterator last, __false_type);

        public:
            
            //------------------------Constructors------------------------------
            list();
            
            explicit list(size_type n);

            explicit list(size_type, const value_type& val);

            //copy
            list (const list& x);
            //move
            list (list&& x) noexcept;

            list(std::initializer_list<value_type> il);

            template<typename InputIterator>
            list(InputIterator first, InputIterator last);

            //copy assign
            list& operator=(std::initializer_list<value_type> il) {
                assign(il);
                return *this;
            }

            list& operator=(const list& x);

            list& operator=(list&& x) noexcept;
            
            //------------------------Destructors-------------------------------
			~list() {
                if (__end) {
                    clear();
                //note we never initialize the value of __end -> val, so we never need to 
                //call the destructor of __end -> val
                    __destroy_node(__end);
                }
			}

            //--------------------------------begin and end----------------------------------//
            
            //begin, end, front, back, size, maxsize
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

            //--------------------------------rbegin and rend---------------------------------//
            reverse_iterator rbegin() noexcept {
                return reverse_iterator(end());
            }

            const_reverse_iterator rbegin() const noexcept {
                return const_reverse_iterator(end());
            }

            reverse_iterator rend() noexcept {
                return reverse_iterator(begin());
            }

            const_reverse_iterator rend() const noexcept {
                return const_reverse_iterator(begin());
            }

            //------------------------------cbegin and cend----------------------------------//
            const_iterator cbegin() const noexcept{
                return const_iterator(__end -> next);
            };

            const_iterator cend() const noexcept{
                return const_iterator(__end);
            }

            //------------------------------rcbegin and rcend---------------------------------//
            const_reverse_iterator crbegin() const noexcept {
                return const_reverse_iterator(cend());
            }

            const_reverse_iterator crend() const noexcept {
                return const_reverse_iterator(cbegin());
            }

            //------------------------------front and back------------------------------------//

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

            size_type max_size() const noexcept{
                //from LLVM
                return size_type(-1);
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
            //-------------------------Assign operation --------------------------------------
            //--------------------------------------------------------------------------------
            //range assign, we need to dispatch through whether the type parameter is a integral
            template<typename InputIterator>
            void assign(InputIterator first, InputIterator last);
            
            //fill assign
            void assign(size_type n, const value_type& val);

            //initializer assign
            void assign(std::initializer_list<value_type> il);
            
            //--------------------------------------------------------------------------------
            //-------------------------Insert operation --------------------------------------
            //
            //insert single value at a single position
            iterator insert(const_iterator __position, const value_type& value);

            //fill elements insert
            iterator insert(const_iterator __position, size_type n, const value_type& val);

            //range insert
            template <typename InputIterator>
            iterator insert(const_iterator __position, InputIterator first, InputIterator last);

            //move insert
            /* inline iterator insert(const_iterator __position, value_type&& val); */

            //initializer list
            iterator insert(const_iterator __position, std::initializer_list<value_type> il);
            //********************Insert operation end*****************************************

            //-------------------------------------------------------------------------------
            //------------------------Erase operation----------------------------------------
            //erase a single position
            iterator erase(const_iterator __position);

            //erase a range of values
            iterator erase(const_iterator __first, const_iterator __last);

            //empty and size
            bool empty() const {
                return __end -> next == __end;
            }

            size_type size() const noexcept{
                return distance(cbegin(), cend());
            }

            //push and pop
            void push_back(const value_type& value);

            void push_front(const value_type& value);

            void pop_back();
            
            void pop_front();

            //remove the same value
            void remove(const value_type& value);

            //unique will remove all the adjacent duplicate values 
            void unique();

            //-----------------------------------------------------------------------
            //-----------------------------Splice operation--------------------------
            //-----------------------------------------------------------------------
            //entire list
            void splice(const_iterator __position, list& __other) noexcept;
            
            void splice(const_iterator __position, list&& __other) noexcept;
            
            //single element
            void splice(const_iterator __position, list& __other, const_iterator __i);

            void splice(const_iterator __position, list&& __other, const_iterator __i);

            //element range
            void splice(const_iterator __position, list& __other, 
                    const_iterator __first, const_iterator __last);

            void splice(const_iterator __position, list&& __other, 
                    const_iterator __first, const_iterator __last);

            //-----------------------------------------------------------------------------
            //-------------------------Merge operation ------------------------------------
            //-----------------------------------------------------------------------------
            void merge(list& x);

            template<typename _Comp>
            void merge(list& x, _Comp comp);

            //-----------------------------------------------------------------------
            //-----------------------------Sort operation--------------------------
            //-----------------------------------------------------------------------
            void sort();

            template<typename _Comp>
            void sort(_Comp comp);
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
    list<_Tp, Alloc>::list(size_type n): list() {
        insert(cend(), n, _Tp());
    }
    
    template<typename _Tp, typename Alloc>
    list<_Tp, Alloc>::list(size_type n, const value_type& val): list() {
        insert(cend(), n, val);
    }

    //copy
    template<typename _Tp, typename Alloc>
    list<_Tp, Alloc>::list (const list& x):list() {
        insert(cend(), x.cbegin(), x.cend());
    }
    
    //move
    template<typename _Tp, typename Alloc>
    list<_Tp, Alloc>::list (list&& x) noexcept: __end(x.__end) {
        x.__end = nullptr;
    }
    
    //range
    template<typename _Tp, typename Alloc>
    template<typename InputIterator>
    list<_Tp, Alloc>::list(InputIterator first, InputIterator last): list() {
        insert(cend(), first, last);
    }

    //initializer list
    template<typename _Tp, typename Alloc>
    list<_Tp, Alloc>::list(std::initializer_list<value_type> il): list() {
        insert(cend(), il.begin(), il.end());
    }

    //--------------------------------------------------------------------------------
    //-------------------------Assign operation --------------------------------------
    //--------------------------------------------------------------------------------
    //range assign, we need to dispatch through whether the type parameter is a integral
    template<typename _Tp, typename Alloc>
    template<typename InputIterator>
    inline void list<_Tp, Alloc>::assign(InputIterator first, InputIterator last) {
        __assign_dispatch(first, last, typename _Is_integer<InputIterator>::integral());
    }

    //fill assign
    template<typename _Tp, typename Alloc>
    inline void list<_Tp, Alloc>::assign(size_type n, const value_type& val) {
        //clear and assign is a way to go, but clearly we can reuse the allocated space
        auto iter = begin();
        for (; n > 0 && iter != end(); ++iter, --n) {
            *iter = val;
        }
        if (n > 0) {
            //more to assign
            insert(cend(), n, val);
        }
        else {
            //clear the rest
            erase(iter, n);
        }
    }

    //initializer assign
    template<typename _Tp, typename Alloc>
    inline void list<_Tp, Alloc>::assign(std::initializer_list<value_type> il) {
        assign(il.begin(), il.end());
    }

    //dispatch
    template<typename _Tp, typename Alloc>
    template<typename _Integer>
    inline void list<_Tp, Alloc>::__assign_dispatch(_Integer n, _Integer val, __true_type) {
        assign((size_type) n, val);
    }

    //dispatch
    template<typename _Tp, typename Alloc>
    template<typename _InputIterator>
    inline void list<_Tp, Alloc>::__assign_dispatch(_InputIterator first, _InputIterator last, __false_type) {
        //reuse the allocated space
        auto iter = begin();
        for (; iter != end() && first != last; ++iter, ++first) {
            *iter = *first;
        }
        if (iter == end()) {
            insert(cend(), first, last);
        }
        else {
            erase(iter, cend());
        }
    }

    //-------------------------------------------------------------------------------
    //-------------------------Operators---------------------------------------------
    template<typename _Tp, typename Alloc>
    inline bool operator==(const list<_Tp, Alloc>& lhs, const list<_Tp, Alloc>& rhs) {
        auto _it1 = lhs.cbegin();
        auto _it2 = rhs.cbegin();
        auto _e1 = lhs.cend();
        auto _e2 = rhs.cend();
        for (; _it1 != _e1 && _it2 != _e2 && *_it1 == *_it2; ++_it1, ++_it2) {}
        return _it1 == _e1 && _it2 == _e2;
    }

    //copy assign
    template<typename _Tp, typename Alloc>
    inline list<_Tp, Alloc>& list<_Tp, Alloc>::operator=(const list& _x) {
        //make strong exception gurantee
        list temp(_x);
        swap(temp);
        return *this;
    }


    //move assign
    template<typename _Tp, typename Alloc>
    inline list<_Tp, Alloc>& list<_Tp, Alloc>::operator=(list&& _x) noexcept{
        if (this != &_x) {
            clear();
            splice(end(), _x);
        }
        return *this;
    }


    
    //--------------------------------------------------------------------------------
    //-------------------------Insert operation --------------------------------------
    //--------------------------------------------------------------------------------
    //implementations
    template<typename _Tp, typename Alloc>
    typename list<_Tp, Alloc>::iterator 
    inline list<_Tp, Alloc>::insert(list::const_iterator __position, const _Tp& value) {
        __node_ptr __a_node = __create_node(value);
        __a_node -> next = __position.node_ptr;
        __a_node -> prev = __position.node_ptr -> prev;
        __position.node_ptr -> prev -> next = __a_node;
        __position.node_ptr -> prev = __a_node;
        return iterator(__a_node);
    }

    //fill elements insert
    template<typename _Tp, typename Alloc>
    inline typename list<_Tp, Alloc>::iterator
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
    inline typename list<_Tp, Alloc>::iterator 
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
    inline typename list<_Tp, Alloc>::iterator 
    list<_Tp, Alloc>::insert(const_iterator __position, std::initializer_list<value_type> il) {
        return insert(__position, il.begin(), il.end());
    }


    //dispatch functions, to resolve the overloading conflict when calling from list<int>
    //.insert(it, int, int). the iterator range insert and fill insert will collide
    template<typename _Tp, typename Alloc>
    template<typename _Integer>
    inline typename list<_Tp, Alloc>::iterator
    list<_Tp, Alloc>::__insert_dispatch(const_iterator __position, _Integer _i1, _Integer _i2,
                __true_type) {
        //this is the fill insert
        return insert(__position, (size_type) _i1, _i2);
    }

    template<typename _Tp, typename Alloc>
    template<typename InputIterator>
    inline typename list<_Tp, Alloc>::iterator
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
    inline void list<_Tp, Alloc>::push_back(const _Tp& value) {
        insert(cend(), value);
    }

    template<typename _Tp, typename Alloc>
    inline void list<_Tp, Alloc>::push_front(const _Tp& value) {
        insert(cbegin(), value);
    }
    
    //erase implementations
    template<typename _Tp, typename Alloc>
    inline __list_iterator<_Tp> list<_Tp, Alloc>::erase(list::const_iterator __position) {
        iterator temp (__position.node_ptr -> next);
        __unlink_nodes(__position.node_ptr, __position.node_ptr);
        my_stl::destroy(&__position.node_ptr -> val);
        __destroy_node(__position.node_ptr);
        return temp;
    }
    
    //range based erase
    template<typename _Tp, typename Alloc>
    inline __list_iterator<_Tp> list<_Tp, Alloc>::erase(const_iterator __first, const_iterator __last) {
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
    inline void list<_Tp, Alloc>::pop_back() {
        erase(--cend());
    }

    template<typename _Tp, typename Alloc>
    inline void list<_Tp, Alloc>::pop_front() {
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

    //--------------------------------------------------------------------------------
    //-------------------------Splice operation --------------------------------------
    //--------------------------------------------------------------------------------
    //LLVM implementation is preferred
    //entire list
    template<typename _Tp, typename Alloc>
    void list<_Tp, Alloc>::splice(const_iterator __pos, list& __x) noexcept{
        if (!__x.empty()) {
            //unlink the old list
            __node_ptr first = __x.__end -> next;
            __node_ptr last = __x.__end -> prev;
            __unlink_nodes(first, last);
            //link the new list
            __link_nodes(__pos.node_ptr, first, last);
        }
    }

    template<typename _Tp, typename Alloc>
    void list<_Tp, Alloc>::splice(const_iterator __pos, list&& __x) noexcept{
        //just a delegate method
        splice(__pos, __x);
    }

    //single element
    template<typename _Tp, typename Alloc>
    void list<_Tp, Alloc>::splice(const_iterator __pos, list& __x, const_iterator __i) {
        //note the edge case, if __pos == __i, the unlinked nodes will never be linked back
        if (__pos != __i && __pos.node_ptr != __i.node_ptr -> next) {
            __node_ptr shift_node = __i.node_ptr;
            __unlink_nodes(shift_node, shift_node);
            __link_nodes(__pos.node_ptr, shift_node, shift_node);
        }
    }

    template<typename _Tp, typename Alloc>
    void list<_Tp, Alloc>::splice(const_iterator __pos, list&& __x, const_iterator __i){
        //just a delegate method
        splice(__pos, __x, __i);
    }

    //range of elements
    template<typename _Tp, typename Alloc>
    void list<_Tp, Alloc>::splice(const_iterator __pos, list& __x, const_iterator __first,
            const_iterator __last) {
        if (__first != __last) {
            __node_ptr start = __first.node_ptr;
            __node_ptr end = __last.node_ptr -> prev;
            __unlink_nodes(start, end);
            __link_nodes(__pos.node_ptr, start, end);
        }
    }

    template<typename _Tp, typename Alloc>
    void list<_Tp, Alloc>::splice(const_iterator __pos, list&& __x, const_iterator __first,
            const_iterator __last) {
        //just a delegate method
        splice(__pos, __x, __first, __last);
    }
    
    //-----------------------------------------------------------------------
    //-----------------------------Merge operation--------------------------
    //-----------------------------------------------------------------------
    //merge, assume the two lists have been sorted already
    template<typename _Tp, typename Alloc>
    void list<_Tp, Alloc>::merge(list& _x) {

    }

    template<typename _Tp, typename Alloc>
    template<typename _Comp>
    void list<_Tp, Alloc>::merge(list& _x, _Comp comp) {
        //do not merge with ourself
        //llvm implementation is referred
        if (this != &_x) {
            __node_ptr _f1 = begin().node_ptr;
            const __node_ptr _l1 = end().node_ptr;
            __node_ptr _f2 = _x.begin().node_ptr;
            const __node_ptr _l2 = _x.end().node_ptr;
            for (;_f1 != _l1 && _f2 != _l2;) {
                if (comp(_f2 -> val, _f1 -> val)) {
                    //note that f2 is less than f1, we have to insert f2 into before f1
                    //if we unlink f2 and insert into f1, we might need to repeatedly set
                    //a lot of pointers, think about the case when all the subsequent nodes in
                    //f2 is less than f1
                    //What we want to do is unlink as many nodes as possible at once and link it
                    __node_ptr _temp = _f2 -> next;
                    for (; _temp != _l2 && comp(_temp -> val, _f1 -> val); _temp = _temp -> next);
                    __node_ptr _e2 = _temp -> prev;
                    __unlink_nodes(_f2, _e2);
                    __link_nodes(_f1, _f2, _e2);
                    _f2 = _temp;
                }
                _f1 = _f1 -> next;
            }
            splice (const_iterator(_l1), _x);
        }
        
    }

    //-----------------------------------------------------------------------
    //-----------------------------Sort operation--------------------------
    //-----------------------------------------------------------------------
    template<typename _Tp, typename Alloc>
    void list<_Tp, Alloc>::sort() {
        //if size less than or equal 1, return
        if (__end -> next -> next == __end) return;
    }
}

#endif
