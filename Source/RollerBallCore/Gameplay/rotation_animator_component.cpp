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
    _paused = false;
    _current_asleep = 0;
    _current_awake = 0;
    _saved_paused = false;
}

void rotation_animator_component::reset_component(){
    for (size_t i = 0; i < _nodes.size(); i++) {
        _nodes[i]->transform(_saved_transforms[i]);
    }
    _current_asleep = _asleep_duration;
    _current_awake = _awake_duration;
    _paused = _saved_paused;
}

void rotation_animator_component::playing(){
    if(_nodes.size() == 0){
        _nodes = parent_scene()->node_with_one_class(_class);
        for (auto _n : _nodes)
            _saved_transforms.push_back(_n->transform());
        
        _current_asleep = _asleep_duration;
        _current_awake = _awake_duration;
        _saved_paused = _paused;
    }
}

void rotation_animator_component::update(float dt){
    if(_paused)
        return;
    
    if(_current_asleep > 0){
        _current_asleep -= dt;
        if(_current_asleep <= 0){
            _current_awake = _awake_duration;
            _current_asleep = 0;
        }
        return;
    }
    
    for (auto _n : _nodes){
        auto _t = _n->transform();
        _t.rotate_by(vec2(_r_velocity * dt, _r_velocity * dt));
        _n->transform(_t);
    }
    
    if(_current_awake > 0){
        _current_awake -= dt;
        if(_current_awake <= 0){
            _current_asleep = _asleep_duration;
            _current_awake = 0;
        }
    }
}

void rotation_animator_component::reset_animation(){
    _current_asleep = _asleep_duration;
    _current_awake = _awake_duration;
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



















