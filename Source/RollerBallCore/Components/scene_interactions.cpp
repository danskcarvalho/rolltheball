//
//  scene_interactions.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 20/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "scene.h"
#include "layer.h"
#include "director.h"
#include "node.h"
#include "texture_atlas.h"
#include "program_manager.h"
#include "apple_program_manager.h"
#include "state_manager.h"
#include "extended_dynamic_mesh_batch.h"
#include "misc_classes.h"
#include "misc_classes_2.h"
#include "misc_classes_3.h"

using namespace rb;

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

float snap_angle(const float angle){
    float _360 = 2 * M_PI;
    float _s = sgn(angle);
    float _angle = angle;
    if(fabsf(angle) >= _360)
        _angle = _s * (fabsf(angle) - floorf(fabsf(angle) / _360) * _360);
    if(_angle < 0)
        _angle = _360 - _angle;
    
    float _s_angles[8] = { TO_RADIANS(0 * 45.0), TO_RADIANS(1 * 45.0), TO_RADIANS(2 * 45.0), TO_RADIANS(3 * 45.0), TO_RADIANS(4 * 45.0), TO_RADIANS(5 * 45.0), TO_RADIANS(6 * 45.0), TO_RADIANS(7 * 45.0) };
    float _min = std::numeric_limits<float>::max();
    float _snapped;
    for(int i = 0; i < 8; i++){
        auto _diff = fabsf(_s_angles[i] - _angle);
        if(_diff < _min){
            _snapped = _s_angles[i];
            _min = _diff;
        }
    }
    return _snapped;
}

vec2 snap_vector(const vec2& delta){
    if(delta == vec2::zero)
        return delta;
    float _angle = vec2::up.angle_between(delta, rotation_direction::ccw);
    _angle = snap_angle(_angle);
    vec2 _dir = matrix3x3::build_rotation(_angle).transformed_vector(vec2::up);
    return _dir * delta.length();
}

responder* scene::get_responder() {
    return this;
}

void scene::keydown(const uint32_t keycode, const keyboard_modifier modifier){
    if(playing()){
        bool swallowed = false;
        for (auto& _kvp : registered_nodes(registrable_event::keyboard_events)){
            if(unregistered(registrable_event::keyboard_events, _kvp.second))
                continue;
            if(!_kvp.second->parent_layer()->playing() || _kvp.second->parent_layer()->static_layer())
                continue;
            _kvp.second->keydown(keycode, modifier, swallowed);
            if(swallowed)
                return;
        }
        return;
    }
    if(in_editor() && !playing()) {
        if((modifier == (keyboard_modifier)0) ||
           (modifier == (keyboard_modifier)0xa00000)){
            if(keycode == KEY_DOWN){
                move_selected(vec2::down);
            }
            else if(keycode == KEY_UP){
                move_selected(vec2::up);
            }
            else if(keycode == KEY_LEFT){
                move_selected(vec2::left);
            }
            else if(keycode == KEY_RIGHT){
                move_selected(vec2::right);
            }
            else if(keycode == KEY_DELETE){
                _original_transforms.clear();
                delete_selected();
                if(current())
                    current()->clear_selection();
            }
        }
    }
}
void scene::keyup(const uint32_t keycode, const keyboard_modifier modifier){
    if(playing()){
        bool swallowed = false;
        for (auto& _kvp : registered_nodes(registrable_event::keyboard_events)){
            if(unregistered(registrable_event::keyboard_events, _kvp.second))
                continue;
            if(!_kvp.second->parent_layer()->playing() || _kvp.second->parent_layer()->static_layer())
                continue;
            _kvp.second->keyup(keycode, modifier, swallowed);
            if(swallowed)
                return;
        }
        return;
    }
}

void scene::begin_gesture(){
}

void scene::end_gesture(){
}

//gestures
void scene::magnify(const float delta){
    if(in_editor() && !playing() && !_in_transformation) {
        if(responder::modifiers() != (keyboard_modifier)0)
            return;
        
        auto _cam = camera();
        auto _scale = _cam.scale() + vec2(1, 1) * -delta * SCALE_MULTIPLIER;
        if (_scale.x() < MIN_CAMERA_ZOOM)
            _scale.x(MIN_CAMERA_ZOOM);
        if (_scale.y() < MIN_CAMERA_ZOOM)
            _scale.y(MIN_CAMERA_ZOOM);
        _cam.scale(_scale);
        camera(_cam);
        
    }
}
void scene::rotate(const float delta){
    if(in_editor() && !playing() && !_in_transformation) {
        if(responder::modifiers() != (keyboard_modifier)0)
            return;
        auto _cam = camera();
        _cam.rotate_by(-delta, -delta);
        camera(_cam);
    }
}
void scene::swipe(const vec2& delta) {
    
}
void scene::scroll(const vec2& delta){
    if(in_editor() && !playing() && responder::modifiers() == (keyboard_modifier)0 && !_in_transformation) {
        if(_navigation_mode == navigation_mode::scroll){
            auto _cam = camera();
            auto _viewport_size = state_manager::viewport_size();
            auto _t =  vec2(-2.0 / _viewport_size.x(), 2.0 / _viewport_size.y()) * delta * SCROLL_MULTIPLIER;
            //we use the camera axis...
            _t = _t.x() * _cam.from_space_to_base().x_vector() +
                _t.y() * _cam.from_space_to_base().y_vector();
            _cam.origin(_t + _cam.origin());
            camera(_cam);
        }
        else if(_navigation_mode == navigation_mode::rotate){
            
            rotate(-delta.x() * ROTATION_SCROLL_MULTIPLIER);
        }
        else { //zoom
            magnify(-delta.y() * ZOOM_SCROLL_MULTIPLIER);
        }
    }
}

//touches
void scene::touches_began(const std::vector<touch>& touches){
    if(playing()){
        bool swallowed = false;
        for (auto& _kvp : registered_nodes(registrable_event::touch_events)){
            if(unregistered(registrable_event::touch_events, _kvp.second))
                continue;
            if(!_kvp.second->parent_layer()->playing())
                continue;
            _kvp.second->touches_began(touches, swallowed);
            if(swallowed)
                return;
        }
        return;
    }
}
void scene::touches_moved(const std::vector<touch>& touches) {
    if(playing()){
        bool swallowed = false;
        for (auto& _kvp : registered_nodes(registrable_event::touch_events)){
            if(unregistered(registrable_event::touch_events, _kvp.second))
                continue;
            if(!_kvp.second->parent_layer()->playing() || _kvp.second->parent_layer()->static_layer())
                continue;
            _kvp.second->touches_moved(touches, swallowed);
            if(swallowed)
                return;
        }
        return;
    }

}
void scene::touches_ended(const std::vector<touch>& touches) {
    if(playing()){
        bool swallowed = false;
        for (auto& _kvp : registered_nodes(registrable_event::touch_events)){
            if(unregistered(registrable_event::touch_events, _kvp.second))
                continue;
            if(!_kvp.second->parent_layer()->playing() || _kvp.second->parent_layer()->static_layer())
                continue;
            _kvp.second->touches_ended(touches, swallowed);
            if(swallowed)
                return;
        }
        return;
    }
}
void scene::touches_cancelled(const std::vector<touch>& touches) {
    if(playing()){
        bool swallowed = false;
        for (auto& _kvp : registered_nodes(registrable_event::touch_events)){
            if(unregistered(registrable_event::touch_events, _kvp.second))
                continue;
            if(!_kvp.second->parent_layer()->playing() || _kvp.second->parent_layer()->static_layer())
                continue;
            _kvp.second->touches_cancelled(touches, swallowed);
            if(swallowed)
                return;
        }
        return;
    }
}

void scene::start_transformation(){
    if(current()){
        std::vector<node*> _selected;
        current()->fill_with_selection(_selected, node_filter::renderable);
        _original_transforms.clear();
        for (auto _s : _selected){
            _original_transforms[_s] = _s->transform();
        }
        _selected_nodes_bounds = compute_selection_bounds(_selected);
        _delta_transform = transform_space(vec2::zero, vec2::zero, vec2::zero);
    }
}

bool scene::start_selection(const vec2& normalized_position){
    bool _sel_rc = false;
    if(current() && in_editor() && !playing()) {
        _sel_rc = true;
        auto _current_pos = from_space_to_another(space::normalized_screen, space::screen).from_space_to_base().transformed_point(normalized_position);
        
        //we should add or remove?
        auto _add = rb::has_flag(responder::modifiers(), keyboard_modifier::shift);
        auto _remove = rb::has_flag(responder::modifiers(),
                                    keyboard_modifier::alternate);
        
        for (auto _it = current()->_child_nodes.rbegin(); _it != current()->_child_nodes.rend(); _it++){
            if(!(*_it)->renderable() || (*_it)->_new_template || (*_it)->blocked() || (*_it)->in_editor_hidden())
                continue;
            
            bool _fast_test = (*_it)->fast_hit_test(_current_pos);
            if(_fast_test && (*_it)->hit_test(_current_pos)){
                if(_add){
                    current()->add_to_selection(*_it);
                    _sel_rc = false;
                    break;
                }
                else if(_remove){
                    current()->remove_from_selection(*_it);
                    _sel_rc = false;
                    break;
                }
                else {
                    if(!(*_it)->is_selected()){
                        current()->clear_selection();
                        current()->add_to_selection(*_it);
                    }
                    _sel_rc = false;
                    break;
                }
            }
        }
    }
    return _sel_rc;
}

//basic mouse events
//those events isn't delivered in iOS...
void scene::mouse_down(const vec2& normalized_position) {
    if(playing()){
        bool swallowed = false;
        for (auto& _kvp : registered_nodes(registrable_event::mouse_events)){
            if(unregistered(registrable_event::mouse_events, _kvp.second))
                continue;
            if(!_kvp.second->parent_layer()->playing() || _kvp.second->parent_layer()->static_layer())
                continue;
            _kvp.second->mouse_down(normalized_position, swallowed);
            if(swallowed)
                return;
        }
        return;
    }
    
    if(_new_template){
        _current_new = dynamic_cast<node*>(_new_template->node()->clone());
        assert(_current_new);
        _current_new->_new_template = false;
        rename_nodes_recursively(_current_new);
        _new_start_point = current()->from_space_to(space::normalized_screen).from_base_to_space().transformed_point(normalized_position);
        //transform_space _new_tr = transform_space(_new_start_point, vec2::zero, vec2::zero);
        transform_space _new_tr = transform_space(_new_start_point, vec2(0.001, 0.001), 0);
        _current_new->transform(_new_tr);
        if(!current()->add_node(_current_new)) {
            delete _current_new;
            _current_new = nullptr;
            alert(u"Can't add " + _new_template->displayable_type_name() + u" to current node.");
        }
        return;
    }
    
    auto _h_handler = hit_test_handler(normalized_position);
    bool _started = false;
    if(!_h_handler.has_value())
        _started = start_selection(normalized_position);
    
    if(_started){
        start_transformation();
        auto _current_pos = from_space_to_another(space::normalized_screen, space::screen).from_space_to_base().transformed_point(normalized_position);
        _saved_selection = current()->_selection;
        _selection_rectangle = rectangle(_current_pos, vec2::zero);
        _initial_mouse_pos = normalized_position;
        _last_mouse_pos = normalized_position;
        _dragged_mouse = false;
        _in_transformation = false;
        _handler_transformation = nullptr;
    }
    else if(in_editor() && !playing()) {
        if(rb::has_flag(responder::modifiers(), keyboard_modifier::control)){
            if((!_h_handler.has_value() || _h_handler.value() == vec2::zero)){
                std::vector<node*> _selection;
                //we clone everyone
                if(current()){
                    current()->fill_with_selection(_selection, node_filter::renderable);
                    for (size_t i = 0; i < _selection.size(); i++){
                        _selection[i]->remove_from_selection();
                        _selection[i] = dynamic_cast<node*>(_selection[i]->clone());
                        //avoid duplicate names
                        rename_nodes_recursively(_selection[i]);
                        //add to scene...
                        current()->add_node(_selection[i]);
                    }
                    for (size_t i = 0; i < _selection.size(); i++){
                        _selection[i]->add_to_selection();
                    }
                }
            }
        }
        
        start_transformation();
        
        _handler_transformation = _h_handler;
        _in_transformation = true;
        _dragged_mouse = false;
        _selection_rectangle = nullptr;
        _initial_mouse_pos = normalized_position;
        _last_mouse_pos = normalized_position;
        _transform_mode = tm_none;
        
        for (auto& _kvp : registered_nodes(registrable_event::live_edit)){
            if(unregistered(registrable_event::live_edit, _kvp.second))
                continue;
            if(_kvp.second->is_currently_selected())
                _kvp.second->begin_live_edit(live_edit::transformation);
        }
    }
}

void scene::mouse_up(const vec2& normalized_position) {
    if(playing()){
        bool swallowed = false;
        for (auto& _kvp : registered_nodes(registrable_event::mouse_events)){
            if(unregistered(registrable_event::mouse_events, _kvp.second))
                continue;
            if(!_kvp.second->parent_layer()->playing() || _kvp.second->parent_layer()->static_layer())
                continue;
            _kvp.second->mouse_up(normalized_position, swallowed);
            if(swallowed)
                return;
        }
        return;
    }
    
    if(_current_new){
        _current_new = nullptr;
        _new_start_point = vec2::zero;
        return;
    }
    
    if(_selection_rectangle.has_value()){
        _selection_rectangle = nullptr;
        _saved_selection.clear();
        if(!_dragged_mouse && responder::modifiers() == (keyboard_modifier)0){
            current()->clear_selection();
        }
    }
    else if(current()) {
        for (auto& _kvp : registered_nodes(registrable_event::live_edit)){
            if(unregistered(registrable_event::live_edit, _kvp.second))
                continue;
            if(_kvp.second->is_currently_selected())
                _kvp.second->end_live_edit();
        }
        
        if(!_dragged_mouse  && responder::modifiers() == (keyboard_modifier)0 && in_editor() && !playing()){
            try_select_one(normalized_position);
        }
        //we commit...
        if(_in_transformation){
            std::vector<node*> _selected;
            current()->fill_with_selection(_selected, node_filter::renderable);
            transform_nodes(_selected);
            start_transformation();
        }
    }
    _handler_transformation = nullptr;
    _in_transformation = false;
    _dragged_mouse = false;
}
void scene::try_select_one(const vec2& normalized_position){
    auto _current_pos = from_space_to_another(space::normalized_screen, space::screen).from_space_to_base().transformed_point(normalized_position);
    
    for (auto _it = current()->_child_nodes.rbegin(); _it != current()->_child_nodes.rend(); _it++){
        if(!(*_it)->renderable() || (*_it)->_new_template || (*_it)->blocked() || (*_it)->in_editor_hidden())
            continue;
        
        bool _fast_test = (*_it)->fast_hit_test(_current_pos);
        
        if(_fast_test && (*_it)->hit_test(_current_pos)){
            current()->clear_selection();
            current()->add_to_selection(*_it);
            break;
        }
    }
}

void scene::mouse_dragged(const vec2& normalized_position) {
    if(playing()){
        bool swallowed = false;
        for (auto& _kvp : registered_nodes(registrable_event::mouse_events)){
            if(unregistered(registrable_event::mouse_events, _kvp.second))
                continue;
            if(!_kvp.second->parent_layer()->playing() || _kvp.second->parent_layer()->static_layer())
                continue;
            _kvp.second->mouse_dragged(normalized_position, swallowed);
            if(swallowed)
                return;
        }
        return;
    }
    
    if(_current_new){
        auto _current_point = current()->from_space_to(space::normalized_screen).from_base_to_space().transformed_point(normalized_position);
        auto _y_vec = _current_point - _new_start_point;
        _y_vec = has_flag(responder::modifiers(), keyboard_modifier::shift) ? snap_vector(_y_vec) : _y_vec;
        auto _x_vec = _y_vec.rotated(TO_RADIANS(-90));
        auto _canonical_x = vec2(1, 0);
        transform_space _new_tr;
        if(_x_vec != vec2::zero)
            _new_tr = transform_space(_new_start_point, vec2(_x_vec.length(), _y_vec.length()), _canonical_x.angle_between(_x_vec, rotation_direction::ccw));
        else
            _new_tr = transform_space(_new_start_point, vec2(0, 0), 0);
        _current_new->transform(_new_tr);
        return;
    }
    
    if(_selection_rectangle.has_value()){
        drag_selection_marquee(normalized_position);
    }
    else {
        if(in_editor() && !playing()) {
            if(_original_transforms.size() != 0 && _in_transformation)
            {
                _dragged_mouse = true;
                if((!_handler_transformation.has_value() || _handler_transformation.value() == vec2::zero) && (_transform_mode == tm_move || _transform_mode == tm_none)){
                    drag_selected(normalized_position - _last_mouse_pos);
                    _transform_mode = tm_move;
                }
                else
                    handler_transform(normalized_position);
                _last_mouse_pos = normalized_position;
            }
        }
    }
}

transform_space add_spaces(const transform_space& t1, const transform_space& t2){
    return transform_space(t1.origin() + t2.origin(), t1.scale() + t2.scale(), t1.rotation() + t2.rotation());
}

void scene::scale_selected(const vec2& delta){
    if(delta == vec2::zero)
        return;
    auto _tr = current()->from_space_to(space::normalized_screen);
    if(!_tr.test_direction(transform_direction::from_base_to_space))
        return;
    auto _delta = _tr.from_base_to_space().transformed_vector(delta); //in current space
    transform_space _original_transform;
    if(current()->selection_count() == 1)
        _original_transform = _original_transforms.begin().operator*().second;
    
    transform_space _rotated = _original_transform.rotated(_original_transform.rotation() + _delta_transform.rotation());
    
    auto _t_x = _rotated.from_space_to_base().x_vector();
    auto _t_y = _rotated.from_space_to_base().y_vector();
    vec2 _delta_x;
    vec2 _delta_y;
    if(_t_x != vec2::zero)
        _delta_x = _delta.projection(_t_x);
    else
        _delta_x = vec2::zero;
    
    if(_t_y != vec2::zero)
        _delta_y = _delta.projection(_t_y);
    else
        _delta_y = vec2::zero;
    
    auto _s_delta_x = _delta_x;
    auto _s_delta_y = _delta_y;
    
    if(_handler_transformation.value() == vec2::right)
        _delta_y = vec2::zero;
    if(_handler_transformation.value() == vec2::up)
        _delta_x = vec2::zero;
    
    float _x_sin = sgn(vec2::dot(_delta, _t_x));
    float _y_sin = sgn(vec2::dot(_delta, _t_y));
    
    if(_x_sin >= 0) //then it goes in the same direction
        _x_sin = sgn(_rotated.scale().x());
    else
        _x_sin = -sgn(_rotated.scale().x());
    
    if(_y_sin >= 0) //then it goes in the same direction
        _y_sin = sgn(_rotated.scale().y());
    else
        _y_sin = -sgn(_rotated.scale().y());
    
    std::vector<node*> _selected;
    current()->fill_with_selection(_selected, node_filter::renderable);
    auto _shift = rb::has_flag(responder::modifiers(), keyboard_modifier::shift) || (_selected.size() == 1 && _selected[0]->_transformation->uniform());
    if(_shift){
        if(_delta_y == vec2::zero){
            _delta_transform.scale(vec2(_delta_transform.scale().x() + _delta_x.length() * _x_sin, _delta_transform.scale().y()));
            auto _final_x = _delta_transform.scale().x() + _original_transform.scale().x();
            auto _x_prop = _final_x / _original_transform.scale().x();
            auto _final_y = _original_transform.scale().y() * _x_prop;
            _delta_transform.scale(vec2(_delta_transform.scale().x(), - _original_transform.scale().y() + _final_y));
        }
        else if(_delta_x == vec2::zero){
            _delta_transform.scale(vec2(_delta_transform.scale().x(), _delta_transform.scale().y() + _delta_y.length() * _y_sin));
            auto _final_y = _delta_transform.scale().y() + _original_transform.scale().y();
            auto _y_prop = _final_y / _original_transform.scale().y();
            auto _final_x = _original_transform.scale().x() * _y_prop;
            _delta_transform.scale(vec2(- _original_transform.scale().x() + _final_x, _delta_transform.scale().y()));
        }
    }
    else {
        _delta_transform.scale(vec2(_delta_transform.scale().x() + _delta_x.length() * _x_sin, _delta_transform.scale().y()));
        _delta_transform.scale(vec2(_delta_transform.scale().x(), _delta_transform.scale().y() + _delta_y.length() * _y_sin));
    }
    transform_nodes(_selected);
}

void scene::rotate_selected(const rb::vec2 &normalized_position){
    auto _tr = current()->from_space_to(space::normalized_screen);
    if(!_tr.test_direction(transform_direction::from_base_to_space))
        return;
    auto _position = _tr.from_base_to_space().transformed_point(normalized_position); //in current space
    
    std::vector<node*> _selection;
    current()->fill_with_selection(_selection, node_filter::renderable);
    
    vec2 _axis = _handler_transformation.value();
    if(_axis == vec2::right){
        if(_selection.size() == 1)
            _axis = _original_transforms[_selection[0]].from_space_to_base().x_vector();
    }
    else {
        if(_selection.size() == 1)
            _axis = _original_transforms[_selection[0]].from_space_to_base().y_vector();
    }
    
    vec2 _center;
    
    if(_selection.size() == 1)
        _center = _selection[0]->transform().origin();
    else
        _center = compute_selection_bounds(_selection).center();
    
    _position = _position - _center;
    _position.normalize();
    
    auto _angle = _axis.angle_between(_position, rotation_direction::ccw);
    if(rb::has_flag(responder::modifiers(), keyboard_modifier::shift))
        _angle = snap_angle(_angle);
    
    vec2 _v_angle;
    
    auto _alt = rb::has_flag(responder::modifiers(), keyboard_modifier::alternate);
    if(_selection.size() == 1 && _selection[0]->_transformation->orthogonal())
        _alt = false;
    
    if(_alt){
        if(_handler_transformation.value() == vec2::right)
            _v_angle = vec2(_angle, 0);
        else
            _v_angle = vec2(0, _angle);
    }
    else
        _v_angle = vec2(_angle, _angle);
    
    _delta_transform.rotation(_v_angle);
    transform_nodes(_selection);
}

void scene::handler_transform(const vec2& normalized_position){
    if(!current())
        return;
    if((_transform_mode == tm_rotate  || _transform_mode == tm_none) && rb::has_flag(responder::modifiers(), keyboard_modifier::control)){
        _transform_mode = tm_rotate;
        rotate_selected(normalized_position);
    }
    else if (_transform_mode == tm_none || _transform_mode == tm_scale) {
        _transform_mode = tm_scale;
        scale_selected(normalized_position - _last_mouse_pos);
    }
}

void scene::drag_selection_marquee(const vec2& normalized_position){
    _dragged_mouse = true;
    auto _current_pos = from_space_to_another(space::normalized_screen, space::screen).from_space_to_base().transformed_point(normalized_position);
    auto _initial_screen_pos = from_space_to_another(space::normalized_screen, space::screen).from_space_to_base().transformed_point(_initial_mouse_pos);
    vec2 _size = _initial_screen_pos - _current_pos;
    _size = vec2(fabsf(_size.x()), fabsf(_size.y()));
    _selection_rectangle = rectangle((_initial_screen_pos + _current_pos) / 2.0, _size);
    
    //we should add or remove?
    auto _add = rb::has_flag(responder::modifiers(), keyboard_modifier::shift);
    auto _remove = rb::has_flag(responder::modifiers(),
                                keyboard_modifier::alternate);
    auto _sel = _saved_selection;
    
    if(!_add && !_remove)
        _sel.clear();
    
    for (auto _it = current()->_child_nodes.rbegin(); _it != current()->_child_nodes.rend(); _it++){
        if(!(*_it)->renderable() || (*_it)->_new_template || (*_it)->blocked() || (*_it)->in_editor_hidden())
            continue;
        bool _fast_test = (*_it)->fast_hit_test(_selection_rectangle.value());
        if(_fast_test && (*_it)->hit_test(_selection_rectangle.value())){
            if(_add){
                _sel.insert(*_it);
            }
            else if(_remove){
                _sel.erase(*_it);
            }
            else {
                _sel.insert(*_it);
            }
        }
    }
    //remove from selection
    auto _current = current()->_selection.begin();
    while (_current != current()->_selection.end()) {
        if(_sel.count(*_current) == 0)
            _current = current()->internal_remove_from_selection(_current);
        else
            _current++;
    }
    //add to selection
    for (auto _n : _sel)
        current()->add_to_selection(_n);
}

void scene::delete_selected(){
    std::vector<node*> _selected;
    if(!current())
        return;
    current()->fill_with_selection(_selected);
    for(auto _n : _selected){
        current()->remove_from_selection(_n);
        current()->remove_node(_n, true);
    }
}

void scene::move_selected(const rb::vec2 &dir){
    vec2 _dir = dir * MOVE_MULTIPLIER;
    if(!current())
        return;
    auto _tr = current()->from_space_to(space::screen);
    if(!_tr.test_direction(transform_direction::from_base_to_space))
        return;
    _tr.from_base_to_space().transform_vector(_dir);
    if(_move10x)
        _dir *= 10;
    _delta_transform.move_by(_dir);
    std::vector<node*> _selected;
    current()->fill_with_selection(_selected, node_filter::renderable);
    transform_nodes(_selected);
}

void scene::drag_selected(const rb::vec2 &delta){
    if(!current())
        return;
    auto _tr = current()->from_space_to(space::normalized_screen);
    if(!_tr.test_direction(transform_direction::from_base_to_space))
        return;
    auto _delta = _tr.from_base_to_space().transformed_vector(delta);
    
    _delta_transform.move_by(_delta);
    std::vector<node*> _selected;
    current()->fill_with_selection(_selected, node_filter::renderable);
    transform_nodes(_selected);
}

transform_space scene::get_current_transform(bool live){
    transform_space _transform;
    if(!current())
        return _transform;
    
    if(current()->selection_count() == 1)
        _transform = _original_transforms[(*current()->_selection.begin())];
    else {
        std::vector<node*> _selection;
        current()->fill_with_selection(_selection, node_filter::renderable);
        auto _bounds = compute_selection_bounds(_selection, live);
        _transform = transform_space(_bounds.center());
    }
    
    vec2 _move = _delta_transform.origin();
//    if(rb::has_flag(responder::modifiers(), keyboard_modifier::shift) && _transform_mode == tm_move)
//        _move = snap_vector(_move);
    
    _transform.scale(_transform.scale() + _delta_transform.scale());
    _transform.origin(_transform.origin() + _move);
    _transform.rotation(_transform.rotation() + _delta_transform.rotation());
    return _transform;
}

void scene::transform_nodes(const std::vector<node *>& selection){
    if(selection.size() == 1){
        selection[0]->transform(get_current_transform());
        return;
    }
    
    for(auto n : selection){
        auto _n_t = _original_transforms[n];
        auto _new_t = transform_space(_original_transforms[n].origin() - _selected_nodes_bounds.center(), _original_transforms[n].scale(), _original_transforms[n].rotation());
        n->transform(get_current_transform() * _new_t);
    }
}

rectangle scene::compute_selection_bounds(const std::vector<node *>& selection, bool live){
    rectangle _rc = rectangle(vec2::zero, vec2::zero);
    for (auto _n : selection){
        auto _current = _n->transform();
        _n->shutdown_transform_notifications();
        if(!live)
            _n->transform(_original_transforms[_n]);
        auto _b = _n->bounds();
        _n->from_node_space_to(current()).from_space_to_base().transform_rectangle(_b);
        _rc = _rc.compute_union(_b);
        if(!live)
            _n->transform(_current);
        _n->restore_transform_notification();
    }
    return _rc;
}





























