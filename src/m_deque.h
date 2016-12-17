#ifndef __MY_STL_DEQUE_H
#define __MY_STL_DEQUE_H


#include "m_memory.h"   //for allocator
#include "m_algobase.h"     
#include "m_iterator.h"  //for iterator traits and reverse iterator

namespace my_stl {
    //iterator design
    template<typename _Tp>
    struct __deque_iterator {
        //required by standard
        using iterator_category = random_access_iterator_tag;
        using value_type = _Tp;
        using pointer = _Tp*;
        using reference = _Tp&;
        using difference_type = ptrdiff_t;
        using size_type = size_t;
    };

    //const iterator design
    template<typename _Tp>
    struct __deque_const_iterator {
        //required by standard
        using iterator_category = random_access_iterator_tag;
        using value_type = _Tp;
        using pointer = const _Tp*;
        using reference = const _Tp&;
        using difference_type = ptrdiff_t;
        using size_type = size_t;
    };


    template<typename _Tp, typename Alloc = alloc> 
    class deque {
        public:
            using value_type = _Tp;
            using size_type = size_t;
            using difference_type = ptrdiff_t;
            using pointer = _Tp*;
            using const_pointer = const _Tp*;
            using reference = _Tp&;
            using const_reference = const _Tp&;
            using iterator = __deque_iterator<_Tp>;
            using const_iterator = __deque_const_iterator<_Tp>;

            using reverse_iterator = my_stl::reverse_iterator<iterator>;
            using const_reverse_iterator = my_stl::reverse_iterator<const_iterator>;
        //public interface
        public:
            //default
            explicit deque();

            //fill
            explicit deque(size_type _n, const value_type& val = value_type());

            //range
            template<typename InputIterator>
            explicit deque(InputIterator first, InputIterator last);

            //copy
            deque(const deque& __other);

            //dtor
            ~deque();

            //--------------------------------------------------------------------------
            //------------------------------Assign--------------------------------------
            //--------------------------------------------------------------------------
            template<typename InputIterator>
            void assign(InputIterator first, InputIterator last);  //range

            void assign(size_type n, const value_type& val);  //fill

            void assign(std::initializer_list<value_type> il); //initializer list
            //**************************************************************************

            //--------------------------------------------------------------------------
            //--------------------------Operator Overload-------------------------------
            //--------------------------------------------------------------------------
            reference operator[] (size_type n);
            const_reference operator[] (size_type n) const;
            //**************************************************************************


            //--------------------------------------------------------------------------
            //--------------------------Iterators---------------------------------------
            //--------------------------------------------------------------------------
            iterator begin() noexcept;
            const_iterator begin() const noexcept;
            
            iterator end() noexcept;
            const_iterator end() const noexcept;
            
            reverse_iterator rbegin() noexcept;
            const_reverse_iterator rbegin() const noexcept;

            reverse_iterator rend() noexcept;
            const_reverse_iterator rend() const noexcept;

            const_iterator cbegin() const noexcept;
            const_iterator crend() const noexcept;
            //**************************************************************************


            //--------------------------------------------------------------------------
            //------------------------element operation---------------------------------
            //--------------------------------------------------------------------------
            reference at(size_type n);
            const_reference at(size_type n) const;

            reference front();
            const_reference front() const;

            reference back();
            const_reference back() const;
            //**************************************************************************
            
            //--------------------------------------------------------------------------
            //------------------------------emplace--------------------------------------
            //--------------------------------------------------------------------------
            template <typename... Args>
            void emplace_front(Args&&... args);

            template <typename... Args>
            void emplace_back(Args&&... args); 

            template <typename... Args>
            void emplace(const_iterator position, Args&&... args); 
            //**************************************************************************

            //--------------------------------------------------------------------------
            //------------------------------insert--------------------------------------
            //--------------------------------------------------------------------------
            iterator insert(const_iterator position, const value_type& val);  //single element
            iterator insert(const_iterator position, size_type n, const value_type& val); //fill
            
            template <typename InputIterator>
            iterator insert(const_iterator position, InputIterator first, InputIterator last); //range

            iterator insert(const_iterator position, value_type&& val);  //move
            iterator insert(const_iterator position, std::initializer_list<value_type> il);   //initializer list
            //**************************************************************************
            

            //--------------------------------------------------------------------------
            //------------------------------erase--------------------------------------
            //--------------------------------------------------------------------------
            iterator erase(const_iterator position);
            iterator erase(const_iterator first, const_iterator last);
            //**************************************************************************



            //--------------------------------------------------------------------------
            //-------------------------------pop----------------------------------------
            //--------------------------------------------------------------------------
            void pop_back();
            void pop_front();
            //**************************************************************************


            //--------------------------------------------------------------------------
            //------------------------------push--------------------------------------
            //--------------------------------------------------------------------------
            void push_back(const value_type& val);
            void push_back(const value_type&& val);
            void push_front(const value_type& val);
            void push_front(const value_type&& val);
            //**************************************************************************


            //--------------------------------------------------------------------------
            //------------------------------swap----------------------------------------
            //--------------------------------------------------------------------------
            void swap(deque& x);
            //**************************************************************************

            //--------------------------------------------------------------------------
            //------------------------------Capacity------------------------------------
            //--------------------------------------------------------------------------
            size_type size() const noexcept;
            size_type max_size() const noexcept;
            bool empty() const noexcept;

            void resize(size_type n);
            void resize(size_type n, const value_type& val);

            void shrink_to_fit();
            //**************************************************************************
    };
}

#endif
