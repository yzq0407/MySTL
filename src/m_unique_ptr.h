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
    template <typename _Tp1, typename _Tp2> class __compressed_pair;
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
        using element_type = _Tp;
        using deleter_type = _Dp;

    private:
        pointer _ptr;
        deleter_type _deleter;

    public:
        //Default construct, a unique_ptr with nothing
        constexpr unique_ptr() noexcept {}

        explicit unique_ptr(pointer _p) noexcept: _ptr(_p) {
        }
    };
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
                 bool = is_class_v<_Tp1> && is_empty_v<_Tp1>,
                 bool = is_class_v<_Tp2> && is_empty_v<_Tp2>>
        struct compressed_pair {
            //default set to be two types not empty
            using first_param_type = _Tp1;
            using second_param_type = _Tp2;

            using first_reference = add_lvalue_reference_t<_Tp1>;
            using second_reference = add_lvalue_reference_t<_Tp2>;

            using first_const_reference = const first_reference;
            using second_const_reference = const second_reference;

            first_param_type _first;
            second_param_type _second;

            compressed_pair() = default;

            compressed_pair(first_param_type _x = first_param_type(),
                    second_param_type _y = second_param_type()): _first(_x), _second(_y) {}

            /* explicit compressed_pair(first_param_type _x): _first(_x), _second() {} */

            /* explicit compressed_pair(second_param_type _y): _first(), _second(_y) {} */

            first_reference first() {
                return _first;
            }
            
            first_const_reference first() const {
                return _first;
            }

            second_reference second() {
                return _second;
            }

            second_const_reference second() const {
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

            using first_const_reference = const first_reference;
            using second_const_reference = const second_reference;

            second_param_type _second;

            compressed_pair() = default;

            explicit compressed_pair(second_param_type _y): _second(_y) {};

            constexpr first_reference first() {
                return static_cast<first_reference>(*this);
            }
            
            constexpr first_const_reference first() const {
                return static_cast<first_const_reference>(*this);
            }

            second_reference second() {
                return _second;
            }

            second_const_reference second() const {
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

            using first_const_reference = const first_reference;
            using second_const_reference = add_const_t<second_reference>;

            first_param_type _first;

            compressed_pair() = default;
            explicit compressed_pair(first_param_type _y): _first() {};

            first_reference first() {
                return _first;
            }

            first_const_reference first() const {
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

            using first_const_reference = const first_reference;
            using second_const_reference = const second_reference;
            
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
