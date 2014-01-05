//
//  animation_manager_component.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 01/01/14.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#include "animation_manager_component.h"
#include "animation_function.h"

using namespace rb;

animation_info::animation_info(){
    initial_delay = 0;
    delay = 0;
    duration = 0;
    state = animation_state::stopped;
    loop_count = 0;
    mirror = false;
    auto_destroy = true;
    time_function = nullptr;
    user_data = nullptr;
}

animation_manager_component::ex_animation_info::ex_animation_info(){
    initial_delay_remaining = 0;
    delay_remaining = 0;
    duration_remaining = 0;
    mirroring = false;
    current_loop_count = 0;
}

animation_manager_component::ex_animation_info::~ex_animation_info(){
    if(ai.time_function)
        delete ai.time_function;
    if(ai.destroy_function)
        ai.destroy_function(&ai);
}

animation_manager_component::animation_manager_component(){
    classes(u"animationManager");
    name(u"Animation Manager");
    _iterator_changed = false;
}

animation_manager_component::~animation_manager_component(){
    for (auto _ai : _animations)
        delete _ai;
}

rb_string animation_manager_component::type_name() const {
    return u"rb::animation_manager_component";
}

rb_string animation_manager_component::displayable_type_name() const {
    return u"Animation Manager";
}

void animation_manager_component::describe_type(){
    nvnode::describe_type();
    start_type<animation_manager_component>([]() { return new animation_manager_component(); });
    end_type();
}

void animation_manager_component::after_becoming_active(bool node_was_moved){
    register_for(registrable_event::update, 0);
}

void animation_manager_component::before_becoming_inactive(bool node_was_moved){
    
}

animation_id animation_manager_component::add_animation(rb::animation_info *ai){
    assert(ai);
    auto _exi = new ex_animation_info();
    _exi->ai = *ai;
    _animations.push_back(_exi);
    reset_animation(_exi);
    return _exi;
}

void animation_manager_component::destroy_animation(animation_id aid){
    assert(aid);
    auto _it = _animations.begin();
    auto _end = _animations.end();
    bool _erased = false;
    while (_it != _end) {
        if(*_it == aid){
            if(_it == _current_iterator){
                _it = _current_iterator = _animations.erase(_it);
                _iterator_changed = true;
            }
            else
                _it = _animations.erase(_it);
            _erased = true;
            break;
        }
        else
            _it++;
    }
    assert(_erased);
    delete (ex_animation_info*)aid;
}

animation_info* animation_manager_component::animation(animation_id aid){
    assert(aid);
    return &((ex_animation_info*)aid)->ai;
}

void animation_manager_component::reset_animation(animation_id aid){
    ex_animation_info* eii = (ex_animation_info*)aid;
    eii->initial_delay_remaining = eii->ai.initial_delay;
    eii->delay_remaining = eii->ai.delay;
    eii->duration_remaining = eii->ai.duration;
    eii->mirroring = false;
    eii->current_loop_count = 0;
}

void animation_manager_component::update(float dt){
    _current_iterator = _animations.begin();
    auto _end = _animations.end();
    _iterator_changed = false;
    while (_current_iterator != _end) {
        run_animation(*_current_iterator, dt);
        if(!_iterator_changed)
            _current_iterator++;
        _iterator_changed = false;
    }
}

void animation_manager_component::run_animation(rb::animation_manager_component::ex_animation_info *ai, float dt){
    
    if(ai->ai.state != animation_state::playing)
        return;
    
    if(ai->initial_delay_remaining > 0){
        ai->initial_delay_remaining -= dt;
        if(ai->initial_delay_remaining <= 0)
            ai->initial_delay_remaining = 0;
        return;
    }
    
    if(ai->delay_remaining > 0){
        ai->delay_remaining -= dt;
        if(ai->delay_remaining <= 0)
            ai->delay_remaining = 0;
        return;
    }
    
    if(ai->duration_remaining <= 0)
        return;
    
    ai->duration_remaining -= dt;
    
    if(ai->duration_remaining <= 0)
        ai->duration_remaining = 0;
    
    float _t = ai->duration_remaining / ai->ai.duration;
    if(!ai->mirroring)
        _t = 1 - _t;
    
    ai->ai.update_function(ai->ai.time_function ? (*ai->ai.time_function)(_t) : _t, &ai->ai);
    
    bool _ended = false;
    bool _reset = false;
    
    if(ai->duration_remaining <= 0){
        if (ai->ai.loop_count != 0){
            if(ai->ai.loop_count > 0){ //No Infinite Loop
                ai->current_loop_count++;
                if(ai->current_loop_count == ai->ai.loop_count){
                    _ended = true;
                }
                else {
                    _reset = true;
                }
            }
            else
                _reset = true;
        }
        else
            _ended = true;
    }
    
    
    if (_ended && ai->ai.auto_destroy){
        destroy_animation(ai);
        return;
    }
    if(_reset){
        ai->delay_remaining = ai->ai.delay;
        ai->duration_remaining = ai->ai.duration;
        if(ai->ai.mirror)
            ai->mirroring = !ai->mirroring;
    }
    else {
        if(ai->duration_remaining == 0)
            ai->ai.state = animation_state::stopped;
    }
}


























