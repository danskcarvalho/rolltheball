//
//  edge.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 04/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "edge.h"
#include "ray.h"

using namespace rb;

edge::edge(){
    _pt0 = vec2::zero;
    _pt1 = vec2::zero;
    _normal = vec2::zero;
}
edge::edge(const vec2& pt0, const vec2& pt1, const vec2& normal){
    _pt0 = pt0;
    _pt1 = pt1;
    _normal = normal;
    if(!_normal.is_unit())
        _normal = _normal.normalized();
    else
        _normal = normal;
}
bool edge::is_degenerate() const{
    return almost_equal(length(), 0);
}
float edge::length() const{
    return vec2::distance(_pt0, _pt1);
}
float edge::distance(const vec2& pt) const{
    //let's construct a ray that starts at pt0
    ray r = ray(_pt0, _pt1 - _pt0);
    auto t = r.get_parameter(pt);
    
    if(almost_less_or_equal(t, 0))
        return vec2::distance(_pt0, pt);
    else if(almost_greather_or_equal(t, length()))
        return vec2::distance(_pt1, pt);
    else
        return r.distance(pt);
}
nullable<vec2> edge::intersection(const edge& e1, const edge& e2){
    float _e1_len = e1.length();
    float _e2_len = e2.length();
    if(almost_equal(_e1_len, 0) || almost_equal(_e2_len, 0)) {
        if(almost_equal(_e1_len, 0) && almost_equal(_e2_len, 0)){
            if(e1.pt0() == e2.pt0())
                return e1.pt0();
            else
                return nullptr;
        }
        else {
            if(almost_equal(_e1_len, 0)){
                if(almost_equal(e2.distance(e1.pt0()), 0))
                    return e1.pt0();
                else
                    return nullptr;
            }
            else {
                if(almost_equal(e1.distance(e2.pt0()), 0))
                    return e2.pt0();
                else
                    return nullptr;
            }
        }
    }
    auto r1 = ray(e1._pt0, e1._pt1 - e1._pt0);
    auto r2 = ray(e2._pt0, e2._pt1 - e2._pt0);
    
    auto _i = ray::intersection(r1, r2);
    
    if(!_i.has_value())
        return nullptr;
    
    auto _r1_p = r1.get_parameter(_i.value());
    auto _r2_p = r2.get_parameter(_i.value());
    
    //its out of the edges?
    if ((almost_greather(_r1_p, e1.length()) || almost_less(_r1_p, 0)) ||
        (almost_greather(_r2_p, e2.length()) || almost_less(_r2_p, 0)))
        return nullptr;
    
    //we don't report an intersection if the intersection point are contained in the edge points of both edges...
    if ((almost_equal(_r1_p, e1.length()) || almost_equal(_r1_p, 0)) &&
        (almost_equal(_r2_p, e2.length()) || almost_equal(_r2_p, 0)))
        return nullptr;
    
    return _i;
}

rb_string edge::to_string() const{
    return rb::to_string("[", _pt0, ", ", _pt1, ", ", _normal, "]");
}

bool edge::test_intersection(const edge& e1, const edge& e2){
    float _e1_len = e1.length();
    float _e2_len = e2.length();
    if(almost_equal(_e1_len, 0) || almost_equal(_e2_len, 0))
    {
        //TODO: Fazer quando eu chegar...
        if(almost_equal(_e1_len, 0) && almost_equal(_e2_len, 0))
            return e1.pt0() == e2.pt0();
        
        if(almost_equal(_e1_len, 0))
            return almost_equal(e2.distance(e1.pt0()), 0);
        else
            return almost_equal(e1.distance(e2.pt0()), 0);
    }
    
    auto r1 = ray(e1._pt0, e1._pt1 - e1._pt0);
    auto r2 = ray(e2._pt0, e2._pt1 - e2._pt0);
    if(r1.is_collinear_to(r2)){
        bool _e1_eq_e2 = e1._pt0 == e2._pt0 && e1._pt1 == e2._pt1;
        bool _e1_eq_e2_inv = e1._pt0 == e2._pt1 && e1._pt1 == e2._pt0;
        
        if(_e1_eq_e2 || _e1_eq_e2_inv)
            return true;
        
        auto _e2_len = e2.length();
        auto _t_e1_0 = r2.get_parameter(e1._pt0);
        if(almost_greather(_t_e1_0, 0) && almost_less(_t_e1_0, _e2_len))
            return true;
        
        auto _t_e1_1 = r2.get_parameter(e1._pt1);
        if(almost_greather(_t_e1_1, 0) && almost_less(_t_e1_1, _e2_len))
            return true;
        
        auto _e1_len = e1.length();
        auto _t_e2_0 = r1.get_parameter(e2._pt0);
        if(almost_greather(_t_e2_0, 0) && almost_less(_t_e2_0, _e1_len))
            return true;
        
        auto _t_e2_1 = r1.get_parameter(e2._pt1);
        if(almost_greather(_t_e2_1, 0) && almost_less(_t_e2_1, _e1_len))
            return true;
        
        return false;
    }
    else
        return intersection(e1, e2).has_value();
}
 
edge edge::translate(const vec2& m, bool flip_normal) const{
    return edge(_pt0 + m, _pt1 + m, flip_normal ? -_normal : _normal);
}

bool rb::operator ==(const edge& e1, const edge& e2){
    return e1._pt0 == e2._pt0 && e1._pt1 == e2._pt1 && e1._normal == e2._normal;
}
bool rb::operator !=(const edge& e1, const edge& e2){
    return e1._pt0 != e2._pt0 || e1._pt1 != e2._pt1 || e1._normal != e2._normal;
}