//
//  circle.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 04/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "circle.h"
#include "polygon.h"
#include "edge.h"

using namespace rb;

circle::circle(){
    _origin = vec2::zero;
    _radius = 1;
}
circle::circle(const vec2& origin, const float radius){
    _origin = origin;
    assert(radius > 0);
    _radius = radius;
}
polygon& circle::to_polygon(polygon& storage, int32_t quality) const{
    std::vector<vec2> _points;
    if(quality <= 4)
        quality = 4;
    
    float _step = (2.0f * M_PI) / (float)(quality);
    
    for (int i = 0; i < quality; ++i) {
        float _t = _step * i;
        float _x = _origin.x() + _radius * cosf(_t);
        float _y = _origin.y() + _radius * sinf(_t);
        _points.push_back(vec2(_x, _y));
    }
    
    return polygon::build_closed_polygon(_points, storage);
}
const circle circle::unit = circle(vec2::zero, 1);
bool circle::contains_point(const vec2& pt) const{
    return almost_less_or_equal(vec2::distance(_origin, pt), _radius);
}
float circle::distance(const vec2& pt) const{
    return vec2::distance(_origin, pt) - _radius;
}
float circle::distance(const circle& c1, const circle& c2){
    return vec2::distance(c1._origin, c2._origin) - (c1._radius + c2._radius);
}
circle& circle::move_by(const vec2& v){
    _origin += v;
    return *this;
}
circle& circle::move_by(const float x, const float y){
    _origin.x(_origin.x() + x);
    _origin.y(_origin.y() + y);
    return *this;
}
rb_string circle::to_string() const{
    return rb::to_string("[", _origin, ", ", _radius, "]");
}
bool rb::operator ==(const circle& c1, const circle& c2){
    return c1._origin == c2._origin && almost_equal(c1._radius, c2._radius);
}
bool rb::operator !=(const circle& c1, const circle& c2){
    return c1._origin != c2._origin || !almost_equal(c1._radius, c2._radius);
}