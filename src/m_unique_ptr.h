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

        template <typename _Tp1, typename _Tp2>
        struct compressed_pair_base {
            using type1 = _Tp1;
            using type2 = _Tp2;

            using reference1 = add_lvalue_reference_t<_Tp1>;
            using reference2 = add_lvalue_reference_t<_Tp2>;

            using const_reference1 = const reference1;
            using const_reference2 = const reference2;
        };

        template <typename _Tp1, typename _Tp2, 
                 bool = is_same_v<remove_cv_t<_Tp1>, remove_cv_t<_Tp2>>,
                 bool = is_class_v<_Tp1> && is_empty_v<_Tp1>,
                 bool = is_class_v<_Tp1> && is_empty_v<_Tp2>>
        struct compressed_pair {
            //default set to be two types not empty
            using first_type = _Tp1;
            using second_type = _Tp2;

            using first_param_type = remove_reference_t<_Tp1>;
            using second_param_type = remove_reference_t<_Tp2>;

            using first_reference = add_lvalue_reference_t<_Tp1>;
            using second_reference = add_lvalue_reference_t<_Tp2>;

            using first_const_reference = const first_reference;
            using second_const_reference = const second_reference;

            first_type _first;
            second_type _second;

            compressed_pair() = default;

            compressed_pair(first_param_type _x, second_param_type _y): _first(_x), _second(_y) {}

            explicit compressed_pair(first_param_type _x): _first(_x), _second() {}

            explicit compressed_pair(second_param_type _y): _first(), _second(_y) {}

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
        template <typename _Tp1, typename _Tp2, bool isSame>
        struct compressed_pair<_Tp1, _Tp2, isSame, true, false>
            :compressed_pair_base<_Tp1, _Tp2>{};

        template <typename _Tp1, typename _Tp2, bool isSame>
        struct compressed_pair<_Tp1, _Tp2, isSame, false, true>
            :compressed_pair_base<_Tp1, _Tp2>{};

        //specialization for two different empty class
        template <typename _Tp1, typename _Tp2>
        struct compressed_pair<_Tp1, _Tp2, false, true, true>
            :compressed_pair_base<_Tp1, _Tp2>{};

        //do not forget the case of two same empty class
        template <typename _Tp1, typename _Tp2>
        struct compressed_pair<_Tp1, _Tp2, true, true, true>
            :compressed_pair_base<_Tp1, _Tp2> {};

    }// __compressed_pair_imp


    //--------------------------end of compressed_pair----------------------------------
} //my_stl
#endif
