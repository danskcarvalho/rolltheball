//
//  saw_rotation_animator.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 05/01/14.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#include "saw_rotation_animator_component.h"
#include "scene.h"

using namespace rb;

saw_rotation_animator_component::saw_rotation_animator_component(){
    _r_velocity = M_PI / 2;
}

void saw_rotation_animator_component::update(float dt){
    if(_nodes.size() == 0){
        _nodes = parent_scene()->node_with_one_class(u"saw");
    }
    
    for (auto _n : _nodes){
        auto _t = _n->transform();
        _t.rotate_by(vec2(_r_velocity * dt, _r_velocity * dt));
        _n->transform(_t);
    }
}

void saw_rotation_animator_component::describe_type() {
    nvnode::describe_type();
    start_type<saw_rotation_animator_component>([]() { return new saw_rotation_animator_component(); });
    single_property<saw_rotation_animator_component>(u"rotation_velocity", u"Rot Vel", true, {
        [](const saw_rotation_animator_component* site){
            return TO_DEGREES(site->_r_velocity);
        },
        [](saw_rotation_animator_component* site, float value){
            site->_r_velocity = TO_RADIANS(value);
        }
    });
    end_type();
}

void saw_rotation_animator_component::after_becoming_active(bool node_was_moved){
    register_for(registrable_event::update, 0);
}

rb_string saw_rotation_animator_component::type_name() const {
    return u"rb::saw_rotation_animator_component";
}

rb_string saw_rotation_animator_component::displayable_type_name() const {
    return u"Saw Rotation Animator";
}

float saw_rotation_animator_component::rotation_velocity() const {
    return _r_velocity;
}

float saw_rotation_animator_component::rotation_velocity(float value){
    return _r_velocity = value;
}



















