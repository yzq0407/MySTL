//Implement the basic unique pointer interfaces and pointer traits
//most of the code take reference from LLVM's libc++ implementation:
//http://llvm.org/svn/llvm-project/libcxx/trunk/include/memory
//
//Created by Theodore Yang on 12/17/2016
#ifndef __MY_STL_UNIQUE_PTR_H
#define __MY_STL_UNIQUE_PTR_H

#include "m_type_traits.h"
namespace my_stl {
    //-----------------------------------synopsis--------------------------------------
    template <typename _Tp> struct default_delete;
    template <typename _Tp> struct default_delete<_Tp[]>;
    template <typename _Tp, typename _Dp> class unique_ptr;
    template <typename _Tp1, typename _Tp2> struct compressed_pair;
    //-------------------------------end of synopsis-----------------------------------

    //--------------------------------default_delete-----------------------------------
    template <typename _Tp>
    struct default_delete {
        constexpr default_delete() noexcept = default;

        void operator()(_Tp* _ptr) const {
            delete _ptr;
        }
    };

    //partially speciallized form for array deletion
    template <typename _Tp>
    struct default_delete<_Tp[]> {
        constexpr default_delete() noexcept = default;

        void operator()(_Tp* _ptr) const {
            delete[] _ptr;
        }
    };
    //----------------------------end of default_delete--------------------------------

    //----------------------------unique_ptr------------------------------------------
    template <typename _Tp, typename _Dp = default_delete<_Tp>> 
    class unique_ptr {
    //the principle of unique_ptr is that we need to put the raw pointer and deleter into
    //a compressed pair, if the deleter is an empty class (most of the time it is), it will
    //only takes 8 bytes (64 platform), otherwise, it will be the composite size
    public:
        using pointer = _Tp*;
        using const_pointer = const _Tp*;
        using element_type = _Tp;
        using reference = _Tp&;
        using const_reference = const _Tp&;
        using deleter_type = _Dp;

    private:
        compressed_pair<pointer, deleter_type> __pair;

    public:
        //Default construct, a unique_ptr with nothing
        unique_ptr() noexcept;

        explicit unique_ptr(pointer _p) noexcept;
        
        unique_ptr(pointer _p, deleter_type _d) noexcept;

        unique_ptr(unique_ptr&& _ptr) noexcept;

        unique_ptr(std::nullptr_t) noexcept; 

        ~unique_ptr() {
            __pair.second()(__pair.first());
        }

        element_type& operator*() noexcept;

        const element_type& operator*() const noexcept;

        pointer operator->() noexcept;

        const_pointer operator->() const noexcept;

    };

    //unique_ptr---------------implementation
    template <typename _Tp, typename _Dp>
    unique_ptr<_Tp, _Dp>::unique_ptr() noexcept: __pair(pointer()) {
        //make sure the delete type is not a function pointer
        static_assert(!is_pointer_v<_Dp>, "cannot construct unique_ptr using function delete type");
    }

    template <typename _Tp, typename _Dp>
    unique_ptr<_Tp, _Dp>::unique_ptr(pointer _ptr) noexcept: __pair(_ptr) {
        //make sure the delete type is not a function pointer
        static_assert(!is_pointer_v<_Dp>, "cannot construct unique_ptr using function delete type");
    }

    template <typename _Tp, typename _Dp>
    unique_ptr<_Tp, _Dp>::unique_ptr(std::nullptr_t) noexcept: __pair(pointer()) {
        //make sure the delete type is not a function pointer
        static_assert(!is_pointer_v<_Dp>, "cannot construct unique_ptr using function delete type");
    }

    //------------------operator overloading-----------------------------
    template <typename _Tp, typename _Dp>
    _Tp& unique_ptr<_Tp, _Dp>::operator*() noexcept {
        return *__pair.first();
    }
    
    template <typename _Tp, typename _Dp>
    const _Tp& unique_ptr<_Tp, _Dp>::operator*() const noexcept {
        return *__pair.first();
    }

    template <typename _Tp, typename _Dp>
    _Tp* unique_ptr<_Tp, _Dp>::operator->() noexcept {
        return __pair.first();
    }

    template <typename _Tp, typename _Dp>
    const _Tp* unique_ptr<_Tp, _Dp>::operator->() const noexcept {
        return __pair.first();
    }
    //--------------------------end of unique_ptr---------------------------------------

    //--------------------------compressed_pair----------------------------------------
    namespace __compressed_pair_imp {
        //essentially, since we have two parameters, based on whether each one could be
        //empty or not, split into four different cases, if one class is empty, using inheritance
        //rather than composition
        //also, there is one more thing to worry, is that whether two empty class is same
        //if that is the case, we can not inherit from both of them, we have to force one to be composite
        //that gives us five branches (specialization in total)
        template <typename _Tp1, typename _Tp2, 
                 bool = is_class<_Tp1>::value && is_empty<_Tp1>::value,
                 bool = is_class<_Tp2>::value && is_empty<_Tp2>::value>
        struct compressed_pair {
            //default set to be two types not empty
            using first_param_type = _Tp1;
            using second_param_type = _Tp2;

            using first_reference = add_lvalue_reference_t<_Tp1>;
            using second_reference = add_lvalue_reference_t<_Tp2>;

            using first_const_reference = add_lvalue_reference_t<add_const_t<_Tp1>>;
            using second_const_reference = add_lvalue_reference_t<add_const_t<_Tp2>>;

            first_param_type _first;
            second_param_type _second;

            constexpr compressed_pair() = default;

            explicit constexpr compressed_pair(first_param_type _x = first_param_type(),
                    second_param_type _y = second_param_type()): _first(_x), _second(_y) {}

            /* explicit compressed_pair(first_param_type _x): _first(_x), _second() {} */

            /* explicit compressed_pair(second_param_type _y): _first(), _second(_y) {} */

            constexpr first_reference first() {
                return _first;
            }
            
            constexpr first_const_reference first() const {
                return _first;
            }

            constexpr second_reference second() {
                return _second;
            }

            constexpr second_const_reference second() const {
                return _second;
            }
        };
        
        //specialization for one empty class
        template <typename _Tp1, typename _Tp2>
        struct compressed_pair<_Tp1, _Tp2, true, false>: _Tp1{
            //the first param is an empty class, inherit from it
            using first_param_type = _Tp1;
            using second_param_type = _Tp2;

            using first_reference = add_lvalue_reference_t<_Tp1>;
            using second_reference = add_lvalue_reference_t<_Tp2>;

            using first_const_reference = add_lvalue_reference_t<add_const_t<_Tp1>>;
            using second_const_reference = add_lvalue_reference_t<add_const_t<_Tp2>>;

            second_param_type _second;

            constexpr compressed_pair() = default;

            explicit compressed_pair(second_param_type _y): _second(_y) {};

            constexpr first_reference first() {
                return static_cast<first_reference>(*this);
            }
            
            constexpr first_const_reference first() const {
                return static_cast<first_const_reference>(*this);
            }

            constexpr second_reference second() {
                return _second;
            }

            constexpr second_const_reference second() const {
                return _second;
            }
        };

        template <typename _Tp1, typename _Tp2>
        struct compressed_pair<_Tp1, _Tp2, false, true>: _Tp2{
            //the first param is an empty class, inherit from it
            using first_param_type = _Tp1;
            using second_param_type = _Tp2;

            using first_reference = add_lvalue_reference_t<_Tp1>;
            using second_reference = add_lvalue_reference_t<_Tp2>;

            using first_const_reference = add_lvalue_reference_t<add_const_t<_Tp1>>;
            using second_const_reference = add_lvalue_reference_t<add_const_t<_Tp2>>;

            first_param_type _first;

            constexpr compressed_pair() = default;
            explicit constexpr compressed_pair(first_param_type _x): _first(_x) {};

            constexpr first_reference first() {
                return _first;
            }

            constexpr first_const_reference first() const {
                return _first;
            }

            constexpr second_reference second() {
                return static_cast<second_reference>(*this);
            }

            constexpr second_const_reference second() const {
                return static_cast<second_const_reference>(*this);
            }
        };

        //one more case left, where two classes are both empty, this is the 
        //really tricky one, where there are several corner case:
        //1. these two classes are identical (we can not inherit from same class twice)
        //2. these two classes is base/derive classes (this create ambiguity when cast back)
        //The basic idea is to create a wraper class for each type and inherit the 
        //pair from both wraper classes, and this is the idea used in std::tuple (though it 
        //has to use recursive TMP to wrap all classes)
        //note that the boost::compressed_pair does not handle corner case 2 well
        
        template <typename _Tp, int _ins>
        struct pair_leaf: remove_cv_t<_Tp> {};

        template <typename _Tp1, typename _Tp2>
        struct compressed_pair<_Tp1, _Tp2, true, true>: 
            pair_leaf<_Tp1, 1>, pair_leaf<_Tp2, 2> {
            //specialization for two different empty class
            using first_param_type = _Tp1;
            using second_param_type = _Tp2;

            using first_reference = add_lvalue_reference_t<_Tp1>;
            using second_reference = add_lvalue_reference_t<_Tp2>;

            using first_const_reference = add_lvalue_reference_t<add_const_t<_Tp1>>;
            using second_const_reference = add_lvalue_reference_t<add_const_t<_Tp2>>;
            
            compressed_pair() {}

            constexpr first_reference first() {
                return static_cast<first_reference>(
                        static_cast<pair_leaf<_Tp1, 1>&>(*this));
            }
            
            constexpr first_const_reference first() const{
                return static_cast<first_const_reference>(
                        static_cast<pair_leaf<_Tp1, 1>&>(*this));
            }

            constexpr second_reference second() {
                return static_cast<second_reference>(
                        static_cast<pair_leaf<_Tp2, 2>&>(*this));
            }
            
            constexpr second_const_reference second() const{
                return static_cast<second_const_reference>(
                        static_cast<pair_leaf<_Tp2, 2>&>(*this));
            }
        };

    }// __compressed_pair_imp

    template <typename _Tp1, typename _Tp2>
    struct compressed_pair: 
        __compressed_pair_imp::compressed_pair<_Tp1, _Tp2> {
            using base =  __compressed_pair_imp::compressed_pair<_Tp1, _Tp2>;
            using base::base;
    };

    //--------------------------end of compressed_pair----------------------------------
} //my_stl
#endif
