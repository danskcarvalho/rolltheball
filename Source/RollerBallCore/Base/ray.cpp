//
//  ray.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 04/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "ray.h"

using namespace rb;

ray::ray(){
    _origin = vec2::zero;
    _direction = vec2::right;
}
ray::ray(const vec2& origin, const vec2& direction){
    _origin = origin;
    assert(!direction.is_zero());
    _direction = direction;
    if(!_direction.is_unit())
        _direction.normalize();
}
const ray ray::up = ray(vec2::zero, vec2::up);
const ray ray::down = ray(vec2::zero, vec2::down);
const ray ray::left = ray(vec2::zero, vec2::left);
const ray ray::right = ray(vec2::zero, vec2::right);
bool ray::is_parallel_to(const ray& r) const{
    return _direction.is_parallel_to(r._direction);
}
bool ray::is_collinear_to(const ray& r) const{
    return is_parallel_to(r) && is_in_line(r._origin);
}
bool ray::contains_point(const vec2& pt) const{
    return almost_equal(distance(pt), 0) && almost_greather_or_equal(get_parameter(pt), 0);
}
bool ray::is_in_line(const vec2& pt) const{
    vec2 pt_origin = pt - _origin;
    vec2 perp = pt_origin - pt_origin.projection(_direction);
    return perp.is_zero();
}
float ray::distance(const vec2& pt) const {
    vec2 pt_origin = pt - _origin;
    vec2 perp = pt_origin - pt_origin.projection(_direction);
    return almost_less(get_parameter(pt), 0) ? vec2::distance(_origin, pt) : perp.length();
}
bool ray::is_perpendicular_to(const ray& r) const{
    return _direction.is_perpendicular_to(r._direction);
}
vec2 ray::sample(const float t) const{
    return _origin + t * _direction;
}
float ray::get_parameter(const vec2& pt) const{
    vec2 pt_origin = pt - _origin;
    pt_origin.project(_direction);
    return vec2::dot(pt_origin, _direction) >= 0 ?
             pt_origin.length() : //points at same direction
            -pt_origin.length() ;
}
ray ray::rotated90(const rotation_direction rd) const {
    return ray(_origin, _direction.rotated90(rd));
}
ray& ray::rotate90(const rotation_direction rd){
    _direction.rotate90(rd);
    return *this;
}
ray ray::rotated(const float angle) const{
    return ray(_origin, _direction.rotated(angle));
}
ray& ray::rotate(const float angle){
    _direction.rotated(angle);
    return *this;
}
rb_string ray::to_string() const{
    return rb::to_string("[", _origin, ", ", _direction, "]");
}
bool rb::operator ==(const ray& r1, const ray& r2){
    return r1._origin == r2._origin && r1._direction == r2._direction;
}
bool rb::operator !=(const ray& r1, const ray& r2){
    return r1._origin != r2._origin || r1._direction != r2._direction;
}

nullable<vec2> ray::intersection(const ray& r1, const ray& r2){
    vec2   as = r1._origin,
            ad = r1._direction,
            bs = r2._origin,
            bd = r2._direction;
    
    float dx = bs.x() - as.x();
    float dy = bs.y() - as.y();
    float det = bd.x() * ad.y() - bd.y() * ad.x();
    if(almost_equal(det, 0))
        return nullptr;
    float u = (dy * bd.x() - dx * bd.y()) / det;
    float v = (dy * ad.x() - dx * ad.y()) / det;
    
    auto pt1 = r1.sample(u);
    auto pt2 = r2.sample(v);
    
    assert(pt1 == pt2);
    return almost_less(u, 0) || almost_less(v, 0) ? nullptr : nullable<vec2>(pt1);
}








