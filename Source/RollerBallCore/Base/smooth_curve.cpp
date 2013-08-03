//
//  smooth_curve.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 08/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "smooth_curve.h"
#include "polygon.h"
#include "edge.h"

using namespace rb;
using namespace std;

template<class T>
void adjust_to_size(std::vector<T>& v, int32_t size){
    assert(size >= 0);
    if(v.size() == size)
        return;
    
    if (v.size() > size) {
        while (v.size() != size)
            v.pop_back();
    }
    else {
        while (v.size() != size)
            v.push_back(T());
    }
}

void smooth_curve::update_knots(){
    if(_cvs.size() < 4)
        return;
    auto _non_const = const_cast<smooth_curve*>(this);
    if (_is_closed) {
        auto _knot_count = _cvs.size() + 3 + 4; //we define 3 implicity control points...
        adjust_to_size(_non_const->_knots, (int32_t)_knot_count);
        for (int32_t i = 0; i < _knot_count; i++) {
            float _step = (1.0 / (float)(_knot_count - 1.0f));
            float _knot = i * _step;
            _non_const->_knots[i] = _knot;
        }
    }
    else {
        auto _knot_count = _cvs.size() + 4;
        adjust_to_size(_non_const->_knots, (int32_t)_knot_count);
        
        _non_const->_knots[0] = _non_const->_knots[1] = _non_const->_knots[2] = _non_const->_knots[3] = 0;
        _non_const->_knots[0] = -0.000001;
        
        auto _l = _knot_count - 1;
        _non_const->_knots[_l] = _non_const->_knots[_l - 1] = _non_const->_knots[_l - 2] = _non_const->_knots[_l - 3] = 1;
        _non_const->_knots[_l - 3] = 1.000001;
        
        float _int_knots = _knot_count - 8;
        
        for (int32_t i = 0; i < (_knot_count - 8); ++i) {
            float _step = (1.0f / (_int_knots + 1.0));
            float _knot = _step * (i + 1.0f);
            _non_const->_knots[4 + i] = _knot;
        }
    }
}

void smooth_curve::update_internal_state(){
    if(_cvs.size() < 4)
        return;
    update_knots();
    _cvs_c = _cvs;
    _cvs_c.push_back(_cvs[0]);
    _cvs_c.push_back(_cvs[1]);
    _cvs_c.push_back(_cvs[2]);
}

smooth_curve::smooth_curve(){
    _cvs = std::vector<vec2>();
    _knots = std::vector<float>();
    _is_closed = false;
}
smooth_curve& smooth_curve::reset(){
    _cvs = std::vector<vec2>();
    _knots = std::vector<float>();
    _is_closed = false;
    return *this;
}
smooth_curve& smooth_curve::build_open_curve(const std::vector<vec2>& cvs, smooth_curve& storage){
    assert(cvs.size() >= 4);
    storage.reset();
    storage._cvs = cvs;
    storage.open_curve();
    return storage;
}
smooth_curve& smooth_curve::build_closed_curve(const std::vector<vec2>& cvs, smooth_curve& storage){
    assert(cvs.size() >= 4);
    storage.reset();
    storage._cvs = cvs;
    storage.close_curve();
    return storage;
}

 
template<int32_t n>
float basis(int32_t j, float t, const std::vector<float>& knots){
    auto _k1 = (t - knots[j]) / (knots[j + n] - knots[j]);
    if(isnan(_k1) || isinf(_k1))
        _k1 = 0;
    auto _k2 = (knots[j + n + 1] - t) / (knots[j + n + 1] - knots[j + 1]);
    if(isnan(_k2) || isinf(_k2))
        _k2 = 0;
    auto _b1 = basis<n - 1>(j, t, knots);
    auto _b2 = basis<n - 1>(j + 1, t, knots);
    auto _result =  _k1 * _b1 + _k2 * _b2;
    return _result;
}

template<>
float basis<0>(int32_t j, float t, const std::vector<float>& knots){
    if(knots[j] <= t && t < knots[j + 1])
        return 1;
    else
        return 0;
}

vec2 smooth_curve::sample_along_path(const float u) const{
    float _fu = floorf(u);
    float _u = !almost_equal(_fu, u) ? u - floorf(u) : 1;
    if(almost_equal(u, 0))
        _u = 0;
    
    assert(_cvs.size() >= 4);
    if(_is_closed){
        //we redefine u...
        float _us = _knots[3];
        float _ue = _knots[_knots.size() - 4];
        _u = (1 - _u)*_us + _u*_ue; //remap to [us, ue] range...
        vec2 _sum = vec2::zero;
        for (int32_t i = 0; i <= _knots.size() - 5; ++i) {
            _sum += _cvs_c[i] * basis<3>(i, _u, _knots);
        }
        return _sum;
    }
    else {
        vec2 _sum = vec2::zero;
        for (int32_t i = 0; i <= _knots.size() - 5; ++i) {
            auto _b = basis<3>(i, _u, _knots);
            _sum += _cvs[i] * _b;
        }
        return _sum;
    }
}

int32_t smooth_curve::cv_count() const{
    return (int32_t)_cvs.size();
}
const vec2& smooth_curve::get_cv(const int32_t at) const{
    assert(at >= 0);
    assert(at < _cvs.size());
    return _cvs[at];
}
smooth_curve& smooth_curve::add_cv_after(const vec2& cv, const int32_t at){
    assert(at >= 0);
    if(_cvs.size() != 0){
        assert(at < _cvs.size());
        _cvs.insert(_cvs.begin() + at + 1, cv);
    }
    else {
        assert(at == 0);
        _cvs.push_back(cv);
    }
    update_internal_state();
    return *this;
}
smooth_curve& smooth_curve::add_cv_before(const vec2& cv, const int32_t at){
    assert(at >= 0);
    if(_cvs.size() != 0){
        assert(at < _cvs.size());
        _cvs.insert(_cvs.begin() + at, cv);
    }
    else {
        assert(at == 0);
        _cvs.push_back(cv);
    }
    update_internal_state();
    return *this;
}
smooth_curve& smooth_curve::remove_cv(const int32_t at){
    assert(at >= 0);
    assert(at < _cvs.size());
    _cvs.erase(_cvs.begin() + at);
    update_internal_state();
    return *this;
}
smooth_curve& smooth_curve::set_cv(const vec2& cv, int32_t at){
    assert(at >= 0);
    assert(at < _cvs.size());
    _cvs[at] = cv;
    update_internal_state();
    return *this;
}
bool smooth_curve::is_closed() const{
    return _is_closed;
}
smooth_curve& smooth_curve::close_curve(){
    _is_closed = true;
    update_internal_state();
    return *this;
}
smooth_curve& smooth_curve::open_curve(){
    _is_closed = false;
    update_internal_state();
    return *this;
}
rb_string smooth_curve::to_string() const{
    return rb::to_string(_cvs);
}

float get_quality(const smooth_curve& curve, const float u0, const float u1, const int32_t division_level){
    auto _new_level = division_level * 3;
    auto _step = (u1 - u0) / (float)(_new_level - 1);
    auto _max_distance = 0.0f;
    auto _pt0 = curve.sample_along_path(u0);
    auto _pt1 = curve.sample_along_path(u1);
    auto _edge = edge(_pt0, _pt1, vec2::zero);
    
    for (int32_t i = 0; i < _new_level; ++i) {
        auto u = u0 + _step * (float)i;
        vec2 pt = curve.sample_along_path(u);
        auto _d = _edge.distance(pt);
        if(_d > _max_distance)
            _max_distance = _d;
    }
    return _max_distance;
}

void get_path_points(std::vector<vec2>& points, const smooth_curve& curve, const float u0, const float u1, const int32_t division_level, float quality){
    points.clear();
    auto _step = (u1 - u0) / (float)(division_level - 1);
    
    for (int32_t i = 0; i < (division_level - 1); ++i) {
        auto us = u0 + _step * (float)i;
        auto ue = u0 + _step * (float)(i + 1);
        auto _q = get_quality(curve, us, ue, division_level);
        if(_q <= quality){
            points.push_back(curve.sample_along_path(us));
            points.push_back(curve.sample_along_path(ue));
        }
        else {
            std::vector<vec2> _current;
            get_path_points(_current, curve, us, ue, division_level, quality);
            points.insert(points.end(), _current.begin(), _current.end());
        }
    }
}

polygon smooth_curve::to_polygon(polygon& storage, float quality, int32_t division_level) const{
    std::vector<vec2> _points;
    get_path_points(_points, *this, 0, 1, division_level, quality);
    if(_is_closed)
        storage = polygon::build_closed_polygon(_points, storage);
    else
        storage = polygon::build_open_polygon(_points, storage);
    storage.optimize();
    return storage;
}

bool rb::operator ==(const smooth_curve& c1, const smooth_curve& c2){
    if(c1._is_closed != c2._is_closed)
        return false;
    
    if(c1._cvs.size() != c2._cvs.size())
        return false;
    
    for (int i = 0; i < c2._cvs.size(); i++) {
        if(c1._cvs[i] != c2._cvs[i])
            return false;
    }
    
    return true;
}
bool rb::operator !=(const smooth_curve& c1, const smooth_curve& c2){
    if(c1._is_closed != c2._is_closed)
        return true;
    
    if(c1._cvs.size() != c2._cvs.size())
        return true;
    
    for (int i = 0; i < c2._cvs.size(); i++) {
        if(c1._cvs[i] != c2._cvs[i])
            return true;
    }
    
    return false;
}