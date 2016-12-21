//type traits is not one of the library requirement for stl standard, but it used the same approach
//as the trait technique in the iterator to seperate objects with or without trivial ctor, dtor, 
//copy ctor, assignment oper, and whether it is a POD (plain old type)
//
#ifndef __MY_STL_TYPE_TRAITS_H
#define __MY_STL_TYPE_TRAITS_H

#include <cstddef> //for nullptr_t

namespace my_stl {
    //************************************************************
    //                      synopsis
    //************************************************************
    template<typename _Tp, _Tp _val> struct integral_constant;
    template<typename _Tp> struct is_void;
    template<typename _Tp> struct is_null_pointer;
    template<typename _Tp> struct is_integral;
    template<typename _Tp> struct is_floating_point;
    template<typename _Tp> struct is_array;
    template<typename _Tp> struct is_pointer;
    template<typename _Tp> struct is_lvalue_reference;
    template<typename _Tp> struct is_rvalue_reference;
    template<typename _Tp> struct is_member_object_pointer;
    template<typename _Tp> struct is_member_function_pointer;
    template<typename _Tp> struct is_enum;
    template<typename _Tp> struct is_union;
    template<typename _Tp> struct is_class;
    template<typename _Tp> struct is_function;

    //secondary classification traits
    template <class T> struct is_reference;
    template <class T> struct is_arithmetic;
    template <class T> struct is_fundamental;
    template <class T> struct is_member_pointer;
    template <class T> struct is_scalar;
    template <class T> struct is_object;
    template <class T> struct is_compound;

    //cv properties and transformation
    template <class T> struct is_const;
    template <class T> struct is_volatile;
    template <class T> struct remove_const;
    template <class T> struct remove_volatile;
    template <class T> struct remove_cv;
    template <class T> struct add_const;
    template <class T> struct add_volatile;
    template <class T> struct add_cv;

    //************************************************************
    //                      end of synopsis
    //************************************************************



    //integral constant, provide compile time constant
    template <typename _Tp, _Tp _val>
    struct integral_constant {
        using value_type = _Tp;
        using type = integral_constant<_Tp, _val>;

        static constexpr _Tp value = _val;

        constexpr operator _Tp() const noexcept {return _val;}
        constexpr _Tp operator()() const noexcept {return _val;}
    };

    //now we can typedef true type and false type to be integral type
    typedef integral_constant<bool, true> __true_type;
    typedef integral_constant<bool, false> __false_type;
    typedef integral_constant<bool, true> true_type;
    typedef integral_constant<bool, false> false_type;

    //a conditional evaluation struct comes really handy if we are evaluation 
    //sequence of boolean clause
    template <bool, typename _If, typename _Else>
    struct conditional {
        //this is the default true
        using type = _If;
    };

    template <typename _If, typename _Else>
    struct conditional<false, _If, _Else> {
        //partially specialize the false situation
        using type = _Else;
    };

    //c++14
    template <bool _B, typename _If, typename _Else>
    using conditional_t = typename conditional<_B, _If, _Else>::type;

    //----------is same type----------------
    template <typename, typename>
    struct is_same: false_type {};

    template <typename _Tp>
    struct is_same<_Tp, _Tp>: true_type {};


    //all the logical operation applied to true/false type
    //-------------logical or-------------------------
    template <typename...>
    struct __or__: false_type{};

    template <typename _B>
    struct __or__<_B>: _B {};

    template <typename _B1, typename _B2>
    struct __or__<_B1, _B2>: conditional_t<_B1::value, _B1, _B2> {};

    template <typename _B1, typename _B2, typename _B3, typename... _Bn>
    struct __or__<_B1, _B2, _B3, _Bn...>:
            conditional_t<_B1::value, _B1, __or__<_B2, _B3, _Bn...>> {};

    //--------------logical and------------------------
    template <typename...>
    struct __and__: true_type{};

    template <typename _B>
    struct __and__<_B>: _B {};

    template <typename _B1, typename _B2>
    struct __and__<_B1, _B2>: conditional_t<_B1::value, _B2, _B1> {};

    template <typename _B1, typename _B2, typename _B3, typename... _Bn>
    struct __and__<_B1, _B2, _B3, _Bn...>:
            conditional_t<_B1::value, __and__<_B2, _B3, _Bn...>, _B1> {};

    //primary classification traits:

    //-----------------is void---------------------------
    template <typename _Tp>
    struct is_void: is_same<void, typename remove_cv<_Tp>::type> {};

    //c++17
    template <typename _Tp>
    constexpr bool is_void_v = is_void<_Tp>::value;

    //-----------------is nullptr-------------------------
    template <typename _Tp>
    struct is_null_pointer: 
        is_same<std::nullptr_t, typename remove_cv<_Tp>::type>{};

    //c++17
    template <typename _Tp>
    constexpr bool is_null_pointer_v = is_null_pointer<_Tp>::value;

    //-----------------is union----------------------------
    //"is union" is a compiler specific implementation, here we do not assume
    //any of the union
    template <typename _Tp>
    struct is_union: false_type {};

    template <typename _Tp>
    constexpr bool is_union_v = is_union<_Tp>::value;

    //-----------------is class----------------------------
    //implemented using SFINAE idiom
    struct __is_class_aux_struc{char _member[2];};

    template <typename _Tp>
    char __is_class_aux_func (int _Tp::*);

    template <typename _Tp>
    __is_class_aux_struc __is_class_aux_func(...);

    template <typename _Tp>
    struct is_class: integral_constant<bool, 
        sizeof(__is_class_aux_func<_Tp>(0)) == 1> {};

    //c++17
    template <typename _Tp>
    constexpr bool is_class_v = is_class<_Tp>::value;

    //---------------------integer traits---------------------
    //note that I made a typo to be is_integer (it is supposed to be is_integral
    //but since it has been intensively used, I tend to let it stay as it is)
    template<typename _Tp>
    struct is_integer: false_type {};

    //specialized from
    template<>
    struct is_integer<bool>:true_type {};
    
    template<>
    struct is_integer<int>:true_type {};

    template<>
    struct is_integer<unsigned int>:true_type{};

    template<>
    struct is_integer<unsigned short>:true_type {};
    
    template<>
    struct is_integer<short>:true_type {};
    
    template<>
    struct is_integer<long>:true_type {};

    template<>
    struct is_integer<unsigned long>:true_type {};

    template<>
    struct is_integer<long long>:true_type {};
    
    template<>
    struct is_integer<unsigned long long>:true_type {};
    
    template<>
    struct is_integer<char>: true_type{};

    template<>
    struct is_integer<signed char>: true_type{};

    template<>
    struct is_integer<unsigned char>:true_type {};

    //c++17
    template<typename _Tp>
    constexpr bool is_intergral_v = is_integer<_Tp>::value;

    //----------------is floating points traits------------------
    template<typename _Tp>
    struct is_floating_point: false_type {};

    template<>
    struct is_floating_point<double>:true_type{};

    template<>
    struct is_floating_point<float>:true_type{};
    
    template<>
    struct is_floating_point<long double>:true_type{};

    //c++17
    template<typename _Tp>
    constexpr bool is_floating_point_v = is_floating_point<_Tp>::value;

    //-----------------is array--------------------------------
    template<typename _Tp>
    struct is_array: false_type{};

    template<typename _Tp, size_t _N>
    struct is_array<_Tp[_N]>: true_type{};
    
    template<typename _Tp>
    struct is_array<_Tp[]>: true_type{};

    //c++17
    template<typename _Tp>
    constexpr bool is_array_v = is_array<_Tp>::value;

    //----------------is pointer------------------------------
    template<typename _Tp>
    struct __is_pointer_aux: false_type{};

    template<typename _Tp>
    struct __is_pointer_aux<_Tp*>: true_type{};

    template<typename _Tp>
    struct is_pointer: __is_pointer_aux<typename remove_cv<_Tp>::type> {};

    template<typename _Tp>
    constexpr bool is_pointer_v = is_pointer<_Tp>::value;

    //--------------is lvalue/rvalue reference--------------------
    template<typename _Tp>
    struct is_lvalue_reference: false_type{};

    template<typename _Tp>
    struct is_lvalue_reference<_Tp&>: true_type{};

    template<typename _Tp>
    struct is_rvalue_reference: false_type{};

    template<typename _Tp>
    struct is_rvalue_reference<_Tp&&>: false_type{};

    //c++17
    template<typename _Tp>
    constexpr bool is_lvalue_reference_v = is_lvalue_reference<_Tp>::value;

    template<typename _Tp>
    constexpr bool is_rvalue_reference_v = is_rvalue_reference<_Tp>::value;

    //---------------is reference---------------------------------
    template<typename _Tp>
    struct is_reference: __or__<typename is_lvalue_reference<_Tp>::type,
        typename is_rvalue_reference<_Tp>::type> {};

    template<typename _Tp>
    constexpr bool is_reference_v = is_reference<_Tp>::value;


    //---------------is reference---------------------------------
    //copied from cppference.com website
    template<class>
    struct is_function: false_type { };

    // specialization for regular functions
    template<class Ret, class... Args>
    struct is_function<Ret(Args...)>: true_type {};

    //---------------is function----------------------------------

    // specialization for variadic functions such as std::printf
    template<class Ret, class... Args>
    struct is_function<Ret(Args..., ...)>: true_type {};

    // specialization for function types that have cv-qualifiers
    template<class Ret, class... Args>
    struct is_function<Ret(Args...)const>: true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...)volatile>: true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...)const volatile>: true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args..., ...)const>: true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args..., ...)volatile>: true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args..., ...)const volatile>: true_type {};

    // specialization for function types that have ref-qualifiers
    template<class Ret, class... Args>
    struct is_function<Ret(Args...) &>: true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...)const &>: true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...)volatile &>: true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...)const volatile &>: true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args..., ...) &>: true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args..., ...)const &>: true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args..., ...)volatile &>: true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args..., ...)const volatile &>: true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...) &&>: true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...)const &&>: true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...)volatile &&>: true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...)const volatile &&>: true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args..., ...) &&>: true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args..., ...)const &&>: true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args..., ...)volatile&&>: true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args..., ...)const volatile &&>: true_type {};

    template<typename _Tp>
    constexpr bool is_function_v = is_function<_Tp>::value;

    // specializations for noexcept versions of all the above (C++17 and later)
    // it has to add noexcept for all the sepcialization forms since it was counted
    // as the function signature in C++17

    //-----------------type traits-------------------------------
    //it is the original effort in SGI STL implementation to using TMP to 
    //staticly dispatch functions based on there type(whether can we call memmove/memcpy etc)
    //it is not used in any of the modern compiler (MSVC, GCC, LLVM-Clang) but I still keep it
    //as a reference
    template <typename TYPE>
    struct __type_traits {
        //be conservative, meaning we want to set all the type to be false
        typedef __false_type has_trivial_ctor;
        typedef __false_type has_trivial_copy_ctor;
        typedef __false_type has_trivial_assignment_operator;
        typedef __false_type has_trivial_dtor;
        typedef __false_type is_POD_type;
    };

    //template partial specialization, for all primitive types
    //if for any class we want it to have the same copy, assignment behavior as primitive types
    //we can define it by our own specialization
    template<>
    struct __type_traits <char> {
        typedef __true_type has_trivial_ctor;
        typedef __true_type has_trivial_copy_ctor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_dtor;
        typedef __true_type is_POD_type;
    };

    template<>
    struct __type_traits <unsigned char> {
        typedef __true_type has_trivial_ctor;
        typedef __true_type has_trivial_copy_ctor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_dtor;
        typedef __true_type is_POD_type;
    };

    template<>
    struct __type_traits <signed char> {
        typedef __true_type has_trivial_ctor;
        typedef __true_type has_trivial_copy_ctor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_dtor;
        typedef __true_type is_POD_type;
    };
    
    template<>
    struct __type_traits <short> {
        typedef __true_type has_trivial_ctor;
        typedef __true_type has_trivial_copy_ctor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_dtor;
        typedef __true_type is_POD_type;
    };

    template<>
    struct __type_traits <unsigned short> {
        typedef __true_type has_trivial_ctor;
        typedef __true_type has_trivial_copy_ctor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_dtor;
        typedef __true_type is_POD_type;
    };

    template<>
    struct __type_traits <int> {
        typedef __true_type has_trivial_ctor;
        typedef __true_type has_trivial_copy_ctor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_dtor;
        typedef __true_type is_POD_type;
    };

    template<>
    struct __type_traits <unsigned int> {
        typedef __true_type has_trivial_ctor;
        typedef __true_type has_trivial_copy_ctor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_dtor;
        typedef __true_type is_POD_type;
    };

    template<>
    struct __type_traits <long> {
        typedef __true_type has_trivial_ctor;
        typedef __true_type has_trivial_copy_ctor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_dtor;
        typedef __true_type is_POD_type;
    };
    
    template<>
    struct __type_traits <unsigned long> {
        typedef __true_type has_trivial_ctor;
        typedef __true_type has_trivial_copy_ctor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_dtor;
        typedef __true_type is_POD_type;
    };

    template<>
    struct __type_traits <float> {
        typedef __true_type has_trivial_ctor;
        typedef __true_type has_trivial_copy_ctor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_dtor;
        typedef __true_type is_POD_type;
    };

    template<>
    struct __type_traits <double> {
        typedef __true_type has_trivial_ctor;
        typedef __true_type has_trivial_copy_ctor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_dtor;
        typedef __true_type is_POD_type;
    };

    template<>
    struct __type_traits <long double> {
        typedef __true_type has_trivial_ctor;
        typedef __true_type has_trivial_copy_ctor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_dtor;
        typedef __true_type is_POD_type;
    };

    //pointer type
    template<typename T>
    struct __type_traits <T*> {
        typedef __true_type has_trivial_ctor;
        typedef __true_type has_trivial_copy_ctor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_dtor;
        typedef __true_type is_POD_type;
    };


        //------------remove reference-----------
    template <typename _Tp> 
    struct remove_reference {
        typedef _Tp type;
    };

    template <typename _Tp>
    struct remove_reference<_Tp&> {
        typedef _Tp type;
    };

    template <typename _Tp>
    struct remove_reference<_Tp&&> {
        typedef _Tp type;
    };

    //c++14, type aliasing allows us to do so
    template <typename _Tp>
    using remove_reference_t = typename remove_reference<_Tp>::type;

    //------------remove const--------------
    template <typename _Tp>
    struct remove_const {
        typedef _Tp type;
    };

    template <typename _Tp>
    struct remove_const <const _Tp> {
        typedef _Tp type;
    };

    //c++14
    template <typename _Tp>
    using remove_const_t = typename remove_const<_Tp>::type;

    //----------remove volatile------------
    template <typename _Tp>
    struct remove_volatile {
        typedef _Tp type;
    };

    template <typename _Tp>
    struct remove_volatile <volatile _Tp> {
        typedef _Tp type;
    };

    //c++14
    template <typename _Tp>
    using remove_volatile_t = typename remove_volatile<_Tp>::type;

    //----------remove const and volatile----
    template <typename _Tp>
    struct remove_cv {
        typedef typename remove_volatile<typename remove_const<_Tp>::type>::type type;
    };

    //c++14
    template <typename _Tp>
    using remove_cv_t = typename remove_cv<_Tp>::type;
}

#endif
