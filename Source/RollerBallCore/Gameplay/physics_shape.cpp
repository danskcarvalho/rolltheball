//
//  physics_shape.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 16/08/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#include "physics_shape.h"
#include "physics_engine.h"
#include "scene.h"
#include "physics_mask.h"
#include <Box2D/Box2D.h>

using namespace rb;

physics_shape::physics_shape(){
    _world = nullptr;
    _phys_initialized = false;
    _body = nullptr;
    _gravity = 10;
    _type = kStaticPlanet;
    _planet_name = u"";
    _planet = nullptr;
}

physics_shape::~physics_shape(){
    
}

void physics_shape::describe_type(){
    polygon_component::describe_type();
    
    start_type<physics_shape>([](){ return new physics_shape(); });
    enumeration_property<physics_shape, type>(u"shape_type", u"Type", {{u"Static Planet", kStaticPlanet}, {u"Static Gravity Zone", kStaticGravityZone}}, true, {
        [](const physics_shape* site){
            return site->_type;
        },
        [](physics_shape* site, type value){
            site->_type = value;
        }
    });
    single_property<physics_shape>(u"gravity", u"Gravity", true, {
        [](const physics_shape* site){
            return site->_gravity;
        },
        [](physics_shape* site, float value){
            site->_gravity = value;
        }
    });
    string_property<physics_shape>(u"planet_name", u"Planet", true, true, {
        [](const physics_shape* site){
            return site->_planet_name;
        },
        [](physics_shape* site, const rb_string& value){
            site->_planet_name = value;
        }
    });
    end_type();
}

void physics_shape::after_becoming_active(bool node_was_moved){
    polygon_component::after_becoming_active(node_was_moved);
}

void physics_shape::before_becoming_inactive(bool node_was_moved){
    polygon_component::before_becoming_inactive(node_was_moved);
}

rb_string physics_shape::type_name() const {
    return u"rb::physics_shape";
}

rb_string physics_shape::displayable_type_name() const {
    return u"Physic's Shape";
}

void physics_shape::playing() {
    if(!_phys_initialized){
        _phys_initialized = true;
        auto _t = transform();
        auto _engine = dynamic_cast<physics_engine*>(parent_scene()->node_with_name(u"Physic's Engine"));
        _planet = dynamic_cast<physics_shape*>(parent_scene()->node_with_name(_planet_name));
        _world = _engine->world();
        
        b2BodyDef _bDef;
        _bDef.active = true;
        _bDef.angle = 0;
        _bDef.linearDamping = 0.0f;
        _bDef.angularDamping = 0.1f;
        _bDef.position = b2Vec2(_t.origin().x(), _t.origin().y());
        _bDef.type = b2_staticBody;
        _bDef.userData = (node*)this;
        _body = _world->CreateBody(&_bDef);
        
        auto _new_t = transform_space(_t.origin(), vec2(1, 1), 0);
        
        b2FixtureDef _fDef;
        _fDef.isSensor = _type == kStaticGravityZone;
        _fDef.friction = 1;
        _fDef.restitution = 0;
        _fDef.filter.categoryBits = _type == kStaticPlanet ? PHYS_MASK_SHAPE : PHYS_MASK_GRAVITY_REGION;
        _fDef.filter.maskBits = PHYS_MASK_CHARACTER;
        if(_type == kStaticPlanet){
            b2ChainShape _c;
            b2Vec2* _v = new b2Vec2[node_count()];
            for (uint32_t i = 0; i < node_count(); i++) {
                auto _p = _t.from_space_to_base().transformed_point(node_at(i)->transform().origin());
                _p = _new_t.from_base_to_space().transformed_point(_p);
                _v[i] = b2Vec2(_p.x(), _p.y());
            }
            _c.CreateLoop(_v, node_count());
            _fDef.shape = &_c;
            _body->CreateFixture(&_fDef);
        }
        else {
            b2PolygonShape _c;
            b2Vec2* _v = new b2Vec2[node_count()];
            for (uint32_t i = 0; i < node_count(); i++) {
                auto _p = _t.from_space_to_base().transformed_point(node_at(i)->transform().origin());
                _p = _new_t.from_base_to_space().transformed_point(_p);
                _v[i] = b2Vec2(_p.x(), _p.y());
            }
            _c.Set(_v, node_count());
            _fDef.shape = &_c;
            _body->CreateFixture(&_fDef);
        }
    }
}

physics_shape::type physics_shape::shape_type() const {
    return _type;
}

physics_shape::type physics_shape::shape_type(const rb::physics_shape::type value){
    _type = value;
    return _type;
}

float physics_shape::gravity() const {
    return _gravity;
}

float physics_shape::gravity(const float value){
    _gravity = value;
    return _gravity;
}

vec2 physics_shape::gravity_vector(const rb::vec2 &position){
    auto _pol = to_polygon();
    transform().from_space_to_base().transform_polygon(_pol); //we will cache this thing...
    
    auto _e = _pol.closest_edge(position).normal();
    _e.normalize();
    return _e * (-_gravity);
}

const rb_string& physics_shape::planet_name() const {
    return _planet_name;
}

const rb_string& physics_shape::planet_name(const rb_string &value){
    _planet_name = value;
    return _planet_name;
}

physics_shape* physics_shape::planet() const {
    return _planet;
}
































