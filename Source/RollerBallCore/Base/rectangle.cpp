//
//  rectangle.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 04/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "rectangle.h"
#include "polygon.h"
#include "edge.h"
#include "mesh.h"
#include "transform_space.h"
#include "texture_map.h"
#include <CoreGraphics/CGGeometry.h>

using namespace rb;

class simple_texture_map : public texture_map {
private:
    transform_space _texture_transform;
    rectangle _bounds;
public:
    simple_texture_map(const transform_space& texture_transform, const rectangle& bounds);
    virtual void set_texture_coords(vertex& v) const;
    virtual const rectangle& bounds() const;
    virtual const transform_space& get_texture_space() const;
    virtual ~simple_texture_map();
};

simple_texture_map::simple_texture_map(const transform_space& texture_transform, const rectangle& bounds){
    _texture_transform = texture_transform;
    _bounds = bounds;
}
void simple_texture_map::set_texture_coords(vertex& v) const {
    auto _coords = _texture_transform.from_base_to_space().transformed_point(v.get_position());
    v.set_texture_coords(_coords);
    v.set_texture_bounds(_bounds);
}

const rectangle& simple_texture_map::bounds() const{
    return _bounds;
}

const transform_space& simple_texture_map::get_texture_space() const {
    return _texture_transform;
}

simple_texture_map::~simple_texture_map(){
    
}

rectangle::rectangle(){
    _center = vec2::zero;
    _size = vec2::zero;
}
rectangle::rectangle(const vec2& center, const vec2& size){
    _center = center;
    assert(almost_greather_or_equal(_size.x(), 0));
    assert(almost_greather_or_equal(_size.y(), 0));
    _size = size;
    
    if(_size.x() < 0)
        _size.x(0);
    
    if(_size.y() < 0)
        _size.y(0);
}
rectangle::rectangle(const float center_x, const float center_y, const float width, const float height){
    _center = vec2(center_x, center_y);
    assert(almost_greather_or_equal(width, 0));
    assert(almost_greather_or_equal(height, 0));
    _size = vec2(width, height);
    if(_size.x() < 0)
        _size.x(0);
    
    if(_size.y() < 0)
        _size.y(0);
}
mesh& rectangle::to_mesh(mesh& storage, const rectangle& texture_bounds, const float max_s, const float max_t, const uint32_t subdivisions){
    polygon _polygon;
    assert(max_s > 0);
    assert(max_t > 0);
    assert(subdivisions >= 0);
    
    to_polygon(_polygon);
    auto _x_len = (bottom_right().x() - bottom_left().x()) / max_s;
    auto _y_len = (top_left().y() - bottom_left().y()) / max_t;
    auto _transform = transform_space(bottom_left(), vec2(_x_len, _y_len), 0);
    simple_texture_map* _map = new simple_texture_map(_transform, texture_bounds);
    _polygon.to_mesh(storage, subdivisions, *_map);
    delete _map;
    return storage;
}

std::vector<mesh*> rectangle::to_meshes(const rectangle& texture_bounds, const uint32_t h_subdivs, const uint32_t v_subdivs){
    //Texture
    auto _tw = texture_bounds.size().x() / (float)h_subdivs;
    auto _th = texture_bounds.size().y() / (float)v_subdivs;
    auto _tstart = texture_bounds.bottom_left();
    //Rectangle
    auto _w = size().x() / (float)h_subdivs;
    auto _h = size().y() / (float)v_subdivs;
    auto _start = bottom_left();
    
    std::vector<mesh*> _meshes;
    
    for (uint32_t i = 0; i < h_subdivs; i++) {
        for (uint32_t j = 0; j < v_subdivs; j++) {
            //Texture
            auto _tsw = _tstart + vec2::right * i * _tw;
            auto _tew = _tsw + vec2::right * _tw;
            auto _tsh = _tstart + vec2::up * j * _th;
            auto _teh = _tsh + vec2::up * _th;
            auto _tr = rectangle((_tsw.x() + _tew.x()) / 2.0f, (_tsh.y() + _teh.y()) / 2.0f, fabsf(_tew.x() - _tsw.x()), fabs(_teh.y() - _tsh.y()));
            //Rectangle
            auto _sw = _start + vec2::right * i * _w;
            auto _ew = _sw + vec2::right * _w;
            auto _sh = _start + vec2::up * j * _h;
            auto _eh = _sh + vec2::up * _h;
            auto _r = rectangle((_sw.x() + _ew.x()) / 2.0f, (_sh.y() + _eh.y()) / 2.0f, fabsf(_ew.x() - _sw.x()), fabs(_eh.y() - _sh.y()));
            
            mesh* _m = new mesh();
            _r.to_mesh(*_m, _tr);
            _meshes.push_back(_m);
        }
    }
    return _meshes;
}

polygon& rectangle::to_polygon(polygon& storage) const{
    std::vector<vec2> _points;
    _points.push_back(top_left());
    _points.push_back(bottom_left());
    _points.push_back(bottom_right());
    _points.push_back(top_right());
    return polygon::build_closed_polygon(_points, storage);
}
const rectangle rectangle::unit = rectangle(0, 0, 1, 1);
float rectangle::area() const{
    return _size.x() * _size.y();
}
rectangle& rectangle::move_by(const vec2& v){
    _center += v;
    return *this;
}
rb_string rectangle::to_string() const{
    return rb::to_string("[", _center, ", ", _size, "]");
}
bool rb::operator ==(const rectangle& first, const rectangle& second){
    return first._center == second._center && first._size == second._size;
}
bool rb::operator!=(const rectangle& first, const rectangle& second){
    return first._center != second._center || first._size != second._size;
}

bool rectangle::is_null() const {
    return _center == vec2::zero && _size == vec2::zero;
}

bool rectangle::intersects(const rb::vec2 &pt) const {
    auto _rc = CGRectMake(bottom_left().x(), bottom_left().y(), size().x(), size().y());
    auto _pt = CGPointMake(pt.x(), pt.y());
    return CGRectContainsPoint(_rc, _pt);
}

bool rectangle::intersects(const rb::rectangle &rc) const {
    auto _rc = CGRectMake(bottom_left().x(), bottom_left().y(), size().x(), size().y());
    auto _rc2 = CGRectMake(rc.bottom_left().x(), rc.bottom_left().y(), rc.size().x(), rc.size().y());
    return CGRectIntersectsRect(_rc, _rc2);
}

bool rectangle::contains(const rb::rectangle &rc) const {
    auto _rc = CGRectMake(bottom_left().x(), bottom_left().y(), size().x(), size().y());
    auto _rc2 = CGRectMake(rc.bottom_left().x(), rc.bottom_left().y(), rc.size().x(), rc.size().y());
    return CGRectContainsRect(_rc, _rc2);
}

rectangle rectangle::compute_intersection(const rb::rectangle &another) const {
    if(is_null() || another.is_null())
        return rectangle(vec2::zero, vec2::zero);
    
    auto _rc = CGRectMake(bottom_left().x(), bottom_left().y(), size().x(), size().y());
    auto _rc2 = CGRectMake(another.bottom_left().x(), another.bottom_left().y(), another.size().x(), another.size().y());
    auto _frc = CGRectIntersection(_rc, _rc2);
    return rectangle(CGRectGetMidX(_frc), CGRectGetMidY(_frc), _frc.size.width, _frc.size.height);
}

rectangle rectangle::compute_union(const rb::rectangle &another) const {
    if(is_null())
        return another;
    if(another.is_null())
        return *this;
    
    auto _rc = CGRectMake(bottom_left().x(), bottom_left().y(), size().x(), size().y());
    auto _rc2 = CGRectMake(another.bottom_left().x(), another.bottom_left().y(), another.size().x(), another.size().y());
    auto _frc = CGRectUnion(_rc, _rc2);
    return rectangle(CGRectGetMidX(_frc), CGRectGetMidY(_frc), _frc.size.width, _frc.size.height);
}






















