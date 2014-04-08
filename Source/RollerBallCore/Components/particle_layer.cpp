//
//  particle_layer.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 23/12/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#include "particle_layer.h"
#include "mesh.h"
#include "vertex.h"
#include "layer.h"
#include "extended_dynamic_mesh_batch.h"
#include "rectangle.h"
#include "process.h"
#include "basic_process.h"
#include "texture_map.h"
#include "scene.h"
#include "texture_atlas.h"

using namespace rb;

void quad_mesh_allocator::alloc_meshes(size_t n_meshes, std::vector<mesh *> &meshes){
    assert(n_meshes >= 0);
    if(n_meshes == 0)
        return;
    
    auto _ms = new mesh[n_meshes];
    _meshes.push_back(_ms);
    
    vertex* _vb = (vertex*)malloc(sizeof(vertex) * n_meshes * 4);
    uint16_t* _ib = (uint16_t*)malloc(sizeof(uint16_t) * n_meshes * 6);
    memset(_vb, 0, sizeof(vertex) * n_meshes * 4);
    memset(_ib, 0, sizeof(uint16_t) * n_meshes * 6);
    _blocks.push_back(_vb);
    _blocks.push_back(_ib);
    for (size_t i = 0; i < n_meshes; i++) {
        _ms[i].set_buffers(_vb, 4, _ib, 6, false);
        _vb += 4;
        _ib += 6;
        meshes.push_back(&_ms[i]);
    }
}

quad_mesh_allocator::~quad_mesh_allocator(){
    for (size_t i = 0; i < _blocks.size(); i++) {
        free(_blocks[i]);
    }
    
    for (size_t i = 0; i < _meshes.size(); i++) {
        delete [] _meshes[i];
    }
}

particle_allocator::particle_allocator(){
    _first_block = nullptr;
    _spare_block = nullptr;
    _current_block = nullptr;
    _m_allocator = nullptr;
    _iteration_count = 0;
}

particle_block* particle_allocator::compress_block(rb::particle_block *block){
    size_t _ai = 0;
    size_t _rai = PARTICLE_BLOCK_SIZE - 1;
    for (size_t i = 0; i < PARTICLE_BLOCK_SIZE; i++) {
        if(block->particles[i].state != particle_state::stopped){
            _spare_block->particles[_ai] = block->particles[i];
            _spare_block->particles[_ai].block = _spare_block;
            _ai++;
        }
        else {
            _spare_block->particles[_rai] = block->particles[i];
            _spare_block->particles[_rai].block = _spare_block;
            _rai--;
        }
    }
    _spare_block->allocated_blocks = _ai;
    _spare_block->allocation_index = _ai;
    //fix links
    if(block->previous)
        block->previous->next = _spare_block;
    if(block->next)
        block->next->previous = _spare_block;
    if(_current_block == block)
        _current_block = _spare_block;
    if(_first_block == block)
        _first_block = _spare_block;
    
    _spare_block->previous = block->previous;
    _spare_block->next = block->next;
    auto _nblock = _spare_block;
    _spare_block = block;
    return _nblock;
}

bool particle_allocator::needs_compresssion(rb::particle_block *block){
    float _frag = 1 - ((float)block->allocated_blocks / (float)block->allocation_index);
    return _frag >= MAX_FRAGMENTATION_ALLOWED;
}

ex_particle_info* particle_allocator::alloc(size_t n_particles, size_t &allocated, extended_dynamic_mesh_batch* edmb, process* p){
    assert(n_particles >= 0);
    assert(_iteration_count == 0);
    allocated = 0;
    if(n_particles == 0)
        return nullptr;
    if(_current_block == nullptr){
        _first_block = new particle_block();
        _spare_block = new particle_block();
        _current_block = _first_block;
        _iteration_count = 0;
        
        memset(_first_block, 0, sizeof(particle_block));
        _m_allocator = new quad_mesh_allocator();
        std::vector<mesh*> _meshes;
        _meshes.reserve(PARTICLE_BLOCK_SIZE);
        _m_allocator->alloc_meshes(PARTICLE_BLOCK_SIZE, _meshes);
        for (size_t i = 0; i < PARTICLE_BLOCK_SIZE; i++) {
            edmb->add_mesh(_meshes[i], p);
            _first_block->particles[i].assigned_mesh = _meshes[i];
            _first_block->particles[i].block = _first_block;
        }
        return alloc(n_particles, allocated, edmb, p);
    }
    else if (_current_block->allocation_index < (PARTICLE_BLOCK_SIZE - 1)){
        allocated = std::min(PARTICLE_BLOCK_SIZE - _current_block->allocation_index - 1, n_particles);
        auto _allocation = &_current_block->particles[_current_block->allocation_index];
        for (size_t i = 0; i < allocated; i++) {
            _allocation[i].state = particle_state::showing;
        }
        _current_block->allocation_index += allocated;
        _current_block->allocated_blocks += allocated;
        return _allocation;
    }
    else {
        //try to compress one of the blocks
        particle_block* _to_compress = nullptr;
        particle_block* _current = _first_block;
        while (_current != nullptr) {
            if(needs_compresssion(_current)){
                _to_compress = _current;
                break;
            }
            _current = _current->next;
        }
        if(_to_compress){
            _to_compress = compress_block(_to_compress);
            _current_block = _to_compress;
            return alloc(n_particles, allocated, edmb, p);
        }
        else { //allocate a new particle block and make it the first
            auto _new_block = new particle_block();
            memset(_new_block, 0, sizeof(particle_block));
            std::vector<mesh*> _meshes;
            _meshes.reserve(PARTICLE_BLOCK_SIZE);
            _m_allocator->alloc_meshes(PARTICLE_BLOCK_SIZE, _meshes);
            for (size_t i = 0; i < PARTICLE_BLOCK_SIZE; i++) {
                edmb->add_mesh(_meshes[i], p);
                _new_block->particles[i].assigned_mesh = _meshes[i];
                _new_block->particles[i].block = _new_block;
            }
            _new_block->previous = nullptr;
            _new_block->next = _first_block;
            _first_block->previous = _new_block;
            _first_block = _new_block;
            _current_block = _new_block;
            return alloc(n_particles, allocated, edmb, p);
        }
    }
}

void particle_allocator::free(rb::ex_particle_info *particle){
    assert(particle);
    assert(particle->state != particle_state::stopped);
    particle->state = particle_state::stopped;
    particle->block->allocated_blocks--;
}

particle_iterator particle_allocator::iterate(bool include_freed){
    particle_iterator _it;
    _it._current = nullptr;
    _it._parent = this;
    _it._index = -1;
    _it._include_freed = include_freed;
    return _it;
}

particle_allocator::~particle_allocator(){
    if(_m_allocator)
        delete _m_allocator;
    if(_spare_block)
        delete _spare_block;
    
    particle_block* _current = _first_block;
    while (_current != nullptr) {
        auto _backup = _current;
        _current = _current->next;
        delete _backup;
    }
}

ex_particle_info* particle_iterator::next(){
    if(!_parent->_first_block)
        return nullptr;
    if(_index == -1){
        _parent->_iteration_count++;
        _current = _parent->_first_block;
        _index = 0;
    }
    
Again:
    if(_current == nullptr)
        return nullptr;
    
    while (_index < PARTICLE_BLOCK_SIZE && (_include_freed || _current->particles[_index].state == particle_state::stopped))
        _index++;
    
    if(_index < PARTICLE_BLOCK_SIZE){
        _index++;
        return &_current->particles[_index - 1];
    }
    else {
        _current = _current->next;
        _index = 0;
        if(_current == nullptr)
            _parent->_iteration_count--;
        goto Again;
    }
}

particle_iterator::particle_iterator(){
    
}

void particle_iterator::reset(){
    if (_index == -1)
        return;
    
    if(_current != nullptr)
    {
        _current = nullptr;
        _index = -1;
        _parent->_iteration_count--;
        return;
    }
    else {
        _index = -1;
        return;
    }
}

particle_iterator::~particle_iterator(){
    reset();
}

particle_iterator::particle_iterator(const particle_iterator& it){
    _index = it._index;
    _parent = it._parent;
    _current = it._current;
    _include_freed = it._include_freed;
    if(_current != nullptr)
        _parent->_iteration_count++;
}

particle_iterator::particle_iterator(particle_iterator&& it){
    _index = it._index;
    _parent = it._parent;
    _current = it._current;
    _include_freed = it._include_freed;
    if(_current != nullptr)
        _parent->_iteration_count++;
    it.reset();
}

particle_iterator& particle_iterator::operator=(const rb::particle_iterator &other){
    if(this == &other)
        return *this;
    reset();
    _index = other._index;
    _parent = other._parent;
    _current = other._current;
    _include_freed = other._include_freed;
    if(_current != nullptr)
        _parent->_iteration_count++;
    return *this;
}

particle_iterator& particle_iterator::operator=(rb::particle_iterator &&other){
    if(this == &other)
        return *this;
    
    reset();
    _index = other._index;
    _parent = other._parent;
    _current = other._current;
    _include_freed = other._include_freed;
    if(_current != nullptr)
        _parent->_iteration_count++;
    other.reset();
    return *this;
}

particle_layer::particle_layer(class layer* layer)
: _zero_matrix(vec2::zero, vec2::zero, vec2::zero),
  _distribution(0.0f, 1.0f)
{
    assert(layer);
    _parent_layer = layer;
    _p_allocator = new particle_allocator();
    _batch = new extended_dynamic_mesh_batch();
    _state = particle_state::showing;
    _invalidated = false;
    _quad_ref = new mesh();
    rectangle _rc(0, 0, 1, 1);
    _rc.to_mesh(*_quad_ref, rectangle(0.5, 0.5, 1, 1));
    _map = nullptr;
}

particle_layer::~particle_layer(){
    auto _e_copy = _emitters;
    for (auto _e : _e_copy)
        unregister_emitter(_e);
    if(_p_allocator)
        delete _p_allocator;
    _p_allocator = nullptr;
    if(_batch)
        delete _batch;
    _batch = nullptr;
    if(_quad_ref)
        delete _quad_ref;
    _quad_ref = nullptr;
    if(_map)
        delete _map;
    _map = nullptr;
}

void particle_layer::invalidate_buffers(){
    _invalidated = true;
}

emitter_id particle_layer::register_emitter(rb::emitter_info *einfo){
    assert(einfo);
    ex_emitter_info* _new_info = new ex_emitter_info();
    _new_info->einfo = *einfo;
    _new_info->state = particle_state::showing;
    _emitters.push_back(_new_info);
    return _new_info;
}

void particle_layer::unregister_emitter(emitter_id eid){
    assert(eid);
    //remove all particles for this emitter
    ex_emitter_info* _einfo = (ex_emitter_info*)eid;
    auto _p_it = _p_allocator->iterate(false);
    ex_particle_info* _pinfo = nullptr;
    while ((_pinfo = _p_it.next()) != nullptr) {
        if(_pinfo->emitter == _einfo)
            free_particle(&_pinfo->pinfo);
    }
    
    if(_einfo->einfo.emitter_destroy_func)
        _einfo->einfo.emitter_destroy_func(eid);
    //remove from list
    _emitters.erase(
    std::remove_if(_emitters.begin(), _emitters.end(), [_einfo](const ex_emitter_info* ei){
        return ei == _einfo;
    }), _emitters.end());
    delete _einfo;
}

emitter_info* particle_layer::emitter(emitter_id eid){
    return &((ex_emitter_info*)eid)->einfo;
}

particle_state particle_layer::state() const {
    return _state;
}

particle_state particle_layer::state(rb::particle_state value){
    assert(value != particle_state::stopped);
    _state = value;
    return _state;
}

const class layer* particle_layer::parent_layer() const {
    return _parent_layer;
}

class layer* particle_layer::parent_layer() {
    return _parent_layer;
}

particle_info* particle_layer::alloc_particle(rb::particle_info *pinfo, emitter_id eid, rb::particle_state initial_state){
    assert(pinfo);
    assert(eid);
    assert(initial_state != particle_state::stopped);
    size_t _allocated;
    auto _p = _p_allocator->alloc(1, _allocated, _batch, _parent_layer->_textureless_process);
    assert(_allocated == 1);
    assert(_p);
    auto _einfo = (ex_emitter_info*)eid;
    _p->pinfo = *pinfo;
    _p->emitter = _einfo;
    _p->state = initial_state;
    _p->collapsed = false;
    *_p->assigned_mesh = *_quad_ref;
    if(!_map){
        _map = (texture_map*)_parent_layer->create_mapping(_einfo->einfo.image_name, transform_space());
        if(_map)
            _p->assigned_mesh->remap(_map->bounds(), texture_mapping_type::untransformable);
    }
    else {
        auto _temp = (texture_map*)_parent_layer->create_mapping(_einfo->einfo.image_name, transform_space(), _map);
        if(_temp){
            _map = _temp;
            _p->assigned_mesh->remap(_map->bounds(), texture_mapping_type::untransformable);
        }
    }
    
    auto& _tx_map = _parent_layer->_tx_proc_map[EFFECT_TEXTURED];
    if(_tx_map.count(_einfo->einfo.image_name) != 0)
        _batch->process(_p->assigned_mesh, _tx_map.at(_einfo->einfo.image_name));
    else
        _batch->process(_p->assigned_mesh, _parent_layer->_textureless_process); //we get the first texture
    return &_p->pinfo;
}

void particle_layer::free_particle(rb::particle_info *pinfo){
    assert(pinfo);
    auto _ex_p = (ex_particle_info*)pinfo;
    _zero_matrix.transform_mesh(*_ex_p->assigned_mesh);
    if(_ex_p->emitter->einfo.particle_destroy_func)
        _ex_p->emitter->einfo.particle_destroy_func(&_ex_p->pinfo);
    _p_allocator->free(_ex_p);
}

emitter_id particle_layer::get_emitter(rb::particle_info *pinfo){
    assert(pinfo);
    auto _ex_p = (ex_particle_info*)pinfo;
    return _ex_p->emitter;
}

enum particle_state particle_layer::particle_state(rb::particle_info *pinfo) const {
    assert(pinfo);
    auto _ex_p = (ex_particle_info*)pinfo;
    return _ex_p->state;
}

enum particle_state particle_layer::particle_state(rb::particle_info *pinfo, enum particle_state value){
    assert(pinfo);
    assert(value != particle_state::stopped);
    auto _ex_p = (ex_particle_info*)pinfo;
    _ex_p->state = value;
    return _ex_p->state;
}

void particle_layer::unregister_all_emitters(){
    auto _e_copy = _emitters;
    for (auto _e : _e_copy)
        unregister_emitter(_e);
}

enum particle_state particle_layer::emitter_state(emitter_id eid) const {
    assert(eid);
    ex_emitter_info* _einfo = (ex_emitter_info*)eid;
    return _einfo->state;
}

enum particle_state particle_layer::emitter_state(emitter_id eid, enum particle_state value){
    assert(eid);
    ex_emitter_info* _einfo = (ex_emitter_info*)eid;
    _einfo->state = value;
    return value;
}

void particle_layer::reset_emitter(emitter_id eid){
    assert(eid);
    ex_emitter_info* _einfo = (ex_emitter_info*)eid;
    _einfo->delay_remaining = _einfo->einfo.delay;
    _einfo->duration_remaining = _einfo->einfo.duration;
    _einfo->emission_acc = 0;
    //_einfo->state = particle_state::showing;
}

void particle_layer::update(float dt){
    if(_state == particle_state::paused)
        return;
    
    //1. Update all Emitters
    for (auto _e : _emitters){
        update_emitter(_e, dt);
    }
    //2. Update all particles
    auto _p_it = _p_allocator->iterate(false);
    ex_particle_info* _pinfo = nullptr;
    while ((_pinfo = _p_it.next()) != nullptr) {
        update_particle(_pinfo, dt);
        update_particle_mesh(_pinfo);
    }
}

void particle_layer::render(){
    if (_state == particle_state::hidden)
        return;
    
    if(_invalidated){
        _batch->clear_meshes();
        //re-add meshes
        auto _p_it = _p_allocator->iterate(true);
        ex_particle_info* _pinfo = nullptr;
        auto& _tx_map = _parent_layer->_tx_proc_map[EFFECT_TEXTURED];
        while ((_pinfo = _p_it.next()) != nullptr) {
            if(_pinfo->assigned_mesh){
                if(_tx_map.count(_pinfo->emitter->einfo.image_name) != 0)
                    _batch->add_mesh(_pinfo->assigned_mesh, _tx_map.at(_pinfo->emitter->einfo.image_name));
                else
                    _batch->add_mesh(_pinfo->assigned_mesh, _parent_layer->_textureless_process); //we get the first texture
            }
        }
        _invalidated = false;
    }
    _batch->draw();
}

enum blend_mode particle_layer::blend_mode() const {
    return _batch->blend_mode();
}

enum blend_mode particle_layer::blend_mode(enum blend_mode value){
    return _batch->blend_mode(value);
}

void particle_layer::update_particle_mesh(ex_particle_info* pinfo){
    if(pinfo->state == particle_state::hidden || pinfo->emitter->state == particle_state::hidden || pinfo->state == particle_state::stopped)
        _zero_matrix.transform_mesh(*pinfo->assigned_mesh);
    else {
        _quad_ref->copy_position_to(*pinfo->assigned_mesh);
        matrix3x3 _mSize = matrix3x3::build_scale(pinfo->pinfo.size);
        matrix3x3 _mTrans = matrix3x3::build_translation(pinfo->pinfo.translation);
        matrix3x3 _mRot = matrix3x3::build_rotation(pinfo->pinfo.angle);
        matrix3x3 _m = pinfo->pinfo.base_transform;
        _m *= _mTrans;
        _m *= _mRot;
        _m *= _mSize;
        _m.transform_mesh(*pinfo->assigned_mesh);
        pinfo->assigned_mesh->set_alpha(pinfo->pinfo.opacity);
        pinfo->assigned_mesh->set_blend(pinfo->pinfo.blend);
        pinfo->assigned_mesh->set_color(color::lerp(pinfo->pinfo.current_color_mix, pinfo->pinfo.start_color, pinfo->pinfo.end_color));
    }
}

inline float lerpf(float r, float f1, float f2){
    return (1 - r)*f1 + r*f2;
}

void particle_layer::update_emitter(rb::ex_emitter_info *einfo, float dt){
UpdateEmitterAgain:
    if(einfo->einfo.emitter_func){
        einfo->einfo.emitter_func(einfo, this, dt);
        return;
    }
    if(einfo->state == particle_state::paused || einfo->state == particle_state::stopped)
        return;
    
    if(!einfo->einfo.initialized){
        einfo->einfo.aspect_correction_factor = einfo->einfo.aspect_correction ? aspect_correction_factor(einfo->einfo.image_name) : vec2(1, 1);
        einfo->einfo.initialized = true;
    }
    
    if(einfo->delay_remaining != 0){
        einfo->delay_remaining -= dt;
        if(einfo->delay_remaining <= 0)
            einfo->delay_remaining = 0;
        return;
    }
    
    if(einfo->duration_remaining != INFINITE_DURATION){
        einfo->duration_remaining -= dt;
        if(einfo->duration_remaining <= 0){
            if(!einfo->einfo.loop){
                einfo->duration_remaining = 0;
                einfo->state = einfo->state == particle_state::hidden ? particle_state::hidden : particle_state::stopped;
                return;
            }
            else {
                reset_emitter(einfo);
                goto UpdateEmitterAgain;
            }
        }
    }
    
    einfo->emission_acc += dt;
    auto _to_be_emitted = (int)(einfo->emission_acc / einfo->einfo.inv_emission_rate);
    einfo->emission_acc -= _to_be_emitted * einfo->einfo.inv_emission_rate;
    
    for (int i = 0; i < _to_be_emitted; i++) {
        particle_info pi;
        pi.base_transform = einfo->einfo.transform;
        float _r = lerpf(random_0_1(), einfo->einfo.emission_radius_01, einfo->einfo.emission_radius_02);
        float _angle = lerpf(random_0_1(), 0, 2 * M_PI);
        vec2 _displacement = vec2::right.rotated(_angle) * _r;
        pi.translation = _displacement;
        pi.size = vec2::lerp(random_0_1(), einfo->einfo.size_01, einfo->einfo.size_02);
        
        pi.angle = lerpf(random_0_1(), einfo->einfo.angle_01, einfo->einfo.angle_02);
        
        pi.size *= einfo->einfo.aspect_correction_factor;
        
        vec2 _velocity = vec2::right.rotated(lerpf(random_0_1(), einfo->einfo.velocity_angle_01, einfo->einfo.velocity_angle_02));
        _velocity *= lerpf(random_0_1(), einfo->einfo.velocity_01, einfo->einfo.velocity_02);
        
        pi.tangential_velocity = lerpf(random_0_1(), einfo->einfo.tangential_velocity_01, einfo->einfo.tangential_velocity_02);
        pi.perpendicular_velocity = lerpf(random_0_1(), einfo->einfo.perpendicular_velocity_01, einfo->einfo.perpendicular_velocity_02);
        
        vec2 _acceleration = vec2::right.rotated(lerpf(random_0_1(), einfo->einfo.acceleration_angle_01, einfo->einfo.acceleration_angle_02));
        _acceleration *= lerpf(random_0_1(), einfo->einfo.acceleration_01, einfo->einfo.acceleration_02);
        
        pi.tangential_acceleration = lerpf(random_0_1(), einfo->einfo.tangential_acceleration_01, einfo->einfo.tangential_acceleration_02);
        pi.perpendicular_acceleration = lerpf(random_0_1(), einfo->einfo.perpendicular_acceleration_01, einfo->einfo.perpendicular_acceleration_02);
        
        pi.velocity = _velocity;
        pi.acceleration = _acceleration + einfo->einfo.gravity_acceleration;
        pi.velocity_exp_rate = lerpf(random_0_1(), einfo->einfo.velocity_rate_01, einfo->einfo.velocity_rate_02);
        pi.radial_velocity = lerpf(random_0_1(), einfo->einfo.radial_velocity_01, einfo->einfo.radial_velocity_02);
        pi.radial_acceleration = lerpf(random_0_1(), einfo->einfo.radial_acceleration_01, einfo->einfo.radial_acceleration_02);
        pi.max_size = einfo->einfo.max_size;
        pi.size_rate = einfo->einfo.uniform_size ?
                        vec2::lerp(random_0_1(), einfo->einfo.size_rate_01, einfo->einfo.size_rate_02) :
                        vec2(lerpf(random_0_1(), einfo->einfo.size_rate_01.x(), einfo->einfo.size_rate_02.x()), lerpf(random_0_1(), einfo->einfo.size_rate_01.y(), einfo->einfo.size_rate_02.y()));
        pi.opacity = lerpf(random_0_1(), einfo->einfo.opacity_01, einfo->einfo.opacity_02);
        pi.opacity_rate = lerpf(random_0_1(), einfo->einfo.opacity_rate_01, einfo->einfo.opacity_rate_02);
        pi.blend = lerpf(random_0_1(), einfo->einfo.blend_01, einfo->einfo.blend_02);
        pi.blend_rate = lerpf(random_0_1(), einfo->einfo.blend_rate_01, einfo->einfo.blend_rate_02);
        pi.start_color = color::lerp(random_0_1(), einfo->einfo.start_color_01, einfo->einfo.start_color_02);
        pi.end_color = color::lerp(random_0_1(), einfo->einfo.end_color_01, einfo->einfo.end_color_02);
        pi.current_color_mix = lerpf(random_0_1(), einfo->einfo.start_color_mix_01, einfo->einfo.start_color_mix_02);
        pi.color_mixing_velocity = lerpf(random_0_1(), einfo->einfo.color_mixing_velocity_01, einfo->einfo.color_mixing_velocity_02);
        pi.life_remaining = lerpf(random_0_1(), einfo->einfo.life_01, einfo->einfo.life_02);
        alloc_particle(&pi, einfo, particle_state::showing);
    }
}

vec2 particle_layer::aspect_correction_factor(const rb_string& img_name) const {
    vec2 _tex_s = size_of_tex(img_name);
    if(almost_equal(_tex_s.x(), 0))
        _tex_s.x(1);
    if(almost_equal(_tex_s.y(), 0))
        _tex_s.y(1);
    transform_space _ac;
    auto _tx = _tex_s.x() / _tex_s.y();
    auto _ty = _tex_s.y() / _tex_s.x();
    if(_tex_s.x() > _tex_s.y())
        _tx = 1;
    else
        _ty = 1;
    
    return vec2(_tx, _ty);
}

vec2 particle_layer::size_of_tex(const rb_string &img_name) const{
    if(_parent_layer->parent_scene() && _parent_layer->parent_scene()->atlas()){
        if(_parent_layer->parent_scene()->atlas()->contains_texture(img_name))
        {
            std::vector<rb_string> _groups;
            _parent_layer->parent_scene()->atlas()->get_groups(img_name, _groups);
            return _parent_layer->parent_scene()->atlas()->get_bounds_in_pixels(_groups[0], img_name).size();
        }
        else
            return vec2::zero;
    }
    else
        return vec2::zero;
}

void particle_layer::seed(uint32_t value){
    _generator.seed((std::mt19937::result_type)value);
}

float particle_layer::random_0_1(){
    return _distribution(_generator);
}

void particle_layer::update_particle(rb::ex_particle_info *pinfo, float dt){
    if(pinfo->emitter->einfo.updater_func) {
        pinfo->emitter->einfo.updater_func(&pinfo->pinfo, dt);
        return;
    }
    if(pinfo->state == particle_state::paused || pinfo->emitter->state == particle_state::paused)
        return;
    
    pinfo->pinfo.life_remaining -= dt;
    if(pinfo->pinfo.life_remaining <= 0){
        this->free_particle(&pinfo->pinfo);
        return;
    }
    pinfo->pinfo.size += pinfo->pinfo.size_rate * dt;
    if(pinfo->pinfo.max_size != vec2::zero){
        if(pinfo->pinfo.size.x() > pinfo->pinfo.max_size.x())
            pinfo->pinfo.size.x(pinfo->pinfo.max_size.x());
        if(pinfo->pinfo.size.y() > pinfo->pinfo.max_size.y())
            pinfo->pinfo.size.y(pinfo->pinfo.max_size.y());
    }
    if(pinfo->pinfo.velocity_exp_rate != 0.0f)
        pinfo->pinfo.velocity += pinfo->pinfo.velocity.normalized() * pinfo->pinfo.velocity_exp_rate * dt;
    pinfo->pinfo.velocity += pinfo->pinfo.acceleration * dt;
    pinfo->pinfo.tangential_velocity += pinfo->pinfo.tangential_acceleration * dt;
    pinfo->pinfo.perpendicular_velocity += pinfo->pinfo.perpendicular_acceleration * dt;
    vec2 _perp_v = pinfo->pinfo.translation.normalized();
    vec2 _tgt_v = _perp_v.rotated90() * pinfo->pinfo.tangential_velocity;
    _perp_v *= pinfo->pinfo.perpendicular_velocity;
    pinfo->pinfo.translation += (pinfo->pinfo.velocity + _perp_v + _tgt_v) * dt;
    pinfo->pinfo.radial_velocity += pinfo->pinfo.radial_acceleration * dt;
    pinfo->pinfo.angle += pinfo->pinfo.radial_velocity * dt;
    pinfo->pinfo.opacity += pinfo->pinfo.opacity_rate * dt;
    if(pinfo->pinfo.opacity < 0)
        pinfo->pinfo.opacity = 0;
    if(pinfo->pinfo.opacity > 1)
        pinfo->pinfo.opacity = 1;
    pinfo->pinfo.blend += pinfo->pinfo.blend_rate * dt;
    if(pinfo->pinfo.blend < 0)
        pinfo->pinfo.blend = 0;
    if(pinfo->pinfo.blend > 1)
        pinfo->pinfo.blend = 1;
    pinfo->pinfo.current_color_mix += pinfo->pinfo.color_mixing_velocity * dt;
    if(pinfo->pinfo.current_color_mix < 0)
        pinfo->pinfo.current_color_mix = 0;
    if(pinfo->pinfo.current_color_mix > 1)
        pinfo->pinfo.current_color_mix = 1;
}










































