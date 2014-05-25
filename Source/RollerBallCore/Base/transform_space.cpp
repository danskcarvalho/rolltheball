//
//  transform_space.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 03/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "transform_space.h"
#include "buffer.h"

using namespace rb;

void transform_space::init(){
    _dirty = true;
    _axisRotation = vec2(0, M_PI / 2.0f);
    _axisScale = vec2(1, 1);
    _origin = vec2::zero;
    _matrix = matrix3x3::identity;
    _inv_matrix = nullptr;
}
void transform_space::update_matrix(){
    if (!_dirty)
        return;
    
    vec2 _x_vec = vec2::right;
    vec2 _y_vec = vec2::right;
    matrix3x3 _x_rotation = matrix3x3::build_rotation(_axisRotation.x());
    matrix3x3 _y_rotation = matrix3x3::build_rotation(_axisRotation.y());
    _x_vec *= _axisScale.x();
    _y_vec *= _axisScale.y();
    _x_rotation.transform_vector(_x_vec);
    _y_rotation.transform_vector(_y_vec);
    _matrix = matrix3x3(_x_vec, _y_vec, _origin);
    _inv_matrix = nullptr;
    _dirty = false;
}

void transform_space::update_inv_matrix(){
    if(_inv_matrix.has_value())
        return;
    auto _determinant = _matrix.determinant();
    if (_determinant == 0){
        //not inversible
        _inv_matrix = nullptr;
    }
    else {
        _inv_matrix = _matrix.inverse();
    }
}

void transform_space::update_from_matrix(const matrix3x3& m){
    vec2 _x_vec = m.x_vector();
    vec2 _y_vec = m.y_vector();
    _origin = m.translation();
    _axisScale = vec2(_x_vec.length(), _y_vec.length());
    _axisRotation = vec2::zero;
    if((_x_vec != vec2::zero))
        _axisRotation.x(vec2::right.angle_between(_x_vec, rotation_direction::ccw));
    if((_y_vec != vec2::zero))
        _axisRotation.y(vec2::right.angle_between(_y_vec, rotation_direction::ccw));
    //Reduce numeric errors...
    if (almost_equal(_axisRotation.x(), 0))
        _axisRotation.x(0);
    if (almost_equal(_axisRotation.y(), 0))
        _axisRotation.y(0);
    
    auto _m_pi_2 = M_PI / 2;
    if (almost_equal(_axisRotation.x(), _m_pi_2))
        _axisRotation.x(_m_pi_2);
    if (almost_equal(_axisRotation.y(), _m_pi_2))
        _axisRotation.y(_m_pi_2);
    
    _matrix = m;
    _inv_matrix = nullptr;
    _dirty = false;
}
transform_space transform_space::from_matrix(const matrix3x3& m){
    transform_space ts;
    ts.update_from_matrix(m);
    return ts;
}
transform_space::transform_space(buffer buffer){
    memcpy(this, buffer.internal_buffer(), sizeof(transform_space));
}
transform_space::transform_space(){
    init();
}
transform_space::transform_space(const transform_space& other){
    _dirty = other._dirty;
    _axisRotation = other._axisRotation;
    _axisScale = other._axisScale;
    _origin = other._origin;
    _matrix = other._matrix;
    _inv_matrix = other._inv_matrix;
}
transform_space::transform_space(transform_space&& other){
    _dirty = other._dirty;
    _axisRotation = other._axisRotation;
    _axisScale = other._axisScale;
    _origin = other._origin;
    _matrix = other._matrix;
    _inv_matrix = std::move(other._inv_matrix);
}
transform_space::transform_space(const vec2& origin, const vec2& scale, const vec2& rotation){
    _dirty = true;
    _axisRotation = rotation;
    _axisScale = scale;
    _origin = origin;
    _matrix = matrix3x3::identity;
    _inv_matrix = nullptr;
}
transform_space::transform_space(const vec2& origin, const vec2& scale, const float rotation){
    _dirty = true;
    _axisRotation = vec2(rotation, (M_PI / 2.0f) + rotation);
    _axisScale = scale;
    _origin = origin;
    _matrix = matrix3x3::identity;
    _inv_matrix = nullptr;
}
transform_space::transform_space(const vec2& origin, const float scale, const float rotation){
    _dirty = true;
    _axisRotation = vec2(rotation, (M_PI / 2.0f) + rotation);
    _axisScale = vec2(scale, scale);
    _origin = origin;
    _matrix = matrix3x3::identity;
    _inv_matrix = nullptr;
}
transform_space& transform_space::move_by(const vec2& m){
    _dirty = true;
    _origin += m;
    return *this;
}
transform_space& transform_space::move_by(const float x, const float y){
    _dirty = true;
    _origin += vec2(x, y);
    return *this;
}
transform_space& transform_space::rotate_by(const vec2& angles){
    _dirty = true;
    _axisRotation += angles;
    return *this;
}
transform_space& transform_space::rotate_by(const float angle_x, const float angle_y){
    _dirty = true;
    _axisRotation += vec2(angle_x, angle_y);
    return *this;
}
transform_space& transform_space::rotate_by(const float angle){
    _dirty = true;
    _axisRotation += vec2(angle, angle);
    return *this;
}
transform_space& transform_space::scale_by(const vec2& s){
    _dirty = true;
    _axisScale += s;
    return *this;
}
transform_space& transform_space::scale_by(const float sx, const float sy){
    _dirty = true;
    _axisScale += vec2(sx, sy);
    return *this;
}
transform_space& transform_space::scale_by(const float s){
    _dirty = true;
    _axisScale += vec2(s, s);
    return *this;
}
transform_space transform_space::inverse() const {
    transform_space _inv_space;
    const_cast<transform_space*>(this)->update_matrix();
    const_cast<transform_space*>(this)->update_inv_matrix();
    _inv_space.update_from_matrix(_inv_matrix.value());
    return _inv_space;
}
transform_space& transform_space::invert(){
    update_matrix();
    update_inv_matrix();
    update_from_matrix(_inv_matrix.value());
    return *this;
}
transform_space transform_space::canonical() const{
    transform_space _canonical;
    const_cast<transform_space*>(this)->update_matrix();
    _canonical.update_from_matrix(_matrix);
    return _canonical;
}
transform_space& transform_space::to_canonical(){
    update_matrix();
    update_from_matrix(_matrix);
    return *this;
}
bool transform_space::both_directions() const{
    return test_direction(transform_direction::from_base_to_space) &&
            test_direction(transform_direction::from_space_to_base);
}
const matrix3x3& transform_space::from_base_to_space() const{
    const_cast<transform_space*>(this)->update_matrix();
    const_cast<transform_space*>(this)->update_inv_matrix();
    return _inv_matrix.value();
}
bool transform_space::test_direction(transform_direction d) const{
    const_cast<transform_space*>(this)->update_matrix();
    if (d == transform_direction::from_base_to_space){
        const_cast<transform_space*>(this)->update_inv_matrix();
        return _inv_matrix.has_value();
    }
    else
        return true;
}
rb_string transform_space::to_string() const{
    return rb::to_string("[ o:", _origin, ", s: ", _axisScale, ", r:", _axisRotation, "]");
}
buffer transform_space::to_buffer() const {
    return buffer(this, sizeof(transform_space));
}
transform_space& transform_space::operator=(const transform_space& other){
    _dirty = other._dirty;
    _axisRotation = other._axisRotation;
    _axisScale = other._axisScale;
    _origin = other._origin;
    _matrix = other._matrix;
    _inv_matrix = other._inv_matrix;
    return *this;
}
transform_space& transform_space::operator=(transform_space&& other){
    _dirty = other._dirty;
    _axisRotation = other._axisRotation;
    _axisScale = other._axisScale;
    _origin = other._origin;
    _matrix = other._matrix;
    _inv_matrix = other._inv_matrix;
    return *this;
}
const transform_space& transform_space::operator*=(const transform_space& other){
    update_matrix();
    const_cast<transform_space*>(&other)->update_matrix();
    auto _c_matrix = _matrix * other._matrix;
    update_from_matrix(_c_matrix);
    return *this;
}
transform_space rb::operator *(const transform_space & first, const transform_space & second){
    const_cast<transform_space*>(&first)->update_matrix();
    const_cast<transform_space*>(&second)->update_matrix();
    auto _c_matrix = first._matrix * second._matrix;
    transform_space _result;
    _result.update_from_matrix(_c_matrix);
    return _result;
}
bool rb::operator ==(const transform_space& first, const transform_space& second){
    const_cast<transform_space*>(&first)->update_matrix();
    const_cast<transform_space*>(&second)->update_matrix();
    return first._matrix == second._matrix;
}
bool rb::operator!=(const transform_space& first, const transform_space& second){
    const_cast<transform_space*>(&first)->update_matrix();
    const_cast<transform_space*>(&second)->update_matrix();
    return first._matrix != second._matrix;
}

transform_space transform_space::moved(const vec2& m) const{
    return transform_space(m, scale(), rotation());
}
transform_space transform_space::moved(const float x, const float y) const{
    return transform_space(vec2(x, y), scale(), rotation());
}
transform_space transform_space::rotated(const vec2& angles) const{
    return transform_space(origin(), scale(), angles);
}
transform_space transform_space::rotated(const float angle_x, const float angle_y) const{
    return transform_space(origin(), scale(), vec2(angle_x, angle_y));
}
transform_space transform_space::scaled(const vec2& s) const{
    return transform_space(origin(), s, rotation());
}
transform_space transform_space::scaled(const float sx, const float sy) const{
    return transform_space(origin(), vec2(sx, sy), rotation());
}
transform_space transform_space::scaled(const float s) const{
    return transform_space(origin(), vec2(s, s), rotation());
}

void transform_space::load_from_buffer(const void *buffer, const void **next){
    if(next)
        *next = nullptr;
    memcpy(this, buffer, sizeof(transform_space));
    if (next)
        *next = ((char*)buffer) + sizeof(transform_space);
}

void transform_space::store_in_buffer(void *buffer, size_t *size, void **next) const{
    if(next)
        *next = nullptr;
    if(size)
        *size = 0;
    if(size && !buffer){
        *size = sizeof(transform_space);
        return;
    }
    memcpy(buffer, this, sizeof(transform_space));
    if (next)
        *next = ((char*)buffer) + sizeof(transform_space);
}

buffer transform_space::to_buffer(const std::vector<transform_space> &ts){
    size_t _full_size = 0;
    size_t _s;
    uint32_t _count = (uint32_t)ts.size();
    for (auto _t : ts){
        _t.store_in_buffer(nullptr, &_s, nullptr);
        _full_size += _s;
    }
    void* _mem = malloc(sizeof(uint32_t) + _full_size);
    uint32_t* _u32_mem = (uint32_t*)_mem;
    *_u32_mem = (uint32_t)_count;
    _u32_mem += 1;
    void * _cont = _u32_mem;
    for (auto _t : ts){
        void* _n_cont;
        _t.store_in_buffer(_cont, nullptr, &_n_cont);
        _cont = _n_cont;
    }
    buffer _b(_mem, _full_size + sizeof(uint32_t));
    free(_mem);
    return _b;
}

std::vector<transform_space> transform_space::from_buffer(rb::buffer b){
    const void* _mem = b.internal_buffer();
    assert(_mem);
    const uint32_t* _u32_mem = (const uint32*)_mem;
    auto _count = *_u32_mem;
    _u32_mem += 1;
    const void* _cont = _u32_mem;
    std::vector<transform_space> _ts;
    for (uint32_t i = 0; i < _count; i++) {
        transform_space _t;
        const void* _n_cont;
        _t.load_from_buffer(_cont, &_n_cont);
        _cont = _n_cont;
        _ts.push_back(_t);
    }
    return _ts;
}































