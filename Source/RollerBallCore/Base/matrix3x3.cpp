//
//  matrix3x3.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 02/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "matrix3x3.h"
#include "rectangle.h"
#include "ray.h"
#include "circle.h"
#include "polygon.h"
#include "edge.h"
#include "smooth_curve.h"
#include "mesh.h"
#include "vertex.h"

using namespace rb;

matrix3x3::matrix3x3(){
    this->_translation = vec2::zero;
    this->_x_vector = vec2::zero;
    this->_y_vector = vec2::zero;
}

matrix3x3::matrix3x3(const matrix3x3& other){
    this->_translation = other._translation;
    this->_x_vector = other._x_vector;
    this->_y_vector = other._y_vector;
}

matrix3x3::matrix3x3(matrix3x3&& other){
    _translation = other._translation;
    _x_vector = other._x_vector;
    _y_vector = other._y_vector;
}

matrix3x3::matrix3x3(const vec2& x_vector, const vec2& y_vector, const vec2& translation){
    _translation = translation;
    _x_vector = x_vector;
    _y_vector = y_vector;
}

const matrix3x3 matrix3x3::identity = matrix3x3(vec2::right, vec2::up, vec2::zero);

const matrix3x3& matrix3x3::operator*=(const rb::matrix3x3 &other){
    float l0[] = {_x_vector.x(), _y_vector.x(), _translation.x()};
    float l1[] = {_x_vector.y(), _y_vector.y(), _translation.y()};
    
    float c0[] = {other._x_vector.x(), other._x_vector.y(), 0};
    float c1[] = {other._y_vector.x(), other._y_vector.y(), 0};
    float c2[] = {other._translation.x(), other._translation.y(), 1};
    
    float m00 = l0[0]*c0[0] + l0[1]*c0[1] + l0[2]*c0[2];
    float m01 = l0[0]*c1[0] + l0[1]*c1[1] + l0[2]*c1[2];
    float m02 = l0[0]*c2[0] + l0[1]*c2[1] + l0[2]*c2[2];
    
    float m10 = l1[0]*c0[0] + l1[1]*c0[1] + l1[2]*c0[2];
    float m11 = l1[0]*c1[0] + l1[1]*c1[1] + l1[2]*c1[2];
    float m12 = l1[0]*c2[0] + l1[1]*c2[1] + l1[2]*c2[2];
    
    _x_vector.x(m00);
    _x_vector.y(m10);
    _translation.x(m02);
    
    _y_vector.x(m01);
    _y_vector.y(m11);
    _translation.y(m12);
    
    //_x_vector.snap();
    //_y_vector.snap();
    //_translation.snap();
    
    return *this;
}

matrix3x3 rb::operator*(const matrix3x3 & first, const matrix3x3 & second){
    float l0[] = {first._x_vector.x(), first._y_vector.x(), first._translation.x()};
    float l1[] = {first._x_vector.y(), first._y_vector.y(), first._translation.y()};
    
    float c0[] = {second._x_vector.x(), second._x_vector.y(), 0};
    float c1[] = {second._y_vector.x(), second._y_vector.y(), 0};
    float c2[] = {second._translation.x(), second._translation.y(), 1};
    
    float m00 = l0[0]*c0[0] + l0[1]*c0[1] + l0[2]*c0[2];
    float m01 = l0[0]*c1[0] + l0[1]*c1[1] + l0[2]*c1[2];
    float m02 = l0[0]*c2[0] + l0[1]*c2[1] + l0[2]*c2[2];
    
    float m10 = l1[0]*c0[0] + l1[1]*c0[1] + l1[2]*c0[2];
    float m11 = l1[0]*c1[0] + l1[1]*c1[1] + l1[2]*c1[2];
    float m12 = l1[0]*c2[0] + l1[1]*c2[1] + l1[2]*c2[2];
    
    auto _result = matrix3x3(vec2(m00, m10), vec2(m01, m11), vec2(m02, m12));
//    _result._x_vector.snap();
//    _result._y_vector.snap();
//    _result._translation.snap();
    return _result;
}

float matrix3x3::determinant() const {
    float   a = _x_vector.x(),
    b = _y_vector.x(),
    d = _x_vector.y(),
    e = _y_vector.y();
    
    return a*e  - b*d;
}

void matrix3x3::_invert(rb::matrix3x3 *m_ptr, bool set){
    float
    a = _x_vector.x(),
    b = _y_vector.x(),
    c = _translation.x(),
    d = _x_vector.y(),
    e = _y_vector.y(),
    f = _translation.y(),
    g = 0,
    h = 0,
    k = 1;
    
    float   A = e*k - f*h,
    B = f*g - d*k,
    C = d*h - e*g,
    D = c*h - b*k,
    E = a*k - c*g,
    F = g*b - a*h,
    G = b*f - c*e,
    H = c*d - a*f,
    K = a*e - b*d;
    
    float _determinant = determinant();
    assert(!almost_equal(_determinant, 0));
    float inv_det = 1.0f / _determinant;
    
    assert(almost_equal(inv_det * C, 0));
    assert(almost_equal(inv_det * F, 0));
    assert(almost_equal(inv_det * K, 1));
    
    if(!set){
        *m_ptr = matrix3x3(vec2(inv_det * A, inv_det * B), vec2(inv_det * D, inv_det * E), vec2(inv_det * G, inv_det * H));
        m_ptr->_x_vector.snap();
        m_ptr->_y_vector.snap();
        m_ptr->_translation.snap();
    }
    else {
        this->_x_vector = vec2(inv_det * A, inv_det * D);
        this->_y_vector = vec2(inv_det * B, inv_det * E);
        this->_translation = vec2(inv_det * G, inv_det * H);
        this->_x_vector.snap();
        this->_y_vector.snap();
        this->_translation.snap();
    }
}

matrix3x3 matrix3x3::inverse() const {
    matrix3x3 _result;
    const_cast<matrix3x3*>(this)->_invert(&_result, false);
    return _result;
}

matrix3x3& matrix3x3::invert(){
    _invert(nullptr, true);
    return *this;
}

bool matrix3x3::is_identity() const{
    return almost_equal(_x_vector.x(), 1) && almost_equal(_x_vector.y(), 0) && almost_equal(_translation.x(), 0)
    && almost_equal(_y_vector.x(), 0) && almost_equal(_y_vector.y(), 1) && almost_equal(_translation.y(), 0);
}

bool matrix3x3::is_inversible() const{
    return !almost_equal(determinant(), 0);
}

const matrix3x3& matrix3x3::operator=(const rb::matrix3x3 &other){
    this->_translation = other._translation;
    this->_x_vector = other._x_vector;
    this->_y_vector = other._y_vector;
    return *this;
}

bool rb::operator==(const matrix3x3& m1, const matrix3x3& m2){
    return almost_equal(m1._x_vector.x(), m2._x_vector.x()) && almost_equal(m1._x_vector.y(), m2._x_vector.y()) &&
        almost_equal(m1._y_vector.x(), m2._y_vector.x()) && almost_equal(m1._y_vector.y(), m2._y_vector.y()) &&
        almost_equal(m1._translation.x(), m2._translation.x()) && almost_equal(m1._translation.y(), m2._translation.y());
}

bool rb::operator!=(const matrix3x3& m1, const matrix3x3& m2){
    return !almost_equal(m1._x_vector.x(), m2._x_vector.x()) || !almost_equal(m1._x_vector.y(), m2._x_vector.y()) ||
    !almost_equal(m1._y_vector.x(), m2._y_vector.x()) || !almost_equal(m1._y_vector.y(), m2._y_vector.y()) ||
    !almost_equal(m1._translation.x(), m2._translation.x()) || !almost_equal(m1._translation.y(), m2._translation.y());
}

vec2 matrix3x3::transformed_vector(const vec2& vec) const{
    auto _result = vec2(
        _x_vector.x() * vec.x() + _y_vector.x() * vec.y(),
        _x_vector.y() * vec.x() + _y_vector.y() * vec.y()
    );
    _result.snap();
    return _result;
}

vec2& matrix3x3::transform_vector(vec2& vec) const {
    vec2 _o = vec;
    vec.x(_x_vector.x() * _o.x() + _y_vector.x() * _o.y());
    vec.y(_x_vector.y() * _o.x() + _y_vector.y() * _o.y());
    vec.snap();
    return vec;
}

vec2 matrix3x3::transformed_point(const vec2& vec) const{
    return vec2(
        _x_vector.x() * vec.x() + _y_vector.x() * vec.y() + _translation.x(),
        _x_vector.y() * vec.x() + _y_vector.y() * vec.y() + _translation.y()
    );
}

vec2& matrix3x3::transform_point(vec2& vec) const {
    vec2 _o = vec;
    vec.x(_x_vector.x() * _o.x() + _y_vector.x() * _o.y() + _translation.x());
    vec.y(_x_vector.y() * _o.x() + _y_vector.y() * _o.y() + _translation.y());
    return vec;
}

rectangle matrix3x3::transformed_rectangle(const rectangle& rc) const{
    vec2 top_left = rc.top_left();
    vec2 top_right = rc.top_right();
    vec2 bottom_left = rc.bottom_left();
    vec2 bottom_right = rc.bottom_right();
    transform_point(top_left);
    transform_point(top_right);
    transform_point(bottom_right);
    transform_point(bottom_left);
    
    vec2 _min = vec2(std::min({top_left.x(), top_right.x(), bottom_left.x(), bottom_right.x()}),
                     std::min({top_left.y(), top_right.y(), bottom_left.y(), bottom_right.y()}));
    
    vec2 _max = vec2(std::max({top_left.x(), top_right.x(), bottom_left.x(), bottom_right.x()}),
                     std::max({top_left.y(), top_right.y(), bottom_left.y(), bottom_right.y()}));
    return rectangle((_min + _max) / 2, (_max - _min));
}
rectangle& matrix3x3::transform_rectangle(rectangle& rc) const{
    vec2 top_left = rc.top_left();
    vec2 top_right = rc.top_right();
    vec2 bottom_left = rc.bottom_left();
    vec2 bottom_right = rc.bottom_right();
    transform_point(top_left);
    transform_point(top_right);
    transform_point(bottom_right);
    transform_point(bottom_left);
    
    vec2 _min = vec2(std::min({top_left.x(), top_right.x(), bottom_left.x(), bottom_right.x()}),
                     std::min({top_left.y(), top_right.y(), bottom_left.y(), bottom_right.y()}));
    
    vec2 _max = vec2(std::max({top_left.x(), top_right.x(), bottom_left.x(), bottom_right.x()}),
                     std::max({top_left.y(), top_right.y(), bottom_left.y(), bottom_right.y()}));
    rc.center((_min + _max) / 2.0f);
    rc.size(_max - _min);
    return rc;
}

ray matrix3x3::transformed_ray(const ray& r) const {
    auto _pt0 = r.origin();
    auto _pt1 = r.origin() + r.direction();
    transform_point(_pt0);
    transform_point(_pt1);
    return ray(_pt0, (_pt1 - _pt0));
}
ray& matrix3x3::transform_ray(ray& r) const {
    auto _pt0 = r.origin();
    auto _pt1 = r.origin() + r.direction();
    transform_point(_pt0);
    transform_point(_pt1);
    r.origin(_pt0);
    r.direction(_pt1 - _pt0);
    return r;
}
circle matrix3x3::transformed_circle(const circle& c) const{
    auto _pt0 = c.origin();
    auto _pt1 = c.origin() + vec2::right * c.radius();
    transform_point(_pt0);
    transform_point(_pt1);
    return circle(_pt0, vec2::distance(_pt0, _pt1));
}
circle& matrix3x3::transform_circle(circle& c) const{
    auto _pt0 = c.origin();
    auto _pt1 = c.origin() + vec2::right * c.radius();
    transform_point(_pt0);
    transform_point(_pt1);
    c.origin(_pt0);
    c.radius(vec2::distance(_pt0, _pt1));
    return c;
}

polygon matrix3x3::transformed_polygon(const polygon& p) const{
    polygon _c = p;
    return transform_polygon(_c);
}

polygon& matrix3x3::transform_polygon(polygon& p) const{
    p.clear_cache();
    for (auto& _p : p._points){
        transform_point(_p);
    }
    return p;
}

smooth_curve matrix3x3::transformed_smooth_curve(const smooth_curve& c) const{
    smooth_curve _c = c;
    return transform_smooth_curve(_c);
}
smooth_curve& matrix3x3::transform_smooth_curve(smooth_curve& c) const{
    for (auto& _c : c._cvs){
        transform_point(_c);
    }
    c.update_internal_state();
    return c;
}
mesh matrix3x3::transformed_mesh(const mesh& m) const{
    mesh _copy = m;
    return transform_mesh(_copy);
}
mesh& matrix3x3::transform_mesh(mesh& m) const{
    m.lock_vertex_buffer([this, &m](vertex* vb) {
        auto _vb_count = m.vertex_count();
        for (int i = 0; i < _vb_count; i++) {
            float _x = vb[i].x;
            vb[i].x = _x_vector.x() * _x + _y_vector.x() * vb[i].y + _translation.x();
            vb[i].y = _x_vector.y() * _x + _y_vector.y() * vb[i].y + _translation.y();
        }
    });
    return m;
}

mesh matrix3x3::transformed_texture(const mesh& m) const{
    mesh _copy = m;
    return transform_texture(_copy);
}
mesh& matrix3x3::transform_texture(mesh& m) const{
    m.lock_vertex_buffer([this, &m](vertex* vb) {
        for (int i = 0; i < m.vertex_count(); i++) {
            float _s = vb[i].s;
            vb[i].s = _x_vector.x() * _s + _y_vector.x() * vb[i].t + _translation.x();
            vb[i].t = _x_vector.y() * _s + _y_vector.y() * vb[i].t + _translation.y();
        }
    });
    return m;
}

rb_string matrix3x3::to_string() const {
    return rb::to_string("[", _x_vector, ", ", _y_vector, ", ",  _translation, "]");
}

matrix3x3 matrix3x3::build_rotation(const float angle){
    return matrix3x3(vec2(cosf(angle), sinf(angle)), vec2(-sinf(angle), cosf(angle)), vec2::zero);
}
matrix3x3 matrix3x3::build_translation(const vec2& translation){
    return matrix3x3(vec2::right, vec2::up, translation);
}
matrix3x3 matrix3x3::build_translation(const float tx, const float ty){
    return matrix3x3(vec2::right, vec2::up, vec2(tx, ty));
}
matrix3x3 matrix3x3::build_scale(const vec2& scale){
    return matrix3x3(vec2(scale.x(), 0), vec2(0, scale.y()), vec2::zero);
}
matrix3x3 matrix3x3::build_scale(const float sx, const float sy){
    return matrix3x3(vec2(sx, 0), vec2(0, sy), vec2::zero);
}
matrix3x3 matrix3x3::build_skew(const vec2& skew){
    return matrix3x3(vec2(1, skew.y()), vec2(skew.x(), 1), vec2::zero);
}
matrix3x3 matrix3x3::build_skew(const float sx, const float sy){
    return matrix3x3(vec2(1, sy), vec2(sx, 1), vec2::zero);
}
void matrix3x3::set_matrix3x3(_matrix3x3& matrix) const{
    matrix.m00 = _x_vector.x();
    matrix.m01 = _y_vector.x();
    matrix.m02 = _translation.x();
    matrix.m10 = _x_vector.y();
    matrix.m11 = _y_vector.y();
    matrix.m12 = _translation.y();
    matrix.m20 = 0;
    matrix.m21 = 0;
    matrix.m22 = 1;
}






