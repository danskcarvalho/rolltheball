//
//  touch.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 07/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#if defined(IOS_TARGET)
#import <UIKit/UIKit.h>
#endif
#include "touch.h"

using namespace rb;

#if defined(MACOSX_TARGET)
touch::touch(NSTouch* native_touch){
    _native_touch = [[native_touch identity] copyWithZone: NULL];
    _normalized_pos.x([native_touch normalizedPosition].x);
    _normalized_pos.y([native_touch normalizedPosition].y);
    _normalized_pos *= 2;
    _normalized_pos -= vec2(1, 1);
}
#elif defined(IOS_TARGET)
touch::touch(UITouch* native_touch, const vec2& normalized_position){
    _native_touch = native_touch;
    _normalized_pos = normalized_position;
}
#endif

bool touch::compare_identity(const rb::touch &another) const{
#if defined(IOS_TARGET)
    return _native_touch == another._native_touch;
#else
    NSTouch* _id1 = _native_touch;
    NSTouch* _id2 = another._native_touch;
    return [[_id1 identity] isEqual: [_id2 identity]];
#endif
}

const vec2& touch::normalized_position() const {
    return _normalized_pos;
}

touch::touch(const touch& another){
#if defined(IOS_TARGET)
    _native_touch = another._native_touch;
    _normalized_pos = another._normalized_pos;
#else
    id _n_t = another._native_touch;
    _native_touch = _n_t;
    _normalized_pos = another._normalized_pos;
#endif
}

touch::touch(touch&& another){
    _native_touch = another._native_touch;
    _normalized_pos = another._normalized_pos;
    another._native_touch = nil;
}

touch& touch::operator=(const touch& another){
    if(this == &another)
        return *this;
#if defined(IOS_TARGET)
    _native_touch = another._native_touch;
    _normalized_pos = another._normalized_pos;
#else
    id _n_t = another._native_touch;
    _native_touch = _n_t;
    _normalized_pos = another._normalized_pos;
#endif
    return *this;
}

touch& touch::operator=(touch&& another){
    if(this == &another)
        return *this;
#if defined(IOS_TARGET)
    _native_touch = another._native_touch;
    _normalized_pos = another._normalized_pos;
#else
    _native_touch = another._native_touch;
    _normalized_pos = another._normalized_pos;
    another._native_touch = nullptr;
#endif
    return *this;
}

touch::touch(){
    _native_touch = nullptr;
    _normalized_pos = vec2::zero;
}

touch::~touch(){
}

float touch::x() const {
    return _normalized_pos.x();
}

float touch::y() const {
    return _normalized_pos.y();
}























