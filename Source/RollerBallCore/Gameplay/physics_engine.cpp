//
//  physics_engine.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 13/08/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#include "physics_engine.h"
#include <Box2D/Box2D.h>

using namespace rb;

physics_engine::physics_engine(){
    _world = nullptr;
    classes(u"physEngine");
    name(u"Physic's Engine");
    _initialized = false;
    _saved_gravity = vec2::zero;
}

physics_engine::~physics_engine(){
    if(_world)
        delete _world;
    _world = nullptr;
}

void physics_engine::describe_type(){
    nvnode::describe_type();
    start_type<physics_engine>([](){ return new physics_engine(); });
    vec2_property<physics_engine>(u"default_gravity", u"Def Gravity", true, {
        [](const physics_engine* site){
            return site->_default_gravity;
        },
        [](physics_engine* site, const vec2& value){
            site->_default_gravity = value;
        }
    });
    end_type();
}

rb_string physics_engine::type_name() const {
    return u"rb::physics_engine";
}

rb_string physics_engine::displayable_type_name() const {
    return u"Physics Engine";
}

void physics_engine::after_becoming_active(bool node_was_moved){
    if(!_world){
        _world = new b2World(b2Vec2(0, 0));
        _world->SetAutoClearForces(false);
    }
    register_for(registrable_event::update, PHYS_ENGINE_UPDATE_PRIORITY);
}

void physics_engine::before_becoming_inactive(bool node_was_moved){
    if(_world)
        delete _world;
    _world = nullptr;
}

b2World* physics_engine::world() const {
    if(_world == nullptr){
        const_cast<physics_engine*>(this)->_world = new b2World(b2Vec2(0, 0));
        _world->SetAutoClearForces(false);
    }
    return _world;
}

void physics_engine::update(float dt){
    for (size_t i = 0; i < PHYS_ENGINE_STEPS_PER_FRAME; i++) {
        if(_world)
            _world->Step(PHYS_ENGINE_TIME_STEP, 6, 2);
    }
    _world->ClearForces();
}

const vec2& physics_engine::default_gravity() const {
    return _default_gravity;
}

const vec2& physics_engine::default_gravity(const rb::vec2 &value){
    return _default_gravity = value;
}

void physics_engine::do_action(const rb_string& action_name, const rb_string& arg){
    if(action_name == u"changeGravityY"){
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert; // ... filled in with a codecvt to do UTF-8 <-> UTF-16
        std::string utf8_string = convert.to_bytes(arg);
        
        float _value;
        std::stringstream ss(utf8_string);
        ss >> _value;
        _default_gravity = vec2(_default_gravity.x(), _value);
    }
    else if(action_name == u"changeGravityX"){
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert; // ... filled in with a codecvt to do UTF-8 <-> UTF-16
        std::string utf8_string = convert.to_bytes(arg);
        
        float _value;
        std::stringstream ss(utf8_string);
        ss >> _value;
        _default_gravity = vec2(_value, _default_gravity.y());
    }
}

void physics_engine::reset_component(){
    _default_gravity = _saved_gravity;
}

void physics_engine::playing(){
    if(!_initialized){
        _initialized = true;
        _saved_gravity = _default_gravity;
    }
}










































