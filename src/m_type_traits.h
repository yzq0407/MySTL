//type traits is not one of the library requirement for stl standard, but it used the same approach
//as the trait technique in the iterator to seperate objects with or without trivial ctor, dtor, 
//copy ctor, assignment oper, and whether it is a POD (plain old type)
//
#ifndef __MY_STL_TYPE_TRAITS_H
#define __MY_STL_TYPE_TRAITS_H

namespace my_stl {
//two types to identify true and false, we cannot use variables since we want compile time
//identification of the types
    struct __true_type {};
    struct __false_type {};

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
    struct __are_same {
        typedef __false_type __type;
        enum {__value = 1};
    };

    template <typename _Tp>
    struct __are_same<_Tp, _Tp> {
        typedef __true_type __type;
        enum {__value = 0};
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
}

#endif
