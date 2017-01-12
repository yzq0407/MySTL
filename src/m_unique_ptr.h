//Implement the basic unique pointer interfaces and pointer traits
//most of the code take reference from LLVM's libc++ implementation:
//http://llvm.org/svn/llvm-project/libcxx/trunk/include/memory
//
//Created by Theodore Yang on 12/17/2016
#ifndef __MY_STL_UNIQUE_PTR_H
#define __MY_STL_UNIQUE_PTR_H

#include "m_type_traits.h"
#include <utility>

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

        //implicit conversion from other convertible class
        template <typename _Up>
        default_delete(const default_delete<_Up>& other) noexcept {}

        void operator()(_Tp* _ptr) const {
            delete _ptr;
        }
    };

    //partially speciallized form for array deletion
    template <typename _Tp>
    struct default_delete<_Tp[]> {
        constexpr default_delete() noexcept = default;

        template <typename _Up>
        default_delete(const default_delete<_Up[]>& other) noexcept {}

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
        //-------------------CTORS-----------------------
        //Default construct, a unique_ptr with nothing
        constexpr unique_ptr() noexcept;

        constexpr unique_ptr(std::nullptr_t) noexcept; 

        //construct using a poineter
        explicit unique_ptr(pointer _p) noexcept;
        
        //construct unique_ptr with custom deleter, the rules are:
        //a) If D is non-reference type A, then the signatures are:
        //unique_ptr(pointer p, const A& d); (requires that Deleter is nothrow-CopyConstructible)
        //unique_ptr(pointer p, A&& d); (requires that Deleter is nothrow-MoveConstructible)
        //b) If D is an lvalue-reference type A&, then the signatures are:
        //unique_ptr(pointer p, A& d);
        //unique_ptr(pointer p, A&& d);
        //c) If D is an lvalue-reference type const A&, then the signatures are:
        //unique_ptr(pointer p, const A& d);
        //unique_ptr(pointer p, const A&& d);
        unique_ptr(pointer _ptr,
                typename conditional<is_reference<deleter_type>::value, 
                deleter_type, 
                add_lvalue_reference_t<add_const_t<deleter_type>>>::type _dl) noexcept;

        unique_ptr(pointer _ptr, 
                add_rvalue_reference_t<remove_reference_t<element_type>> _dl) noexcept;

        unique_ptr(unique_ptr&& _ptr) noexcept;

        template <typename _Up, typename _Ep>
        unique_ptr(unique_ptr<_Up, _Ep>&& _ptr) noexcept;

        ~unique_ptr() {
            __pair.second()(__pair.first());
        }

        //no copy is allowed
        unique_ptr(const unique_ptr& _ptr) = delete;
        
        unique_ptr& operator=(const unique_ptr& _ptr) = delete;

        //move assignment
        //http://en.cppreference.com/w/cpp/memory/unique_ptr/operator%3D
        unique_ptr& operator=(unique_ptr&& _ptr) noexcept;
        
        template <typename _Up, typename _Ep>
        unique_ptr& operator=(unique_ptr<_Up, _Ep>&& _ptr) noexcept;

        unique_ptr& operator=(std::nullptr_t) noexcept;

        //Modifiers
        pointer release() noexcept;

        void reset(pointer _ptr = pointer()) noexcept;

        void swap(unique_ptr& other) noexcept;

        //Observers
        pointer get() noexcept;

        const_pointer get() const noexcept;

        deleter_type& get_deleter() noexcept;

        const deleter_type& get_deleter() const noexcept;

        explicit operator bool() const noexcept;

        //pointer operations
        element_type& operator*() noexcept;

        const element_type& operator*() const noexcept;

        pointer operator->() noexcept;

        const_pointer operator->() const noexcept;
    };

    //non member function declarations
    template <typename _Tp, typename... Args>
    unique_ptr<_Tp> make_unique(Args&&... args) {
        return unique_ptr<_Tp>(new _Tp(std::forward<Args>(args)...));
    }

    template <typename _Tp1, typename _Dp1, typename _Tp2, typename _Dp2>
    bool operator==(const unique_ptr<_Tp1, _Dp1>& ptr1, const unique_ptr<_Tp1, _Dp1>& ptr2) {
        return ptr1.get() == ptr2.get();
    }

    template <typename _Tp1, typename _Dp1, typename _Tp2, typename _Dp2>
    bool operator!=(const unique_ptr<_Tp1, _Dp1>& ptr1, const unique_ptr<_Tp1, _Dp1>& ptr2) {
        return ptr1.get() != ptr2.get();
    }
    //unique_ptr---------------implementation

    //---------------------------------------------------------------------------
    //-------------------------Ctors---------------------------------------------
    //---------------------------------------------------------------------------
    template <typename _Tp, typename _Dp>
    constexpr unique_ptr<_Tp, _Dp>::unique_ptr() noexcept: __pair(pointer()) {
        //if the deleter is a function pointer, it can not be null
        static_assert(!is_pointer_v<_Dp>, "cannot construct unique_ptr with null function pointer");
    }

    template <typename _Tp, typename _Dp>
    constexpr unique_ptr<_Tp, _Dp>::unique_ptr(std::nullptr_t) noexcept: __pair(pointer()) {
        //make sure the delete type is not a function pointer
        static_assert(!is_pointer_v<_Dp>, "cannot construct unique_ptr with null function pointer");
    }

    template <typename _Tp, typename _Dp>
    unique_ptr<_Tp, _Dp>::unique_ptr(pointer _ptr) noexcept: __pair(_ptr) {
        //make sure the delete type is not a function pointer
        static_assert(!is_pointer_v<_Dp>, "cannot construct unique_ptr with null function pointer");
    }

    template <typename _Tp, typename _Dp>
    unique_ptr<_Tp, _Dp>::unique_ptr(pointer _ptr,
        typename conditional<is_reference<deleter_type>::value, 
        deleter_type, 
        add_lvalue_reference_t<add_const_t<deleter_type>>>::type _dl) noexcept
    : __pair(_ptr, _dl) {
        //make sure the delete type is not a function pointer
        static_assert(!is_pointer_v<_Dp>, "cannot construct unique_ptr using function delete type");
    }

    template <typename _Tp, typename _Dp>
    unique_ptr<_Tp, _Dp>::unique_ptr(pointer _ptr,
            add_rvalue_reference_t<remove_reference_t<element_type>> _dl) noexcept
    : __pair(_ptr, std::move(_dl)) {
        //make sure the delete type is not a function pointer
        static_assert(!is_pointer_v<_Dp>, "cannot construct unique_ptr using function delete type");
    }

    template <typename _Tp, typename _Dp>
    unique_ptr<_Tp, _Dp>::unique_ptr(unique_ptr&& _ptr) noexcept:
        __pair(_ptr.release(), std::move(_ptr.get_deleter())) {
        }

    template <typename _Tp, typename _Dp>
    template <typename _Up, typename _Ep>
    unique_ptr<_Tp, _Dp>::unique_ptr(unique_ptr<_Up, _Ep>&& _ptr) noexcept:
    __pair(_ptr.release(), std::move(_ptr.get_deleter())) {
    }
    //****************************************************************************
    //-------------------------End of Ctors---------------------------------------
    //****************************************************************************

    //---------------------------------------------------------------------------
    //--------------------------Operator Overloading-----------------------------
    //---------------------------------------------------------------------------
    //copy assignment
    template <typename _Tp, typename _Dp>
    unique_ptr<_Tp, _Dp>& unique_ptr<_Tp, _Dp>::operator=(unique_ptr&& _rhs) noexcept{
        reset(_rhs.release());
        __pair.second() = std::move(_rhs.get_deleter());
        return *this;
    }

    template <typename _Tp, typename _Dp>
    template <typename _Up, typename _Ep>
    unique_ptr<_Tp, _Dp>& unique_ptr<_Tp, _Dp>::operator=(unique_ptr<_Up, _Ep>&& _rhs) noexcept{
        reset(_rhs.release());
        __pair.second() = std::move(_rhs.get_deleter());
        return *this;
    }

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

    //Modifiers
    template <typename _Tp, typename _Dp>
    typename unique_ptr<_Tp, _Dp>::pointer 
    unique_ptr<_Tp, _Dp>::release() noexcept {
        pointer _ptr = __pair.first();
        __pair.first() = nullptr;
        return _ptr;
    }

    template <typename _Tp, typename _Dp>
    void unique_ptr<_Tp, _Dp>::reset(pointer _ptr) noexcept {
        __pair.second()(__pair.first());
        __pair.first() = _ptr;
    }

    template <typename _Tp, typename _Dp>
    void unique_ptr<_Tp, _Dp>::swap(unique_ptr& _ptr) noexcept {
        __pair.swap(_ptr.__pair);
    }

    //Observers
    template <typename _Tp, typename _Dp>
    typename unique_ptr<_Tp, _Dp>::pointer
    unique_ptr<_Tp, _Dp>::get() noexcept {
        return __pair.first();
    }

    template <typename _Tp, typename _Dp>
    typename unique_ptr<_Tp, _Dp>::const_pointer
    unique_ptr<_Tp, _Dp>::get() const noexcept {
        return __pair.first();
    }

    template <typename _Tp, typename _Dp>
    typename unique_ptr<_Tp, _Dp>::deleter_type& 
    unique_ptr<_Tp, _Dp>::get_deleter() noexcept  {
        return __pair.second();
    }

    template <typename _Tp, typename _Dp>
    const typename unique_ptr<_Tp, _Dp>::deleter_type& 
    unique_ptr<_Tp, _Dp>::get_deleter() const noexcept {
        return __pair.second();
    }

    template <typename _Tp, typename _Dp>
    unique_ptr<_Tp, _Dp>::operator bool() const noexcept{
        return get() != nullptr;
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

            void swap(compressed_pair& _pair2) noexcept {
                using std::swap;
                swap(_first, _pair2.first);
                swap(_second, _pair2.second);
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

            explicit compressed_pair(second_param_type _y): _second(_y) {}

            //note that a lambda doesn't have a default constructor, we need to 
            //copy construct the base
            compressed_pair(first_param_type _x, second_param_type _y): 
                first_param_type(_x), _second(_y) {}

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

            void swap(compressed_pair& _pair2) noexcept {
                using std::swap;
                swap(_second, _pair2.second);
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

            constexpr compressed_pair(): _first() {};
            explicit constexpr compressed_pair(first_param_type _x): _first(_x) {};

            constexpr compressed_pair(first_param_type _x,
                    second_param_type _y): _Tp2(_y), 
                                           _first(_x) {};

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

            void swap(compressed_pair& _pair) noexcept {
                using std::swap;
                swap(_first, _pair._first);
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
        struct pair_leaf: remove_cv_t<_Tp> {
            using base = remove_cv_t<_Tp>;
            pair_leaf(): base() {}

            pair_leaf(_Tp _x): base(_x) {}
        };

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
            
            explicit compressed_pair(first_param_type _x = _Tp1(), 
                    second_param_type _y = _Tp2()):
                pair_leaf<_Tp1, 1>(_x),
                pair_leaf<_Tp2, 2>(_y) {}

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

            void swap(compressed_pair& _pair) noexcept {
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
