//
//  vec2.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 01/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__vec2__
#define __RollerBallCore__vec2__

#include "base.h"

namespace rb {
    class vec2 {
    private:
        float _x, _y;
    public:
        const static vec2 zero;
        const static vec2 up;
        const static vec2 right;
        const static vec2 down;
        const static vec2 left;
        
        //constructors
        inline __attribute__ ((always_inline)) vec2(){
            _x = 0;
            _y = 0;
        }
        
        inline __attribute__ ((always_inline)) vec2(float x, float y){
            _x = x;
            _y = y;
        }
        
        inline __attribute__ ((always_inline)) vec2(const vec2& other){
            _x = other._x;
            _y = other._y;
        }
        
        inline __attribute__ ((always_inline)) vec2(vec2&& other){
            _x = other._x;
            _y = other._y;
        }
        
        //get methods
        inline __attribute__ ((always_inline)) float x() const {
            return _x;
        }
        
        inline __attribute__ ((always_inline)) float y() const {
            return _y;
        }
        
        inline __attribute__ ((always_inline)) float x(float value)  {
            _x = value;
            return _x;
        }
        
        inline __attribute__ ((always_inline)) float y(float value)  {
            _y = value;
            return _y;
        }
        
        //tests
        bool is_zero() const;
        bool is_unit() const;
        bool is_perpendicular_to(const vec2& other) const;
        bool is_parallel_to(const vec2& other) const;
        bool points_at_same_direction(const vec2& other) const;
        
        //information
        //angle between two vectors [0, +2PI]
        float angle_between(const vec2& other, const rotation_direction rd = rotation_direction::shortest) const;
        static float distance(const vec2& first, const vec2& second);
        
        //operations
//        vec2& snap();
        vec2 rotated90(const rotation_direction rd = rotation_direction::ccw) const;
        vec2& rotate90(const rotation_direction rd = rotation_direction::ccw);
        vec2 rotated(const float angle) const;
        vec2& rotate(const float angle);
        
        //operators
        inline __attribute__ ((always_inline)) const vec2& operator=(const rb::vec2 &other){
            this->_x = other._x;
            this->_y = other._y;
            return *this;
        }
        inline __attribute__ ((always_inline)) const vec2& operator +=(const vec2& v2){
            _x += v2._x;
            _y += v2._y;
            return *this;
        }
        inline __attribute__ ((always_inline)) const vec2& operator -=(const vec2& v2){
            _x -= v2._x;
            _y -= v2._y;
            return *this;
        }
        inline __attribute__ ((always_inline)) const vec2& operator *=(const vec2& v2){
            _x *= v2._x;
            _y *= v2._y;
            return *this;
        }
        inline __attribute__ ((always_inline)) const vec2& operator /=(const vec2& v2){
            assert(!almost_equal(v2._x, 0));
            assert(!almost_equal(v2._y, 0));
            _x /= v2._x;
            _y /= v2._y;
            return *this;
        }
        inline __attribute__ ((always_inline)) const vec2& operator *=(const float t){
            _x *= t;
            _y *= t;
            return *this;
        }
        inline __attribute__ ((always_inline)) const vec2& operator /=(const float t){
            assert(!almost_equal(t, 0));
            _x /= t;
            _y /= t;
            return *this;
        }
        
        //basic mathematical
        float squared_length() const;
        float length() const;
        vec2 normalized() const;
        vec2& normalize();
        
        //other functions
        static float dot(const vec2& v1, const vec2& v2);
        static float cross(const vec2& v1, const vec2& v2);
        static vec2 lerp(const float f, const vec2& v1, const vec2& v2);
        static vec2 slerp(const float f, const vec2& v1, const vec2& v2, const rotation_direction rd = rotation_direction::shortest);
        vec2 projection(const vec2& v) const;
        vec2& project(const vec2& v);
        
        //to_string
        rb_string to_string() const;
        
        bool is_nan() const;
        bool is_inf() const;
        bool is_valid() const;
        
        friend vec2 operator +(const vec2& v1, const vec2& v2);
        friend vec2 operator -(const vec2& v1, const vec2& v2);
        friend vec2 operator *(const vec2& v1, const vec2& v2);
        friend vec2 operator /(const vec2& v1, const vec2& v2);
        friend vec2 operator *(const vec2& v1, const float t);
        friend vec2 operator *(const float t, const vec2& v1);
        friend vec2 operator /(const vec2& v1, const float t);
        friend vec2 operator -(const vec2& v1);
        friend bool operator ==(const vec2& v1, const vec2& v2);
        friend bool operator !=(const vec2& v1, const vec2& v2);
        friend bool operator >(const vec2& v1, const vec2& v2);
        friend bool operator <(const vec2& v1, const vec2& v2);
        friend bool operator >=(const vec2& v1, const vec2& v2);
        friend bool operator <=(const vec2& v1, const vec2& v2);
    };
    
    
    inline __attribute__ ((always_inline)) vec2 operator +(const vec2& v1, const vec2& v2){
        return vec2(v1._x + v2._x, v1._y + v2._y);
    }
    
    inline __attribute__ ((always_inline)) vec2 operator -(const vec2& v1, const vec2& v2){
        return vec2(v1._x - v2._x, v1._y - v2._y);
    }
    
    inline __attribute__ ((always_inline)) vec2 operator *(const vec2& v1, const vec2& v2){
        return vec2(v1._x * v2._x, v1._y * v2._y);
    }
    
    inline __attribute__ ((always_inline)) vec2 operator /(const vec2& v1, const vec2& v2){
        assert(!almost_equal(v2._x, 0));
        assert(!almost_equal(v2._y, 0));
        return vec2(v1._x / v2._x, v1._y / v2._y);
    }
    
    inline __attribute__ ((always_inline)) vec2 operator *(const vec2& v1, const float t){
        return vec2(v1._x * t, v1._y * t);
    }
    
    inline __attribute__ ((always_inline)) vec2 operator *(const float t, const vec2& v1){
        return vec2(v1._x * t, v1._y * t);
    }
    
    inline __attribute__ ((always_inline)) vec2 operator /(const vec2& v1, const float t){
        assert(!almost_equal(t, 0));
        return vec2(v1._x / t, v1._y / t);
    }
    
    inline __attribute__ ((always_inline)) vec2 operator -(const vec2& v1){
        return vec2(-v1._x, -v1._y);
    }
    
    inline __attribute__ ((always_inline)) bool operator ==(const vec2& v1, const vec2& v2){
        return almost_equal(v1._x, v2._x) && almost_equal(v1._y, v2._y);
    }
    
    inline __attribute__ ((always_inline)) bool exact_match(const rb::vec2 &v1, const rb::vec2 &v2){
        return v1.x() == v2.x() && v1.y() == v2.y();
    }
    
    inline __attribute__ ((always_inline)) bool operator !=(const vec2& v1, const vec2& v2){
        return !almost_equal(v1._x, v2._x) || !almost_equal(v1._y, v2._y);
    }
    
    //those are provided so vec2 can be used with container as keys
    inline __attribute__ ((always_inline)) bool operator >(const vec2& v1, const vec2& v2){
        if(v1._x == v2._x)
            return v1._y > v2._y;
        else
            return v1._x > v2._x;
    }
    inline __attribute__ ((always_inline)) bool operator <(const vec2& v1, const vec2& v2){
        if(v1._x == v2._x)
            return v1._y < v2._y;
        else
            return v1._x < v2._x;
    }
    inline __attribute__ ((always_inline)) bool operator >=(const vec2& v1, const vec2& v2){
        if(v1._x == v2._x && v1._y == v2._y)
            return true;
        
        if(v1._x == v2._x)
            return v1._y > v2._y;
        else
            return v1._x > v2._x;
    }
    inline __attribute__ ((always_inline)) bool operator <=(const vec2& v1, const vec2& v2){
        if(v1._x == v2._x && v1._y == v2._y)
            return true;
        
        if(v1._x == v2._x)
            return v1._y < v2._y;
        else
            return v1._x < v2._x;
    }
}

DEFINE_HASH(rb::vec2);
CLASS_NAME(rb::vec2);

namespace std {
    template<>
    struct equal_to<rb::vec2> {
        bool operator()( const rb::vec2& lhs, const rb::vec2& rhs ) const{
            return exact_match(lhs, rhs);
        }
    };
}

#endif /* defined(__RollerBallCore__vec2__) */
