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
    _dirty = true;
    _line_width = 1;
    _geom_type = geometry_type::triangle;
}
extended_dynamic_mesh_batch::~extended_dynamic_mesh_batch(){
    for(auto _b : _batches)
        delete _b;
}
uint32_t extended_dynamic_mesh_batch::mesh_count() const{
    return (uint32_t)_meshes.size();
}
void extended_dynamic_mesh_batch::add_mesh(mesh* m, const class process* p){
    assert(m);
    assert(p);
    _meshes.push_back(m);
    auto _p = const_cast<class process*>(p);
    _mesh_proc_map[m] = _p;
    _dirty = true;
}
void extended_dynamic_mesh_batch::clear_meshes(){
    _meshes.clear();
    _mesh_proc_map.clear();
    _dirty = true;
}

bool extended_dynamic_mesh_batch::contains_mesh(const mesh* m) const{
    assert(m);
    for (auto _item : _meshes){
        if(_item == m)
            return true;
    }
    return false;
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
