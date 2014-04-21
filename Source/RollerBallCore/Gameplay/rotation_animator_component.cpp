//
//  saw_rotation_animator.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 05/01/14.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#include "rotation_animator_component.h"
#include "scene.h"

using namespace rb;

#define ANIMATOR_UPDATE_PRIORITY -30000

rotation_animator_component::rotation_animator_component(){
    _r_velocity = M_PI;
    _asleep_duration = 0;
    _awake_duration = 0;
    _asleep_duration_start = 0;
    _asleep_duration_endpoint = 0;
    _paused = false;
    _current_asleep_start = 0;
    _saved_paused = false;
    _min_angle = nullptr;
    _max_angle = nullptr;
    _change_dir_endpoint = false;
}

void rotation_animator_component::reset_component(){
    for (size_t i = 0; i < _nodes.size(); i++) {
        _nodes[i]->transform(_saved_transforms[i]);
        _directions[i] = 1;
        _current_asleep_endpoint[i] = 0;
        _current_asleep[i] = 0;
        _current_awake[i] = _awake_duration;
    }
    _paused = _saved_paused;
    _current_asleep_start = _asleep_duration_start;
}

void rotation_animator_component::playing(){
    if(_nodes.size() == 0){
        _nodes = parent_scene()->node_with_one_class(_class);
        for (auto _n : _nodes){
            _saved_transforms.push_back(_n->transform());
            _directions.push_back(_n->has_class(u"revDir") ? -1 : 1);
            _current_asleep_endpoint.push_back(0);
            _current_asleep.push_back(0);
            _current_awake.push_back(_awake_duration);
            
            if(_min_angle.has_value())
                _current_min_angles.push_back(_n->transform().rotation().x() + _min_angle.value());
            else
                _current_min_angles.push_back(nullptr);
            
            if(_max_angle.has_value())
                _current_max_angles.push_back(_n->transform().rotation().x() + _max_angle.value());
            else
                _current_max_angles.push_back(nullptr);
        }
        
        _current_asleep_start = _asleep_duration_start;
        _saved_paused = _paused;
    }
}

void rotation_animator_component::update(float dt){
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
    
    size_t _index = 0;
    for (auto _n : _nodes){
        if(_current_asleep_endpoint[_index] > 0){
            _current_asleep_endpoint[_index] -= dt;
            if(_current_asleep_endpoint[_index] <= 0){
                _current_awake[_index] = _awake_duration;
                _current_asleep_endpoint[_index] = 0;
            }
            _index++;
            continue;
        }
        
        if(_current_asleep[_index] > 0){
            _current_asleep[_index] -= dt;
            if(_current_asleep[_index] <= 0){
                _current_awake[_index] = _awake_duration;
                _current_asleep[_index] = 0;
            }
            _index++;
            continue;
        }
        
        auto _t = _n->transform();
        _t.rotate_by(vec2(_r_velocity * dt * _directions[_index], _r_velocity * dt * _directions[_index]));
        
        if(_min_angle.has_value() && _t.rotation().x() < _current_min_angles[_index].value()){
            _t = _t.rotated(_current_min_angles[_index].value(), _current_min_angles[_index].value() + M_PI_2);
            if(_change_dir_endpoint)
                _directions[_index] *= -1;
            _current_asleep_endpoint[_index] = _asleep_duration_endpoint;
        }
        if(_max_angle.has_value() && _t.rotation().x() > _current_max_angles[_index].value()){
            _t = _t.rotated(_current_max_angles[_index].value(), _current_max_angles[_index].value() + M_PI_2);
            if(_change_dir_endpoint)
                _directions[_index] *= -1;
            _current_asleep_endpoint[_index] = _asleep_duration_endpoint;
        }
        
        _n->transform(_t);
        if(_current_awake[_index] > 0){
            _current_awake[_index] -= dt;
            if(_current_awake[_index] <= 0){
                _current_asleep[_index] = _asleep_duration;
                _current_awake[_index] = 0;
            }
        }
        
        _index++;
    }
}

void rotation_animator_component::reset_animation(bool initial_delay){
    for (size_t i = 0; i < _nodes.size(); i++) {
        _current_asleep[i] = 0;
        _current_awake[i] = _awake_duration;
        _current_asleep_endpoint[i] = 0;
    }
    if(initial_delay)
        _current_asleep_start = _asleep_duration_start;
}

bool rotation_animator_component::paused_animation() const {
    return _paused;
}

bool rotation_animator_component::paused_animation(bool value){
    _paused = value;
    return _paused;
}

const rb_string& rotation_animator_component::animated_class() const {
    return _class;
}

const rb_string& rotation_animator_component::animated_class(const rb_string &value){
    assert(!is_playing());
    _class = value;
    return _class;
}

float rotation_animator_component::asleep_duration() const{
    return _asleep_duration;
}

float rotation_animator_component::asleep_duration(float value){
    _asleep_duration = value;
    return _asleep_duration;
}

float rotation_animator_component::awake_duration() const {
    return _awake_duration;
}

float rotation_animator_component::awake_duration(float value){
    _awake_duration = value;
    return _awake_duration;
}

void rotation_animator_component::describe_type() {
    nvnode::describe_type();
    start_type<rotation_animator_component>([]() { return new rotation_animator_component(); });
    single_property<rotation_animator_component>(u"rotation_velocity", u"Rot Vel", true, {
        [](const rotation_animator_component* site){
            return TO_DEGREES(site->_r_velocity);
        },
        [](rotation_animator_component* site, float value){
            site->_r_velocity = TO_RADIANS(value);
        }
    });
    nullable_single_property<rotation_animator_component>(u"min_angle", u"Min Angle", true, {
        [](const rotation_animator_component* site){
            auto _r = site->min_angle();
            if(_r.has_value())
                return (nullable<float>)TO_DEGREES(_r.value());
            else
                return _r;
        },
        [](rotation_animator_component* site, const nullable<float>& value){
            if(value.has_value())
                site->min_angle(TO_RADIANS(value.value()));
            else
                site->min_angle(nullptr);
        }
    });
    nullable_single_property<rotation_animator_component>(u"max_angle", u"Max Angle", true, {
        [](const rotation_animator_component* site){
            auto _r = site->max_angle();
            if(_r.has_value())
                return (nullable<float>)TO_DEGREES(_r.value());
            else
                return _r;
        },
        [](rotation_animator_component* site, const nullable<float>& value){
            if(value.has_value())
                site->max_angle(TO_RADIANS(value.value()));
            else
                site->max_angle(nullptr);
        }
    });
    string_property<rotation_animator_component>(u"animated_class", u"Anim Class", true, false, {
        [](const rotation_animator_component* site){
            return site->animated_class();
        },
        [](rotation_animator_component* site, const rb_string& value){
            site->animated_class(value);
        }
    });
    boolean_property<rotation_animator_component>(u"paused_animation",u"Paused", true, {
        [](const rotation_animator_component* site){
            return site->paused_animation();
        },
        [](rotation_animator_component* site, bool value){
            site->paused_animation(value);
        }
    });
    boolean_property<rotation_animator_component>(u"change_dir_endpoint",u"Change Dir", true, {
        [](const rotation_animator_component* site){
            return site->change_dir_endpoint();
        },
        [](rotation_animator_component* site, bool value){
            site->change_dir_endpoint(value);
        }
    });
    single_property<rotation_animator_component>(u"asleep_duration", u"Asleep", true, {
        [](const rotation_animator_component* site){
            return site->asleep_duration();
        },
        [](rotation_animator_component* site, float value){
            site->asleep_duration(value);
        }
    });
    single_property<rotation_animator_component>(u"awake_duration", u"Awake", true, {
        [](const rotation_animator_component* site){
            return site->awake_duration();
        },
        [](rotation_animator_component* site, float value){
            site->awake_duration(value);
        }
    });
    single_property<rotation_animator_component>(u"initial_asleep_duration", u"St Asleep", true, {
        [](const rotation_animator_component* site){
            return site->start_asleep_duration();
        },
        [](rotation_animator_component* site, float value){
            site->start_asleep_duration(value);
        }
    });
    single_property<rotation_animator_component>(u"endpoint_asleep_duration", u"End Asleep", true, {
        [](const rotation_animator_component* site){
            return site->endpoint_asleep_duration();
        },
        [](rotation_animator_component* site, float value){
            site->endpoint_asleep_duration(value);
        }
    });
    end_type();
}

void rotation_animator_component::after_becoming_active(bool node_was_moved){
    register_for(registrable_event::update, ANIMATOR_UPDATE_PRIORITY);
}

rb_string rotation_animator_component::type_name() const {
    return u"rb::rotation_animator_component";
}

rb_string rotation_animator_component::displayable_type_name() const {
    return u"Rotation Animator";
}

float rotation_animator_component::rotation_velocity() const {
    return _r_velocity;
}

float rotation_animator_component::rotation_velocity(float value){
    return _r_velocity = value;
}

const nullable<float>& rotation_animator_component::min_angle() const {
    return _min_angle;
}

const nullable<float>& rotation_animator_component::min_angle(const nullable<float> &value){
    return _min_angle = value;
}

const nullable<float>& rotation_animator_component::max_angle() const {
    return _max_angle;
}

const nullable<float>& rotation_animator_component::max_angle(const nullable<float> &value){
    return _max_angle = value;
}

bool rotation_animator_component::change_dir_endpoint() const {
    return _change_dir_endpoint;
}

bool rotation_animator_component::change_dir_endpoint(bool value){
    return _change_dir_endpoint = value;
}

float rotation_animator_component::start_asleep_duration() const {
    return _asleep_duration_start;
}

float rotation_animator_component::start_asleep_duration(float value){
    return _asleep_duration_start = value;
}

float rotation_animator_component::endpoint_asleep_duration() const {
    return _asleep_duration_endpoint;
}

float rotation_animator_component::endpoint_asleep_duration(float value){
    return _asleep_duration_endpoint = value;
}

void rotation_animator_component::do_action(const rb_string& action_name, const rb_string& arg){
    if(action_name == u"play")
        paused_animation(false);
    else if(action_name == u"pause")
        paused_animation(true);
}
















