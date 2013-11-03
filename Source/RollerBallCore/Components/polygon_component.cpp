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
#include "null_texture_map.h"
#include "scene.h"
#include "transform_gizmo.h"

using namespace rb;

polygon_point_component::polygon_point_component(){
    _in_live_edit = false;
}

void polygon_point_component::after_becoming_active(bool node_was_moved){
    register_for(registrable_event::live_edit, 0);
    register_for(registrable_event::transform_changed, 0);
    enabled(node_capability::rendering, false);
    enabled(node_capability::children_rendering, false);
    enabled(node_capability::can_become_current, false);
    enabled(node_capability::selection_rectangle, false);
    enabled(node_capability::gizmo_rendering, true);
}

void polygon_point_component::render_gizmo(){
    //Do nothing
}

void polygon_point_component::make_itself_first(){
    std::vector<node*> _nodes;
    parent()->copy_nodes_to_vector(_nodes);
    auto _index = parent()->search_node(this);
    assert(_index.has_value());
    if(_index.value() == 0)
        return;
    
    auto _i = 0;
    while (_i != _index.value()){
        parent()->send_to_back(_nodes[_i]);
        _i++;
    }
}

void polygon_point_component::split(){
    auto _i = 0;
    
    while (_i < parent()->node_count()) {
        nullable<uint32_t> _insertion_pt = nullptr;
        vec2 _pt0, _pt1;
        if(_i != (parent()->node_count() - 1)){
            if(parent()->node_at(_i)->is_selected() && parent()->node_at(_i + 1)->is_selected()){
                _insertion_pt = _i + 1;
                _pt0 = parent()->node_at(_i)->transform().origin();
                _pt1 = parent()->node_at(_i + 1)->transform().origin();
            }
        }
        else {
            if(parent()->node_at(_i)->is_selected() && parent()->node_at(0)->is_selected()){
                _insertion_pt = (uint32_t)0;
                _pt0 = parent()->node_at(_i)->transform().origin();
                _pt1 = parent()->node_at(0)->transform().origin();
            }
        }
        
        if(_insertion_pt.has_value()){
            auto _new_pt = new polygon_point_component();
            _new_pt->transform(_new_pt->transform().moved((_pt0 + _pt1) / 2.0f));
            parent()->add_node_at(_new_pt, _insertion_pt.value());
            _i++;
        }
        else
            _i++;
    }
}

void polygon_point_component::transform_changed(){
    if(!_in_live_edit){
        auto _p = dynamic_cast<polygon_component*>(parent());
        if(_p){
            _p->_flags.type = polygon_component::kPolFreeform;
            _p->_flags.dirty_polygon = true;
            _p->_flags.dirty_border_polygon = true;
            if(active())
                invalidate_buffers();
        }
    }
    else {
        auto _p = dynamic_cast<polygon_component*>(parent());
        if(_p){
            auto _index = _p->search_node(this);
            assert(_index.has_value());
            _p->_polygon.set_point(this->node::transform().origin(), _index.value());
        }
    }
}

void polygon_point_component::begin_live_edit(rb::live_edit kind){
    _in_live_edit = true;
    auto _p = dynamic_cast<polygon_component*>(parent());
    if(_p){
        _p->_flags.in_live_edit = true;
        _p->_flags.type = polygon_component::kPolFreeform;
        _p->_flags.dirty_polygon = true;
        _p->_flags.dirty_border_polygon = true;
        if(active())
            invalidate_buffers();
    }
}

void polygon_point_component::end_live_edit(){
    _in_live_edit = false;
    auto _p = dynamic_cast<polygon_component*>(parent());
    if(_p){
        _p->_flags.in_live_edit = false;
        _p->_flags.dirty_polygon = true;
        _p->_flags.dirty_border_polygon = true;
        if(active())
            invalidate_buffers();
    }
}

rectangle polygon_point_component::bounds() const {
    rectangle _rc(0, 0, POINT_SIZE, POINT_SIZE);
    auto _t = from_node_space_to(space::screen);
    if(_t.test_direction(transform_direction::from_base_to_space)){
        auto _center = _t.from_space_to_base().transformed_point(vec2::zero);
        _rc.center(_center);
        return _t.from_base_to_space().transformed_rectangle(_rc);
    }
    else
        return node::bounds();
}

void polygon_point_component::describe_type() {
    node::describe_type();
    
    start_type<polygon_point_component>([](){ return new polygon_point_component(); });
    action<polygon_point_component>(u"make_itself_first", u"", action_flags::single_dispatch, {u"Make Itself First"}, [](polygon_point_component* site, const rb_string& action_name){
        site->make_itself_first();
    });
    action<polygon_point_component>(u"split", u"", action_flags::single_dispatch, {u"Split"}, [](polygon_point_component* site, const rb_string& action_name){
        site->split();
    });
    end_type();
}

rb_string polygon_point_component::type_name() const {
    return u"rb::polygon_point_component";
}

rb_string polygon_point_component::displayable_type_name() const {
    return u"Polygon Point";
}

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
    _flags.border_collapsed = true;
    _flags.smooth = false;
    _flags.open = false;
    _flags.dirty_border_polygon = true;
    _flags.dirty_border_color = true;
    _flags.renderable = true;
    _flags.dirty_border_map = false;
    _flags._new = true;
    _m = nullptr;
    _m_copy = nullptr;
    _b = nullptr;
    _b_copy = nullptr;
    _skeleton = nullptr;
    
    _map = nullptr;
    _before = transform_space();
    _before_b = transform_space();
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
    
    reset_children(kPolQuad);
}

void polygon_component::destroy_polygon(){
    if(_m)
        delete _m;
    _m = nullptr;
    if(_m_copy)
        delete _m_copy;
    _m_copy = nullptr;
    if(_b)
        delete _b;
    _b = nullptr;
    if(_b_copy)
        delete _b_copy;
    _b_copy = nullptr;
    if(_map)
        delete _map;
    _map = nullptr;
    if(_skeleton)
        delete _skeleton;
    _skeleton = nullptr;
}

polygon_component::~polygon_component(){
    destroy_polygon();
}

void polygon_component::create_polygon_data(){
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
            _polygon.add_point_after(_n->transform().origin(), _polygon.point_count() == 0 ? 0 : _polygon.point_count() - 1);
        }
        
        if(_flags.open)
            _polygon.open_polygon();
        else
            _polygon.close_polygon();
    }
}

void polygon_component::reset_children(polygon_component::PolType pt){
    if(_flags.type != kPolFreeform){
        std::vector<node*> _nodes;
        this->copy_nodes_to_vector(_nodes);
        for (auto _n : _nodes)
            this->remove_node(_n, true);
        _flags.type = pt;
        auto _p = to_polygon();
        for (uint32_t i = 0; i < _p.point_count(); i++) {
            auto _pt = new polygon_point_component();
            _pt->transform(transform_space(_p.get_point(i)));
            this->add_node(_pt);
        }
        _flags.type = pt;
    }
}

void polygon_component::recreate_polygon(){
    if(_m)
        delete _m;
    _m = nullptr;
    if(_m_copy)
        delete _m_copy;
    _m_copy = nullptr;
    _polygon.reset();
    create_polygon_data();
    
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
    
    if(_flags.in_live_edit || !_flags.renderable)
        return;
    
    polygon _f_pol;
    
    if(_flags.smooth && _polygon.point_count() >= 4){
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
    if(!_f_pol.is_empty() && _f_pol.point_count() >= 3 && _f_pol.is_simple().has_value() && _f_pol.is_simple().value() && _f_pol.area().has_value() && _f_pol.area().value()){
        if(_map)
            _f_pol.to_mesh(*_m, 0, *_map);
        else {
            null_texture_map _nm;
            _f_pol.to_mesh(*_m, 0, _nm);
        }
    }
    *_m_copy = *_m;
    
    if(!_map){
        if(_m)
            _m->set_blend(0);
        if(_m_copy)
            _m_copy->set_blend(0);
    }
    
    if(_m && !this->_m->is_empty()){
        add_mesh_for_rendering(_m, _map ? _image : no_texture, _flags.transformable || _flags.in_texture_transformation);
    }
}

void polygon_component::recreate_border(){
    if (_b)
        delete _b;
    _b = nullptr;
    if (_b_copy)
        delete _b_copy;
    _b_copy = nullptr;
    _polygon.reset();
    create_polygon_data();
    
    if(_border_size <= 0 || _flags.in_live_edit || !_flags.renderable)
        return;
    
    polygon _f_pol;
    if(_flags.smooth && _polygon.point_count() >= 4){
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
    
    _f_pol.optimize();
    if(!_f_pol.is_empty() && _f_pol.point_count() >= 3 && _f_pol.is_simple().has_value() && _f_pol.is_simple().value() && _f_pol.area().has_value() && _f_pol.area().value()){
        if(_border_color.has_value()){
            _b = new mesh();
            auto _nm = null_texture_map();
            _f_pol.to_outline_mesh(*_b, _nm, _border_size, _border_corner_type, true);
        }
        else {
            auto _bm = create_mapping(_border_texture, transform_space());
            if(_bm){
                _b = new mesh();
                _f_pol.textured_outline(*_b, _bm->bounds(), _max_s, _border_size);
                delete _bm;
            }
        }
    }
    
    if(_b){
        _b_copy = new mesh();
        *_b_copy = *_b;
    }
    
    if(_b && !_b->is_empty()){
        add_mesh_for_rendering(_b, _border_color.has_value() ? no_texture : _border_texture, false);
    }
}

void polygon_component::update_polygon(bool refill_buffers){
    bool _already_added = false;
    if(_flags.dirty_border_polygon){ //we should recreate the border polygon...
        recreate_border();
        _already_added = true;
        _before_b = transform_space();
        //the following becomes dirty
        _flags.dirty_border_polygon = false;
        _flags.dirty_opacity = true;
        _flags.dirty_border_color = true;
        _flags.border_collapsed = false;
    }
    
    if (!_already_added && refill_buffers && _b && !_b->is_empty()){
        add_mesh_for_rendering(_b, _border_color.has_value() ? no_texture : _border_texture, false);
    }
    
    _already_added = false;
    
    if(_flags.dirty_polygon){ //we should recreate the polygon
        //we recreate the polygon, maps, everything...
        //we set _flags.polygon_transformable
        recreate_polygon();
        _already_added = true;
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
    
    if(!_already_added && refill_buffers && _m && !_m->is_empty())
        add_mesh_for_rendering(_m, _map ? _image : no_texture, _flags.transformable || _flags.in_texture_transformation);
    
    update_collapsed_flag();
    
    if(_flags.dirty_transformable) {
        auto _t = _flags.transformable || _flags.in_texture_transformation;
        if(_t != _flags.polygon_transformable){
            assert(_t); //must be transformable because if it's not... there's nothing we can do
            if(_m && _map){
                _m->expand_from_atlas_bounds(*_map, false);
                set_texture_for_mesh(_m, _image, _flags.transformable || _flags.in_texture_transformation);
            }
            if(_m_copy && _map){
                _m_copy->expand_from_atlas_bounds(*_map, false);
            }
            _flags.polygon_transformable = true;
        }
    }
    _flags.dirty_transformable = false;
    
    if(_flags.dirty_map){ //we recreate map
        auto _temp = const_cast<texture_map*>(create_mapping(_image, _tx_space, _map));
        if(!_temp){ //delete refurbished...
            if(_map)
                delete _map;
            _map = nullptr;
            
            if(_m)
                _m->set_blend(0);
            if(_m_copy)
                _m_copy->set_blend(0);
            
            if(_m)
                set_texture_for_mesh(_m, no_texture, false);
        }
        else {
            _map = _temp;
            _flags.dirty_blend = true;
            
            if(!_flags.collapsed){
                if(_m){
                    _m->remap(_map->bounds(), _flags.polygon_transformable ? texture_mapping_type::transformable : texture_mapping_type::untransformable);
                    set_texture_for_mesh(_m, _image, _flags.transformable || _flags.in_texture_transformation);
                    
                    if(_flags.polygon_transformable){ //apply the transform
                        _m->lock_vertex_buffer([=](vertex* vb){
                            _m_copy->lock_vertex_buffer([=](vertex* vb2){
                                for (uint32_t i = 0; i < _m->vertex_count(); i++) {
                                    auto _saved_pos = vb[i].get_position();
                                    vb[i].set_position(vb2[i].get_position());
                                    _map->set_texture_coords(vb[i]);
                                    vb[i].set_position(_saved_pos);
                                }
                            });
                        });
                    }
                }
                if(_m_copy){
                    _m_copy->remap(_map->bounds(), _flags.polygon_transformable ? texture_mapping_type::transformable : texture_mapping_type::untransformable);
                    if(_flags.polygon_transformable){ //apply the transform
                        _m_copy->lock_vertex_buffer([=](vertex* vb){
                            for (uint32_t i = 0; i < _m_copy->vertex_count(); i++) {
                                _map->set_texture_coords(vb[i]);
                            }
                        });
                    }
                }
            }
        }
    }
    _flags.dirty_map = false;
    
    if(_flags.dirty_border_map){
        auto _b_map = create_mapping(_border_texture, transform_space());
        if(_b && _b_map){
            _b->remap(_b_map->bounds(), texture_mapping_type::untransformable);
            set_texture_for_mesh(_b, _border_texture, false);
        }
        
        if(_b_copy && _b_map){
            _b_copy->remap(_b_map->bounds(), texture_mapping_type::untransformable);
        }
        
        if(_b_map)
            delete _b_map;
    }
    _flags.dirty_border_map = false;
    
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
        if(_map){
            if(_m)
                _m->set_blend(_blend);
            if(_m_copy)
                _m_copy->set_blend(_blend);
        }
        else {
            if(_m)
                _m->set_blend(0);
            if(_m_copy)
                _m_copy->set_blend(0);
        }
    }
    _flags.dirty_blend = false;
    
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
        if(_b_copy)
        {
            if(_border_color.has_value()){
                _b_copy->set_color(_border_color.value());
                _b_copy->set_blend(0);
            }
            else
                _b_copy->set_blend(1);
        }
    }
    _flags.dirty_border_color = false;
    
    transform_mesh();
}

void polygon_component::update_collapsed_flag(){
    auto _previous = _flags.collapsed;
    auto _previous_b = _flags.border_collapsed;
    if(!_map)
        _map = (texture_map*)create_mapping(_image, _tx_space);
    
    if((!_flags.visible && (!in_editor() || is_playing()))){
        _flags.collapsed = true;
        _flags.border_collapsed = true;
    }
    else {
        _flags.collapsed = false;
        _flags.border_collapsed = false;
    }
    
    if(_previous != _flags.collapsed)
        update_collapsed_mesh();
    
    if(_previous_b != _flags.border_collapsed)
        update_collapsed_border_mesh();
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

void polygon_component::update_collapsed_border_mesh(){
    if(!_b)
        return;
    
    if(_flags.border_collapsed){
        _b->lock_vertex_buffer([=](vertex* buffer){
            for (auto i = 0; i < _b->vertex_count(); i++) {
                buffer[i].set_position(vec2::zero);
            }
        });
    }
    else {
        *_b = *_b_copy;
        _before_b = transform_space();
    }
}

void polygon_component::transform_mesh(){
    if(_m && !_flags.collapsed){
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
    
    if(_b && !_flags.border_collapsed){
        transform_space _to_layer = from_node_space_to(space::layer);
        if(_before_b != _to_layer){
            if(_before_b.both_directions()){
                transform_space _final_transform = _to_layer * _before_b.inverse();
                _before_b = _to_layer;
                _final_transform.from_space_to_base().transform_mesh(*_b);
            }
            else {
                *_b = *_b_copy;
                _before_b = _to_layer;
                _to_layer.from_space_to_base().transform_mesh(*_b);
            }
        }
    }
}

mesh* polygon_component::create_skeleton(const polygon& p){
    if(p.point_count() == 0)
        return nullptr;
    mesh* _sk = new mesh();
    auto _nm = null_texture_map();
    auto _t = from_node_space_to(space::screen).from_space_to_base();
    
    for (uint32_t i = 0; i < p.edge_count(); i++) {
        auto _e1 = _t.transformed_point(p.get_edge(i).pt0());
        auto _e2 = _t.transformed_point(p.get_edge(i).pt1());
        if(_e1 == _e2)
            continue;
        polygon _p;
        polygon::build_open_polygon({_e1, _e2}, _p);
        mesh* _e = new mesh();
        _p.to_outline_mesh(*_e, _nm, NODE_SELECTION_SIZE, corner_type::miter, true);
        _e->set_blend(0);
        _e->set_color(marker() ? parent_scene()->alternate_gizmo_color() : parent_scene()->selection_color());
        
        auto _temp = mesh::merge_meshes({_sk, _e});
        delete _sk;
        delete _e;
        _sk = _temp;
    }
    
    auto _u_rc = rectangle(0.5, 0.5, 1, 1);
    if(this->parent_scene() && this->parent_scene()->current() == this){
        for (uint32_t i = 0; i < p.point_count(); i++) {
            auto _pt = _t.transformed_point(p.get_point(i));
            auto _s_factor = this->node_at(i)->is_selected() ? 2.0f : 1.0f;
            rectangle _rc = rectangle(_pt.x(), _pt.y(), POINT_SIZE * _s_factor, POINT_SIZE * _s_factor);
            mesh* _p = new mesh();
            _rc.to_mesh(*_p, _u_rc);
            _p->set_blend(0);
            _p->set_color(this->node_at(i)->is_selected() ? parent_scene()->alternate_gizmo_color() : parent_scene()->gizmo_color());
            
            auto _temp = mesh::merge_meshes({_sk, _p});
            delete _sk;
            delete _p;
            _sk = _temp;
        }
    }
    
    return _sk;
}

bool polygon_component::add_node_at(rb::node *n, uint32_t at){
    if (dynamic_cast<polygon_point_component*>(n)){
        bool _b = node::add_node_at(n, at);
        if(_b)
        {
            _flags.type = kPolFreeform;
            _polygon.reset();
            _flags.dirty_polygon = true;
            _flags.dirty_border_polygon = true;
            if(active())
                invalidate_buffers();
                
        }
        return _b;
    }
    else
        return false;
}

bool polygon_component::remove_node(rb::node *n, bool cleanup){
    bool _b = node::remove_node(n, cleanup);
    if(_b){
        _flags.type = kPolFreeform;
        _polygon.reset();
        _flags.dirty_polygon = true;
        _flags.dirty_border_polygon = true;
        if(active())
            invalidate_buffers();
    }
    return _b;
}

bool polygon_component::bring_to_front(rb::node *n){
    bool _b = node::bring_to_front(n);
    if(_b){
        _flags.type = kPolFreeform;
        _polygon.reset();
        _flags.dirty_polygon = true;
        _flags.dirty_border_polygon = true;
        if(active())
            invalidate_buffers();
    }
    return _b;
}

bool polygon_component::send_to_back(rb::node *n){
    bool _b = node::send_to_back(n);
    if(_b){
        _flags.type = kPolFreeform;
        _polygon.reset();
        _flags.dirty_polygon = true;
        _flags.dirty_border_polygon = true;
        if(active())
            invalidate_buffers();
    }
    return _b;
}

bool polygon_component::send_backward(rb::node *n){
    bool _b = node::send_backward(n);
    if(_b){
        _flags.type = kPolFreeform;
        _polygon.reset();
        _flags.dirty_polygon = true;
        _flags.dirty_border_polygon = true;
        if(active())
            invalidate_buffers();
    }
    return _b;
}

bool polygon_component::bring_forward(rb::node *n){
    bool _b = node::bring_forward(n);
    if(_b){
        _flags.type = kPolFreeform;
        _polygon.reset();
        _flags.dirty_polygon = true;
        _flags.dirty_border_polygon = true;
        if(active())
            invalidate_buffers();
    }
    return _b;
}

void polygon_component::after_becoming_active(bool node_was_moved){
    enabled(node_capability::children_rendering, false);
    enabled(node_capability::rendering, true);
    enabled(node_capability::can_become_current, true);
    enabled(node_capability::gizmo_rendering, true);
}

void polygon_component::before_becoming_inactive(bool node_was_moved){
//    if(!node_was_moved)
//        destroy_polygon();
}

void polygon_component::render(const bool refill_buffers){
    update_polygon(refill_buffers);
    _flags._new = false;
}

typed_object* polygon_component::clone() const {
    auto _p = dynamic_cast<polygon_component*>(node::clone());
    if(!_flags._new){
        _p->_flags.dirty_polygon = false;
        _p->_flags.dirty_border_polygon = false;
    }
    return _p;
}

void polygon_component::describe_type(){
    node::describe_type();
    
    start_type<polygon_component>([](){ return new polygon_component(); });
    integer_property<polygon_component>(u"circle_sides", u"Circle Sides", true, {
        [](const polygon_component* site){
            return site->_circle_sides;
        },
        [](polygon_component* site, long value){
            if(value < 4){
                site->notify_property_changed(u"circle_sides");
                return;
            }
            site->circle_sides((uint32_t)value);
        }
    });
    action<polygon_component>(u"reset", u"Reset", action_flags::multi_dispatch, {u"Quad", u"Circle"},
        [](polygon_component* site, const rb_string& action_name){
            if(action_name == u"Quad")
                site->reset_to_quad();
            else
                site->reset_to_circle();
        }
    );
    boolean_property<polygon_component>(u"visible", u"Visible", true, {
        [](const polygon_component* site){
            return site->visible();
        },
        [](polygon_component* site, bool value){
            site->visible(value);
        }
    });
    boolean_property<polygon_component>(u"marker", u"Marker", true, {
        [](const polygon_component* site){
            return site->marker();
        },
        [](polygon_component* site, bool value){
            site->marker(value);
        }
    });
    boolean_property<polygon_component>(u"opened", u"Opened", true, {
        [](const polygon_component* site){
            return site->opened();
        },
        [](polygon_component* site, bool value){
            site->opened(value);
        }
    });
    ranged_property<polygon_component>(u"opacity", u"Opacity", true, true, 5, {
        [](const polygon_component* site){
            return site->opacity();
        },
        [](polygon_component* site, float value){
            site->opacity(value);
        }
    });
    boolean_property<polygon_component>(u"smooth", u"Smooth", true, {
        [](const polygon_component* site){
            return site->smooth();
        },
        [](polygon_component* site, bool value){
            site->smooth(value);
        }
    });
    single_property<polygon_component>(u"smooth_quality", u"S Quality", true, {
        [](const polygon_component* site){
            return site->smooth_quality();
        },
        [](polygon_component* site, float value){
            if(value <= 0){
                site->notify_property_changed(u"smooth_quality");
                return;
            }
            site->smooth_quality(value);
        }
    });
    single_property<polygon_component>(u"smooth_divisions", u"S Divisions", true, {
        [](const polygon_component* site){
            return site->smooth_divisions();
        },
        [](polygon_component* site, float value){
            if(value < 4){
                site->notify_property_changed(u"smooth_divisions");
                return;
            }
            site->smooth_divisions(value);
        }
    });
    //Fill
    image_property<polygon_component>(u"image_name", u"Image", true, {
        [](const polygon_component* site){
            return site->image_name();
        },
        [](polygon_component* site, const rb_string& value){
            site->image_name(value);
        }
    });
    color_property<polygon_component>(u"tint", u"Tint", true, true, {
        [](const polygon_component* site){
            return site->tint();
        },
        [](polygon_component* site, const color& value){
            site->tint(value);
        }
    });
    ranged_property<polygon_component>(u"blend", u"Blend", true, true, 5, {
        [](const polygon_component* site){
            return site->blend();
        },
        [](polygon_component* site, float value){
            site->blend(value);
        }
    });
    boolean_property<polygon_component>(u"transformable", u"Transformable", true, {
        [](const polygon_component* site){
            return site->transformable();
        },
        [](polygon_component* site, bool value){
            site->transformable(value);
        }
    });
    //Border
    enumeration_property<polygon_component, corner_type>(u"border_corner_type", u"Corner Type", {{u"Bevel", corner_type::bevel}, {u"Miter", corner_type::miter}}, true, {
        [](const polygon_component* site){
            return site->border_corner_type();
        },
        [](polygon_component* site, corner_type value){
            site->border_corner_type(value);
        }
    });
    single_property<polygon_component>(u"border_size", u"Border Size", true, {
        [](const polygon_component* site){
            return site->border_size();
        },
        [](polygon_component* site, float value){
            if(value < 0){
                site->notify_property_changed(u"border_size");
                return;
            }
            site->border_size(value);
        }
    });
    nullable_color_property<polygon_component>(u"border_color", u"Border Color", true, true, {
        [](const polygon_component* site){
            return site->border_color();
        },
        [](polygon_component* site, const nullable<color>& value){
            site->border_color(value);
        }
    });
    image_property<polygon_component>(u"border_image_name", u"Border Image", true, {
        [](const polygon_component* site){
            return site->border_image_name();
        },
        [](polygon_component* site, const rb_string& value){
            site->border_image_name(value);
        }
    });
    single_property<polygon_component>(u"max_s", u"Max S", true, {
        [](const polygon_component* site){
            return site->max_s();
        },
        [](polygon_component* site, float value){
            if(value <= 0){
                site->notify_property_changed(u"max_s");
                return;
            }
            site->max_s(value);
        }
    });
    
    action<polygon_component>(u"bool_operations", u"Bool Ops", action_flags::single_dispatch, {u"Union", u"Intersection"}, [](polygon_component* site, const rb_string& action_name){
        if(action_name == u"Union")
            site->compute_union();
        else
            site->compute_intersection();
    });
    
    action<polygon_component>(u"misc_operations", u"Misc Ops", action_flags::multi_dispatch, {u"Convex Hull", u"To CCW"}, [](polygon_component* site, const rb_string& action_name){
        if(action_name == u"Convex Hull")
            site->to_convex_hull();
        else
            site->to_ccw();
    });
    
    begin_private_properties();
    buffer_property<polygon_component>(u"flags", u"Flags", {
        [](const polygon_component* site){
            return buffer(&site->_flags, sizeof(site->_flags));
        },
        [](polygon_component* site, buffer value){
            memcpy(&site->_flags, value.internal_buffer(), sizeof(site->_flags));
            site->_flags.dirty_map = true;
            site->_flags.dirty_border_map = true;
        }
    });
    
    buffer_property<polygon_component>(u"polygon", u"Polygon", {
        [](const polygon_component* site){
            return site->_polygon.to_buffer();
        },
        [](polygon_component* site, buffer value){
            site->_polygon = polygon(value);
        }
    });
    
    nullable_buffer_property<polygon_component>(u"m", u"M", {
        [](const polygon_component* site){
            if(site->_m)
                return (nullable<buffer>)site->_m->to_buffer();
            else
                return (nullable<buffer>)nullptr;
        },
        [](polygon_component* site, const nullable<buffer>& value){
            if(site->_m)
                delete site->_m;
            if(value.has_value())
                site->_m = new mesh(value.value());
            else
                site->_m = nullptr;
        }
    });
    nullable_buffer_property<polygon_component>(u"m_copy", u"M Copy", {
        [](const polygon_component* site){
            if(site->_m_copy)
                return (nullable<buffer>)site->_m_copy->to_buffer();
            else
                return (nullable<buffer>)nullptr;
        },
        [](polygon_component* site, const nullable<buffer>& value){
            if(site->_m_copy)
                delete site->_m_copy;
            if(value.has_value())
                site->_m_copy = new mesh(value.value());
            else
                site->_m_copy = nullptr;
        }
    });
    nullable_buffer_property<polygon_component>(u"b", u"B", {
        [](const polygon_component* site){
            if(site->_b)
                return (nullable<buffer>)site->_b->to_buffer();
            else
                return (nullable<buffer>)nullptr;
        },
        [](polygon_component* site, const nullable<buffer>& value){
            if(site->_b)
                delete site->_b;
            if(value.has_value())
                site->_b = new mesh(value.value());
            else
                site->_b = nullptr;
        }
    });
    nullable_buffer_property<polygon_component>(u"b", u"B Copy", {
        [](const polygon_component* site){
            if(site->_b_copy)
                return (nullable<buffer>)site->_b_copy->to_buffer();
            else
                return (nullable<buffer>)nullptr;
        },
        [](polygon_component* site, const nullable<buffer>& value){
            if(site->_b_copy)
                delete site->_b_copy;
            if(value.has_value())
                site->_b_copy = new mesh(value.value());
            else
                site->_b_copy = nullptr;
        }
    });
    
    buffer_property<polygon_component>(u"before", u"Before", {
        [](const polygon_component* site){
            return site->_before.to_buffer();
        },
        [](polygon_component* site, buffer value){
            site->_before = transform_space(value);
        }
    });
    buffer_property<polygon_component>(u"before_b", u"Before B", {
        [](const polygon_component* site){
            return site->_before_b.to_buffer();
        },
        [](polygon_component* site, buffer value){
            site->_before_b = transform_space(value);
        }
    });
    buffer_property<polygon_component>(u"tx_space", u"Tx Space", {
        [](const polygon_component* site){
            return site->_tx_space.to_buffer();
        },
        [](polygon_component* site, buffer value){
            site->_tx_space = transform_space(value);
        }
    });
    end_private_properties();
    end_type();
}

rb_string polygon_component::type_name() const {
    return u"rb::polygon_component";
}

rb_string polygon_component::displayable_type_name() const {
    return u"Polygon";
}

rectangle polygon_component::bounds() const {
    auto& _p = to_polygon();
    if(_p.is_empty())
        return node::bounds();
    auto _rc = _p.bounds();
    if(!_rc.has_value())
        return node::bounds();
    else
        return _rc.value();
}

bool polygon_component::hit_test(const rb::vec2 &pt) const {
    auto& _pol = to_polygon();
    if(_pol.is_empty())
        return false;
    auto _t = from_node_space_to(space::screen);
    if(!_t.test_direction(transform_direction::from_base_to_space))
        return false;
    auto _p = _t.from_base_to_space().transformed_point(pt);
    return _pol.test_intersection(_p);
}

bool polygon_component::hit_test(const rb::rectangle &rc) const {
    auto& _pol = to_polygon();
    if(_pol.is_empty())
        return false;
    auto _t = from_node_space_to(space::screen);
    if(!_t.test_direction(transform_direction::from_base_to_space))
        return false;
    polygon _other;
    rc.to_polygon(_other);
    _t.from_base_to_space().transform_polygon(_other);
    return _pol.test_intersection(_other);
}

const polygon& polygon_component::to_polygon() const{
    if(_polygon.is_empty())
        const_cast<polygon_component*>(this)->create_polygon_data();
    return _polygon;
}
polygon polygon_component::to_smooth_polygon() const {
    auto& _polygon = to_polygon();
    
    if (_flags.smooth && _polygon.point_count() >= 4){
        smooth_curve _sc;
        std::vector<vec2> _points;
        for (uint32_t i = 0; i < _polygon.point_count(); i++)
            _points.push_back(_polygon.get_point(i));
        
        if(_flags.open)
            _sc = smooth_curve::build_open_curve(_points, _sc);
        else
            _sc = smooth_curve::build_closed_curve(_points, _sc);
        
        polygon _f_pol;
        return _sc.to_polygon(_f_pol, _smooth_quality, _smooth_divisions);
    }
    else
        return _polygon;
}

const rb_string& polygon_component::image_name() const{
    return _image;
}
const rb_string& polygon_component::image_name(const rb_string& value){
    if(_image == value)
        return _image;
    auto _previous = _image;
    _image = value;
    _flags.dirty_map = true;
    if(_previous == no_texture){
        _flags.dirty_polygon = true;
        if(parent_scene() && parent_scene()->active())
            invalidate_buffers();
    }
    return _image;
}
uint32_t polygon_component::circle_sides() const{
    return _circle_sides;
    
}
uint32_t polygon_component::circle_sides(const uint32_t value){
    if(_circle_sides == value)
        return _circle_sides;
    assert(value >= 4);
    _circle_sides = value;
    _flags.dirty_polygon = true;
    _flags.dirty_border_polygon = true;
    if(parent_scene() && parent_scene()->active())
        invalidate_buffers();
    if(_flags.type == kPolCircle)
        reset_children(kPolCircle);
    return _circle_sides;
}
float polygon_component::opacity() const{
    return _opacity;
}
float polygon_component::opacity(const float value){
    if(_opacity == value)
        return _opacity;
    _opacity = value;
    _flags.dirty_opacity = true;
    return _circle_sides;
}
const color& polygon_component::tint() const{
    return _tint;
}
const color& polygon_component::tint(const color& value){
    if(_tint == value)
        return _tint;
    _tint = value;
    _flags.dirty_tint = true;
    return _tint;
}
float polygon_component::blend() const{
    return _blend;
}
float polygon_component::blend(const float value){
    if(_blend == value)
        return _blend;
    _blend = value;
    _flags.dirty_blend = value;
    return _blend;
}
float polygon_component::smooth_quality() const{
    return _smooth_quality;
}
float polygon_component::smooth_quality(const float value){
    if(_smooth_quality == value)
        return _smooth_quality;
    assert(_smooth_quality > 0);
    _smooth_quality = value;
    _flags.dirty_polygon = true;
    _flags.dirty_border_polygon = true;
    if(parent_scene() && parent_scene()->active())
        invalidate_buffers();
    return _smooth_quality;
}
float polygon_component::smooth_divisions() const{
    return _smooth_divisions;
}
float polygon_component::smooth_divisions(const float value){
    if(_smooth_divisions == value)
        return _smooth_divisions;
    assert(_smooth_divisions >= 4);
    _smooth_divisions = value;
    _flags.dirty_polygon = true;
    _flags.dirty_border_polygon = true;
    if(parent_scene() && parent_scene()->active())
        invalidate_buffers();
    return _smooth_divisions;
}
const transform_space& polygon_component::texture_space() const{
    return _tx_space;
}
const transform_space& polygon_component::texture_space(const transform_space& value) {
    if(_tx_space == value)
        return _tx_space;
    _tx_space = value;
    if(_flags.transformable || _flags.in_texture_transformation){
        _flags.dirty_map = true;
    }
    else {
        _flags.dirty_polygon = true;
        if(parent_scene() && parent_scene()->active())
            invalidate_buffers();
    }
    return _tx_space;
}
float polygon_component::border_size() const{
    return _border_size;
}
float polygon_component::border_size(const float value){
    if(_border_size == value)
        return _border_size;
    assert(_border_size >= 0);
    _border_size = value;
    _flags.dirty_border_polygon = true;
    if(parent_scene() && parent_scene()->active())
        invalidate_buffers();
    return _border_size;
}
corner_type polygon_component::border_corner_type() const{
    return _border_corner_type;
}
corner_type polygon_component::border_corner_type(const corner_type value){
    if(_border_corner_type == value)
        return _border_corner_type;
    _border_corner_type = value;
    _flags.dirty_border_polygon = true;
    if(parent_scene() && parent_scene()->active())
        invalidate_buffers();
    return _border_corner_type;
}
const nullable<color>& polygon_component::border_color() const{
    return _border_color;
}
const nullable<color>& polygon_component::border_color(const nullable<color>& value){
    if(!_border_color.has_value() && !value.has_value())
        return _border_color;
    if(_border_color.has_value() && value.has_value() && _border_color.value() == value.value())
        return _border_color;
    auto _previous = _border_color;
    _border_color = value;
    if(_previous.has_value() && _border_color.has_value()) //only color changed
        _flags.dirty_border_color = true;
    else
    {
        _flags.dirty_border_polygon = true;
        if(parent_scene() && parent_scene()->active())
            invalidate_buffers();
    }
    return _border_color;
}
const rb_string& polygon_component::border_image_name() const{
    return _border_texture;
}
const rb_string& polygon_component::border_image_name(const rb_string& value){
    if(_border_texture == value)
        return _border_texture;
    _border_texture = value;
    _flags.dirty_border_polygon = true;
    if(parent_scene() && parent_scene()->active())
        invalidate_buffers();
    return _border_texture;
}
float polygon_component::max_s() const{
    return _max_s;
}
float polygon_component::max_s(const float value){
    if(_max_s == value)
        return _max_s;
    assert(value > 0);
    _max_s = value;
    _flags.dirty_border_polygon = true;
    if(parent_scene() && parent_scene()->active())
        invalidate_buffers();
    return _max_s;
}
bool polygon_component::transformable() const{
    return _flags.transformable;
}
bool polygon_component::transformable(const bool value){
    if(_flags.transformable == value)
        return _flags.transformable;
    _flags.transformable = value;
    _flags.dirty_transformable = true;
    if(!_flags.transformable){
        _flags.dirty_polygon = true;
        if(parent_scene() && parent_scene()->active())
            invalidate_buffers();
    }
    return _flags.transformable;
}
bool polygon_component::visible() const{
    return _flags.visible;
}
bool polygon_component::visible(const bool value){
    if(_flags.visible == value)
        return _flags.visible;
    _flags.visible = value;
    return _flags.visible;
}
bool polygon_component::smooth() const{
    return _flags.smooth;
}
bool polygon_component::smooth(const bool value){
    if(_flags.smooth == value)
        return _flags.smooth;
    _flags.smooth = value;
    _flags.dirty_polygon = true;
    _flags.dirty_border_polygon = true;
    if(parent_scene() && parent_scene()->active())
        invalidate_buffers();
    return _flags.smooth;
}
bool polygon_component::opened() const{
    return _flags.open;
}
bool polygon_component::opened(const bool value){
    if(_flags.open == value)
        return _flags.open;
    _flags.open = value;
    _flags.dirty_polygon = true;
    _flags.dirty_border_polygon = true;
    if(parent_scene() && parent_scene()->active())
        invalidate_buffers();
    return _flags.open;
}
bool polygon_component::marker() const{
    return !_flags.renderable;
}
bool polygon_component::marker(const bool value){
    auto _value = !value;
    if(_flags.renderable == _value)
        return _flags.renderable;
    _flags.renderable = _value;
    _flags.dirty_polygon = true;
    _flags.dirty_border_polygon = true;
    if(parent_scene() && parent_scene()->active())
        invalidate_buffers();
    return _flags.renderable;
}

void polygon_component::render_gizmo() {
    auto _p = to_polygon();
    if((is_currently_selected() || !_flags.renderable || !_m || is_current()) && !_p.is_empty()){
        auto _g = create_skeleton(_p);
        if(_skeleton)
            delete _skeleton;
        _skeleton = _g;
        add_gizmo(_skeleton, no_texture, false);
    }
}

void polygon_component::reset_to_quad(){
    _flags.type = kPolQuad;
    _flags.dirty_polygon = true;
    _flags.dirty_border_polygon = true;
    if(parent_scene() && parent_scene()->active())
        invalidate_buffers();
    _polygon.reset();
    reset_children(kPolQuad);
}

void polygon_component::reset_to_circle(){
    _flags.type = kPolCircle;
    _flags.dirty_polygon = true;
    _flags.dirty_border_polygon = true;
    if(parent_scene() && parent_scene()->active())
        invalidate_buffers();
    _polygon.reset();
    reset_children(kPolCircle);
}

bool polygon_component::is_degenerated() const{
    bool _b = node::is_degenerated();
    
    if(_b)
        return _b;
    
    auto _pol = to_polygon();
    if(!_pol.is_empty() && _pol.point_count() >= 3 && _pol.is_simple().has_value() && _pol.is_simple().value() && _pol.area().has_value() && _pol.area().value())
        return false;
    else
        return true;
}

std::vector<rb_string> polygon_component::transformables() {
    return { u"Polygon's Center", u"Texture" };
}

void polygon_component::start_transformation(long index){
    if(index == 0){
        transform_gizmo::start_transformation(this->parent(), this->transform(), rectangle(0, 0, 1, 1), false, [=](transform_gizmo* g, const transform_space& t){
            auto _new_t = this->transform();
            _new_t.origin(t.origin());
            _new_t.rotation(t.rotation());
            this->adjust_transformation(_new_t);
        });
    }
    else {
        if(!this->transform().both_directions())
            return;
        _flags.in_texture_transformation = true;
        _flags.dirty_transformable = true;
        transform_gizmo::start_transformation(this->parent(), this->transform() * this->texture_space(), rectangle(0, 0, 1, 1), true, [=](transform_gizmo* g, const transform_space& t){
            this->texture_space(this->transform().inverse() * t);
        }, [=](){ //end
            _flags.in_texture_transformation = false;
            _flags.dirty_transformable = true;
            _flags.dirty_polygon = true;
            this->invalidate_buffers();
        });
    }
}

void polygon_component::compute_union(){
    std::vector<node *> _selection;
    std::vector<polygon_component*> _polygons;
    parent()->fill_with_selection(_selection);
    for (auto _n : _selection){
        if(_n->is_selected() && dynamic_cast<polygon_component*>(_n))
            _polygons.push_back(dynamic_cast<polygon_component*>(_n));
    }
    
    if(_polygons.size() <= 1)
        return;
    
    polygon _p;
    for (size_t i = 0; i < _polygons.size(); i++) {
        auto _pol = _polygons[i]->to_polygon();
        _polygons[i]->from_node_space_to(parent()).from_space_to_base().transform_polygon(_pol);
        if(i == 0)
            _p = _pol;
        else {
            _p.join(_pol);
        }
    }
    
    auto _rc = _p.bounds();
    transform_space _t = this->transform();
    if(_rc.has_value()){
        _t = transform_space();
        _t.origin(_rc.value().center());
    }
    
    _t.from_base_to_space().transform_polygon(_p);
    
    polygon_component* _new_p = new polygon_component();
    std::vector<node*> _nodes;
    _new_p->copy_nodes_to_vector(_nodes);
    for (auto _n : _nodes)
        _new_p->remove_node(_n, true);
    for (uint32_t i = 0; i < _p.point_count(); i++) {
        auto _pt = new polygon_point_component();
        _pt->transform(transform_space(_p.get_point(i)));
        _new_p->add_node(_pt);
    }
    _new_p->transform(_t);
    _new_p->image_name(this->image_name());
    _new_p->opacity(this->opacity());
    _new_p->tint(this->tint());
    _new_p->blend(this->blend());
    _new_p->smooth_quality(this->smooth_quality());
    _new_p->smooth_divisions(this->smooth_divisions());
    _new_p->smooth(this->smooth());
    _new_p->texture_space(this->texture_space());
    _new_p->border_size(this->border_size());
    _new_p->border_corner_type(this->border_corner_type());
    _new_p->border_color(this->border_color());
    _new_p->border_image_name(this->border_image_name());
    _new_p->max_s(this->max_s());
    _new_p->transformable(this->transformable());
    _new_p->visible(this->visible());
    _new_p->opened(this->opened());
    _new_p->marker(this->marker());
    
    parent()->clear_selection();
    parent()->add_node(_new_p);
    _new_p->add_to_selection();
}

void polygon_component::compute_intersection(){
    std::vector<node *> _selection;
    std::vector<polygon_component*> _polygons;
    parent()->fill_with_selection(_selection);
    for (auto _n : _selection){
        if(_n->is_selected() && dynamic_cast<polygon_component*>(_n))
            _polygons.push_back(dynamic_cast<polygon_component*>(_n));
    }
    
    if(_polygons.size() <= 1)
        return;
    
    polygon _p;
    for (size_t i = 0; i < _polygons.size(); i++) {
        auto _pol = _polygons[i]->to_polygon();
        _polygons[i]->from_node_space_to(parent()).from_space_to_base().transform_polygon(_pol);
        if(i == 0)
            _p = _pol;
        else {
            std::vector<polygon> _others;
            _p.intersection(_pol, _others);
        }
    }
    
    auto _rc = _p.bounds();
    transform_space _t = this->transform();
    if(_rc.has_value()){
        _t = transform_space();
        _t.origin(_rc.value().center());
    }
    
    _t.from_base_to_space().transform_polygon(_p);
    
    polygon_component* _new_p = new polygon_component();
    std::vector<node*> _nodes;
    _new_p->copy_nodes_to_vector(_nodes);
    for (auto _n : _nodes)
        _new_p->remove_node(_n, true);
    for (uint32_t i = 0; i < _p.point_count(); i++) {
        auto _pt = new polygon_point_component();
        _pt->transform(transform_space(_p.get_point(i)));
        _new_p->add_node(_pt);
    }
    _new_p->transform(_t);
    _new_p->image_name(this->image_name());
    _new_p->opacity(this->opacity());
    _new_p->tint(this->tint());
    _new_p->blend(this->blend());
    _new_p->smooth_quality(this->smooth_quality());
    _new_p->smooth_divisions(this->smooth_divisions());
    _new_p->smooth(this->smooth());
    _new_p->texture_space(this->texture_space());
    _new_p->border_size(this->border_size());
    _new_p->border_corner_type(this->border_corner_type());
    _new_p->border_color(this->border_color());
    _new_p->border_image_name(this->border_image_name());
    _new_p->max_s(this->max_s());
    _new_p->transformable(this->transformable());
    _new_p->visible(this->visible());
    _new_p->opened(this->opened());
    _new_p->marker(this->marker());
    
    parent()->clear_selection();
    parent()->add_node(_new_p);
    _new_p->add_to_selection();
}

void polygon_component::to_convex_hull() {
    auto _p = to_polygon();
    _p.convex_hull(_p);
    
    std::vector<node*> _nodes;
    this->copy_nodes_to_vector(_nodes);
    for (auto _n : _nodes)
        this->remove_node(_n, true);
    for (uint32_t i = 0; i < _p.point_count(); i++) {
        auto _pt = new polygon_point_component();
        _pt->transform(transform_space(_p.get_point(i)));
        this->add_node(_pt);
    }
}

void polygon_component::to_ccw(){
    auto _p = to_polygon();
    auto _o = _p.get_ordering();
    if(_o == point_ordering::unknown || _o == point_ordering::ccw)
        return;
    _p.revert();
    assert(_p.get_ordering() == point_ordering::ccw);
    
    std::vector<node*> _nodes;
    this->copy_nodes_to_vector(_nodes);
    for (auto _n : _nodes)
        this->remove_node(_n, true);
    for (uint32_t i = 0; i < _p.point_count(); i++) {
        auto _pt = new polygon_point_component();
        _pt->transform(transform_space(_p.get_point(i)));
        this->add_node(_pt);
    }
}































