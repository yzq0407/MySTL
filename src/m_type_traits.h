//type traits is not one of the library requirement for stl standard, but it used the same approach
//as the trait technique in the iterator to seperate objects with or without trivial ctor, dtor, 
//copy ctor, assignment oper, and whether it is a POD (plain old type)
//
#ifndef __MY_STL_TYPE_TRAITS_H
#define __MY_STL_TYPE_TRAITS_H

namespace my_stl {
//two types to identify true and false, we cannot use variables since we want compile time
//identification of the types
    struct __true_type {
        enum {value = 1};
    };
    struct __false_type {
        enum {value = 0};
    };

    //all the logical operation applied to true/false type
    //-------------logical or-------------------------
    template <typename... _B>
    struct __or__ {
        typedef __false_type type;
        enum {value = 0};
    };

    template <typename... _B>
    struct __or__ <__true_type, _B...> {
        typedef __true_type type;
        enum {value = 1};
    };

    template <typename... _B>
    struct __or__ <__false_type, _B...> {
        typedef typename __or__<_B...>::type type;
        enum {value = __or__<_B...>::value};
    };

    //--------------logical and------------------------
    template <typename... _B>
    struct __and__ {
        typedef __true_type type;
        enum {value = 1};
    };

    template <typename... _B>
    struct __and__ <__false_type, _B...> {
        typedef __false_type type;
        enum {value = 0};
    };

    template <typename... _B>
    struct __and__ <__true_type, _B...> {
        typedef typename __and__<_B...>::type type;
        enum {value = __and__<_B...>::value};
    };


    //the core part, exact the type information
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


    //---------------------integer traits---------------------
    template<typename _Tp>
    struct _Is_integer {
        typedef __false_type integral;
    };

    //specialized from
    template<>
    struct _Is_integer<bool> {
        typedef __true_type integral;
    };

    template<>
    struct _Is_integer<int> {
        typedef __true_type integral;
    };

    template<>
    struct _Is_integer<unsigned int> {
        typedef __true_type integral;
    };

    template<>
    struct _Is_integer<unsigned short> {
        typedef __true_type integral;
    };
    
    template<>
    struct _Is_integer<short> {
        typedef __true_type integral;
    };
    
    template<>
    struct _Is_integer<long> {
        typedef __true_type integral;
    };

    template<>
    struct _Is_integer<unsigned long> {
        typedef __true_type integral;
    };

    template<>
    struct _Is_integer<long long> {
        typedef __true_type integral;
    };
    
    template<>
    struct _Is_integer<unsigned long long> {
        typedef __true_type integral;
    };
    
    template<>
    struct _Is_integer<char> {
        typedef __true_type integral;
    };

    template<>
    struct _Is_integer<signed char> {
        typedef __true_type integral;
    };

    template<>
    struct _Is_integer<unsigned char> {
        typedef __true_type integral;
    };

    //----------is same type----------------
    template <typename, typename>
    struct __are_same{
        typedef __false_type type;
        enum {value = 0};
    };

    template <typename _Tp>
    struct __are_same<_Tp, _Tp> {
        typedef __true_type type;
        enum {value = 1};
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
