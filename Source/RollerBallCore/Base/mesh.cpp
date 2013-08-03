//
//  mesh.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 10/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "mesh.h"
#include "vertex.h"
#include "vec2.h"
#include "brush.h"
#include "transform_space.h"
#include "texture_map.h"
#include "rectangle.h"
#include "buffer.h"

using namespace rb;

void mesh::remap(const rectangle& texture_bounds, const texture_mapping_type mapping_type){
    if(mapping_type == texture_mapping_type::transformable){
        lock_vertex_buffer([&](vertex* vb) {
            for (uint32_t i = 0; i < this->vertex_count(); i++) {
                vb[i].set_texture_bounds(texture_bounds);
            }
        });
    }
    else {
        lock_vertex_buffer([&](vertex* vb) {
            for (uint32_t i = 0; i < this->vertex_count(); i++) {
                rectangle _previous_bounds = vb[i].get_texture_bounds();
                //map to normalized coordinates
                vec2 _normalized = vb[i].get_texture_coords();
                _normalized = (_normalized - _previous_bounds.bottom_left()) / _previous_bounds.size();
                _normalized = texture_bounds.bottom_left() + _normalized * texture_bounds.size();
                vb[i].set_texture_bounds(texture_bounds);
                vb[i].set_texture_coords(_normalized);
            }
        });
    }
}

mesh::mesh(){
    _vb = nullptr;
    _vb_count = 0;
    _ib = nullptr;
    _ib_count = 0;
    _external = false;
    _dirty_vb = true;
    _dirty_ib = true;
}
mesh::mesh(vertex* vb, uint32_t vb_count, uint16_t* ib, uint32_t ib_count){
    assert(vb);
    assert(vb_count > 0);
    assert(ib);
    assert(ib_count > 0);
    _vb = (vertex*)malloc(sizeof(vertex) * vb_count);
    _ib = (uint16_t*)malloc(sizeof(uint16_t) * ib_count);
    _vb_count = vb_count;
    _ib_count = ib_count;
    memcpy(_vb, vb, sizeof(vertex) * vb_count);
    memcpy(_ib, ib, sizeof(uint16_t) * ib_count);
    _external = false;
    _dirty_vb = true;
    _dirty_ib = true;
}
mesh& mesh::set_buffers(vertex* vb, uint32_t vb_count, uint16_t* ib, uint32_t ib_count, bool assume_ownership){
    if(_vb && !_external)
        free(_vb);
    if(_ib && !_external)
        free(_ib);
    
    _vb = vb;
    _vb_count = vb_count;
    _ib = ib;
    _ib_count = ib_count;
    _external = !assume_ownership;
    _dirty_vb = true;
    _dirty_ib = true;
    return *this;
}
mesh::mesh(const mesh& other){
    if(other._vb){
        _vb = (vertex*)malloc(sizeof(vertex) * other._vb_count);
        memcpy(_vb, other._vb, sizeof(vertex) * other._vb_count);
    }
    else
        _vb = nullptr;
    
    if(other._ib){
        _ib = (uint16_t*)malloc(sizeof(uint16_t) * other._ib_count);
        memcpy(_ib, other._ib, sizeof(uint16_t) * other._ib_count);
    }
    else
        _ib = nullptr;
    
    _vb_count = other._vb_count;
    _ib_count = other._ib_count;
    _external = false;
    _dirty_vb = true;
    _dirty_ib = true;
}
mesh::mesh(mesh&& other){
    _vb = other._vb;
    _vb_count = other._vb_count;
    _ib = other._ib;
    _ib_count = other._ib_count;
    _external = other._external;
    
    other._ib = nullptr;
    other._vb = nullptr;
    _dirty_vb = true;
    _dirty_ib = true;
}
const mesh& mesh::operator=(const mesh& other){
    if(this == &other)
        return *this;
    
    if(_vb_count == other._vb_count && _ib_count == other._ib_count &&
       _ib_count != 0 && _vb_count != 0)
    {
        memcpy(_vb, other._vb, sizeof(vertex) * other._vb_count);
        memcpy(_ib, other._ib, sizeof(uint16_t) * other._ib_count);
    }
    else {
        if(_vb && !_external)
            free(_vb);
        if(_ib && !_external)
            free(_ib);
        if(other._vb){
            _vb = (vertex*)malloc(sizeof(vertex) * other._vb_count);
            memcpy(_vb, other._vb, sizeof(vertex) * other._vb_count);
        }
        else
            _vb = nullptr;
        
        if(other._ib){
            _ib = (uint16_t*)malloc(sizeof(uint16_t) * other._ib_count);
            memcpy(_ib, other._ib, sizeof(uint16_t) * other._ib_count);
        }
        else
            _ib = nullptr;
    }
    
    _vb_count = other._vb_count;
    _ib_count = other._ib_count;
    _external = false;
    _dirty_vb = true;
    _dirty_ib = true;
    return *this;
}
const mesh& mesh::operator=(mesh&& other){
    if(this == &other)
        return *this;
    
    if(_vb && !_external)
        free(_vb);
    if(_ib && !_external)
        free(_ib);
    
    _vb = other._vb;
    _vb_count = other._vb_count;
    _ib = other._ib;
    _ib_count = other._ib_count;
    _external = other._external;
    
    other._ib = nullptr;
    other._vb = nullptr;
    _dirty_vb = true;
    _dirty_ib = true;
    return *this;
}
bool mesh::is_empty() const{
    return !_vb || !_ib;
}
const vertex* mesh::vertex_buffer() const{
    return _vb;
}
const uint32_t mesh::vertex_count() const{
    return _vb_count;
}

const uint16_t* mesh::index_buffer() const{
    return _ib;
}
const uint32_t mesh::index_count() const{
    return _ib_count;
}

rectangle mesh::compute_bounds() const {
    float _max_x, _max_y, _min_x, _min_y;
    _max_x = _max_y = std::numeric_limits<float>::min();
    _min_x = _min_y = std::numeric_limits<float>::min();
    
    for (int i = 0; i < _ib_count; i++) {
        const vertex& _v = _vb[_ib[i]];
        const vec2 _pos = _v.get_position();
        if (_pos.x() > _max_x)
            _max_x = _pos.x();
        
        if (_pos.y() > _max_y)
            _max_y = _pos.y();
        
        if (_pos.x() < _min_x)
            _min_x = _pos.x();
        
        if (_pos.y() < _min_y)
            _min_y = _pos.y();
    }
    
    rectangle rc((_max_x + _min_x) / 2, (_max_y + _min_y) / 2, (_max_x - _min_x), (_max_y - _min_y));
    return rc;
}

//alterations
void mesh::lock_buffers(std::function<void(vertex* vb, uint16_t* ib)> f){
    f(_vb, _ib);
    _dirty_vb = true;
    _dirty_ib = true;
}

void mesh::lock_vertex_buffer(std::function<void(vertex* vb)> f){
    f(_vb);
    _dirty_vb = true;
}
void mesh::lock_index_buffer(std::function<void(uint16_t* ib)> f){
    f(_ib);
    _dirty_ib = true;
}

mesh* mesh::merge_meshes(const std::vector<mesh *> &meshes){
    //sum of the vertices
    assert(meshes.size() > 0);
    size_t _sum_vertices = 0;
    size_t _sum_indexes = 0;
    for (auto _m : meshes){
        _sum_vertices += _m->vertex_count();
        _sum_indexes += _m->index_count();
    }
    
    if(_sum_vertices > std::numeric_limits<uint16_t>::max())
        return nullptr;
    
    assert(_sum_vertices > 0);
    assert(_sum_indexes > 0);
    
    vertex* _vb = (vertex*)malloc(sizeof(vertex) * _sum_vertices);
    uint16_t* _ib = (uint16_t*)malloc(sizeof(uint16_t) * _sum_indexes);
    
    size_t _vb_offset = 0;
    size_t _ib_offset = 0;
    for (auto _m : meshes){
        memcpy(_vb + _vb_offset, _m->vertex_buffer(), sizeof(vertex) * _m->vertex_count());
        for (int i = 0; i < _m->index_count(); i++) {
            _ib[i + _ib_offset] = _m->index_buffer()[i] + _vb_offset;
        }
        _vb_offset += _m->vertex_count();
        _ib_offset += _m->index_count();
    }
    mesh* _mesh = new mesh();
    _mesh->set_buffers(_vb, (uint32_t)_sum_vertices, _ib, (uint32_t)_sum_indexes, true);
    return _mesh;
}

//operations
void mesh::paint(const vec2& position, const brush& brush){
    auto _space = transform_space(position, 1, 0);
    for (int i = 0; i < _vb_count; i++) {
        auto _c = brush.sample_color(_space, _vb[i].get_position()).clamped(); //this color is pre-multiplied...
        if(almost_equal(_c.a(), 0)) //the vertex won't change
            continue;
        auto _v_c = _vb[i].get_color().clamped(); //this color is pre-multiplied...
        color _f_c;
        
        if(brush.blend_mode() == blend_mode::normal){
            _f_c = _c + (1 - _c.a()) * _v_c;
        }
        else if(brush.blend_mode() == blend_mode::multiply){
            _f_c = _c * _v_c + (1 - _c.a()) * _v_c;
        }
        else if(brush.blend_mode() == blend_mode::screen) {
            color _one = color::from_rgba(1, 1, 1, 1);
            _f_c = _c + (_one - _c) * _v_c;
        }
        else if(brush.blend_mode() == blend_mode::copy_source) {
            _f_c = _c;
        }
        else { //preserve_destination
            _f_c = _v_c;
        }
        
        _vb[i].set_color(_f_c);
    }
    _dirty_vb = true;
}

mesh& mesh::to_line_mesh(mesh& storage){
    auto _new_ib = (uint16_t*)malloc(sizeof(uint16_t) * 2 * index_count());
    size_t _offset = 0;
    auto _triangle_count = index_count() / 3;
    
    for (size_t i = 0; i < _triangle_count; i++) {
        auto _i1 = _ib[i * 3 + 0];
        auto _i2 = _ib[i * 3 + 1];
        auto _i3 = _ib[i * 3 + 2];

        _new_ib[_offset + 0] = _i1;
        _new_ib[_offset + 1] = _i2;
        _new_ib[_offset + 2] = _i2;
        _new_ib[_offset + 3] = _i3;
        _new_ib[_offset + 4] = _i3;
        _new_ib[_offset + 5] = _i1;
        _offset += 6;
    }
    
    auto _new_vb = (vertex*)malloc(sizeof(vertex) * vertex_count());
    memcpy(_new_vb, _vb, sizeof(vertex) * vertex_count());
    
    storage.set_buffers(_new_vb, vertex_count(), _new_ib, index_count() * 2, true);
    return storage;
}

void mesh::expand_from_atlas_bounds(const texture_map& map, bool reapply_mapping){
    this->lock_vertex_buffer([&](vertex* vb) {
        //bound_coord + bound_size * _v1
        for (size_t i = 0; i < this->vertex_count(); i++) {
            if(!reapply_mapping) {
                auto _pos = vb[i].get_texture_coords();
                _pos = (_pos - map.bounds().bottom_left()) / map.bounds().size();
                vb[i].set_texture_coords(_pos);
                vb[i].set_texture_bounds(map.bounds());
            }
            else
                map.set_texture_coords(_vb[i]);
        }
    });
}

void mesh::set_alpha(const float alpha){
    this->lock_vertex_buffer([&](vertex* vb) {
        //bound_coord + bound_size * _v1
        for (size_t i = 0; i < this->vertex_count(); i++) {
            vb[i].a = alpha;
        }
    });
}

void mesh::set_blend(const float blend){
    this->lock_vertex_buffer([&](vertex* vb) {
        //bound_coord + bound_size * _v1
        for (size_t i = 0; i < this->vertex_count(); i++) {
            vb[i].blend = blend;
        }
    });
}

void mesh::set_color(const rb::color &color){
    this->lock_vertex_buffer([&](vertex* vb) {
        //bound_coord + bound_size * _v1
        for (size_t i = 0; i < this->vertex_count(); i++) {
            vb[i].set_color(color);
        }
    });
}

void mesh::set_texture_bounds(const rb::rectangle &bounds){
    this->lock_vertex_buffer([&](vertex* vb) {
        //bound_coord + bound_size * _v1
        for (size_t i = 0; i < this->vertex_count(); i++) {
            vb[i].set_texture_bounds(bounds);
        }
    });
}

//to string
rb_string mesh::to_string() const{
    return rb::to_string("vertices: ", _vb_count, ", indexes: ", _ib_count);
}

mesh::~mesh(){
    if(_vb && !_external)
        free(_vb);
    if(_ib && !_external)
        free(_ib);
    _vb = nullptr;
    _ib = nullptr;
}

buffer mesh::to_buffer() const {
    if(!_vb || !_ib)
        return buffer();
    
    size_t _size =  sizeof(uint32_t) * 2 +
                    sizeof(vertex) * _vb_count +
                    sizeof(uint16_t) * _ib_count;
    void* _mem = malloc(_size);
    uint32_t* _u32_mem = (uint32_t*)_mem;
    *_u32_mem = _ib_count;
    _u32_mem++;
    *_u32_mem = _vb_count;
    _u32_mem++;
    uint16_t* _u16_mem = (uint16_t*)_u32_mem;
    memcpy(_u16_mem, _ib, sizeof(uint16_t) * _ib_count);
    _u16_mem += _ib_count;
    vertex* _v_mem = (vertex*)_u16_mem;
    memcpy(_v_mem, _vb, sizeof(vertex) * _vb_count);
    buffer b(_mem, _size);
    free(_mem);
    return b;
}

mesh::mesh(buffer buffer){
    const void* _mem = buffer.internal_buffer();
    assert(_mem);
    const uint32_t* _u32_mem = (const uint32_t*)_mem;
    _ib_count = *_u32_mem;
    _u32_mem++;
    _vb_count = *_u32_mem;
    _u32_mem++;
    uint16_t* _u16_mem = (uint16_t*)_u32_mem;
    _ib = (uint16_t*)malloc(sizeof(uint16_t) * _ib_count);
    _vb = (vertex*)malloc(sizeof(vertex) * _vb_count);
    memcpy(_ib, _u16_mem, sizeof(uint16_t) * _ib_count);
    _u16_mem += _ib_count;
    vertex* _v_mem = (vertex*)_u16_mem;
    memcpy(_vb, _v_mem, sizeof(vertex) * _vb_count);
    _external = false;
    _dirty_ib = true;
    _dirty_vb = true;
}






































