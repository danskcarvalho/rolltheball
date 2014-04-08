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

using namespace rb;

saw::saw(){
    _initialized = false;
    _animatable = false;
    _sprite = new sprite_component();
    _sprite->classes(u"saw");
    _sprite->image_name(u"saw");
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
    _body->SetTransform(b2Vec2(_saved_transform.origin().x(), _saved_transform.origin().y()), _saved_transform.rotation().x());
    _before = nullptr;
}

void saw::update(float dt){
    if(!_initialized)
        return;
    
    if(!_before.has_value())
    {
        auto _t = this->from_node_space_to(space::layer);
        _before = _t;
        _body->SetTransform(b2Vec2(_t.origin().x(), _t.origin().y()), _t.rotation().x());
    }
    else {
        auto _t = this->from_node_space_to(space::layer);
        auto _v = _t.origin() - _before.value().origin();
        auto _o = _t.rotation().x() - _before.value().rotation().x();
        
        _v *= 30.0f;
        _o *= 30.0f;
        
        _body->SetLinearVelocity(b2Vec2(_v.x(), _v.y()));
        _body->SetAngularVelocity(_o);
        _before = _t;
    }
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























