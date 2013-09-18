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
        vec2();
        vec2(float x, float y);
        vec2(const vec2& other);
        vec2(vec2&& other);
        
        //get methods
        inline float x() const {
            return _x;
        }
        
        inline float y() const {
            return _y;
        }
        
        inline float x(float value)  {
            _x = value;
            return _x;
        }
        
        inline float y(float value)  {
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
        vec2& snap();
        vec2 rotated90(const rotation_direction rd = rotation_direction::ccw) const;
        vec2& rotate90(const rotation_direction rd = rotation_direction::ccw);
        vec2 rotated(const float angle) const;
        vec2& rotate(const float angle);
        
        //operators
        const vec2& operator=(const vec2& other);
        const vec2& operator +=(const vec2& v2);
        const vec2& operator -=(const vec2& v2);
        const vec2& operator *=(const vec2& v2);
        const vec2& operator /=(const vec2& v2);
        const vec2& operator *=(const float t);
        const vec2& operator /=(const float t);
        
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
    
    
    
    vec2 operator +(const vec2& v1, const vec2& v2);
    vec2 operator -(const vec2& v1, const vec2& v2);
    vec2 operator *(const vec2& v1, const vec2& v2);
    vec2 operator /(const vec2& v1, const vec2& v2);
    vec2 operator *(const vec2& v1, const float t);
    vec2 operator *(const float t, const vec2& v1);
    vec2 operator /(const vec2& v1, const float t);
    vec2 operator -(const vec2& v1);
    bool operator ==(const vec2& v1, const vec2& v2);
    bool operator !=(const vec2& v1, const vec2& v2);
    //those are provided so vec2 can be used with container as keys
    bool operator >(const vec2& v1, const vec2& v2);
    bool operator <(const vec2& v1, const vec2& v2);
    bool operator >=(const vec2& v1, const vec2& v2);
    bool operator <=(const vec2& v1, const vec2& v2);
    bool exact_match(const vec2& v1, const vec2& v2);
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
