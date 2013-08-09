//
//  polygon_component.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 08/08/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#include "polygon_component.h"
#include "mesh.h"
#include "texture_map.h"
#include "transform_space.h"
#include "color.h"
#include "vertex.h"
#include "smooth_curve.h"
#include "circle.h"

using namespace rb;

polygon_component::polygon_component(){
    _flags.type = kPolQuad;
    _flags.dirty_polygon = true;
    _flags.in_live_edit = false;
    _flags.dirty_map = true;
    _flags.dirty_opacity = true;
    _flags.dirty_tint = true;
    _flags.dirty_blend = true;
    _flags.dirty_transformable = true;
    _flags.transformable = false;
    _flags.polygon_transformable = false;
    _flags.in_texture_transformation = false;
    _flags.visible = true;
    _flags.collapsed = true;
    _flags.smooth = false;
    _flags.open = false;
    _flags.dirty_border_polygon = true;
    _flags.dirty_border_color = true;
    _flags.renderable = true;
    _m = nullptr;
    _m_copy = nullptr;
    _b = nullptr;
    
    _map = nullptr;
    _before = transform_space();
    _image = u"";
    _circle_sides = 32;
    _opacity = 1;
    _tint = color::from_rgba(1, 1, 1, 1);
    _blend = 1;
    _smooth_quality = 0.1f;
    _smooth_divisions = 5;
    _tx_space = transform_space();
    _border_size = 0;
    _border_corner_type = corner_type::miter;
    _border_color = nullptr;
    _border_texture = u"";
    _max_s = 1;
}

void polygon_component::destroy_polygon(bool destroy_map){
    if(_m)
        delete _m;
    _m = nullptr;
    if(_m_copy)
        delete _m_copy;
    _m_copy = nullptr;
    if(_b)
        delete _b;
    _b = nullptr;
    if(destroy_map){
        if(_map)
            delete _map;
        _map = nullptr;
    }
}

polygon_component::~polygon_component(){
    destroy_polygon(true);
}

void polygon_component::recreate_polygon(){
    if(_m)
        delete _m;
    _m = nullptr;
    if(_m_copy)
        delete _m_copy;
    _m_copy = nullptr;
    
    if(_flags.dirty_map){
        auto _temp = const_cast<texture_map*>(create_mapping(_image, _tx_space, _map));
        if(!_temp){ //delete refurbished...
            if(_map)
                delete _map;
            _map = nullptr;
        }
        else
            _map = _temp;
    }
    
    if(!_map || _flags.in_live_edit || !_flags.renderable)
        return;
    
    polygon _f_pol = _polygon;
    
    if (_flags.type == kPolQuad){
        rectangle _rc = rectangle(0, 0, 1, 1);
        _rc.to_polygon(_polygon);
        
        if(_flags.open)
            _polygon.open_polygon();
        else
            _polygon.close_polygon();
    }
    else if(_flags.type == kPolCircle){
        circle _c = circle(vec2::zero, 0.5);
        _c.to_polygon(_polygon, _circle_sides);
        
        if(_flags.open)
            _polygon.open_polygon();
        else
            _polygon.close_polygon();
    }
    else { //Freeform
        _polygon.reset();
        for (auto _n : *this){
            _polygon.add_point_after(_n->transform().origin(), _polygon.point_count() - 1);
        }
        
        if(_flags.open)
            _polygon.open_polygon();
        else
            _polygon.close_polygon();
    }
    
    if(_flags.type != kPolFreeform){
        std::vector<node*> _nodes;
        this->copy_nodes_to_vector(_nodes);
        for (auto _n : _nodes)
            this->remove_node(_n, true);
        for (uint32_t i = 0; i < _polygon.point_count(); i++) {
            auto _pt = new polygon_point_component();
            _pt->transform(transform_space(_polygon.get_point(i)));
            this->add_node(_pt);
        }
    }
    
    if(_flags.smooth){
        smooth_curve _sc;
        std::vector<vec2> _points;
        for (uint32_t i = 0; i < _polygon.point_count(); i++)
            _points.push_back(_polygon.get_point(i));
        
        if(_flags.open)
            _sc = smooth_curve::build_open_curve(_points, _sc);
        else
            _sc = smooth_curve::build_closed_curve(_points, _sc);
        
        _f_pol = _sc.to_polygon(_f_pol, _smooth_quality, _smooth_divisions);
    }
    else
        _f_pol = _polygon;
    
    _m = new mesh();
    _m_copy = new mesh();
    _f_pol.optimize();
    if(!_f_pol.is_empty() && _f_pol.point_count() >= 3 && _f_pol.is_simple().has_value() && _f_pol.is_simple().value() && _f_pol.area().has_value() && _f_pol.area().value())
        _f_pol.to_mesh(*_m, 0, *_map);
    *_m_copy = *_m;
    
    if(!this->_m->is_empty())
        set_texture_for_mesh(_m, _image, false);
}

void polygon_component::recreate_border(){
    
}

void polygon_component::update_polygon(){
    if(_flags.dirty_polygon){ //we should recreate the polygon
        //we recreate the polygon, maps, everything...
        //we set _flags.polygon_transformable
        recreate_polygon();
        _before = transform_space();
        _flags.polygon_transformable = false;
        
        //the following becomes dirty
        _flags.dirty_polygon = false;
        _flags.dirty_map = false; //no need to remap
        _flags.dirty_opacity = true;
        _flags.dirty_tint = true;
        _flags.dirty_blend = true;
        _flags.dirty_transformable = true;
        _flags.collapsed = false;
    }
    
    if(_flags.dirty_border_polygon){ //we should recreate the border polygon...
        recreate_border();
        
        //the following becomes dirty
        _flags.dirty_opacity = true;
        _flags.dirty_border_color = true;
    }
    
    update_collapsed_flag();
    
    if(_flags.dirty_map){ //we recreate map
        auto _temp = const_cast<texture_map*>(create_mapping(_image, _tx_space, _map));
        if(!_temp){ //delete refurbished...
            if(_map)
                delete _map;
            _map = nullptr;
        }
        else {
            _map = _temp;
            
            if(!_flags.collapsed){
                if(_m){
                    _m->remap(_map->bounds(), texture_mapping_type::untransformable);
                    set_texture_for_mesh(_m, _image, false);
                }
                if(_m_copy){
                    _m_copy->remap(_map->bounds(), texture_mapping_type::untransformable);
                }
            }
        }
    }
    _flags.dirty_map = false;
    
    if(_flags.dirty_opacity){
        if(_m)
            _m->set_alpha(_opacity);
        if(_m_copy)
            _m_copy->set_alpha(_opacity);
        if(_b)
            _b->set_alpha(_opacity);
    }
    _flags.dirty_opacity = false;
    
    if(_flags.dirty_tint){
        if(_m)
            _m->set_color(_tint);
        if(_m_copy)
            _m_copy->set_color(_tint);
    }
    _flags.dirty_tint = false;
    
    if(_flags.dirty_blend)
    {
        if(_m)
            _m->set_blend(_blend);
        if(_m_copy)
            _m_copy->set_blend(_blend);
    }
    _flags.dirty_blend = false;
    
    if(_flags.dirty_transformable) {
        auto _t = _flags.transformable || _flags.in_texture_transformation;
        if(_t != _flags.polygon_transformable){
            assert(_t); //must be transformable because if it's not... there's nothing we can do
            if(_m && _map)
                _m->expand_from_atlas_bounds(*_map, false);
            if(_m_copy && _map)
                _m->expand_from_atlas_bounds(*_map, false);
            _flags.polygon_transformable = true;
        }
    }
    _flags.dirty_transformable = false;
    
    if(_flags.dirty_border_color){
        if(_b)
        {
            if(_border_color.has_value()){
                _b->set_color(_border_color.value());
                _b->set_blend(0);
            }
            else
                _b->set_blend(1);
        }
    }
    _flags.dirty_border_color = false;
    
    if(!_flags.collapsed)
        transform_mesh();
}

void polygon_component::update_collapsed_flag(){
    auto _previous = _flags.collapsed;
    if(!_map)
        _map = (texture_map*)create_mapping(_image, _tx_space);
    
    if((!_flags.visible && (!in_editor() || is_playing())) || !_map)
        _flags.collapsed = true;
    else
        _flags.collapsed = false;
    
    if(_previous != _flags.collapsed)
        update_collapsed_mesh();
}

void polygon_component::update_collapsed_mesh(){
    if(!_m)
        return;
    
    if(_flags.collapsed){
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

void polygon_component::transform_mesh(){
    if(!_m)
        return;
    
    transform_space _to_layer = from_node_space_to(space::layer);
    if(_before != _to_layer){
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































