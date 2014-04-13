//
//  destructible_sprite_component.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 17/11/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#include "destructible_sprite_component.h"
#include "rectangle.h"
#include "mesh.h"
#include "texture_map.h"
#include "transform_space.h"
#include "vertex.h"
#include "scene.h"
#include "texture_atlas.h"

using namespace rb;

destructible_sprite_component::destructible_sprite_component(){
    _map = nullptr;
    _image = u"";
    _reapply_mapping = false;
    _opacity = 1;
    _tint = color::from_rgba(1, 1, 1, 1);
    _blend = 1;
    _collapsed = true;
    _visible = true;
    _aspect_correction = true;
    _matrix = vec2(1, 1);
    _tex_size = nullptr;
    _last_atlas = nullptr;
    
    for (size_t i = 0; i < _matrix.x() * _matrix.y(); i++) {
        _part_transforms.push_back(transform_space());
        _part_befores.push_back(transform_space());
    }
}

void destructible_sprite_component::destroy(){
    for (auto _it_m : _m)
        delete _it_m;
    _m.clear();
    for (auto _it_m : _m_copy)
        delete _it_m;
    _m_copy.clear();
//    _part_befores.clear();
//    _part_transforms.clear();
    for (size_t i = 0; i < _matrix.x() * _matrix.y(); i++) {
        _part_transforms[i] = transform_space();
        _part_befores[i] = transform_space();
    }
    if(_map)
        delete _map;
    _map = nullptr;
}

void destructible_sprite_component::create(){
    if(!_map){
        _map = (texture_map*)create_mapping(_image, transform_space());
    }
    
    if(_m.size() == 0){
        auto _new_matrix = _matrix;
        _new_matrix.x(roundf(_new_matrix.x()));
        _new_matrix.y(roundf(_new_matrix.y()));
        if(_new_matrix.x() < 1)
            _new_matrix.x(1);
        if(_new_matrix.y() < 1)
            _new_matrix.y(1);
        
        rectangle _rc(0, 0, 1, 1);
        if(_map)
            _m = _rc.to_meshes(_map->bounds(), _new_matrix.x(), _new_matrix.y());
        else
            _m = _rc.to_meshes(rectangle(0.5, 0.5, 1, 1), _new_matrix.x(), _new_matrix.y()); //no texture coordinates
        
        for (auto _m_it : _m){
            _m_it->set_alpha(_opacity);
            _m_it->set_color(_tint);
            _m_it->set_blend(_blend);
//            _part_befores.push_back(transform_space());
//            _part_transforms.push_back(transform_space());
            auto _m_it_copy = new mesh();
            *_m_it_copy = *_m_it;
            _m_copy.push_back(_m_it_copy);
        }
        
        _collapsed = false;
    }
}

destructible_sprite_component::~destructible_sprite_component(){
    destroy();
}

void destructible_sprite_component::update_collapsed_mesh(){
    if(_m.size() == 0)
        return;
    
    if(_collapsed){
        for (auto _m_it : _m){
            _m_it->lock_vertex_buffer([=](vertex* buffer){
                for (auto i = 0; i < _m_it->vertex_count(); i++) {
                    buffer[i].set_position(vec2::zero);
                }
            });
        }
    }
    else{
        for (size_t i = 0; i < _m.size(); i++) {
            *_m[i] = *_m_copy[i];
            _part_befores[i] = transform_space();
        }
    }
}

void destructible_sprite_component::update_collapsed_flag(){
    auto _previous = _collapsed;
    if(!_map)
        _map = (texture_map*)create_mapping(_image, transform_space());
    
    if((!_visible && (!in_editor() || is_playing())) || !_map)
        _collapsed = true;
    else
        _collapsed = false;
    
    if(_previous != _collapsed)
        update_collapsed_mesh();
}

void destructible_sprite_component::after_becoming_active(bool node_was_moved){
    enabled(node_capability::children_rendering, false);
    enabled(node_capability::rendering, true);
    enabled(node_capability::can_become_current, false);
    //    if(!node_was_moved)
    //        destroy();
}
void destructible_sprite_component::before_becoming_inactive(bool node_was_moved){
    //    if(!node_was_moved)
    //        destroy();
}
void destructible_sprite_component::reapply_mapping(){
    auto _temp = (texture_map*)create_mapping(_image, transform_space(), _map);
    if(!_temp){ //delete refurbished...
        if(_map)
            delete _map;
        _map = nullptr;
        return;
    }
    
    _map = _temp;
    
    update_collapsed_flag();
    
    if(!_collapsed){
        auto _new_matrix = _matrix;
        _new_matrix.x(roundf(_new_matrix.x()));
        _new_matrix.y(roundf(_new_matrix.y()));
        if(_new_matrix.x() < 1)
            _new_matrix.x(1);
        if(_new_matrix.y() < 1)
            _new_matrix.y(1);
        
        //Texture
        auto _tw = _map->bounds().size().x() / (float)_new_matrix.x();
        auto _th = _map->bounds().size().y() / (float)_new_matrix.y();
        auto _tstart = _map->bounds().bottom_left();
        
        for (uint32_t i = 0; i < (uint32_t)_new_matrix.x(); i++) {
            for (uint32_t j = 0; j < (uint32_t)_new_matrix.y(); j++) {
                //Texture
                auto _tsw = _tstart + vec2::right * i * _tw;
                auto _tew = _tsw + vec2::right * _tw;
                auto _tsh = _tstart + vec2::up * j * _th;
                auto _teh = _tsh + vec2::up * _th;
                auto _tr = rectangle((_tsw.x() + _tew.x()) / 2.0f, (_tsh.y() + _teh.y()) / 2.0f, fabsf(_tew.x() - _tsw.x()), fabs(_teh.y() - _tsh.y()));
                auto _m_index = i * ((uint32_t)_new_matrix.y()) + j;
                if (_m_index < _m.size()){
                    _m[_m_index]->remap(_tr, texture_mapping_type::untransformable);
                    set_texture_for_mesh(_m[_m_index], _image, false);
                }
                if (_m_index < _m_copy.size()){
                    _m_copy[_m_index]->remap(_tr, texture_mapping_type::untransformable);
                }
            }
        }
    }
}

bool destructible_sprite_component::aspect_correction() const{
    return _aspect_correction;
}

bool destructible_sprite_component::aspect_correction(const bool value){
    _aspect_correction = value;
    return _aspect_correction;
}

transform_space destructible_sprite_component::aspect_correction_factor() const {
    vec2 _tex_s = size_of_tex();
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
    
    return _aspect_correction ? transform_space(vec2(0, 0), vec2(_tx, _ty), 0) : transform_space();
}

void destructible_sprite_component::transform_mesh(const bool refill_buffers){
    for (size_t i = 0; i < _m.size(); i++) {
        transform_space _to_layer = from_node_space_to(space::layer) *
            _part_transforms[i] *
            aspect_correction_factor();
        if(_part_befores[i] != _to_layer || refill_buffers){
            *_m[i] = *_m_copy[i];
            _part_befores[i] = _to_layer;
            _to_layer.from_space_to_base().transform_mesh(*_m[i]);
        }
    }
}

void destructible_sprite_component::render(const bool refill_buffers){
    if(_m.size() == 0 || refill_buffers || _reapply_mapping){
        if(_m.size() == 0){
            destroy();
            create();
            _reapply_mapping = false;
        }
        else if(_reapply_mapping){
            reapply_mapping();
            _reapply_mapping = false;
        }
    }
    
    if(_m.size() != 0){
        update_collapsed_flag();
        if(!_collapsed)
            transform_mesh(refill_buffers);
        
        if(refill_buffers){
            for (auto _m_it : _m)
                add_mesh_for_rendering(_m_it, _image, false);
        }
    }
}
rectangle destructible_sprite_component::bounds() const{
    auto _rc = rectangle(0, 0, 1, 1);
    auto _ac = aspect_correction_factor();
    _rc.size(_rc.size() * _ac.scale());
    return _rc;
}

float destructible_sprite_component::opacity() const {
    return _opacity;
}

float destructible_sprite_component::opacity(float value){
    if(value > 1)
        value = 1;
    this->_opacity = value;
    for (size_t i = 0; i < _m.size(); i++) {
        _m[i]->set_alpha(this->_opacity);
        _m_copy[i]->set_alpha(this->_opacity);
    }
    return _opacity;
}

const color& destructible_sprite_component::tint() const {
    return _tint;
}

const color& destructible_sprite_component::tint(const rb::color &value){
    this->_tint = value;
    for (size_t i = 0; i < _m.size(); i++) {
        _m[i]->set_color(this->_tint);
        _m_copy[i]->set_color(this->_tint);
    }
    return _tint;
}

float destructible_sprite_component::blend() const {
    return _blend;
}

float destructible_sprite_component::blend(float value){
    this->_blend = value;
    for (size_t i = 0; i < _m.size(); i++) {
        _m[i]->set_blend(this->_blend);
        _m_copy[i]->set_blend(this->_blend);
    }
    return _blend;
}


bool destructible_sprite_component::visible() const {
    return _visible;
}

bool destructible_sprite_component::visible(bool value){
    this->_visible = value;
    return _visible;
}

void destructible_sprite_component::describe_type(){
    node::describe_type();
    start_type<destructible_sprite_component>([](){ return new destructible_sprite_component(); });
    image_property<destructible_sprite_component>(u"image", u"Image", true, {
        [](const destructible_sprite_component* site){
            return site->_image;
        },
        [](destructible_sprite_component* site, const rb_string& value){
            site->image_name(value);
        }
    });
    ranged_property<destructible_sprite_component>(u"opacity", u"Opacity", true, true, 5, {
        [](const destructible_sprite_component* site){
            return site->_opacity;
        },
        [](destructible_sprite_component* site, const float value){
            site->opacity(value);
        }
    });
    boolean_property<destructible_sprite_component>(u"visible", u"Visible", true, {
        [](const destructible_sprite_component* site){
            return site->_visible;
        },
        [](destructible_sprite_component* site, const float value){
            site->visible(value);
        }
    });
    boolean_property<destructible_sprite_component>(u"aspect_correction", u"Aspect", true, {
        [](const destructible_sprite_component* site){
            return site->_aspect_correction;
        },
        [](destructible_sprite_component* site, const float value){
            site->_aspect_correction = value;
        }
    });
    color_property<destructible_sprite_component>(u"tint", u"Tint", true, true, {
        [](const destructible_sprite_component* site){
            return site->_tint;
        },
        [](destructible_sprite_component* site, const color& value){
            site->tint(value);
        }
    });
    ranged_property<destructible_sprite_component>(u"blend", u"Blend", true, true, 5, {
        [](const destructible_sprite_component* site){
            return site->_blend;
        },
        [](destructible_sprite_component* site, const float value){
            site->blend(value);
        }
    });
    vec2_property<destructible_sprite_component>(u"matrix", u"Matrix", true, {
        [](const destructible_sprite_component* site){
            return site->_matrix;
        },
        [](destructible_sprite_component* site, const vec2& value){
            site->matrix(value);
        }
    });
    
    begin_private_properties();
    buffer_property<destructible_sprite_component>(u"mesh", u"Mesh", {
        [](const destructible_sprite_component* site){
            return mesh::to_buffer(site->_m);
        },
        [](destructible_sprite_component* site, buffer value){
            site->_m = mesh::from_buffer(value);
        }
    });
    
    buffer_property<destructible_sprite_component>(u"mesh_copy", u"Mesh Copy", {
        [](const destructible_sprite_component* site){
            return mesh::to_buffer(site->_m_copy);
        },
        [](destructible_sprite_component* site, buffer value){
            site->_m_copy = mesh::from_buffer(value);
        }
    });
    buffer_property<destructible_sprite_component>(u"before_transformation", u"Before Transformation", {
        [](const destructible_sprite_component* site){
            return transform_space::to_buffer(site->_part_befores);
        },
        [](destructible_sprite_component* site, const buffer value){
            site->_part_befores = transform_space::from_buffer(value);
        }
    });
    buffer_property<destructible_sprite_component>(u"part_transformation", u"Part Transformation", {
        [](const destructible_sprite_component* site){
            return transform_space::to_buffer(site->_part_transforms);
        },
        [](destructible_sprite_component* site, const buffer value){
            site->_part_transforms = transform_space::from_buffer(value);
        }
    });
    boolean_property<destructible_sprite_component>(u"collapsed", u"Collapsed", true, {
        [](const destructible_sprite_component* site){
            return site->_collapsed;
        },
        [](destructible_sprite_component* site, bool value){
            site->_collapsed = value;
        }
    });
    end_private_properties();
    
    end_type();
}

rb_string destructible_sprite_component::type_name() const{
    return u"rb::destructible_sprite_component";
}

rb_string destructible_sprite_component::displayable_type_name() const {
    return u"Destructible Sprite";
}

const rb_string& destructible_sprite_component::image_name() const {
    return _image;
}

const rb_string& destructible_sprite_component::image_name(const rb_string &value){
    this->_image = value;
    this->_reapply_mapping = true;
    if(this->_m.size() == 0 && this->active())
        this->invalidate_buffers();
    _tex_size = nullptr;
    _last_atlas = nullptr;
    return _image;
}

vec2 destructible_sprite_component::size_of_tex() const {
    if(_tex_size.has_value() && parent_scene()->atlas() == _last_atlas)
        return _tex_size.value();
    
    if(parent_scene() && parent_scene()->atlas()){
        if(parent_scene()->atlas()->contains_texture(_image))
        {
            std::vector<rb_string> _groups;
            parent_scene()->atlas()->get_groups(_image, _groups);
            const_cast<destructible_sprite_component*>(this)->_tex_size = parent_scene()->atlas()->get_bounds_in_pixels(_groups[0], _image).size();
            const_cast<destructible_sprite_component*>(this)->_last_atlas = const_cast<texture_atlas*>(parent_scene()->atlas());
            return _tex_size.value();
        }
        else
            return vec2::zero;
    }
    else
        return vec2::zero;
}

const vec2& destructible_sprite_component::matrix() const {
    return _matrix;
}

const vec2& destructible_sprite_component::matrix(const rb::vec2 &value){
    if(_matrix == value)
        return _matrix;
    _matrix = value;
    
    for (auto _it_m : _m)
        delete _it_m;
    _m.clear();
    for (auto _it_m : _m_copy)
        delete _it_m;
    _m_copy.clear();
    _part_befores.clear();
    _part_transforms.clear();
    
    for (size_t i = 0; i < value.x() * value.y(); i++) {
        _part_transforms.push_back(transform_space());
        _part_befores.push_back(transform_space());
    }
    
    if(active())
        invalidate_buffers();
    
    return _matrix;
}

const transform_space& destructible_sprite_component::transform(const uint32_t x, const uint32_t y) const {
    auto _new_matrix = _matrix;
    _new_matrix.x(roundf(_new_matrix.x()));
    _new_matrix.y(roundf(_new_matrix.y()));
    if(_new_matrix.x() < 1)
        _new_matrix.x(1);
    if(_new_matrix.y() < 1)
        _new_matrix.y(1);
    
    auto _m_index = x * ((uint32_t)_new_matrix.y()) + y;
    assert((_m_index < _part_transforms.size()));
    return _part_transforms[_m_index];
}

const transform_space& destructible_sprite_component::transform(const uint32_t x, const uint32_t y, const rb::transform_space &value){
    auto _new_matrix = _matrix;
    _new_matrix.x(roundf(_new_matrix.x()));
    _new_matrix.y(roundf(_new_matrix.y()));
    if(_new_matrix.x() < 1)
        _new_matrix.x(1);
    if(_new_matrix.y() < 1)
        _new_matrix.y(1);
    
    auto _m_index = x * ((uint32_t)_new_matrix.y()) + y;
    assert((_m_index < _part_transforms.size()));
    if(_part_transforms[_m_index] == value)
        return _part_transforms[_m_index];
    _part_transforms[_m_index] = value;
    return _part_transforms[_m_index];
}

























