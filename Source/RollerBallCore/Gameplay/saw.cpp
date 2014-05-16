//
//  saw.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 05/01/14.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#include "saw.h"
#include "sprite_component.h"
#include "physics_engine.h"
#include "scene.h"
#include "physics_mask.h"
#include "animation_manager_component.h"

using namespace rb;

saw::saw(){
    _initialized = false;
    _animatable = false;
    _destructible = false;
    _destroyed = false;
    _sprite = new sprite_component();
    _sprite->classes(u"saw");
    _sprite->image_name(u"saw");
    _sprite->tint(color::from_rgba(1, 1, 1, 1));
    add_node(_sprite);
}

bool saw::should_serialize_children() const {
    return false;
}

void saw::describe_type(){
    node::describe_type();
    start_type<saw>([]() { return new saw(); });
    image_property<saw>(u"image_name", u"Image", true, {
        [](const saw* site){
            return site->_sprite->image_name();
        },
        [](saw* site, const rb_string& value){
            site->_sprite->image_name(value);
        }
    });
    boolean_property<saw>(u"animatable", u"Animatable", true, {
        [](const saw* site){
            return site->animatable();
        },
        [](saw* site, const bool value){
            site->animatable(value);
        }
    });
    boolean_property<saw>(u"destructible", u"Destructible", true, {
        [](const saw* site){
            return site->destructible();
        },
        [](saw* site, const bool value){
            site->destructible(value);
        }
    });
    end_type();
}

bool saw::animatable() const {
    return _animatable;
}

void saw::after_becoming_active(bool node_was_moved){
    if(_animatable)
        register_for(registrable_event::update, PHYS_OBJECT_UPDATE_PRIORITY);
    else
        unregister_for(registrable_event::update);
}

bool saw::animatable(bool value){
    _animatable = value;
    if(active()){
        if(_animatable)
            register_for(registrable_event::update, PHYS_OBJECT_UPDATE_PRIORITY);
        else
            unregister_for(registrable_event::update);
    }
    return _animatable;
}

void saw::reset_component(){
    transform(_saved_transform);
    auto _t = this->from_node_space_to(space::layer);
    _body->SetTransform(b2Vec2(_t.origin().x(), _t.origin().y()), _t.rotation().x());
}

void saw::reset_physics(){
    auto _t = this->from_node_space_to(space::layer);
    _body->SetTransform(b2Vec2(_t.origin().x(), _t.origin().y()), _t.rotation().x());
}

void saw::update(float dt){
    if(!_initialized)
        return;
    _body->SetLinearVelocity(b2Vec2(0, 0));
    _body->SetAngularVelocity(0);
    auto _t = this->from_node_space_to(space::layer);
    _body->SetTransform(b2Vec2(_t.origin().x(), _t.origin().y()), 0);
}

rb_string saw::type_name() const {
    return u"rb::saw";
}

rb_string saw::displayable_type_name() const {
    return u"Saw";
}

void saw::playing(){
    if(!_initialized){
        _initialized = true;
        _an_manager = dynamic_cast<animation_manager_component*>(parent_scene()->node_with_name(u"Animation Manager"));
        _saved_transform = transform();
        
        auto _engine = dynamic_cast<physics_engine*>(parent_scene()->node_with_name(u"Physic's Engine"));
        _world = _engine->world();
        auto _t = this->from_node_space_to(space::layer);
        b2BodyDef _bDef;
        _bDef.active = true;
        _bDef.allowSleep = false;
        _bDef.angle = _t.rotation().x();
        _bDef.awake = true;
        _bDef.position = b2Vec2(_t.origin().x(), _t.origin().y());
        _bDef.type = b2_kinematicBody;
        _bDef.userData = (node*)this;
        _body = _world->CreateBody(&_bDef);
        
        b2FixtureDef _fDef;
        _fDef.isSensor = true;
        _fDef.filter.categoryBits = PHYS_MASK_ENEMY;
        _fDef.filter.maskBits = PHYS_MASK_CHARACTER | PHYS_BREAKABLE_BLOCK;
        b2CircleShape _c;
        _c.m_radius = _t.scale().x() / 2;
        _fDef.shape = &_c;
        _body->CreateFixture(&_fDef);
        
        if(_animatable)
            register_for(registrable_event::update, PHYS_OBJECT_UPDATE_PRIORITY);
        else
            unregister_for(registrable_event::update);
    }
}

void saw::do_action(const rb_string& action_name, const rb_string& arg){
    
}

bool saw::destructible() const {
    return _destructible;
}

bool saw::destructible(bool value){
    return _destructible = value;
}

bool saw::destroyed() const {
    return _destroyed;
}

void saw::destroy_saw(){
    if(_destroyed)
        return;
    _destroyed = true;
    animation_info ai;
    ai.auto_destroy = true;
    ai.duration = 0.5f;
    auto _saved_scale = this->_sprite->transform().scale();
    ai.update_function = [this, _saved_scale](float t, animation_info* ai){
        this->_sprite->blend(1 - t);
        this->_sprite->transform(this->_sprite->transform().scaled(_saved_scale * (1 - t)));
    };
    auto _aidd = _an_manager->add_animation(&ai);
    _an_manager->animation(_aidd)->state = animation_state::playing;
}























