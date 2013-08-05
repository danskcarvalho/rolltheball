//
//  layer.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 07/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "layer.h"
#include "node.h"
#include "scene.h"
#include "extended_dynamic_mesh_batch.h"
#include "extended_static_mesh_batch.h"
#include "process.h"
#include "mesh.h"
#include "texture.h"
#include "texture_atlas.h"
#include "basic_process.h"
#include "director.h"
#include "editor_delegate.h"
#include "transform_gizmo.h"

using namespace rb;

layer::layer(){
    _parent_scene = nullptr;
    _playing = true;
    _dynamic_batch = nullptr;
    _static_batch = nullptr;
    _validated = false;
    _static_layer = false;
    _ambient_color = color::from_rgba(1, 1, 1, 1);
    _blend_mode = blend_mode::normal;
    _textureless_process = nullptr;
    _camera_position_factor = vec2(1, 1);
    _camera_invariant_flags = camera_invariant::none;
}

layer::layer(scene* parent_scene){
    _parent_scene = parent_scene;
    _playing = true;
    _dynamic_batch = nullptr;
    _static_batch = nullptr;
    _validated = false;
    _static_layer = false;
    _ambient_color = color::from_rgba(1, 1, 1, 1);
    _blend_mode = blend_mode::normal;
    _textureless_process = nullptr;
    _camera_position_factor = vec2(1, 1);
    _camera_invariant_flags = camera_invariant::none;
}

layer::~layer(){
    if(_dynamic_batch)
        delete _dynamic_batch;
    if(_static_batch)
        delete _static_batch;
    for (auto _p : _processes)
        delete _p;
    if(_dynamic_batch)
        delete _dynamic_batch;
    if(_static_batch)
        delete _static_batch;
    if(_textureless_process)
        delete _textureless_process;
    _validated = false;
}

//Parent scene
scene* layer::parent_scene(){
    return _parent_scene;
}

const scene* layer::parent_scene() const {
    return _parent_scene;
}

const layer* layer::parent_layer() const {
    return nullptr;
}


//activation listeners
void layer::before_becoming_active(){
    for (auto _child : *this){
        _child->internal_before_becoming_active(false);
    }
}
void layer::after_becoming_active(){
    _dynamic_batch = new extended_dynamic_mesh_batch();
    _static_batch = nullptr;
    if(_parent_scene->_texture_atlas){
        texture_atlas_changed();
    }
    if(_textureless_process)
        delete _textureless_process;
    _textureless_process = new basic_process(parent_scene()->_program_manager);
    _textureless_process->texture_sampler(nullptr);
    
    for (auto _child : *this){
        _child->internal_after_becoming_active(false);
    }
}
void layer::before_becoming_inactive(){
    for (auto _child : *this){
        _child->internal_before_becoming_inactive(false);
    }
}
void layer::after_becoming_inactive(){
    if(_dynamic_batch)
        delete _dynamic_batch;
    if(_static_batch)
        delete _static_batch;
    if(_textureless_process)
        delete _textureless_process;
    for (auto _p : _processes)
        delete _p;
    _processes.clear();
    _tx_group_map.clear();
    for (int i = 0; i < EFFECT_TEXTURED_COUNT; i++)
        _tx_proc_map[i].clear();
    
    _dynamic_batch = nullptr;
    _static_batch = nullptr;
    _textureless_process = nullptr;
    
    for (auto _child : *this){
        _child->internal_after_becoming_inactive(false);
    }
}

bool layer::active() const {
    return _parent_scene->active();
}

bool layer::add_node_at(node* n, uint32_t at){
    assert(n);
    assert(!n->_added);
    bool _n_added = this->node_container::add_node_at(n, at);
    if(_n_added)
    {
        n->_added = true;
        n->_parent_layer = this;
        n->_parent_node = nullptr;
        n->set_layer_recursively(this);
        
        if(parent_scene()->active()){
            n->internal_before_becoming_active(n->_move_flag != 0);
            n->internal_after_becoming_active(n->_move_flag != 0);
        }
    }
    if(_n_added && active())
        invalidate_buffers();
    if(director::editor_delegate())
        director::editor_delegate()->hierarchy_changed(this);
    return _n_added;
}
bool layer::remove_node(node* n, bool cleanup){
    assert(n);
    assert(n->_added);
    bool _n_removed = this->node_container::remove_node(n, false);
    if(_n_removed){
        if(parent_scene()->active()){
            n->internal_before_becoming_inactive(n->_move_flag != 0);
        }
        
        n->_added = false;
        n->_parent_layer = nullptr;
        n->_parent_node = nullptr;
        n->set_layer_recursively(nullptr);
        
        if(parent_scene()->active()){
            n->internal_after_becoming_inactive(n->_move_flag != 0);
        }
        if(cleanup)
            n->commit_suicide();
    }
    if(_n_removed && active())
        invalidate_buffers();
    if(director::editor_delegate())
        director::editor_delegate()->hierarchy_changed(this);
    return _n_removed;
}

bool layer::bring_to_front(node* n){
    bool _r = this->node_container::bring_to_front(n);
    if(_r)
        invalidate_buffers();
    if(director::editor_delegate())
        director::editor_delegate()->hierarchy_changed(this);
    return _r;
}
bool layer::send_to_back(node* n){
    bool _r = this->node_container::send_to_back(n);
    if(_r)
        invalidate_buffers();
    if(director::editor_delegate())
        director::editor_delegate()->hierarchy_changed(this);
    return _r;
}
bool layer::send_backward(node* n){
    bool _r = this->node_container::send_backward(n);
    if(_r)
        invalidate_buffers();
    if(director::editor_delegate())
        director::editor_delegate()->hierarchy_changed(this);
    return _r;
}
bool layer::bring_forward(node* n){
    bool _r = this->node_container::bring_forward(n);
    if(_r)
        invalidate_buffers();
    if(director::editor_delegate())
        director::editor_delegate()->hierarchy_changed(this);
    return _r;
}

bool layer::playing() const {
    return _playing && _parent_scene->playing();
}

void layer::playing(const bool value) {
    if(_playing == value)
        return;
    if(!_parent_scene){
        _playing = value;
        return;
    }
    auto _previous = playing();
    _playing = value;
    auto _now = playing();
    if (_previous == _now)
        return;
    
    for (auto _n : *this){
        if(_now)
            _n->playing();
        else
            _n->paused();
    }
}

void layer::scene_playing(){
    auto _previous = false;
    auto _now = _playing;
    if(_previous == _now)
        return;
    for (auto _n : *this)
    {
        if(_now)
            _n->playing();
        else
            _n->paused();
    }
}

void layer::scene_paused(){
    if(parent_scene()->in_editor() && _static_layer){
        if(!_dynamic_batch)
            _dynamic_batch = new extended_dynamic_mesh_batch();
        if(_static_batch){
            delete _static_batch;
            _static_batch = nullptr;
        }
    }
    auto _previous = _playing;
    auto _now = false;
    if (_previous == _now)
        return;
    for (auto _n : *this){
        if (_now)
            _n->playing();
        else
            _n->paused();
    }
}

bool layer::playing_flag() const {
    return _playing;
}

void layer::create_textured_effect(const int current_effect, const std::vector<rb_string>& texture_names){
    auto _current_effect = current_effect;
    auto _texture_names = texture_names;
    std::unordered_map<texture*, basic_process*> _textures_added;
    std::vector<rb_string> _group_names;
    for(auto& _name : _texture_names){
        auto _tx = parent_scene()->_texture_atlas->get_atlas(_primary_group_name, _name);
        if(!_tx)
            _tx = parent_scene()->_texture_atlas->get_atlas(_secondary_group_name, _name);
        if(!_tx){
            parent_scene()->_texture_atlas->get_groups(_name, _group_names);
            assert(_group_names.size() != 0);
            _tx = parent_scene()->_texture_atlas->get_atlas(_group_names[0], _name);
        }
        assert(_tx);
        if(_textures_added.count(const_cast<texture*>(_tx)) != 0)
        {
            _tx_proc_map[_current_effect].insert(std::make_pair(_name, _textures_added.at((texture*)_tx)));
        }
        else {
            auto _basic_process = new basic_process(parent_scene()->_program_manager, _current_effect == EFFECT_TEXTURED_WITH_WRAPPING);
            _basic_process->texture_sampler(_tx);
            _processes.push_back(_basic_process);
            _textures_added.insert({(texture*)_tx, _basic_process});
            _tx_proc_map[_current_effect].insert(std::make_pair(_name, _basic_process));
        }
    }
}

const rb_string& layer::primary_group_name() const {
    return _primary_group_name;
}
const rb_string& layer::primary_group_name(const rb_string& value) {
    if(_primary_group_name == value)
        return _primary_group_name;
    _primary_group_name = value;
    if(_parent_scene && _parent_scene->active())
        texture_atlas_changed();
    return _primary_group_name;
}
const rb_string& layer::secondary_group_name() const {
    return _secondary_group_name;
}
const rb_string& layer::secondary_group_name(const rb_string& value) {
    if(_secondary_group_name == value)
        return _secondary_group_name;
    _secondary_group_name = value;
    if(_parent_scene && _parent_scene->active())
        texture_atlas_changed();
    return _secondary_group_name;
}

void layer::texture_atlas_changed(){
    invalidate_buffers(); //we invalidate buffers
    for (auto _p : _processes)
        delete _p;
    _processes.clear();
    _tx_group_map.clear();
    for (int i = 0; i < EFFECT_TEXTURED_COUNT; i++)
        _tx_proc_map[i].clear();
    if(!parent_scene()->_texture_atlas)
        return;
    //for each effect...
    std::vector<rb_string> _texture_names;
    parent_scene()->_texture_atlas->get_texture_names(_texture_names);
    
    //Textured Effects
    int _current_effect = EFFECT_TEXTURED;
    while (_current_effect <= EFFECT_TEXTURED_WITH_WRAPPING) {
        create_textured_effect(_current_effect, _texture_names);
        _current_effect++;
    }
    
    create_texture_group_mapping();
}

void layer::create_texture_group_mapping(){
    std::vector<rb_string> _texture_names;
    parent_scene()->_texture_atlas->get_texture_names(_texture_names);
    
    std::vector<rb_string> _group_names;
    //Create texture-name/group mappings
    for(auto& _name : _texture_names){
        rb_string* _group_n;
        auto _tx = parent_scene()->_texture_atlas->get_atlas(_primary_group_name, _name);
        _group_n = &_primary_group_name;
        if(!_tx){
            _tx = parent_scene()->_texture_atlas->get_atlas(_secondary_group_name, _name);
            _group_n = &_secondary_group_name;
        }
        if(!_tx){
            parent_scene()->_texture_atlas->get_groups(_name, _group_names);
            assert(_group_names.size() != 0);
            _tx = parent_scene()->_texture_atlas->get_atlas(_group_names[0], _name);
            _group_n = &_group_names[0];
        }
        assert(_tx);
        _tx_group_map[_name] = *_group_n;
    }
}

void layer::invalidate_buffers(){
    if (_dynamic_batch)
        _dynamic_batch->clear_meshes();
    _validated = false;
}

void layer::add_mesh_for_rendering(rb::mesh *m, const rb_string &texture_name, bool wrapping_in_shader){
    assert(active());
    assert(_dynamic_batch);
    auto& _map = _tx_proc_map[wrapping_in_shader ? EFFECT_TEXTURED_WITH_WRAPPING : EFFECT_TEXTURED];
    if(_map.count(texture_name) != 0)
        _dynamic_batch->add_mesh(m, _map.at(texture_name));
    else {
        _dynamic_batch->add_mesh(m, _textureless_process); //we get the first texture
    }
}

void layer::set_texture_for_mesh(rb::mesh *m, const rb_string &texture_name, bool wrapping_in_shader){
    assert(active());
    assert(_dynamic_batch);
    if(!_dynamic_batch->contains_mesh(m)){
        return;
    }
    auto& _map = _tx_proc_map[wrapping_in_shader ? EFFECT_TEXTURED_WITH_WRAPPING : EFFECT_TEXTURED];
    if(_map.count(texture_name) != 0)
        _dynamic_batch->process(m, _map.at(texture_name));
    else {
        _dynamic_batch->process(m, _textureless_process); //we get the first texture
    }
}

void layer::add_gizmo(rb::mesh *m, const rb_string &texture_name, bool wrapping_in_shader){
    assert(active());
    assert(parent_scene()->_gizmo_layer);
    auto& _map = _tx_proc_map[wrapping_in_shader ? EFFECT_TEXTURED_WITH_WRAPPING : EFFECT_TEXTURED];
    if(_map.count(texture_name) != 0)
        parent_scene()->_gizmo_layer->add_mesh(m, _map.at(texture_name));
    else
        parent_scene()->_gizmo_layer->add_mesh(m, _textureless_process); //we get the first texture
}

const texture_map* layer::create_mapping(const rb_string& texture_name, const transform_space& transform, texture_map* refurbished){
    if(_tx_group_map.count(texture_name) == 0)
        return nullptr;
    return parent_scene()->_texture_atlas->create_mapping(_tx_group_map[texture_name], texture_name, transform, refurbished);
}

bool layer::static_layer(const bool value) {
    if(parent_scene())
        assert(!parent_scene()->_playing);
    _static_layer = value;
    return _static_layer;
}

const color& layer::ambient_color() const {
    return _ambient_color;
}

const color& layer::ambient_color(const rb::color &value) {
    _ambient_color = value;
    return _ambient_color;
}

enum blend_mode layer::blend_mode() const {
    return _blend_mode;
}

enum blend_mode layer::blend_mode(const enum blend_mode value){
    _blend_mode = value;
    return _blend_mode;
}

void layer::setup_processes(){
    const vec2& _viewport_size = parent_scene()->viewport_size();
    float _aspect = _viewport_size.y() / _viewport_size.x();
    transform_space _correction = parent_scene()->_enabled_aspect_correction ? transform_space(vec2::zero, vec2(_aspect, 1), 0) : transform_space();
    auto _camera = parent_scene()->camera();
    if(rb::has_flag(this->camera_invariant_flags(), camera_invariant::position))
        _camera.origin(vec2::zero);
    else
        _camera.origin(_camera.origin() * this->camera_position_factor());
    if(rb::has_flag(this->camera_invariant_flags(), camera_invariant::scale))
        _camera.scale(vec2(1, 1));
    if(rb::has_flag(this->camera_invariant_flags(), camera_invariant::rotation))
        _camera.rotation(0);
    
    auto _final_transform = _correction * _camera.inverse() * transform();
    _textureless_process->ambient_color(_ambient_color.pre_multiplied());
    _textureless_process->position_transform(_final_transform);
    for (auto _p : _processes){
        auto _basic_process = dynamic_cast<basic_process*>(_p);
        if(_basic_process){
            _basic_process->ambient_color(_ambient_color.pre_multiplied());
            _basic_process->position_transform(_final_transform);
        }
    }
}

void layer::setup_processes_for_gizmo_layer(){
    auto _normalize = transform_space(vec2::zero, vec2(1.0 / parent_scene()->viewport_size().x(), 1.0 / parent_scene()->viewport_size().y()), 0);
    auto _multiply = transform_space(vec2::zero, vec2(2, 2), 0);
    auto _translate = transform_space(vec2(-1, -1), vec2(1, 1), 0);
    auto _final_transform = _translate * _multiply * _normalize;
    
    _textureless_process->ambient_color(_ambient_color.pre_multiplied());
    _textureless_process->position_transform(_final_transform);
    
    for (auto _p : _processes){
        auto _basic_process = dynamic_cast<basic_process*>(_p);
        if(_basic_process){
            _basic_process->ambient_color(color::from_rgba(1, 1, 1, 1));
            _basic_process->position_transform(_final_transform);
        }
    }
}

void layer::render(){
    assert(active());
    setup_processes();
    if(_static_layer && _static_batch){
        _static_batch->blend_mode(_blend_mode);
        _static_batch->geometry_type(geometry_type::triangle);
        _static_batch->draw();
        return;
    }
    clear_dirty_transform();
    
    _dynamic_batch->blend_mode(_blend_mode);
    _dynamic_batch->geometry_type(geometry_type::triangle);
    
    for(auto _child : *this){
        if(_child->renderable() && !_child->_new_template && !is_hidden_in_editor() && !_child->is_hidden_in_editor())
            _child->internal_render(!_validated);
    }
    _dynamic_batch->draw();
    
    setup_processes_for_gizmo_layer();
    for (auto _child : *this){
        if(_child->renderable() && !_child->_new_template && !is_hidden_in_editor() && !_child->is_hidden_in_editor())
            _child->internal_render_gizmo();
    }
    
    _validated = true;
    if(_static_layer && !_static_batch && parent_scene()->_playing){
        _static_batch = _dynamic_batch->compile();
        delete _dynamic_batch;
        _dynamic_batch = nullptr;
    }
}

const node_container* layer::parent_node_container() const {
    return nullptr;
}

transform_space layer::from_layer_space_to(const rb::space another)const {
    assert(active());
    return from_space_to(another);
}

transform_space layer::from_layer_space_to(const rb::node_container *another) const {
    assert(active());
    assert(another);
    if(this == another)
        return transform_space(); //identity
    auto _to_scene_1 = another->from_space_to(space::scene);
    auto _to_scene_2 = from_space_to(space::scene);
    return _to_scene_2.inverse() * _to_scene_1;
}

rb_string to_group_name(uint32_t flags){
    if(flags & (1 << 0))
        return u"g00";
    else if(flags & (1 << 1))
        return u"g01";
    else if(flags & (1 << 2))
        return u"g02";
    else if(flags & (1 << 3))
        return u"g03";
    else if(flags & (1 << 4))
        return u"g04";
    else if(flags & (1 << 5))
        return u"g05";
    else if(flags & (1 << 6))
        return u"g06";
    else if(flags & (1 << 7))
        return u"g07";
    else if(flags & (1 << 8))
        return u"g08";
    else if(flags & (1 << 9))
        return u"g09";
    else
        return u"";
}

uint32_t from_group_name(const rb_string& grp_name){
    if(grp_name == u"g00")
        return 1 << 0;
    else if(grp_name == u"g01")
        return 1 << 1;
    else if(grp_name == u"g02")
        return 1 << 2;
    else if(grp_name == u"g03")
        return 1 << 3;
    else if(grp_name == u"g04")
        return 1 << 4;
    else if(grp_name == u"g05")
        return 1 << 5;
    else if(grp_name == u"g06")
        return 1 << 6;
    else if(grp_name == u"g07")
        return 1 << 7;
    else if(grp_name == u"g08")
        return 1 << 8;
    else if(grp_name == u"g09")
        return 1 << 9;
    else
        return 0;
}

void layer::describe_type(){
    node_container::describe_type();
    start_type<layer>([]() { return new layer(); });
    boolean_property<layer>(u"static_layer", u"Static", true, {
        [](const layer* site){
            return site->static_layer();
        },
        [](layer* site, bool value){
            site->static_layer(value);
        }
    });
    boolean_property<layer>(u"playing", u"Playing", true, {
        [](const layer* site){
            return site->playing_flag();
        },
        [](layer* site, bool value){
            site->playing(value);
        }
    });
    const std::map<rb_string, enum blend_mode> _e_vals({{u"Normal", blend_mode::normal}, {u"Multiply", blend_mode::multiply}, {u"Screen", blend_mode::screen}, {u"Add", blend_mode::add}});
    enumeration_property<layer, enum blend_mode>(u"blend_mode", u"Blend Mode", _e_vals, true, {
        [](const layer* site){
            return site->blend_mode();
        },
        [](layer* site, enum blend_mode value){
            site->blend_mode(value);
        }
    });
    color_property<layer>(u"ambient_color", u"Amb Color", true, true, {
        [](const layer* site){
            return site->ambient_color();
        },
        [](layer* site, const color& value){
            site->ambient_color(value);
        }
    });
    //Primary Group
    flags_property<layer, uint32_t>(u"primary_g00_g03", u"Primary Grp", {u"0", u"1", u"2", u"3"}, true, {
        [](const layer* site){
            return from_group_name(site->primary_group_name()) & 0xF;
        },
        [](layer* site, uint32 value){
            if(value == 0)
                return;
            auto _current = from_group_name(site->primary_group_name());
            auto _new = value & (~_current);
            site->primary_group_name(to_group_name(_new));
            site->force_notify_property_changed(u"primary_g00_g03");
            site->force_notify_property_changed(u"primary_g04_g07");
            site->force_notify_property_changed(u"primary_g08_g09");
        }
    });
    flags_property<layer, uint32_t>(u"primary_g04_g07", u"", {u"4", u"5", u"6", u"7"}, true, {
        [](const layer* site){
            return (from_group_name(site->primary_group_name()) & 0xF0) >> 4;
        },
        [](layer* site, uint32 value){
            if(value == 0)
                return;
            value <<= 4;
            auto _current = from_group_name(site->primary_group_name());
            auto _new = value & (~_current);
            site->primary_group_name(to_group_name(_new));
            site->force_notify_property_changed(u"primary_g00_g03");
            site->force_notify_property_changed(u"primary_g04_g07");
            site->force_notify_property_changed(u"primary_g08_g09");
        }
    });
    flags_property<layer, uint32_t>(u"primary_g08_g09", u"", {u"8", u"9"}, true, {
        [](const layer* site){
            return (from_group_name(site->primary_group_name()) & 0xF00) >> 8;
        },
        [](layer* site, uint32 value){
            if(value == 0)
                return;
            value <<= 8;
            auto _current = from_group_name(site->primary_group_name());
            auto _new = value & (~_current);
            site->primary_group_name(to_group_name(_new));
            site->force_notify_property_changed(u"primary_g00_g03");
            site->force_notify_property_changed(u"primary_g04_g07");
            site->force_notify_property_changed(u"primary_g08_g09");
        }
    });
    
    //Secondaru Group
    flags_property<layer, uint32_t>(u"secondary_g00_g03", u"Secondary Grp", {u"0", u"1", u"2", u"3"}, true, {
        [](const layer* site){
            return from_group_name(site->secondary_group_name()) & 0xF;
        },
        [](layer* site, uint32 value){
            if(value == 0)
                return;
            auto _current = from_group_name(site->secondary_group_name());
            auto _new = value & (~_current);
            site->secondary_group_name(to_group_name(_new));
            site->force_notify_property_changed(u"secondary_g00_g03");
            site->force_notify_property_changed(u"secondary_g04_g07");
            site->force_notify_property_changed(u"secondary_g08_g09");
        }
    });
    flags_property<layer, uint32_t>(u"secondary_g04_g07", u"", {u"4", u"5", u"6", u"7"}, true, {
        [](const layer* site){
            return (from_group_name(site->secondary_group_name()) & 0xF0) >> 4;
        },
        [](layer* site, uint32 value){
            if(value == 0)
                return;
            value <<= 4;
            auto _current = from_group_name(site->secondary_group_name());
            auto _new = value & (~_current);
            site->secondary_group_name(to_group_name(_new));
            site->force_notify_property_changed(u"secondary_g00_g03");
            site->force_notify_property_changed(u"secondary_g04_g07");
            site->force_notify_property_changed(u"secondary_g08_g09");
        }
    });
    flags_property<layer, uint32_t>(u"secondary_g08_g09", u"", {u"8", u"9"}, true, {
        [](const layer* site){
            return (from_group_name(site->secondary_group_name()) & 0xF00) >> 8;
        },
        [](layer* site, uint32 value){
            if(value == 0)
                return;
            value <<= 8;
            auto _current = from_group_name(site->secondary_group_name());
            auto _new = value & (~_current);
            site->secondary_group_name(to_group_name(_new));
            site->force_notify_property_changed(u"secondary_g00_g03");
            site->force_notify_property_changed(u"secondary_g04_g07");
            site->force_notify_property_changed(u"secondary_g08_g09");
        }
    });
    //camera
    vec2_property<layer>(u"camera_position_factor", u"Cam Factor", true, {
        [](const layer* site){
            return site->_camera_position_factor;
        },
        [](layer* site, const vec2& value){
            site->camera_position_factor(value);
        }
    });
    flags_property<layer, uint32_t>(u"camera_invariant", u"Cam Invariant", { u"Position", u"Scale", u"Rotate" }, true, {
        [](const layer* site){
            return (uint32_t)site->_camera_invariant_flags;
        },
        [](layer* site, uint32 value){
            site->camera_invariant_flags((camera_invariant)value);
        }
    });
    end_type();
}

rb_string layer::type_name() const {
    return u"rb::layer";
}

rb_string layer::displayable_type_name() const {
    return u"Layer";
}

std::vector<rb_string> layer::transformables(){
    return {
        u"Layer Transform",
        u"Adjust Layer Transform"
    };
}

void layer::start_transformation(long index){
    parent_scene()->hidden_layer()->transform(transform_space());
    if(index == LAYER_TRANSFORM){
        transform_gizmo::start_transformation(parent_scene()->hidden_layer(), transform(), rectangle(0, 0, 1, 1), true, [=](transform_gizmo* g, const transform_space& t){
            this->transform(t);
        });
    }
    else {
        transform_gizmo::start_transformation(parent_scene()->hidden_layer(), transform(), rectangle(0, 0, 1, 1), false, [=](transform_gizmo* g, const transform_space& t){
            this->adjust_transformation(t);
        });
    }
}

void layer::set_children(const std::vector<typed_object *> &children){
    node_container::set_children(children);
    for(auto _c : children){
        auto _n = dynamic_cast<node*>(_c);
        _n->set_layer_recursively(this);
    }
}

const vec2& layer::camera_position_factor() const {
    return _camera_position_factor;
}

const vec2& layer::camera_position_factor(const rb::vec2 &value){
    if(_camera_position_factor == value)
        return _camera_position_factor;
    _camera_position_factor = value;
    _dirty_transform = true;
    return _camera_position_factor;
}

camera_invariant layer::camera_invariant_flags() const {
    return _camera_invariant_flags;
}

camera_invariant layer::camera_invariant_flags(rb::camera_invariant value){
    if(_camera_invariant_flags == value)
        return _camera_invariant_flags;
    _camera_invariant_flags = value;
    _dirty_transform = true;
    return _camera_invariant_flags;
}





























