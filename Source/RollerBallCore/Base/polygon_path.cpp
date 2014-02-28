//
//  polygon_path.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 12/01/14.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#include "polygon_path.h"
#include "polygon.h"
#include "vec2.h"

using namespace rb;

polygon_path::polygon_path(const class polygon& p){
    _polygon = p;
    _total_len = 0;
    for (uint32_t i = 0; i < _polygon.edge_count(); i++) {
        if(i == 0)
            _length.push_back(_polygon.get_edge(i).length());
        else
            _length.push_back(_polygon.get_edge(i).length() + _length[i - 1]);
        _total_len += _polygon.get_edge(i).length();
    }
}

const class polygon& polygon_path::polygon() const {
    return _polygon;
}

float polygon_path::length() const {
    return _total_len;
}

vec2 polygon_path::point_at(float l, bool clamp) const {
    if (l == _total_len)
        return _polygon.get_edge(_polygon.edge_count() - 1).pt1();
    if (l == 0)
        return _polygon.get_edge(0).pt0();
    if(l > _total_len){
        if(clamp)
            return _polygon.get_edge(_polygon.edge_count() - 1).pt1();
        auto _int_part = (int)(l / _total_len);
        l -= _int_part * _total_len;
    }
    if(l < 0){
        if(clamp)
            return _polygon.get_edge(0).pt0();
        //first we remove unnecessary walks throught the polygon...
        auto _int_part = (int)(l / _total_len);
        l = l - _int_part * _total_len;
        //we then make l positive
        l = _total_len + l;
    }
    return point_at(l, 0, _length.size() - 1);
}

vec2 polygon_path::point_at(float l, size_t low_index, size_t hi_index) const {
    if(low_index == hi_index){
        auto _ledge = low_index != 0 ? l - _length[low_index - 1] : l;
        auto _t = _ledge / _polygon.get_edge((uint32_t)low_index).length();
        return vec2::lerp(_t, _polygon.get_edge((uint32_t)low_index).pt0(), _polygon.get_edge((uint32_t)low_index).pt1());
    }
    else {
        auto _mid_index = (hi_index + low_index) / 2;
        auto _mid_index_1 = _mid_index + 1;
        if (l <= _length[_mid_index])
            return point_at(l, low_index, _mid_index);
        else
            return point_at(l, _mid_index_1, hi_index);
    }
}


























