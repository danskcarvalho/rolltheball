//
//  brush.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 10/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "brush.h"
#include "vec2.h"
#include "transform_space.h"
#include "matrix3x3.h"

using namespace rb;

brush::brush(){
    _color = color::from_rgba(1, 1, 1, 1); //opaque white
    _size = 0;
    _hardness = 0;
}
//the color is not pre-multiplied...
brush::brush(const class color& color, const float size, const float hardness, enum blend_mode blend_mode){
    this->color(color);
    this->size(size);
    this->hardness(hardness);
    this->_blend_mode = blend_mode;
}
rb_string brush::to_string() const{
    return rb::to_string("(", _color, ", s: ", _size, ", h: ", _hardness);
}

//the returned color is premultiplied...
class color brush::sample_color(const transform_space& ts, const vec2& position) const{
    vec2 _p = ts.from_base_to_space().transformed_point(position);
    float _len = _p.length();
    if(_len > _size)
        return color::from_rgba(0, 0, 0, 0);
    else {
        float _h_s = _size * _hardness;
        _len -= _h_s;
        if(_len <= 0)
            return _color.pre_multiplied();
        else {
            _h_s = _size - _h_s;
            _len /= _h_s;
            return color::from_rgba(_color.r(), _color.g(), _color.b(), _color.a() * (1 - _len)).pre_multiplied();
        }
    }
}
