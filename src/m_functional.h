#ifndef __MY_STL_FUNCTIONAL_H
#define __MY_STL_FUNCTIONAL_H

namespace my_stl {
    //implement default callable object that can be used in stl algorithm
    template<typename _Tp>
    struct less {
        bool operator()(const _Tp& _x1, const _Tp& _x2) {
            return _x1 < _x2;
        }
    };


    template<typename _Tp>
    struct greater {
        bool operator()(const _Tp& _x1, const _Tp& _x2) {
            return _x1 > _x2;
        }
    };
}
#endif
