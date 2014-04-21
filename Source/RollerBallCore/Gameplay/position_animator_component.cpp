//
//  position_animator_component.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 03/03/14.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#include "position_animator_component.h"
#include "scene.h"
#include "polygon_component.h"
#include "matrix3x3.h"
#include "layer.h"
#include "physics_shape.h"

using namespace rb;

#define ANIMATOR_UPDATE_PRIORITY -30000

position_animator_component::position_animator_component(){
    _velocity = 1;
    _asleep_duration = 0;
    _awake_duration = 0;
    _paused = false;
    _current_asleep_start = 0;
    _start_asleep_duration = 0;
    _endpoint_asleep_duration = 0;
    _saved_paused = false;
    _invert = false;
    _clamp = false;
}

void position_animator_component::reset_component(){
    for (size_t i = 0; i < _nodes.size(); i++) {
        _nodes[i]->transform(_saved_transforms[i]);
        _current_asleep_endpoint[i] = 0;
        _current_asleep[i] = 0;
        _current_awake[i] = _awake_duration;
        
    }
    _lengths = _saved_lengths;
    _dirs = _saved_dirs;
    _paused = _saved_paused;
    _current_asleep_start = _start_asleep_duration;
}

void position_animator_component::adjust_objects_to_path(){
    _nodes = parent_scene()->node_with_one_class(_class);
    auto _temp_paths = parent_scene()->node_with_one_class(_path_class);
    for (auto _p : _temp_paths){
        auto _pc = dynamic_cast<polygon_component*>(_p);
        if(!_pc)
            continue;
        
        //we take scale into consideration
        for (uint32_t i = 0; i < _pc->node_count(); i++){
            auto _pt = _pc->node_at(i);
            auto _ptt = _pt->transform();
            _ptt = _ptt.moved(_ptt.origin() * _pc->transform().scale());
            _pt->transform(_ptt);
        }
        _pc->transform(_pc->transform().scaled(1, 1));
        
        _paths.insert({_p, polygon_path(_pc->to_smooth_polygon())});
    }
    
    for (auto _n : _nodes){
        for (auto _p : _paths){
            if(_n->has_class(_p.first->name())){
                //calc the length offset in the path...
                auto _xy = _n->transform().origin();
                if(_n->parent_node())
                    _n->parent_node()->from_node_space_to(_p.first).from_space_to_base().transform_point(_xy);
                else
                    _n->parent_layer()->from_layer_space_to(_p.first).from_space_to_base().transform_point(_xy);
                
                auto _len = _p.second.length(_xy);
                
                //we adjust the transform of the object
                auto _new_xy = _p.second.point_at(_len, true);
                _p.first->from_node_space_to(_n->parent()).from_space_to_base().transform_point(_new_xy);
                _n->transform(_n->transform().moved(_new_xy));
                break;
            }
        }
    }
    
    _nodes.clear();
    _paths.clear();
}

void position_animator_component::playing(){
    if(_nodes.size() == 0){
        _nodes = parent_scene()->node_with_one_class(_class);
        auto _temp_paths = parent_scene()->node_with_one_class(_path_class);
        for (auto _p : _temp_paths){
            auto _pc = dynamic_cast<polygon_component*>(_p);
            if(!_pc)
                continue;
            
            //we take scale into consideration
            for (uint32_t i = 0; i < _pc->node_count(); i++){
                auto _pt = _pc->node_at(i);
                auto _ptt = _pt->transform();
                _ptt = _ptt.moved(_ptt.origin() * _pc->transform().scale());
                _pt->transform(_ptt);
            }
            _pc->transform(_pc->transform().scaled(1, 1));
            
            _paths.insert({_p, polygon_path(_pc->to_smooth_polygon())});
        }
        
        for (auto _n : _nodes){
            for (auto _p : _paths){
                if(_n->has_class(_p.first->name())){
                    _attached_path.insert({_n, _p.first});
                    //calc the length offset in the path...
                    auto _xy = _n->transform().origin();
                    if(_n->parent_node())
                        _n->parent_node()->from_node_space_to(_p.first).from_space_to_base().transform_point(_xy);
                    else
                        _n->parent_layer()->from_layer_space_to(_p.first).from_space_to_base().transform_point(_xy);
                    
                    auto _len = _p.second.length(_xy);
                    _lengths.push_back(_len);
                    _saved_lengths.push_back(_len);
                    
                    if(_n->has_class(u"revDir"))
                    {
                        _dirs.push_back(-1);
                        _saved_dirs.push_back(-1);
                    }
                    else
                    {
                        _dirs.push_back(1);
                        _saved_dirs.push_back(1);
                    }
                    
                    //we adjust the transform of the object
                    auto _new_xy = _p.second.point_at(_len, true);
                    _p.first->from_node_space_to(_n->parent()).from_space_to_base().transform_point(_new_xy);
                    _n->transform(_n->transform().moved(_new_xy));
                    
                    _saved_transforms.push_back(_n->transform());
                    break;
                }
            }
            _current_asleep_endpoint.push_back(0);
            _current_asleep.push_back(0);
            _current_awake.push_back(_awake_duration);
        }
        
        _current_asleep_start = _start_asleep_duration;
        _saved_paused = _paused;
    }
}

void position_animator_component::update(float dt){
    if(_paused)
        return;
    
    if(_current_asleep_start > 0){
        _current_asleep_start -= dt;
        if(_current_asleep_start <= 0){
            for (size_t i = 0; i < _nodes.size(); i++)
                _current_awake[i] = _awake_duration;
            _current_asleep_start = 0;
        }
        return;
    }
    
    for (size_t i = 0; i < _nodes.size(); i++) {
        if(_current_asleep_endpoint[i] > 0){
            _current_asleep_endpoint[i] -= dt;
            if(_current_asleep_endpoint[i] <= 0){
                _current_awake[i] = _awake_duration;
                _current_asleep_endpoint[i] = 0;
            }
            continue;
        }
        
        if(_current_asleep[i] > 0){
            _current_asleep[i] -= dt;
            if(_current_asleep[i] <= 0){
                _current_awake[i] = _awake_duration;
                _current_asleep[i] = 0;
            }
            continue;
        }
        
        auto _t = _nodes[i]->transform();
        auto _l = _lengths[i];
        auto _d = _dirs[i];
        auto _path = _attached_path[_nodes[i]];
        auto& _pp = _paths[_path];
        
        _l += _d * _velocity * dt;
        if(_invert)
        {
            if(_l < 0){
                _l = -_l;
                _d = -_d;
                _current_asleep_endpoint[i] = _endpoint_asleep_duration;
            }
            else if(_l > _pp.length()){
                _l = 2 * _pp.length() - _l;
                _d = -_d;
                _current_asleep_endpoint[i] = _endpoint_asleep_duration;
            }
        }
        else if(_clamp){
            if(_l < 0)
                _l = 0;
            else if(_l > _pp.length())
                _l = _pp.length();
        }
        
        auto _nxy = _pp.point_at(_l, _clamp);
        _path->from_node_space_to(_nodes[i]->parent()).from_space_to_base().transform_point(_nxy);
        _t = _t.moved(_nxy);
        _nodes[i]->transform(_t);
        _lengths[i] = _l;
        _dirs[i] = _d;
        
        if(_current_awake[i] > 0){
            _current_awake[i] -= dt;
            if(_current_awake[i] <= 0){
                _current_asleep[i] = _asleep_duration;
                _current_awake[i] = 0;
            }
        }
    }
}

void position_animator_component::describe_type(){
    nvnode::describe_type();
    start_type<position_animator_component>([](){return new position_animator_component(); });
    action<position_animator_component>(u"adjust", u"", action_flags::multi_dispatch, {u"Adjust Objects"},
        [](position_animator_component* site, const rb_string& action){
            site->adjust_objects_to_path();
        });
    single_property<position_animator_component>(u"velocity", u"Vel", true, {
        [](const position_animator_component* site){
            return site->velocity();
        },
        [](position_animator_component* site, float value){
            site->velocity(value);
        }
    });
    string_property<position_animator_component>(u"animated_class", u"Anim Class", true, false, {
        [](const position_animator_component* site){
            return site->animated_class();
        },
        [](position_animator_component* site, const rb_string& value){
            site->animated_class(value);
        }
    });
    string_property<position_animator_component>(u"path_class", u"Path Class", true, false, {
        [](const position_animator_component* site){
            return site->path_class();
        },
        [](position_animator_component* site, const rb_string& value){
            site->path_class(value);
        }
    });
    boolean_property<position_animator_component>(u"paused_animation",u"Paused", true, {
        [](const position_animator_component* site){
            return site->paused_animation();
        },
        [](position_animator_component* site, bool value){
            site->paused_animation(value);
        }
    });
    boolean_property<position_animator_component>(u"clamp",u"Clamp", true, {
        [](const position_animator_component* site){
            return site->clamp();
        },
        [](position_animator_component* site, bool value){
            site->clamp(value);
        }
    });
    boolean_property<position_animator_component>(u"invert",u"Invert", true, {
        [](const position_animator_component* site){
            return site->invert();
        },
        [](position_animator_component* site, bool value){
            site->invert(value);
        }
    });
    single_property<position_animator_component>(u"asleep_duration", u"Asleep", true, {
        [](const position_animator_component* site){
            return site->asleep_duration();
        },
        [](position_animator_component* site, float value){
            site->asleep_duration(value);
        }
    });
    single_property<position_animator_component>(u"awake_duration", u"Awake", true, {
        [](const position_animator_component* site){
            return site->awake_duration();
        },
        [](position_animator_component* site, float value){
            site->awake_duration(value);
        }
    });
    single_property<position_animator_component>(u"initial_asleep_duration", u"St Asleep", true, {
        [](const position_animator_component* site){
            return site->start_asleep_duration();
        },
        [](position_animator_component* site, float value){
            site->start_asleep_duration(value);
        }
    });
    single_property<position_animator_component>(u"endpoint_asleep_duration", u"End Asleep", true, {
        [](const position_animator_component* site){
            return site->endpoint_asleep_duration();
        },
        [](position_animator_component* site, float value){
            site->endpoint_asleep_duration(value);
        }
    });
    end_type();
}

void position_animator_component::after_becoming_active(bool node_was_moved){
    register_for(registrable_event::update, ANIMATOR_UPDATE_PRIORITY);
}

rb_string position_animator_component::type_name() const {
    return u"rb::position_animator_component";
}

rb_string position_animator_component::displayable_type_name() const {
    return u"Position Animator";
}

void position_animator_component::reset_animation(bool initial_delay){
    for (size_t i = 0; i < _nodes.size(); i++) {
        _current_asleep[i] = 0;
        _current_awake[i] = _awake_duration;
        _current_asleep_endpoint[i] = 0;
    }
    if(initial_delay)
        _current_asleep_start = _start_asleep_duration;
}

bool position_animator_component::paused_animation() const {
    return _paused;
}

bool position_animator_component::paused_animation(bool value){
    _paused = value;
    return _paused;
}

const rb_string& position_animator_component::animated_class() const {
    return _class;
}

const rb_string& position_animator_component::animated_class(const rb_string &value){
    assert(!is_playing());
    _class = value;
    return _class;
}

const rb_string& position_animator_component::path_class() const {
    return _path_class;
}

const rb_string& position_animator_component::path_class(const rb_string &value){
    assert(!is_playing());
    _path_class = value;
    return _path_class;
}

bool position_animator_component::clamp() const {
    return _clamp;
}

bool position_animator_component::clamp(bool value){
    return _clamp = value;
}

bool position_animator_component::invert() const {
    return _invert;
}

bool position_animator_component::invert(bool value){
    return _invert = value;
}

float position_animator_component::velocity() const {
    return _velocity;
}

float position_animator_component::velocity(float value){
    return _velocity = value;
}

float position_animator_component::asleep_duration() const{
    return _asleep_duration;
}

float position_animator_component::asleep_duration(float value){
    _asleep_duration = value;
    return _asleep_duration;
}

float position_animator_component::awake_duration() const {
    return _awake_duration;
}

float position_animator_component::awake_duration(float value){
    _awake_duration = value;
    return _awake_duration;
}

float position_animator_component::start_asleep_duration() const {
    return _start_asleep_duration;
}

float position_animator_component::start_asleep_duration(float value){
    return _start_asleep_duration = value;
}

float position_animator_component::endpoint_asleep_duration() const {
    return _endpoint_asleep_duration;
}

float position_animator_component::endpoint_asleep_duration(float value){
    return _endpoint_asleep_duration = value;
}

void position_animator_component::do_action(const rb_string& action_name, const rb_string& arg){
    if(action_name == u"play")
        paused_animation(false);
    else if(action_name == u"pause")
        paused_animation(true);
}













































