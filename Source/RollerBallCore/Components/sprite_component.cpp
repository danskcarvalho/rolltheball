//
//  sprite_component.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 21/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "sprite_component.h"
#include "rectangle.h"
#include "mesh.h"
#include "texture_map.h"
#include "transform_space.h"
#include "vertex.h"
#include "scene.h"
#include "texture_atlas.h"

using namespace rb;

sprite_component::sprite_component(){
    _m = nullptr;
    _map = nullptr;
    _m_copy = nullptr;
    _image = u"";
    _reapply_mapping = false;
    _opacity = 1;
    _tint = color::from_rgba(1, 1, 1, 1);
    _blend = 1;
    _collapsed = true;
    _visible = true;
    _aspect_correction = true;
}

void sprite_component::destroy(){
    if(_m)
        delete _m;
    _m = nullptr;
    if(_m_copy)
        delete _m_copy;
    _m_copy = nullptr;
    if(_map)
        delete _map;
    _map = nullptr;
}

void sprite_component::create(){
    if(!_map){
        _map = (texture_map*)create_mapping(_image, transform_space());
    }
    
    if(!_m){
        _m = new mesh();
        rectangle _rc(0, 0, 1, 1);
        if(_map)
            _rc.to_mesh(*_m, _map->bounds());
        else
            _rc.to_mesh(*_m, rectangle(0.5, 0.5, 1, 1)); //no texture coordinates
        _m->set_alpha(_opacity);
        _m->set_color(_tint);
        _m->set_blend(_blend);
        _m_copy = new mesh();
        *_m_copy = *_m;
        _collapsed = false;
        _before = transform_space();
    }
}

sprite_component::~sprite_component(){
    destroy();
}

void sprite_component::update_collapsed_mesh(){
    if(!_m)
        return;
    
    if(_collapsed){
        _m->lock_vertex_buffer([=](vertex* buffer){
            for (auto i = 0; i < _m->vertex_count(); i++) {
                buffer[i].set_position(vec2::zero);
            }
        });
    }
    else{
        *_m = *_m_copy;
        _before = transform_space();
    }
}

void sprite_component::update_collapsed_flag(){
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

void sprite_component::after_becoming_active(bool node_was_moved){
    enabled(node_capability::children_rendering, false);
    enabled(node_capability::rendering, true);
    enabled(node_capability::can_become_current, false);
//    if(!node_was_moved)
//        destroy();
}
void sprite_component::before_becoming_inactive(bool node_was_moved){
//    if(!node_was_moved)
//        destroy();
}
void sprite_component::reapply_mapping(){
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
        if(_m){
            _m->remap(_map->bounds(), texture_mapping_type::untransformable);
            set_texture_for_mesh(_m, _image, false);
        }
        if(_m_copy){
            _m_copy->remap(_map->bounds(), texture_mapping_type::untransformable);
        }
    }
}

bool sprite_component::aspect_correction() const{
    return _aspect_correction;
}

bool sprite_component::aspect_correction(const bool value){
    _aspect_correction = value;
    return _aspect_correction;
}

transform_space sprite_component::aspect_correction_factor() const {
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

void sprite_component::transform_mesh(const bool refill_buffers){
    transform_space _to_layer = from_node_space_to(space::layer) * aspect_correction_factor();
    if(_before != _to_layer || refill_buffers){
        if(_before.both_directions()){
            transform_space _final_transform = _to_layer * _before.inverse();
            _before = _to_layer;
            _final_transform.from_space_to_base().transform_mesh(*_m);
        }
        else {
            *_m = *_m_copy;
            _before = _to_layer;
            _to_layer.from_space_to_base().transform_mesh(*_m);
        }
    }
}

void sprite_component::render(const bool refill_buffers){
    if(!_m || refill_buffers || _reapply_mapping){
        if(!_m){
            destroy();
            create();
            _before = transform_space();
            _reapply_mapping = false;
        }
        else if(_reapply_mapping){
            reapply_mapping();
            _reapply_mapping = false;
        }
    }
    
    if(_m){
        update_collapsed_flag();
        if(!_collapsed)
            transform_mesh(refill_buffers);
        
        if(refill_buffers){
            add_mesh_for_rendering(_m, _image, false);
        }
    }
}
rectangle sprite_component::bounds() const{
    auto _rc = rectangle(0, 0, 1, 1);
    auto _ac = aspect_correction_factor();
    _rc.size(_rc.size() * _ac.scale());
    return _rc;
}

float sprite_component::opacity() const {
    return _opacity;
}

float sprite_component::opacity(float value){
    this->_opacity = value;
    if(this->_m)
        this->_m->set_alpha(this->_opacity);
    if(this->_m_copy)
        this->_m_copy->set_alpha(this->_opacity);
    return _opacity;
}

const color& sprite_component::tint() const {
    return _tint;
}

const color& sprite_component::tint(const rb::color &value){
    this->_tint = value;
    if(this->_m)
        this->_m->set_color(this->_tint);
    if(this->_m_copy)
        this->_m_copy->set_color(this->_tint);
    return _tint;
}

float sprite_component::blend() const {
    return _blend;
}

float sprite_component::blend(float value){
    this->_blend = value;
    if(this->_m)
        this->_m->set_blend(this->_blend);
    if(this->_m_copy)
        this->_m_copy->set_blend(this->_blend);
    return _blend;
}


bool sprite_component::visible() const {
    return _visible;
}

bool sprite_component::visible(bool value){
    this->_visible = value;
    return _visible;
}

void sprite_component::describe_type(){
    node::describe_type();
    start_type<sprite_component>([](){ return new sprite_component(); });
    image_property<sprite_component>(u"image", u"Image", true, {
        [](const sprite_component* site){
            return site->_image;
        },
        [](sprite_component* site, const rb_string& value){
            site->image_name(value);
        }
    });
    ranged_property<sprite_component>(u"opacity", u"Opacity", true, true, 5, {
        [](const sprite_component* site){
            return site->_opacity;
        },
        [](sprite_component* site, const float value){
            site->opacity(value);
        }
    });
    boolean_property<sprite_component>(u"visible", u"Visible", true, {
        [](const sprite_component* site){
            return site->_visible;
        },
        [](sprite_component* site, const float value){
            site->visible(value);
        }
    });
    boolean_property<sprite_component>(u"aspect_correction", u"Aspect", true, {
        [](const sprite_component* site){
            return site->_aspect_correction;
        },
        [](sprite_component* site, const float value){
            site->_aspect_correction = value;
        }
    });
    color_property<sprite_component>(u"tint", u"Tint", true, true, {
        [](const sprite_component* site){
            return site->_tint;
        },
        [](sprite_component* site, const color& value){
            site->tint(value);
        }
    });
    ranged_property<sprite_component>(u"blend", u"Blend", true, true, 5, {
        [](const sprite_component* site){
            return site->_blend;
        },
        [](sprite_component* site, const float value){
            site->blend(value);
        }
    });
    
    begin_private_properties();
    nullable_buffer_property<sprite_component>(u"mesh", u"Mesh", {
        [](const sprite_component* site){
            return site->_m ? site->_m->to_buffer() : (nullable<buffer>)nullptr;
        },
        [](sprite_component* site, const nullable<buffer> value){
            site->_m = value.has_value() ? new mesh(value.value()) : nullptr;
        }
    });
    
    nullable_buffer_property<sprite_component>(u"mesh_copy", u"Mesh Copy", {
        [](const sprite_component* site){
            return site->_m_copy ? site->_m_copy->to_buffer() : (nullable<buffer>)nullptr;
        },
        [](sprite_component* site, const nullable<buffer> value){
            site->_m_copy = value.has_value() ? new mesh(value.value()) : nullptr;
        }
    });
    buffer_property<sprite_component>(u"before_transformation", u"Before Transformation", {
        [](const sprite_component* site){
            return site->_before.to_buffer();
        },
        [](sprite_component* site, const buffer value){
            site->_before = transform_space(value);
        }
    });
    boolean_property<sprite_component>(u"collapsed", u"Collapsed", true, {
        [](const sprite_component* site){
            return site->_collapsed;
        },
        [](sprite_component* site, bool value){
            site->_collapsed = value;
        }
    });
    boolean_property<sprite_component>(u"reapply_mapping", u"Reapply Mapping", true, {
        [](const sprite_component* site){
            return site->_reapply_mapping;
        },
        [](sprite_component* site, bool value){
            site->_reapply_mapping = value;
        }
    });
    end_private_properties();
    
    end_type();
}

rb_string sprite_component::type_name() const{
    return u"rb::sprite_component";
}

rb_string sprite_component::displayable_type_name() const {
    return u"Sprite";
}

const rb_string& sprite_component::image_name() const {
    return _image;
}

const rb_string& sprite_component::image_name(const rb_string &value){
    this->_image = value;
    this->_reapply_mapping = true;
    if(!this->_m && this->active())
        this->invalidate_buffers();
    return _image;
}

vec2 sprite_component::size_of_tex() const {
    if(parent_scene() && parent_scene()->atlas()){
        if(parent_scene()->atlas()->contains_texture(_image))
        {
            std::vector<rb_string> _groups;
            parent_scene()->atlas()->get_groups(_image, _groups);
            return parent_scene()->atlas()->get_bounds_in_pixels(_groups[0], _image).size();
        }
        else
            return vec2::zero;
    }
    else
        return vec2::zero;
}
























