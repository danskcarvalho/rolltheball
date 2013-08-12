//
//  extended_dynamic_mesh_batch.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 12/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "extended_dynamic_mesh_batch.h"
#include "dynamic_mesh_batch.h"
#include "extended_static_mesh_batch.h"

using namespace rb;

extended_dynamic_mesh_batch::extended_dynamic_mesh_batch(){
    _blend_mode = blend_mode::normal;
    _dirty = false;
    _dirty = true;
    _line_width = 1;
    _geom_type = geometry_type::triangle;
}
extended_dynamic_mesh_batch::~extended_dynamic_mesh_batch(){
    for(auto _b : _batches)
        delete _b;
}
extended_dynamic_mesh_batch::mesh_range::mesh_range(){
    _parent = nullptr;
}
extended_dynamic_mesh_batch::const_iterator extended_dynamic_mesh_batch::mesh_range::begin() const{
    return _begin;
}
extended_dynamic_mesh_batch::const_iterator extended_dynamic_mesh_batch::mesh_range::end() const{
    if(!_parent)
        return _end;
    else {
        if(_end == _parent->_meshes.end())
            return _end;
        else
            return std::next(_end);
    }
}

void extended_dynamic_mesh_batch::mesh_range::move_before(const mesh_range& other){
    if(!_parent)
        return;
    
    auto _it = begin();
    auto _old_end = end();
    auto _ins_point = other.begin();
    
    nullable<const_iterator> _new_begin;
    const_iterator _new_end;
    
    while (_it != _old_end) {
        auto _m = *_it;
        _it = _parent->_meshes.erase(_it);
        _parent->_mesh_pos_map[_m] = _parent->_meshes.insert(_ins_point, _m);
        if(_it == _old_end)
            _new_end = _parent->_mesh_pos_map[_m];
        if(!_new_begin.has_value())
            _new_begin = _parent->_mesh_pos_map[_m];
    }
    
    assert(_new_begin.has_value());
    _begin = _new_begin.value();
    _end = _new_end;
    _parent->_dirty = true;
}
void extended_dynamic_mesh_batch::mesh_range::move_after(const mesh_range& other){
    if(!_parent)
        return;
    
    auto _it = begin();
    auto _old_end = end();
    auto _ins_point = other.end();
    
    nullable<const_iterator> _new_begin;
    const_iterator _new_end;
    
    while (_it != _old_end) {
        auto _m = *_it;
        _it = _parent->_meshes.erase(_it);
        _parent->_mesh_pos_map[_m] = _parent->_meshes.insert(_ins_point, _m);
        if(_it == _old_end)
            _new_end = _parent->_mesh_pos_map[_m];
        if(!_new_begin.has_value())
            _new_begin = _parent->_mesh_pos_map[_m];
    }
    
    assert(_new_begin.has_value());
    _begin = _new_begin.value();
    _end = _new_end;
    _parent->_dirty = true;
}

uint32_t extended_dynamic_mesh_batch::mesh_count() const{
    return (uint32_t)_meshes.size();
}
const mesh* extended_dynamic_mesh_batch::first_mesh() const{
    if(_meshes.size() == 0)
        return nullptr;
    return _meshes.front();
}
mesh* extended_dynamic_mesh_batch::first_mesh(){
    if(_meshes.size() == 0)
        return nullptr;
    return _meshes.front();
}
const mesh* extended_dynamic_mesh_batch::last_mesh() const{
    if(_meshes.size() == 0)
        return nullptr;
    return _meshes.back();
}
mesh* extended_dynamic_mesh_batch::last_mesh(){
    if(_meshes.size() == 0)
        return nullptr;
    return _meshes.back();
}
void extended_dynamic_mesh_batch::remove_meshes(const mesh_range& r){
    auto _it = r.begin();
    while (_it != r.end()) {
        auto _m = const_cast<mesh*>(*_it);
        _mesh_proc_map.erase(_m);
        
        _mesh_pos_map.erase(*_it);
        _it = _meshes.erase(_it);
        _dirty = true;
    }
}
void extended_dynamic_mesh_batch::add_mesh_before(mesh* m, const class process* p, const mesh_range& r){
    assert(m);
    assert(p);
    assert(_mesh_pos_map.count(m) == 0);
    _mesh_pos_map.insert({m, _meshes.insert(r.begin(), m)});
    auto _p = const_cast<class process*>(p);
    _mesh_proc_map[m] = _p;
    _dirty = true;
}
void extended_dynamic_mesh_batch::add_mesh_after(mesh* m, const class process* p, const mesh_range& r){
    assert(m);
    assert(p);
    assert(_mesh_pos_map.count(m) == 0);
    auto _end = r.end();
    _mesh_pos_map.insert({m, _meshes.insert(_end, m)});
    auto _p = const_cast<class process*>(p);
    _mesh_proc_map[m] = _p;
    _dirty = true;
}
void extended_dynamic_mesh_batch::add_mesh(mesh* m, const class process* p){
    assert(m);
    assert(p);
    assert(_mesh_pos_map.count(m) == 0);
    _meshes.push_back(m);
    _mesh_pos_map.insert({m, std::prev(_meshes.end())});
    auto _p = const_cast<class process*>(p);
    _mesh_proc_map[m] = _p;
    _dirty = true;
}
void extended_dynamic_mesh_batch::clear_meshes(){
    _meshes.clear();
    _mesh_pos_map.clear();
    _mesh_proc_map.clear();
    _dirty = true;
}

void extended_dynamic_mesh_batch::range(const mesh* start, const mesh* end, mesh_range& r){
    assert(start);
    assert(end);
    auto _start = const_cast<mesh*>(start);
    auto _end =const_cast<mesh*>(end);
    assert(_mesh_pos_map.count(const_cast<mesh*>(_start)) != 0);
    assert(_mesh_pos_map.count(const_cast<mesh*>(_end)) != 0);
    
    auto _start_it = _mesh_pos_map.at(_start);
    auto _end_it = _mesh_pos_map.at(_end);
#if defined(DEBUG)
    auto _test = _start_it;
    bool _reached_end = false;
    while (_test != _meshes.end()) {
        if(_test == _end_it){
            _reached_end = true;
            break;
        }
        _test++;
    }
    assert(_reached_end);
#endif
    r._begin = _start_it;
    r._end = _end_it;
    r._parent = this;
}
bool extended_dynamic_mesh_batch::contains_mesh(const mesh* m) const{
    assert(m);
    auto _m = const_cast<mesh*>(m);
    return _mesh_pos_map.count(_m) != 0;
}
void extended_dynamic_mesh_batch::range(const mesh* m, mesh_range& r){
    assert(m);
    auto _m = const_cast<mesh*>(m);
    assert(_mesh_pos_map.count(const_cast<mesh*>(_m)) != 0);
    
    auto _m_it = _mesh_pos_map.at(_m);
    r._begin = _m_it;
    r._end = _m_it;
    r._parent = this;
}
void extended_dynamic_mesh_batch::entire_range(mesh_range& r){
    r._begin = _meshes.begin();
    r._end = _meshes.end();
    r._parent = this;
}

uint32_t extended_dynamic_mesh_batch::realloc_batches_for_process(uint32_t start_index, class process* p, const std::vector<mesh*, boost::pool_allocator<class mesh*>>& meshes){
    
    //we calculate the needed number of batches
    size_t _n_vb = 0;
    for (auto _mIt : meshes) {
        _n_vb += _mIt->vertex_count();
        
    }
    
    size_t _max_vb_per_batch = std::numeric_limits<uint16_t>::max();
    size_t _needed_batches = _n_vb / _max_vb_per_batch;
    
    if((_n_vb % _max_vb_per_batch) != 0)
        _needed_batches++;
    
    //we add the batches we need
    auto _remaining_batches = (_batches.size() - start_index);
    if(_remaining_batches < _needed_batches){
        for (size_t i = 0; i < (_needed_batches - _remaining_batches); i++) {
            _batches.push_back(new dynamic_mesh_batch());
        }
    }
    
    //we the add the meshes to the batches
    auto _it_b = _batches.begin() + start_index;
    _n_vb = 0;
    for (auto _m : meshes) {
        _n_vb += _m->vertex_count();
        if(_n_vb > _max_vb_per_batch){
            _it_b++;
            _n_vb = _m->vertex_count();
        }
        (*_it_b)->add_mesh(_m);
    }
    
    for (uint32_t i = 0; i < _needed_batches; i++) {
        _batches[start_index + i]->process(p);
    }
    
    return (uint32_t)_needed_batches;
}

void extended_dynamic_mesh_batch::realloc_batches() {
    if(_dirty){
        for (auto _b : _batches)
            _b->clear_meshes(); //removes all meshes...
        
        typedef std::vector<mesh*, boost::pool_allocator<class mesh*>> mesh_vector;
        std::unordered_map<class process*, mesh_vector, std::hash<class process*>, std::equal_to<class process*>, boost::pool_allocator<std::pair<class process*, mesh_vector>>>  _mesh_per_process;
        
        for (auto _m : _meshes){
            auto _p = _mesh_proc_map[_m];
            _mesh_per_process[_p].push_back(_m);
        }
        
        uint32_t _start = 0;
        for (auto _p : _mesh_per_process){
            _start += realloc_batches_for_process(_start, _p.first, _p.second);
        }
        
        _dirty = false;
    }
}

const class process* extended_dynamic_mesh_batch::process(const mesh* m) const{
    assert(m);
    return _mesh_proc_map.at(const_cast<mesh*>(m));
}
class process* extended_dynamic_mesh_batch::process(const mesh* m){
    assert(m);
    return _mesh_proc_map.at(const_cast<mesh*>(m));
}
class process* extended_dynamic_mesh_batch::process(const mesh* m, class process* value){
    assert(m);
    assert(value);
    if(process(m) == value)
        return value;
    
    auto _m = const_cast<mesh*>(m);
    _mesh_proc_map[_m] = value;
    _dirty = true;
    return value;
}

void extended_dynamic_mesh_batch::draw() {
    if(_meshes.size() == 0) //no mesh to draw
        return;
    
    realloc_batches();
    
    //we draw the batches
    for (auto _b : _batches){
        _b->blend_mode(_blend_mode);
        _b->geometry_type(_geom_type);
        _b->line_width(_line_width);
        _b->draw();
    }
}

extended_static_mesh_batch* extended_dynamic_mesh_batch::compile() const{
    if(_meshes.size() == 0)
        return nullptr;
    
    const_cast<extended_dynamic_mesh_batch*>(this)->realloc_batches();
    extended_static_mesh_batch* _s_batch = new extended_static_mesh_batch();
    
    for (auto _b : _batches){
        _s_batch->_batches.push_back(_b->compile());
    }
    
    _s_batch->_blend_mode = _blend_mode;
    _s_batch->_geom_type = _geom_type;
    _s_batch->_line_width = _line_width;
    return _s_batch;
}
