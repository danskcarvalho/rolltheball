//
//  color.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 09/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__color__
#define __RollerBallCore__color__

#include "base.h"

namespace rb {
    class color {
    private:
        float _r, _g, _b, _a;
        void _from_hsl(const float h, const float s, const float l);
        void _from_hsv(const float h, const float s, const float v);
    public:
        color();
        //builder methods
        static color from_rgb(const float r, const float g, const float b);
        static color from_rgba(const float r, const float g, const float b, const float a);
        static color from_packed_rgba(uint32_t rgba);
        static color from_hsl(const float h, const float s, const float l);
        static color from_hsla(const float h, const float s, const float l, const float a);
        static color from_hsv(const float h, const float s, const float v);
        static color from_hsva(const float h, const float s, const float v, const float a);
        
        //inline properties
        inline color rbg() const {
            return from_rgb(r(), g(), b());
        }
        inline float r() const {
            return _r;
        }
        inline float r(const float value) {
            _r = value;
            return _r;
        }
        inline float g() const {
            return _g;
        }
        inline float g(const float value) {
            _g = value;
            return _g;
        }
        inline float b() const {
            return _b;
        }
        inline float b(const float value) {
            _b = value;
            return _b;
        }
        inline float a() const {
            return _a;
        }
        inline float a(const float value) {
            _a = value;
            return _a;
        }
        //other properties
        float h() const;
        float sl() const;
        float sv() const;
        float l() const;
        float v() const;
        uint32_t packed_rgba() const;
        
        //operations
        color& snap();
        color& clamp();
        color clamped() const;
        static color lerp(const float f, const color& c1, const color& c2);
        color pre_multiplied() const;
        color& pre_multiply();
        
        //to_string
        rb_string to_string() const;
        
        bool is_nan() const;
        bool is_inf() const;
        bool is_valid() const;
        
        //operators
        const color& operator +=(const color& c2);
        const color& operator -=(const color& c2);
        const color& operator *=(const color& c2);
        const color& operator /=(const color& c2);
        const color& operator *=(const float t);
        const color& operator /=(const float t);
        
        friend bool operator ==(const color&, const color&);
        friend bool operator !=(const color&, const color&);
        friend bool operator <(const color& c1, const color& c2);
        friend bool operator >(const color& c1, const color& c2);
        friend bool operator <=(const color& c1, const color& c2);
        friend bool operator >=(const color& c1, const color& c2);
        friend bool exact_match(const color& c1, const color& c2);
        friend color operator +(const color&, const color&);
        friend color operator -(const color&, const color&);
        friend color operator *(const color&, const color&);
        friend color operator /(const color&, const color&);
        friend color operator *(const color&, const float);
        friend color operator *(const float, const color&);
        friend color operator /(const color&, const float);
        friend color operator -(const color&);
    };
    
    //operators
    bool operator ==(const color& c1, const color& c2);
    bool operator !=(const color& c1, const color& c2);
    bool operator <(const color& c1, const color& c2);
    bool exact_match(const color& c1, const color& c2);
    color operator +(const color& c1, const color& c2);
    color operator -(const color& c1, const color& c2);
    color operator *(const color& c1, const color& c2);
    color operator /(const color& c1, const color& c2);
    color operator *(const color& c1, const float t);
    color operator *(const float t, const color& c2);
    color operator /(const color& c1, const float t);
    color operator -(const color& c1);
}

DEFINE_HASH(rb::color);
CLASS_NAME(rb::color);

#endif /* defined(__RollerBallCore__color__) */
