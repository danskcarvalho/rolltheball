//
//  color.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 09/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "color.h"
#include <algorithm>

using namespace rb;

color::color(){
    _r = _g = _b = _a = 0.0;
}
color& color::clamp(){
    if(_r <= 0)
        _r = 0;
    else if(_r >= 1)
        _r = 1;
    
    if(_g <= 0)
        _g = 0;
    else if(_g >= 1)
        _g = 1;
    
    if(_b <= 0)
        _b = 0;
    else if(_b >= 1)
        _b = 1;
    
    if(_a <= 0)
        _a = 0;
    else if(_a >= 1)
        _a = 1;
    return *this;
}
color color::clamped() const{
    color _c = *this;
    _c.clamp();
    return _c;
}
color color::from_rgb(const float r, const float g, const float b){
    color _c;
    
    _c._r = r;
    _c._g = g;
    _c._b = b;
    _c._a = 1.0f;
    
    return _c;
}
color color::from_rgba(const float r, const float g, const float b, const float a){
    color _c;
    
    _c._r = r;
    _c._g = g;
    _c._b = b;
    _c._a = a;
    
    return _c;
}
color color::from_packed_rgba(uint32_t rgba){
    uint8_t r = (rgba & 0xFF000000) >> 24;
    uint8_t g = (rgba & 0x00FF0000) >> 16;
    uint8_t b = (rgba & 0x0000FF00) >> 8;
    uint8_t a = (rgba & 0x000000FF);
    
    color _c;
    _c._r = (float)r / 255.0f;
    _c._g = (float)g / 255.0f;
    _c._b = (float)b / 255.0f;
    _c._a = (float)a / 255.0f;
    return _c;
}

void color::_from_hsl(const float h, const float s, const float l){
    float _h = h;
    float _s = s;
    float _l = l;
    
    float _h_s = _h >= 0 ? 1 : -1;
    
    if (fabsf(_h) > (2 * M_PI))
        _h =  _h_s * (fabsf(_h) - floorf(fabsf(_h) / (2 * M_PI)) * (2 * M_PI));
    
    if(_h < 0)
        _h = 2 * M_PI + _h;
    
    if(_s >= 1)
        _s = 1;
    else if(_s <= 0)
        _s = 0;

    if(_l >= 1)
        _l = 1;
    else if(_l <= 0)
        _l = 0;
    
    _h = TO_DEGREES(_h) / 60.0f;
    float c = (1 - fabsf(2 * _l - 1)) * _s;
    float x = c * (1 - fabsf(fmodf(_h, 2) - 1));
    float _r, _g, _b;
    
    if(_h < 1){
        _r = c;
        _g = x;
        _b = 0;
    }
    else if(_h < 2){
        _r = x;
        _g = c;
        _b = 0;
    }
    else if(_h < 3){
        _r = 0;
        _g = c;
        _b = x;
    }
    else if(_h < 4){
        _r = 0;
        _g = x;
        _b = c;
    }
    else if(_h < 5){
        _r = x;
        _g = 0;
        _b = c;
    }
    else if(_h < 6){
        _r = c;
        _g = 0;
        _b = x;
    }
    else {
        _r = _g = _b = 0;
    }
    
    float m = _l - 0.5f * c;
    this->_r = _r + m;
    this->_g = _g + m;
    this->_b = _b + m;
}
void color::_from_hsv(const float h, const float s, const float v){
    float _h = h;
    float _s = s;
    float _v
    = v;
    
    float _h_s = _h >= 0 ? 1 : -1;
    
    if (fabsf(_h) > (2 * M_PI))
        _h =  _h_s * (fabsf(_h) - floorf(fabsf(_h) / (2 * M_PI)) * (2 * M_PI));
    
    if(_h < 0)
        _h = 2 * M_PI + _h;
    
    if(_s >= 1)
        _s = 1;
    else if(_s <= 0)
        _s = 0;
    
    if(_v >= 1)
        _v = 1;
    else if(_v <= 0)
        _v = 0;
    
    _h = TO_DEGREES(_h) / 60.0f;
    float c = _v * _s;
    float x = c * (1 - fabsf(fmodf(_h, 2) - 1));
    float _r, _g, _b;
    
    if(_h < 1){
        _r = c;
        _g = x;
        _b = 0;
    }
    else if(_h < 2){
        _r = x;
        _g = c;
        _b = 0;
    }
    else if(_h < 3){
        _r = 0;
        _g = c;
        _b = x;
    }
    else if(_h < 4){
        _r = 0;
        _g = x;
        _b = c;
    }
    else if(_h < 5){
        _r = x;
        _g = 0;
        _b = c;
    }
    else if(_h < 6){
        _r = c;
        _g = 0;
        _b = x;
    }
    else {
        _r = _g = _b = 0;
    }
    
    float m = _v - c;
    this->_r = _r + m;
    this->_g = _g + m;
    this->_b = _b + m;
}

color color::from_hsl(const float h, const float s, const float l){
    color _c;
    _c._from_hsl(h, s, l);
    return _c;
}
color color::from_hsla(const float h, const float s, const float l, const float a){
    color _c;
    _c._from_hsl(h, s, l);
    _c._a = a;
    return _c;
}
color color::from_hsv(const float h, const float s, const float v){
    color _c;
    _c._from_hsv(h, s, v);
    return _c;
}
color color::from_hsva(const float h, const float s, const float v, const float a){
    color _c;
    _c._from_hsv(h, s, v);
    _c._a = a;
    return _c;
}
float color::h() const{
    float as = 0.5f * (2 * _r - _g - _b);
    float bs = 0.5f * (sqrtf(3) * (_g - _b));
    float h = atan2f(bs, as);
    
    if(h < 0)
        h = 2 * M_PI + h;
    
    return h;
}
float color::sl() const{
    auto M = fmaxf(fmaxf(_r, _g), _b);
    auto m = fminf(fminf(_r, _g), _b);
    float c = M - m;
    if(c == 0)
        return 0;
    else{
        auto _l = l();
        return c / (1 - fabsf(2 * _l - 1));
    }
}
float color::sv() const{
    auto M = fmaxf(fmaxf(_r, _g), _b);
    auto m = fminf(fminf(_r, _g), _b);
    float c = M - m;
    if(c == 0)
        return 0;
    else
    {
        auto _v = v();
        return c / _v;
    }
}
float color::l() const{
    auto M = fmaxf(fmaxf(_r, _g), _b);
    auto m = fminf(fminf(_r, _g), _b);
    return 0.5f * (M + m);
}
float color::v() const{
    return fmaxf(fmaxf(_r, _g), _b);
}
uint32_t color::packed_rgba() const{
    auto _pr = (int32_t)(_r * 255.0f) & 0xFF;
    auto _pg = (int32_t)(_g * 255.0f) & 0xFF;
    auto _pb = (int32_t)(_b * 255.0f) & 0xFF;
    auto _pa = (int32_t)(_a * 255.0f) & 0xFF;
    return (_pr <<  0) | (_pg << 8) | (_pb << 16) | (_pa << 24);
}

color color::pre_multiplied() const {
    return color::from_rgba(_r * _a, _g * _a, _b * _a, _a);
}

color& color::pre_multiply() {
    _r *= _a;
    _b *= _a;
    _g *= _a;
    return *this;
}

color& color::snap(){
    if(almost_equal(_r, 0))
        _r = 0;
    if(almost_equal(_r, 1))
        _r = 1;
    
    if(almost_equal(_g, 0))
        _g = 0;
    if(almost_equal(_g, 1))
        _g = 1;
    
    if(almost_equal(_b, 0))
        _b = 0;
    if(almost_equal(_b, 1))
        _b = 1;
    
    if(almost_equal(_a, 0))
        _a = 0;
    if(almost_equal(_a, 1))
        _a = 1;
    return *this;
}
color color::lerp(const float f, const color& c1, const color& c2){
    return (1 - f)*c1 + f*c2;
}
rb_string color::to_string() const{
    return rb::to_string("(", _r, ", ", _g, ", ", _b, ", ", _a, ")");
}
bool color::is_nan() const{
    return isnan(_r) || isnan(_g) || isnan(_b) || isnan(_a);
}
bool color::is_inf() const{
    return isinf(_r) || isinf(_g) || isinf(_b) || isinf(_a);
}
bool color::is_valid() const{
    return !is_nan() && !is_inf();
}
const color& color::operator +=(const color& c2){
    _r += c2._r;
    _g += c2._g;
    _b += c2._b;
    _a += c2._a;
    return *this;
}
const color& color::operator -=(const color& c2){
    _r -= c2._r;
    _g -= c2._g;
    _b -= c2._b;
    _a -= c2._a;
    return *this;
}
const color& color::operator *=(const color& c2){
    _r *= c2._r;
    _g *= c2._g;
    _b *= c2._b;
    _a *= c2._a;
    return *this;
}
const color& color::operator /=(const color& c2){
    _r /= c2._r;
    _g /= c2._g;
    _b /= c2._b;
    _a /= c2._a;
    return *this;
}
const color& color::operator *=(const float t){
    _r *= t;
    _g *= t;
    _b *= t;
    _a *= t;
    return *this;
}
const color& color::operator /=(const float t){
    _r /= t;
    _g /= t;
    _b /= t;
    _a /= t;
    return *this;
}
bool rb::operator ==(const color& c1, const color& c2){
    return almost_equal(c1._r, c2._r) && almost_equal(c1._g, c2._g) && almost_equal(c1._b, c2._b) && almost_equal(c1._a, c2._a);
}

bool rb::operator <(const color& c1, const color& c2) {
    if(c1._r == c2._r){
        if(c1._g == c2._g){
            if(c1._b == c2._b){
                return c1._a < c2._a;
            }
            else
                return c1._b < c2._b;
        }
        else
            return c1._g < c2._g;
    }
    else
        return c1._r < c2._r;
}

bool rb::operator >(const color& c1, const color& c2) {
    if(c1._r == c2._r){
        if(c1._g == c2._g){
            if(c1._b == c2._b){
                return c1._a > c2._a;
            }
            else
                return c1._b > c2._b;
        }
        else
            return c1._g > c2._g;
    }
    else
        return c1._r > c2._r;
}

bool rb::operator <=(const color& c1, const color& c2) {
    if(exact_match(c1, c2))
        return true;
    
    if(c1._r == c2._r){
        if(c1._g == c2._g){
            if(c1._b == c2._b){
                return c1._a < c2._a;
            }
            else
                return c1._b < c2._b;
        }
        else
            return c1._g < c2._g;
    }
    else
        return c1._r < c2._r;
}

bool rb::operator >=(const color& c1, const color& c2) {
    if(exact_match(c1, c2))
        return true;
    
    if(c1._r == c2._r){
        if(c1._g == c2._g){
            if(c1._b == c2._b){
                return c1._a > c2._a;
            }
            else
                return c1._b > c2._b;
        }
        else
            return c1._g > c2._g;
    }
    else
        return c1._r > c2._r;
}

bool rb::operator !=(const color& c1, const color& c2){
    return !almost_equal(c1._r, c2._r) || !almost_equal(c1._g, c2._g) || !almost_equal(c1._b, c2._b) || !almost_equal(c1._a, c2._a);
}

bool rb::exact_match(const rb::color &c1, const rb::color &c2){
    return c1._r == c2._r && c1._b == c2._b && c1._g == c2._g && c1._a == c2._a;
}

color rb::operator +(const color& c1, const color& c2){
    color _c = c1;
    _c += c2;
    return _c;
}
color rb::operator -(const color& c1, const color& c2){
    color _c = c1;
    _c -= c2;
    return _c;
}
color rb::operator *(const color& c1, const color& c2){
    color _c = c1;
    _c *= c2;
    return _c;
}
color rb::operator /(const color& c1, const color& c2){
    color _c = c1;
    _c /= c2;
    return _c;
}
color rb::operator *(const color& c1, const float t){
    color _c = c1;
    _c *= t;
    return _c;
}
color rb::operator *(const float t, const color& c2){
    color _c = c2;
    _c *= t;
    return _c;
}
color rb::operator /(const color& c1, const float t){
    color _c = c1;
    _c /= t;
    return _c;
}
color rb::operator -(const color& c1){
    return color::from_rgba(-c1._r, -c1._g, -c1._b, -c1._a);
}








