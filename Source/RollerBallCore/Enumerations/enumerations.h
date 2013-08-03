//
//  enumerations.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 01/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef RollerBallCore_enumerations__
#define RollerBallCore_enumerations__

#include <cstdint>

namespace rb {
    template<class T, class... Tps>
    T combine(const T value){
        return value;
    }
    
    template<class T, class... Tps>
    T combine(const T value, const Tps... tail){
        return (T)((uint64_t)value | (uint64_t)combine(tail...));
    }
    
    template<class T>
    inline bool has_flag(T flags, T flag){
        uint64_t _flags = (uint64_t)flags;
        uint64_t _flag = (uint64_t)flag;
        return _flags & _flag;
    }
}

#endif
