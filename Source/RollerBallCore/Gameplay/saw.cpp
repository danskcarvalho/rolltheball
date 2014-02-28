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
    _sprite = new sprite_component();
    _sprite->classes(u"saw");
    _sprite->image_name(u"saw");
    add_node(_sprite);
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
    end_type();
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
        _fDef.filter.maskBits = PHYS_MASK_CHARACTER;
        b2CircleShape _c;
        _c.m_radius = _t.scale().x() / 2;
        _fDef.shape = &_c;
        _body->CreateFixture(&_fDef);
    }
}






















