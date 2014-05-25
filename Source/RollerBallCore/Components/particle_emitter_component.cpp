//
//  particle_emitter.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 28/12/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#include "particle_emitter_component.h"
#include "layer.h"

using namespace rb;

void particle_emitter_component::after_becoming_active(bool node_was_moved){
    auto _saved_duration = _ei.duration;
    auto _saved_delay = _ei.delay;
    auto _saved_loop = _ei.loop;
    if(in_editor()){
        _ei.duration = _editor_duration;
        _ei.delay = _editor_delay;
        _ei.loop = _editor_loop;
    }
    _id = parent_layer()->particle_layer()->register_emitter(&_ei);
    parent_layer()->particle_layer()->reset_emitter(_id);
    parent_layer()->particle_layer()->emitter_state(_id, in_editor() ? (node_container::in_editor_hidden() ? particle_state::hidden : particle_state::showing) : _state);
    if(in_editor()){
        _ei.duration = _saved_duration;
        _ei.delay = _saved_delay;
        _ei.loop = _saved_loop;
    }
    enabled(node_capability::rendering, true);
}

void particle_emitter_component::before_becoming_inactive(bool node_was_moved){
    if(_id){
        parent_layer()->particle_layer()->unregister_emitter(_id);
        _id = nullptr;
    }
}

void particle_emitter_component::reset_emitter(){
    if(_id)
        parent_layer()->particle_layer()->reset_emitter(_id);
}

void particle_emitter_component::playing(){
    if(in_editor()){
        if(!is_playing()){
            parent_layer()->particle_layer()->emitter_state(_id, node_container::in_editor_hidden() ? particle_state::hidden : particle_state::showing);
            parent_layer()->particle_layer()->emitter(_id)->duration = _editor_duration;
            parent_layer()->particle_layer()->emitter(_id)->delay = _editor_delay;
            parent_layer()->particle_layer()->emitter(_id)->loop = _editor_loop;
        }
        else {
            parent_layer()->particle_layer()->emitter(_id)->duration = _ei.duration;
            parent_layer()->particle_layer()->emitter(_id)->delay = _ei.delay;
            parent_layer()->particle_layer()->emitter(_id)->loop = _ei.loop;
            parent_layer()->particle_layer()->reset_emitter(_id);
            parent_layer()->particle_layer()->emitter_state(_id, _state);
        }
    }
}

void particle_emitter_component::paused(){
    
}

void particle_emitter_component::render(const bool refill_buffers){
    matrix3x3 _p_to_layer = parent_node() ? parent_node()->from_node_space_to(space::layer) : matrix3x3::identity;
    matrix3x3 _to_layer = _p_to_layer * transform_space(old_transform().origin()).from_space_to_base();
    _ei.transform = _to_layer;
    if(_id)
        parent_layer()->particle_layer()->emitter(_id)->transform = _to_layer;
}

rb_string particle_emitter_component::type_name() const {
    return u"particle_emitter_component";
}

rb_string particle_emitter_component::displayable_type_name() const {
    return u"Particle Emitter";
}

rectangle particle_emitter_component::bounds() const {
    return rectangle(0, 0, 1, 1);
}

bool particle_emitter_component::in_editor_hidden(const bool value) {
    auto _r = node::in_editor_hidden(value);
    if(_id && in_editor() && !is_playing())
    {
        if(_r)
            parent_layer()->particle_layer()->emitter_state(_id, particle_state::hidden);
        else
            parent_layer()->particle_layer()->emitter_state(_id, particle_state::showing);
    }
    return _r;
}

particle_emitter_component::particle_emitter_component(){
    _editor_duration = INFINITE_DURATION;
    _editor_delay = 0;
    _editor_loop = true;
    _id = nullptr;
    _state = particle_state::showing;
    _ei.blend_01 = 1;
    _ei.blend_02 = 1;
    _ei.start_color_01 = color::from_rgba(1, 1, 1, 1);
    _ei.start_color_02 = color::from_rgba(1, 1, 1, 1);
    _ei.end_color_01 = color::from_rgba(1, 1, 1, 1);
    _ei.end_color_02 = color::from_rgba(1, 1, 1, 1);
    _ei.duration = INFINITE_DURATION;
    _ei.inv_emission_rate = 0.25;
    _ei.life_01 = 2;
    _ei.life_02 = 2;
    _ei.opacity_01 = 1;
    _ei.opacity_02 = 1;
    _ei.size_01 = vec2(1, 1);
    _ei.size_02 = vec2(1, 1);
    _ei.velocity_01 = 2;
    _ei.velocity_02 = 2;
    _ei.velocity_angle_01 = M_PI_2;
    _ei.velocity_angle_02 = M_PI_2;
}

particle_emitter_component::~particle_emitter_component(){
    
}

#define EM_PROPERTY_VEC2N(name, number) \
vec2 particle_emitter_component::name##number() const { \
    return _ei.name##number; \
} \
vec2 particle_emitter_component::name##number(const vec2 &value){ \
    _ei.name##number = value; \
    if(_id) \
        parent_layer()->particle_layer()->emitter(_id)->name##number = value; \
    return _ei.name##number; \
}

#define EM_PROPERTY_VEC2(name) \
EM_PROPERTY_VEC2N(name, _01) \
EM_PROPERTY_VEC2N(name, _02)

EM_PROPERTY_VEC2(size)
EM_PROPERTY_VEC2(size_rate)

#define EM_PROPERTY_VEC2S(name) \
vec2 particle_emitter_component::name() const { \
    return _ei.name; \
} \
vec2 particle_emitter_component::name(const vec2 &value){ \
    _ei.name = value; \
    if(_id) \
        parent_layer()->particle_layer()->emitter(_id)->name = value; \
    return _ei.name; \
}

EM_PROPERTY_VEC2S(gravity_acceleration)
EM_PROPERTY_VEC2S(max_size)

#define EM_PROPERTY_COLORN(name, number) \
color particle_emitter_component::name##number() const { \
    return _ei.name##number; \
} \
color particle_emitter_component::name##number(const color &value){ \
    _ei.name##number = value; \
    if(_id) \
        parent_layer()->particle_layer()->emitter(_id)->name##number = value; \
    return _ei.name##number; \
}

#define EM_PROPERTY_COLOR(name) \
EM_PROPERTY_COLORN(name, _01) \
EM_PROPERTY_COLORN(name, _02)

EM_PROPERTY_COLOR(start_color)
EM_PROPERTY_COLOR(end_color)

#define EM_PROPERTY_N(type, name, number) \
type particle_emitter_component::name##number() const { \
    return _ei.name##number; \
} \
type particle_emitter_component::name##number(type value){ \
    _ei.name##number = value; \
    if(_id) \
        parent_layer()->particle_layer()->emitter(_id)->name##number = value; \
    return _ei.name##number; \
}

#define EM_PROPERTY(type, name) \
EM_PROPERTY_N(type, name, _01) \
EM_PROPERTY_N(type, name, _02)

EM_PROPERTY(float, velocity_angle)
EM_PROPERTY(float, acceleration_angle)
EM_PROPERTY(float, velocity)
EM_PROPERTY(float, velocity_rate)
EM_PROPERTY(float, acceleration)
EM_PROPERTY(float, tangential_velocity)
EM_PROPERTY(float, tangential_acceleration)
EM_PROPERTY(float, perpendicular_velocity)
EM_PROPERTY(float, perpendicular_acceleration)
EM_PROPERTY(float, angle)
EM_PROPERTY(float, radial_velocity)
EM_PROPERTY(float, radial_acceleration)
EM_PROPERTY(float, opacity)
EM_PROPERTY(float, opacity_rate)
EM_PROPERTY(float, blend)
EM_PROPERTY(float, blend_rate)
EM_PROPERTY(float, start_color_mix)
EM_PROPERTY(float, color_mixing_velocity)
EM_PROPERTY(float, life)
EM_PROPERTY(float, emission_radius)

bool particle_emitter_component::aspect_correction() const {
    return _ei.aspect_correction;
}

bool particle_emitter_component::aspect_correction(bool value){
    _ei.aspect_correction = value;
    if(_id){
        parent_layer()->particle_layer()->emitter(_id)->aspect_correction = value;
        parent_layer()->particle_layer()->emitter(_id)->initialized = false;
    }
    return _ei.aspect_correction;
}

const rb_string& particle_emitter_component::image_name() const {
    return _ei.image_name;
}

const rb_string& particle_emitter_component::image_name(const rb_string &value){
    _ei.image_name = value;
    if(_id){
        parent_layer()->particle_layer()->emitter(_id)->image_name = value;
        parent_layer()->particle_layer()->emitter(_id)->initialized = false;
    }
    return _ei.image_name;
}

float particle_emitter_component::inv_emission_rate() const {
    return _ei.inv_emission_rate;
}

float particle_emitter_component::inv_emission_rate(float value){
    _ei.inv_emission_rate = value;
    if(_id)
        parent_layer()->particle_layer()->emitter(_id)->inv_emission_rate = value;
    return _ei.inv_emission_rate;
}

float particle_emitter_component::duration() const {
    return _ei.duration;
}

float particle_emitter_component::duration(float value){
    _ei.duration = value;
    if(_id && (!in_editor() || is_playing()))
        parent_layer()->particle_layer()->emitter(_id)->duration = value;
    return _ei.duration;
}

float particle_emitter_component::delay() const {
    return _ei.delay;
}

float particle_emitter_component::delay(float value){
    _ei.delay = value;
    if(_id && (!in_editor() || is_playing()))
        parent_layer()->particle_layer()->emitter(_id)->delay = value;
    return _ei.delay;
}

float particle_emitter_component::editor_duration() const {
    return _editor_duration;
}

float particle_emitter_component::editor_duration(float value){
    _editor_duration = value;
    if(_id && in_editor() && !is_playing()){
        parent_layer()->particle_layer()->emitter(_id)->duration = value;
        parent_layer()->particle_layer()->reset_emitter(_id);
    }
    return _editor_duration;
}

float particle_emitter_component::editor_delay() const {
    return _editor_delay;
}

float particle_emitter_component::editor_delay(float value){
    _editor_delay = value;
    if(_id && in_editor() && !is_playing()){
        parent_layer()->particle_layer()->emitter(_id)->delay = value;
        parent_layer()->particle_layer()->reset_emitter(_id);
    }
    return _editor_delay;
}

bool particle_emitter_component::loop() const{
    return _ei.loop;
}

bool particle_emitter_component::loop(bool value){
    _ei.loop = value;
    if(_id && (!in_editor() || is_playing()))
        parent_layer()->particle_layer()->emitter(_id)->loop = value;
    return _ei.loop;
}

bool particle_emitter_component::editor_loop() const {
    return _editor_loop;
}

bool particle_emitter_component::editor_loop(bool value) {
    _editor_loop = value;
    if(_id && in_editor() && !is_playing()){
        parent_layer()->particle_layer()->emitter(_id)->loop = value;
        parent_layer()->particle_layer()->reset_emitter(_id);
    }
    return _editor_loop;
}

particle_state particle_emitter_component::state() const {
    return _state;
}

particle_state particle_emitter_component::state(rb::particle_state value) {
    _state = value;
    if(_id && (!in_editor() || is_playing())){
        parent_layer()->particle_layer()->emitter_state(_id, value);
    }
    return _state;
}

bool particle_emitter_component::uniform_size() const {
    return _ei.uniform_size;
}

bool particle_emitter_component::uniform_size(bool value){
    _ei.uniform_size = value;
    if(_id)
        parent_layer()->particle_layer()->emitter(_id)->uniform_size = value;
    return _ei.uniform_size;
}



























