//
//  main_character.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 13/08/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#include "main_character.h"
#include "sprite_component.h"
#include "physics_mask.h"
#include "physics_engine.h"
#include "scene.h"
#include "physics_shape.h"
#include <Box2D/Box2D.h>

//Some constants
#define PHYS_CHARACTER_VELOCITY 5.5f
#define PHYS_CHARACTER_ACCEL (PHYS_CHARACTER_VELOCITY / (0.25f * 30.0f))

using namespace rb;

main_character::main_character(){
    _body = nullptr;
    _world = nullptr;
    _phys_initialized = false;
    _sprite = new sprite_component();
    add_node(_sprite);
    name(u"Main Character");
    _direction = 0;
    _previous_g = nullptr;
    _damping = 0.9;
}

main_character::~main_character(){
}

void main_character::describe_type(){
    node::describe_type();
    
    start_type<main_character>([](){ return new main_character(); });
    image_property<main_character>(u"image_name", u"Image", true, {
        [](const main_character* site){
            return site->_sprite->image_name();
        },
        [](main_character* site, const rb_string& value){
            site->_sprite->image_name(value);
        }
    });
    single_property<main_character>(u"damping", u"Damping", true, {
        [](const main_character* site){
            return site->_damping;
        },
        [](main_character* site, const float value){
            site->damping(value);
        }
    });
    end_type();
}

rb_string main_character::type_name() const{
    return u"rb::main_character";
}

rb_string main_character::displayable_type_name() const {
    return u"Main Character";
}

void main_character::after_becoming_active(bool node_was_moved){
    register_for(registrable_event::update, 0);
    register_for(registrable_event::keyboard_events, 0);
    enabled(node_capability::can_become_current, false);
}

void main_character::before_becoming_inactive(bool node_was_moved){
    
}

inline float sgn(float v){
    if(v == 0)
        return 0;
    return v / fabsf(v);
}

inline float signed_length(const vec2& v, const vec2& axis){
    return v.length() * sgn(vec2::dot(v, axis));
}

void main_character::update(float dt){
    auto _g = vec2::zero;
    for (b2ContactEdge* ce = _body->GetContactList(); ce; ce = ce->next) {
        auto _other = dynamic_cast<physics_shape*>((node*)ce->other->GetUserData());
        if(_other && _other->shape_type() == physics_shape::kStaticGravityZone && ce->contact->IsTouching() && ce->contact->IsEnabled()){
            _g = _other->planet()->gravity_vector(transform().origin());
        }
    }
    
    auto _gy = vec2::zero;
    auto _gx = vec2::zero;
    if(_g != vec2::zero){
        _gy = _g.normalized();
        _gx = _gy.rotated90(rotation_direction::ccw);
    }
    vec2 _v;
    vec2 _vx;
    vec2 _vy;
    
    if(_g != vec2::zero){
        //we get the velocity
        _v = vec2(_body->GetLinearVelocity().x, _body->GetLinearVelocity().y);
        
        if (_previous_g.has_value()){
            auto _pgy = _previous_g.value().normalized();
            auto _pgx = _pgy.rotated90(rotation_direction::ccw);
            _vx = _v.projection(_pgx);
            _vy = _v.projection(_pgy);
            _vx = _gx * signed_length(_vx, _pgx);
            _vy = _gy * signed_length(_vy, _pgy);
            _v = _vx + _vy;
        }
        else {
            _vx = _v.projection(_gx);
            _vy = _v.projection(_gy);
        }
        
        _previous_g = _g;
    }
    else {
        _v = vec2(_body->GetLinearVelocity().x, _body->GetLinearVelocity().y);
        _vx = vec2(_v.x(), 0);
        _vy = vec2(0, _v.y());
        _previous_g = nullptr;
    }
    
    if(_g != vec2::zero){
        auto _lx = signed_length(_vx, _gx);
        if(_direction > 0) { //to right
            if(_lx < PHYS_CHARACTER_VELOCITY)
            {
                _vx += _gx * PHYS_CHARACTER_ACCEL;
                _v = _vx + _vy;
            }
        }
        else if(_direction < 0) { //to left
            if(_lx > (-PHYS_CHARACTER_VELOCITY)){
                _vx -= _gx * PHYS_CHARACTER_ACCEL;
                _v = _vx + _vy;
            }
        }
        else {
            _vx *= _damping;
            _v = _vx + _vy;
        }
    }
    
    _body->SetLinearVelocity(b2Vec2(_v.x(), _v.y()));
    auto _fg = _g * _body->GetMass();
    _body->ApplyForceToCenter(b2Vec2(_fg.x(), _fg.y()));
    
    this->transform(this->transform().moved(_body->GetPosition().x, _body->GetPosition().y).rotated(_body->GetAngle(), _body->GetAngle() + M_PI_2));
}

bool main_character::should_serialize_children() const {
    return false;
}

void main_character::playing(){
    if(!_phys_initialized){
        auto _t = transform();
        _phys_initialized = true;
        auto _engine = dynamic_cast<physics_engine*>(parent_scene()->node_with_name(u"Physic's Engine"));
        _world = _engine->world();
        b2BodyDef _bDef;
        _bDef.active = true;
        _bDef.allowSleep = false;
        _bDef.angle = _t.rotation().x();
        _bDef.linearDamping = 0.0f;
        _bDef.angularDamping = 0.1f;
        _bDef.awake = true;
        _bDef.position = b2Vec2(_t.origin().x(), _t.origin().y());
        _bDef.type = b2_dynamicBody;
        _bDef.userData = (node*)this;
        _body = _world->CreateBody(&_bDef);
        
        b2FixtureDef _fDef;
        _fDef.friction = 1;
        _fDef.restitution = 0;
        _fDef.density = 8050;
        _fDef.filter.categoryBits = PHYS_MASK_CHARACTER;
        _fDef.filter.maskBits = PHYS_MASK_GRAVITY_REGION | PHYS_MASK_SHAPE;
        b2CircleShape _c;
        _c.m_radius = 0.5;
        _fDef.shape = &_c;
        _body->CreateFixture(&_fDef);
    }
}

const rb_string& main_character::image_name() const {
    return _sprite->image_name();
}

const rb_string& main_character::image_name(const rb_string &value){
    return _sprite->image_name(value);
}

void main_character::keydown(const uint32_t keycode, const rb::keyboard_modifier modifier, bool &swallow){
    swallow = false;
    //TODO: Improve this...
    if(keycode == KEY_LEFT)
        _direction = -1;
    else if(keycode == KEY_RIGHT)
        _direction = 1;
}

void main_character::keyup(const uint32_t keycode, const rb::keyboard_modifier modifier, bool &swallow){
    swallow = false;
    _direction = 0;
}

float main_character::damping() const {
    return _damping;
}

float main_character::damping(const float value) {
    return _damping = value;
}








































