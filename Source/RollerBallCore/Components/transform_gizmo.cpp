//
//  transform_gizmo.cpp
//  RollerBallCore
//
//  Created by Danilo Santos de Carvalho on 29/05/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#include "transform_gizmo.h"
#include "layer.h"
#include "director.h"
#include "scene.h"
#include "mesh.h"
#include "polygon.h"
#include "null_texture_map.h"

using namespace rb;

transform_gizmo_space::transform_gizmo_space(){
    _saved_current = nullptr;
    _call_end_function = false;
}

transform_gizmo_space::~transform_gizmo_space(){
    
}

void transform_gizmo_space::describe_type() {
    start_type<transform_gizmo_space>([]() { return nullptr; });
    end_type();
}

rb_string transform_gizmo_space::type_name() const {
    return u"rb::transform_gizmo_space";
}

rb_string transform_gizmo_space::displayable_type_name() const {
    return u"Transform Gizmo Space";
}

void transform_gizmo_space::resign_current() {
    auto _ss = _saved_current;
    auto _scene = parent_scene();
    auto _s_end_function = _end_function;
    auto _s_call_end_function = _call_end_function;
    
    if(parent_node())
        parent_node()->remove_node(this, true);
    else
        parent_layer()->remove_node(this, true);
    
    if(_ss)
        _scene->current(_ss);
    
    if(_s_call_end_function)
        _s_end_function();
}

transform_gizmo::transform_gizmo(const rectangle& bounds, const bool live, std::function<void (transform_gizmo*, const transform_space&)> update_function){
    _bounds = bounds;
    _live = live;
    _update_function = update_function;
    this->name(u"Transform's Gizmo");
    this->_gizmo_mesh = nullptr;
    _activated = false;
    _in_live = false;
    _resetting = false;
}

transform_gizmo::~transform_gizmo(){
    if(this->_gizmo_mesh)
        delete this->_gizmo_mesh;
}

void transform_gizmo::start_transformation(rb::node_container *parent, const rb::transform_space &transform, const rb::rectangle &bounds, const bool live, std::function<void (transform_gizmo*, const transform_space &)> update_function){
    auto _tg = new transform_gizmo(bounds, live, update_function);
    auto _tgs = new transform_gizmo_space();
    _tgs->name(u"Gizmo Space");
    _tg->old_transform(transform);
    _tgs->_saved_current = _tg->_saved_current = director::active_scene()->current();
    _tgs->add_node(_tg);
    _tgs->_call_end_function = false;
    parent->add_node(_tgs);
    director::active_scene()->current(_tgs);
    _tg->add_to_selection();
    _tg->_activated = true;
}

void transform_gizmo::start_transformation(rb::node_container *parent, const rb::transform_space &transform, const rb::rectangle &bounds, const bool live, std::function<void (transform_gizmo*, const transform_space &)> update_function, std::function<void (void)> end_function){
    auto _tg = new transform_gizmo(bounds, live, update_function);
    auto _tgs = new transform_gizmo_space();
    _tgs->name(u"Gizmo Space");
    _tg->old_transform(transform);
    _tgs->_saved_current = _tg->_saved_current = director::active_scene()->current();
    _tgs->add_node(_tg);
    _tgs->_call_end_function = true;
    _tgs->_end_function = end_function;
    parent->add_node(_tgs);
    director::active_scene()->current(_tgs);
    _tg->add_to_selection();
    _tg->_activated = true;
}

void transform_gizmo::describe_type(){
    node::describe_type();
    start_type<transform_gizmo>([]() { return nullptr; });
    
    end_type();
}

rb_string transform_gizmo::type_name() const {
    return u"rb::transform_gizmo";
}

rb_string transform_gizmo::displayable_type_name() const {
    return u"Transform Gizmo";
}

void transform_gizmo::in_editor_update(float dt){
    if(!is_selected() && _activated){
        director::active_scene()->current(_saved_current);
    }
}

void transform_gizmo::after_becoming_active(bool node_was_moved){
    register_for(registrable_event::in_editor_update, 0);
    register_for(registrable_event::live_edit, 0);
    register_for(registrable_event::transform_changed, 0);
    enabled(node_capability::gizmo_rendering, true);
    enabled(node_capability::selection_rectangle, false);
}

void transform_gizmo::transform_changed(){
    if(_resetting)
        return;
    
    if(_live)
        _update_function(this, this->old_transform());
    else {
        if(!_in_live)
            _update_function(this, this->old_transform());
    }
}

void transform_gizmo::begin_live_edit(rb::live_edit kind){
    _in_live = true;
}

void transform_gizmo::end_live_edit(){
    _update_function(this, this->old_transform());
    _in_live = false;
}

void transform_gizmo::render_gizmo(){
    if(!_gizmo_mesh)
        _gizmo_mesh = new mesh();
    auto _b = _bounds;
    polygon _p;
    _b.to_polygon(_p);
    from_node_space_to(space::screen).transform_polygon(_p);
    if(!_p.is_simple())
        return;
    _p.optimize();
    if(_p.point_count() < 2)
        return;
    if(!_p.area().has_value() || almost_equal(_p.area().value(), 0))
        return;
    if(_p.point_count() == 2)
        _p.open_polygon();
    null_texture_map _n_map;
    _p.to_outline_mesh(*_gizmo_mesh, _n_map, SCENE_SELECTION_SIZE, corner_type::miter, true);
    _gizmo_mesh->set_color(parent_scene()->alternate_gizmo_color());
    _gizmo_mesh->set_blend(0);
    add_gizmo(_gizmo_mesh, no_texture, false);
}

rectangle transform_gizmo::bounds() const {
    return _bounds;
}




















