//
//  breakable_block.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 2014-04-06.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#include "breakable_block.h"
#include "destructible_sprite_component.h"
#include "animation_manager_component.h"
#include "main_character.h"
#include "saw.h"
#include "scene.h"
#include "physics_engine.h"
#include "physics_mask.h"
#include "action_buffer.h"
#include <random>
#include <Box2D/Box2D.h> 


#define DESTRUCTION_MATRIX 2
#define DESTRUCTION_ANIM_DURATION 1.0f
#define MAX_DESTRUCTION_VELOCITY 6
#define MIN_DESTRUCTION_VELOCITY 4

#define EXPLOSION_GRAVITY_MAGNITUDE 25 //I'm going to always assume normal (top-down) gravity

using namespace rb;

breakable_block::breakable_block(){
    _explode_character_contact = false;
    _can_kill_character = false;
    _an_manager = nullptr;
    _break_an = nullptr;
    _restore_an = nullptr;
    _last_t = 0;
    _broken = false;
    _saved_broken = false;
    _world = nullptr;
    _body = nullptr;
    _restoration_left = _restoration_time = 0;
    _restoration_enabled = false;
    _should_be_active = true;
    _no_reentrancy = false;
    //sprite
    _sprite = new destructible_sprite_component();
    _sprite->matrix(vec2(DESTRUCTION_MATRIX, DESTRUCTION_MATRIX));
    add_node(_sprite);
}

bool breakable_block::should_serialize_children() const {
    return false;
}

void breakable_block::generate_random_velocities(){
    _velocities.clear();
    auto _parts_count = DESTRUCTION_MATRIX * DESTRUCTION_MATRIX;
    std::mt19937 _generator((unsigned int)clock());
    std::uniform_real_distribution<float> _distribution(0.0f, 1.0f);
    
    for (size_t i = 0; i < _parts_count; i++) {
        auto _angle = (float)(_distribution(_generator) * 2 * M_PI);
        auto _v = _distribution(_generator);
        _v = (1 - _v) * MIN_DESTRUCTION_VELOCITY + _v * MAX_DESTRUCTION_VELOCITY;
        auto _vv = vec2::right.rotated(_angle) * _v;
        _velocities.push_back(_vv);
    }
}

void breakable_block::animate_break(float t){
    if(_last_t == 0){
        generate_random_velocities();
        for (uint32_t i = 0; i < DESTRUCTION_MATRIX; i++) {
            for (uint32_t j = 0; j < DESTRUCTION_MATRIX; j++) {
                _sprite->transform(i, j, matrix3x3::identity); //reset to identity
            }
        }
    }
    
    auto _dt = t - _last_t;
    _last_t = t;
    auto _down_vec = vec2::down;
    this->from_node_space_to(space::camera).inverse().transform_vector(_down_vec).normalize();
    auto _gravity = _down_vec * EXPLOSION_GRAVITY_MAGNITUDE;
    for (size_t i = 0; i < (DESTRUCTION_MATRIX * DESTRUCTION_MATRIX); i++) {
        _velocities[i] += _gravity * _dt;
    }
    for (uint32_t i = 0; i < DESTRUCTION_MATRIX; i++) {
        for (uint32_t j = 0; j < DESTRUCTION_MATRIX; j++) {
            auto _t = _sprite->transform(i, j);
            auto _v = _velocities[i + j * DESTRUCTION_MATRIX];
            _t.translation(_t.translation() + _v * _dt);
            _sprite->transform(i, j, _t);
        }
    }
    if(t > 0.5)
        _sprite->opacity(1 - 2 * (t - 0.5));
}

void breakable_block::animate_restore(float t){
    if(_last_t == 0){
        for (uint32_t i = 0; i < DESTRUCTION_MATRIX; i++) {
            for (uint32_t j = 0; j < DESTRUCTION_MATRIX; j++) {
                _sprite->transform(i, j, matrix3x3::identity); //reset to identity
            }
        }
        _sprite->node::old_transform(_sprite->node::old_transform().scaled(0.0001, 0.0001));
        _last_t = 1;
        return;
    }
    
    _sprite->opacity(2 * t);
    _sprite->node::old_transform(_sprite->node::old_transform().scaled(t, t));
    
    if(t >= 1){
        //check contact with user
        _should_be_active = true;
        if(!is_touching_main_character())
            _body->SetActive(true);
        _sprite->opacity(1);
        _sprite->node::old_transform(_sprite->node::old_transform().scaled(1, 1));
    }
}

void breakable_block::break_block(bool animation){
    if(_broken && animation)
        return;
    _sprite->node::old_transform(_sprite->node::old_transform().scaled(1));
    for (uint32_t i = 0; i < DESTRUCTION_MATRIX; i++) {
        for (uint32_t j = 0; j < DESTRUCTION_MATRIX; j++) {
            _sprite->transform(i, j, matrix3x3::identity); //reset to identity
        }
    }
    
    _restoration_left = _restoration_time;
    _last_t = 0;
    if(!animation){
        _an_manager->animation(_break_an)->state = animation_state::stopped;
        _an_manager->animation(_restore_an)->state = animation_state::stopped;
        _broken = true;
        _should_be_active = false;
        _body->SetActive(false);
        _sprite->visible(false);
        goto CallAction;
    }
    
    _broken = true;
    _an_manager->animation(_restore_an)->state = animation_state::stopped;
    _an_manager->reset_animation(_restore_an);
    _an_manager->reset_animation(_break_an);
    _an_manager->animation(_break_an)->state = animation_state::playing;
    _sprite->visible(true);
    _sprite->opacity(1);
    _should_be_active = false;
    _body->SetActive(false);
CallAction:
    if(!_no_reentrancy){
        if(_on_break_action_buffer != u""){
            auto _buffer = dynamic_cast<action_buffer*>(parent_scene()->node_with_name(_on_break_action_buffer));
            if(_buffer){
                _buffer->perform_action(_on_break_action_name);
            }
        }
    }
}

void breakable_block::restore_block(bool animation){
    if(!_broken  && animation)
        return;
    _sprite->node::old_transform(_sprite->node::old_transform().scaled(1));
    for (uint32_t i = 0; i < DESTRUCTION_MATRIX; i++) {
        for (uint32_t j = 0; j < DESTRUCTION_MATRIX; j++) {
            _sprite->transform(i, j, matrix3x3::identity); //reset to identity
        }
    }
    
    _last_t = 0;
    if(!animation){
        _an_manager->animation(_break_an)->state = animation_state::stopped;
        _an_manager->animation(_restore_an)->state = animation_state::stopped;
        _broken = false;
        _should_be_active = true;
        _body->SetActive(true);
        _sprite->visible(true);
        _sprite->opacity(1);
        goto CallAction;
    }
    
    _broken = false;
    _an_manager->animation(_restore_an)->state = animation_state::playing;
    _an_manager->reset_animation(_restore_an);
    _an_manager->reset_animation(_break_an);
    _an_manager->animation(_break_an)->state = animation_state::stopped;
    _sprite->visible(true);
    _sprite->opacity(0);
    _should_be_active = false;
    _body->SetActive(false);
CallAction:
    if(!_no_reentrancy){
        if(_on_restore_action_buffer != u""){
            auto _buffer = dynamic_cast<action_buffer*>(parent_scene()->node_with_name(_on_restore_action_buffer));
            if(_buffer){
                _buffer->perform_action(_on_restore_action_name);
            }
        }
    }
}

bool breakable_block::is_touching_main_character(){
    if(!_main_character)
        return false;
    auto _t = transform_space::from_matrix(from_node_space_to(space::layer));
    auto _myR = sqrtf(2 * _t.scale().x() * _t.scale().x());
    auto _minR = _t.scale().x() / 2.0f;
    auto _cR = _main_character->old_transform().scale().x() / 2.0f;
    auto _sumR = _myR + _cR;
    auto _distance = vec2::distance(_t.origin(), _main_character->old_transform().origin());
    auto _sumMinR = _minR + _cR;
    auto _penetration = _sumMinR - _distance;
    
    auto _res =  _distance <= _sumR;
    if(_res && _penetration >= (0.25f * _cR) && _can_kill_character)
        dynamic_cast<main_character*>(_main_character)->die();
    return _res;
}

bool breakable_block::is_touching_saw(){
    for (b2ContactEdge* ce = _body->GetContactList(); ce; ce = ce->next) {
        auto _other = dynamic_cast<saw*>((node*)ce->other->GetUserData());
        if(_other && !_other->destroyed() && ce->contact->IsTouching() && ce->contact->IsEnabled()){
            return true;
        }
    }
    return false;
}

void breakable_block::update(float dt){
    auto _t = transform_space::from_matrix(from_node_space_to(space::layer));
    _body->SetTransform(b2Vec2(_t.origin().x(), _t.origin().y()), _t.rotation().x());
    if(!_broken){ //not broken
        if(_should_be_active != _body->IsActive()){
            if(!is_touching_main_character())
                _body->SetActive(_should_be_active);
        }
        
        if(is_touching_saw())
            break_block(true);
    }
    else {
        if(_restoration_enabled){
            if(_restoration_left > 0){
                _restoration_left -= dt;
                if(_restoration_left <= 0){
                    restore_block(true);
                    _restoration_left = 0;
                }
            }
        }
    }
}

void breakable_block::after_becoming_active(bool node_was_moved){
    register_for(registrable_event::update, 0);
    enabled(node_capability::can_become_current, false);
}

void breakable_block::playing(){
    if(!_world){
        _main_character = parent_scene()->node_with_name(u"Main Character");
        _an_manager = dynamic_cast<animation_manager_component*>(parent_scene()->node_with_name(u"Animation Manager"));
        //break animation
        animation_info _ai;
        _ai.auto_destroy = false;
        _ai.duration = DESTRUCTION_ANIM_DURATION;
        _ai.state = animation_state::stopped;
        _ai.update_function = [this](float t, animation_info* ai){
            this->animate_break(t);
        };
        _break_an = _an_manager->add_animation(&_ai);
        //restore animation
        _ai.auto_destroy = false;
        _ai.duration = DESTRUCTION_ANIM_DURATION / 2.0f;
        _ai.state = animation_state::stopped;
        _ai.update_function = [this](float t, animation_info* ai){
            this->animate_restore(t);
        };
        _restore_an = _an_manager->add_animation(&_ai);
        
        //world
        _world = dynamic_cast<physics_engine*>(parent_scene()->node_with_name(u"Physic's Engine"))->world();
        //static body
        auto _t = transform_space::from_matrix(from_node_space_to(space::layer));
        b2BodyDef _bDef;
        _bDef.active = true;
        _bDef.angle = _t.rotation().x();
        _bDef.linearDamping = 0.0f;
        _bDef.angularDamping = 0.1f;
        _bDef.position = b2Vec2(_t.origin().x(), _t.origin().y());
        _bDef.type = b2_dynamicBody;
        _bDef.userData = (node*)this;
        _body = _world->CreateBody(&_bDef);
        
        b2FixtureDef _fDef;
        _fDef.isSensor = false;
        _fDef.friction = 1;
        _fDef.restitution = 0;
        _fDef.filter.categoryBits = PHYS_BREAKABLE_BLOCK | PHYS_MASK_SHAPE;
        _fDef.filter.maskBits = PHYS_MASK_CHARACTER | PHYS_MASK_ENEMY;
        b2PolygonShape _p;
        auto _scale = this->old_transform().scale();
        _p.SetAsBox(_scale.x() / 2.0f, _scale.y() / 2.0f);
        _fDef.shape = &_p;
        _body->CreateFixture(&_fDef);
        
        b2MassData md;
        md.mass = 10000000.0f;
        md.I = 10000000.0f;
        md.center = b2Vec2(0, 0);
        _body->SetMassData(&md);
        
        _saved_broken = _broken;
        _restoration_left = _restoration_time;
        if(_broken)
            break_block(false);
        else
            restore_block(false);
    }
}

void breakable_block::reset_component(){
    _broken = _saved_broken;
    _restoration_left = _restoration_time;
    if(_broken)
        break_block(false);
    else
        restore_block(false);
}

float breakable_block::restoration_time() const {
    return _restoration_time;
}

float breakable_block::restoration_time(float value){
    return _restoration_time = value;
}

bool breakable_block::restoration_enabled() const {
    return _restoration_enabled;
}

bool breakable_block::restoration_enabled(bool value){
    return _restoration_enabled = value;
}

bool breakable_block::broken() const {
    return _broken;
}

bool breakable_block::broken(bool value){
    return _broken = value;
}

const rb_string& breakable_block::image_name() const {
    return _sprite->image_name();
}

const rb_string& breakable_block::image_name(const rb_string &value){
    return _sprite->image_name(value);
}

bool breakable_block::can_kill_character() const {
    return _can_kill_character;
}

bool breakable_block::can_kill_character(bool value){
    return _can_kill_character = value;
}

bool breakable_block::explode_character_contact() const {
    return _explode_character_contact;
}

bool breakable_block::explode_character_contact(bool value){
    return _explode_character_contact = value;
}


rb_string breakable_block::type_name() const {
    return u"rb::breakable_block";
}

rb_string breakable_block::displayable_type_name() const {
    return u"Breakable Block";
}

void breakable_block::describe_type() {
    node::describe_type();
    
    start_type<breakable_block>([](){ return new breakable_block(); });
    image_property<breakable_block>(u"image_name", u"Image", true, {
        [](const breakable_block* site){
            return site->image_name();
        },
        [](breakable_block* site, const rb_string& value){
            site->image_name(value);
        }
    });
    boolean_property<breakable_block>(u"broken", u"Broken", true, {
        [](const breakable_block* site){
            return site->broken();
        },
        [](breakable_block* site, bool value){
            site->broken(value);
        }
    });
    single_property<breakable_block>(u"restoration_time", u"R Time", true, {
        [](const breakable_block* site){
            return site->restoration_time();
        },
        [](breakable_block* site, float value){
            site->restoration_time(value);
        }
    });
    boolean_property<breakable_block>(u"restoration_enabled", u"R Enabled", true, {
        [](const breakable_block* site){
            return site->restoration_enabled();
        },
        [](breakable_block* site, bool value){
            site->restoration_enabled(value);
        }
    });
    boolean_property<breakable_block>(u"can_kill_character", u"Can Kill", true, {
        [](const breakable_block* site){
            return site->can_kill_character();
        },
        [](breakable_block* site, bool value){
            site->can_kill_character(value);
        }
    });
    boolean_property<breakable_block>(u"explode_character_contact", u"Expl Char", true, {
        [](const breakable_block* site){
            return site->explode_character_contact();
        },
        [](breakable_block* site, bool value){
            site->explode_character_contact(value);
        }
    });
    //actions
    string_property<breakable_block>(u"on_break_action_buffer", u"Break Buffer", true, true, {
        [](const breakable_block* site){
            return site->_on_break_action_buffer;
        },
        [](breakable_block* site, const rb_string& value){
            site->_on_break_action_buffer = value;
        }
    });
    string_property<breakable_block>(u"on_break_action_name", u"Break Action", true, true, {
        [](const breakable_block* site){
            return site->_on_break_action_name;
        },
        [](breakable_block* site, const rb_string& value){
            site->_on_break_action_name = value;
        }
    });
    string_property<breakable_block>(u"on_restore_action_buffer", u"Restore Buffer", true, true, {
        [](const breakable_block* site){
            return site->_on_restore_action_buffer;
        },
        [](breakable_block* site, const rb_string& value){
            site->_on_restore_action_buffer = value;
        }
    });
    string_property<breakable_block>(u"on_restore_action_name", u"Restore Name", true, true, {
        [](const breakable_block* site){
            return site->_on_restore_action_name;
        },
        [](breakable_block* site, const rb_string& value){
            site->_on_restore_action_name = value;
        }
    });
    end_type();
}

void breakable_block::do_action(const rb_string& action_name, const rb_string& arg){
    if(action_name == u"break"){
        _no_reentrancy = true;
        break_block(true);
        _no_reentrancy = false;
    }
    else if(action_name == u"restore"){
        _no_reentrancy = true;
        restore_block(true);
        _no_reentrancy = false;
    }
}


















