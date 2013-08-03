//
//  vec2.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 01/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "vec2.h"
#include "logging.h"
#include "matrix3x3.h"

using namespace rb;

vec2& vec2::snap(){
    if(almost_equal(_x, 0))
        _x = 0;
    else if(almost_equal(_x, M_PI / 2))
        _x = M_PI / 2;
    else if(almost_equal(_x, M_PI))
        _x = M_PI;
    else if(almost_equal(_x, 3 * (M_PI / 2)))
        _x = 3* (M_PI / 2);
    
    if(almost_equal(_y, 0))
        _y = 0;
    else if(almost_equal(_y, M_PI / 2))
        _y = M_PI / 2;
    else if(almost_equal(_y, M_PI))
        _y = M_PI;
    else if(almost_equal(_y, 3 * (M_PI / 2)))
        _y = 3* (M_PI / 2);
    
    return *this;
}

vec2::vec2(){
    _x = 0;
    _y = 0;
}

vec2::vec2(float x, float y){
    _x = x;
    _y = y;
}

vec2::vec2(const vec2& other){
    _x = other._x;
    _y = other._y;
}

vec2::vec2(vec2&& other){
    _x = other._x;
    _y = other._y;
}

const vec2& vec2::operator=(const rb::vec2 &other){
    this->_x = other._x;
    this->_y = other._y;
    return *this;
}

const vec2& vec2::operator +=(const vec2& v2){
    _x += v2._x;
    _y += v2._y;
    return *this;
}
const vec2& vec2::operator -=(const vec2& v2){
    _x -= v2._x;
    _y -= v2._y;
    return *this;
}
const vec2& vec2::operator *=(const vec2& v2){
    _x *= v2._x;
    _y *= v2._y;
    return *this;
}
const vec2& vec2::operator /=(const vec2& v2){
    assert(!almost_equal(v2._x, 0));
    assert(!almost_equal(v2._y, 0));
    _x /= v2._x;
    _y /= v2._y;
    return *this;
}
const vec2& vec2::operator *=(const float t){
    _x *= t;
    _y *= t;
    return *this;
}
const vec2& vec2::operator /=(const float t){
    assert(!almost_equal(t, 0));
    _x /= t;
    _y /= t;
    return *this;
}

const vec2 vec2::zero = vec2();
const vec2 vec2::up = vec2(0, 1);
const vec2 vec2::right = vec2(1, 0);
const vec2 vec2::down = vec2(0, -1);
const vec2 vec2::left = vec2(-1, 0);

bool vec2::is_zero() const{
    return almost_equal(_x, 0) && almost_equal(_y, 0);
}
bool vec2::is_unit() const{
    auto _sqrt_len = squared_length();
    return almost_equal(_sqrt_len, 1);
}
bool vec2::is_perpendicular_to(const vec2& other) const{
    if(is_zero() || other.is_zero())
        WARN("is_perpendicular_to: ", *this, other);
    
    return almost_equal(dot(*this, other), 0);
}
vec2 vec2::lerp(const float f, const vec2& v1, const vec2& v2){
    return (1 - f)*v1 + f*v2;
}
bool vec2::is_parallel_to(const vec2& other) const{
    if(is_zero() || other.is_zero())
        WARN("is_parallel_to: testing ", *this, other);
    
    auto _dt = dot(*this, other);
    auto _diff = _dt*_dt - squared_length() * other.squared_length();
    return almost_equal(_diff, 0);
}
bool vec2::points_at_same_direction(const vec2& other) const{
    if(is_zero() || other.is_zero())
        WARN("points_at_same_direction: testing ", *this, other);
    
    auto _dt = dot(*this, other);
    return is_parallel_to(other) && almost_greather_or_equal(_dt, 0);
}

float vec2::angle_between(const vec2& other, const rotation_direction rd) const{
    assert(!is_zero());
    assert(!other.is_zero());
    
    auto _dt = dot(*this, other);
    _dt /= (length() * other.length());
    
    if(rd == rotation_direction::shortest){
        return acosf(_dt);
    }
    else {
        auto _cross = cross(*this, other);
        //_cross may be zero
        if(almost_equal(_cross, 0)) //it may be 0 or +PI
        {
            if(points_at_same_direction(other)) //0
                return 0;
            else
                return M_PI;
        }
        
        if(rd == rotation_direction::ccw){
            if(almost_greather(_cross, 0))
                return acosf(_dt);
            else
                return 2 * M_PI - acosf(_dt);
        }
        else {
            if(almost_greather(_cross, 0)) //is ccw
                return 2 * M_PI - acosf(_dt);
            else
                return acosf(_dt);
        }
    }
}

float vec2::squared_length() const{
    return dot(*this, *this);
}
float vec2::length() const{
    return sqrtf(squared_length());
}
vec2 vec2::normalized() const{
    auto _len = length();
    
    if(!almost_equal(_len, 0))
        return vec2(_x / _len, _y / _len);
    else{
        WARN("normalized zero vector ", *this);
        return *this;
    }
}
vec2& vec2::normalize(){
    auto _len = length();
    
    if(!almost_equal(_len, 0))
    {
        _x /= _len;
        _y /= _len;
    }
    else
        WARN("normalized zero vector ", *this);
    
    return *this;
}

rb_string vec2::to_string() const{
    return rb::to_string("x: ", _x, ", y: ", _y);
}

vec2 vec2::rotated90(const rotation_direction rd) const{
    assert(rd != rotation_direction::shortest);
    
    if(rd == rotation_direction::ccw)
        return vec2(-_y, _x);
    else
        return vec2(_y, -_x);
}
vec2& vec2::rotate90(const rotation_direction rd){
    assert(rd != rotation_direction::shortest);
    
    if(rd == rotation_direction::ccw){
        auto _x_s = _x;
        _x = -_y;
        _y = _x_s;
    }
    else {
        auto _x_s = _x;
        _x = _y;
        _y = -_x_s;
    }
    
    return *this;
}
vec2 vec2::rotated(const float angle) const{
    matrix3x3 _rotation = matrix3x3::build_rotation(angle);
    return _rotation.transformed_vector(*this);
}
vec2& vec2::rotate(const float angle){
    matrix3x3 _rotation = matrix3x3::build_rotation(angle);
    return _rotation.transform_vector(*this);
}


float vec2::dot(const vec2& v1, const vec2& v2){
    return v1._x * v2._x + v1._y * v2._y;
}

float vec2::cross(const vec2& v1, const vec2& v2){
    return v1._x * v2._y - v2._x * v1._y;
}

vec2 vec2::projection(const vec2& v) const{
    assert(!v.is_zero());
    auto _normalized = v.normalized();
    auto _dot = dot(*this, _normalized);
    return vec2(_normalized._x * _dot, _normalized._y * _dot);
}

vec2& vec2::project(const vec2& v){
    assert(!v.is_zero());
    auto _normalized = v.normalized();
    auto _dot = dot(*this, _normalized);
    _x = _normalized._x * _dot;
    _y = _normalized._y * _dot;
    return *this;
}

float vec2::distance(const vec2& first, const vec2& second){
    vec2 diff = first - second;
    return sqrtf(diff.x()*diff.x() + diff.y()*diff.y());
}

vec2 rb::operator +(const vec2& v1, const vec2& v2){
    return vec2(v1._x + v2._x, v1._y + v2._y);
}

vec2 rb::operator -(const vec2& v1, const vec2& v2){
    return vec2(v1._x - v2._x, v1._y - v2._y);
}

vec2 rb::operator *(const vec2& v1, const vec2& v2){
    return vec2(v1._x * v2._x, v1._y * v2._y);
}

vec2 rb::operator /(const vec2& v1, const vec2& v2){
    assert(!almost_equal(v2._x, 0));
    assert(!almost_equal(v2._y, 0));
    return vec2(v1._x / v2._x, v1._y / v2._y);
}

vec2 rb::operator *(const vec2& v1, const float t){
    return vec2(v1._x * t, v1._y * t);
}

vec2 rb::operator *(const float t, const vec2& v1){
    return vec2(v1._x * t, v1._y * t);
}

vec2 rb::operator /(const vec2& v1, const float t){
    assert(!almost_equal(t, 0));
    return vec2(v1._x / t, v1._y / t);
}

vec2 rb::operator -(const vec2& v1){
    return vec2(-v1._x, -v1._y);
}

bool rb::operator ==(const vec2& v1, const vec2& v2){
    return almost_equal(v1._x, v2._x) && almost_equal(v1._y, v2._y);
}

bool rb::exact_match(const rb::vec2 &v1, const rb::vec2 &v2){
    return v1.x() == v2.x() && v1.y() == v2.y();
}

bool rb::operator !=(const vec2& v1, const vec2& v2){
    return !almost_equal(v1._x, v2._x) || !almost_equal(v1._y, v2._y);
}

//those are provided so vec2 can be used with container as keys
bool rb::operator >(const vec2& v1, const vec2& v2){
    if(v1._x == v2._x)
        return v1._y > v2._y;
    else
        return v1._x > v2._x;
}
bool rb::operator <(const vec2& v1, const vec2& v2){
    if(v1._x == v2._x)
        return v1._y < v2._y;
    else
        return v1._x < v2._x;
}
bool rb::operator >=(const vec2& v1, const vec2& v2){
    if(v1._x == v2._x && v1._y == v2._y)
        return true;
    
    if(v1._x == v2._x)
        return v1._y > v2._y;
    else
        return v1._x > v2._x;
}
bool rb::operator <=(const vec2& v1, const vec2& v2){
    if(v1._x == v2._x && v1._y == v2._y)
        return true;
    
    if(v1._x == v2._x)
        return v1._y < v2._y;
    else
        return v1._x < v2._x;
}

bool vec2::is_nan() const{
    return isnan(_x) || isnan(_y);
}
bool vec2::is_inf() const{
    return isinf(_x) || isinf(_y);
}
bool vec2::is_valid() const{
    return !is_nan() && !is_inf();
}







