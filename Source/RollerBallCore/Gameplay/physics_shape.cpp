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
    _priority = 0;
    _active_gravity = true;
    _invert_velocity = true;
    _gravity_ref2 = nullptr;
    _circular_planet = false;
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
    nullable_string_property<physics_shape>(u"gravity_reference", u"Gravity Ref", true, true, {
        [](const physics_shape* site){
            return site->_gravity_ref;
        },
        [](physics_shape* site, const nullable<rb_string>& value){
            site->_gravity_ref = value;
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
    integer_property<physics_shape>(u"priority", u"Priority", true, {
        [](const physics_shape* site){
            return site->_priority;
        },
        [](physics_shape* site, const long value){
            site->_priority = (uint32_t)value;
        }
    });
    boolean_property<physics_shape>(u"invert_velocity", u"Inv Vel", true, {
        [](const physics_shape* site){
            return site->_invert_velocity;
        },
        [](physics_shape* site, const bool value){
            site->_invert_velocity = (bool)value;
        }
    });
    boolean_property<physics_shape>(u"circular_planet", u"Circ Planet", true, {
        [](const physics_shape* site){
            return site->_circular_planet;
        },
        [](physics_shape* site, const bool value){
            site->_circular_planet = (bool)value;
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

const nullable<rb_string>& physics_shape::gravity_reference() const {
    return _gravity_ref;
}

const nullable<rb_string>& physics_shape::gravity_reference(const nullable<rb_string> &value){
    return _gravity_ref = value;
}

void physics_shape::playing() {
    if(!_phys_initialized){
        _phys_initialized = true;
        auto _t = transform();
        auto _engine = dynamic_cast<physics_engine*>(parent_scene()->node_with_name(u"Physic's Engine"));
        _planet = dynamic_cast<physics_shape*>(parent_scene()->node_with_name(_planet_name));
        _world = _engine->world();
        if(_gravity_ref.has_value())
            _gravity_ref2 = parent_scene()->node_with_name(_gravity_ref.value());
        
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
        
        if(_type == kStaticPlanet && smooth()){
            _cached_pol = to_smooth_polygon();
        }
        else if(_type == kStaticPlanet || _type == kStaticGravityZone){
            _cached_pol = to_smooth_polygon();
        }
        
        if(_type == kStaticPlanet){
            if(!_circular_planet){
                b2ChainShape _c;
                b2Vec2* _v = new b2Vec2[_cached_pol.point_count()];
                for (uint32_t i = 0; i < _cached_pol.point_count(); i++) {
                    auto _p = _t.from_space_to_base().transformed_point(_cached_pol.get_point(i));
                    _p = _new_t.from_base_to_space().transformed_point(_p);
                    _v[i] = b2Vec2(_p.x(), _p.y());
                }
                _c.CreateLoop(_v, _cached_pol.point_count());
                delete [] _v;
                _fDef.shape = &_c;
                _body->CreateFixture(&_fDef);
            }
            else {
                b2CircleShape _c;
                vec2 _anyPoint = _t.from_space_to_base().transformed_point(_cached_pol.get_point(0));
                _c.m_p = b2Vec2(0, 0);
                _c.m_radius = vec2::distance(_t.origin(), _anyPoint);
                _fDef.shape = &_c;
                _body->CreateFixture(&_fDef);
            }
        }
        else {
            b2PolygonShape _c;
            b2Vec2* _v = new b2Vec2[_cached_pol.point_count()];
            for (uint32_t i = 0; i < _cached_pol.point_count(); i++) {
                auto _p = _t.from_space_to_base().transformed_point(_cached_pol.get_point(i));
                _p = _new_t.from_base_to_space().transformed_point(_p);
                _v[i] = b2Vec2(_p.x(), _p.y());
            }
            _c.Set(_v, _cached_pol.point_count());
            delete [] _v;
            _fDef.shape = &_c;
            _body->CreateFixture(&_fDef);
        }
        
        if(_type == kStaticPlanet || _type == kStaticGravityZone){
            transform().from_space_to_base().transform_polygon(_cached_pol); //we transform the polygon
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

vec2 physics_shape::gravity_vector(const rb::vec2 &position, vec2& cam_gravity){
    if(_gravity_ref2) //if gravity is fixed
    {
        auto _g = _gravity_ref2->transform().from_space_to_base().y_vector().normalized();
        cam_gravity = -_g;
        return _g * _gravity;
    }
    
    if(_circular_planet){
        auto _g = transform().origin() - position;
        _g.normalize();
        _g *= _gravity;
        cam_gravity = -_g;
        return _g;
    }
    
    auto& _pol = _cached_pol;
    
    uint32_t _index;
    auto _e = _pol.closest_edge(position, _index);
    
    auto _prev_e = _index == 0 ? _pol.get_edge(_pol.edge_count() - 1) : _pol.get_edge(_index - 1);
    auto _next_e = _index == _pol.edge_count() - 1 ? _pol.get_edge(0) : _pol.get_edge(_index + 1);
    cam_gravity = vec2::zero;
    auto _pt_on_e = position - _e.distance_vector(position);
    auto _t = (_pt_on_e - _e.pt0()).length() / _e.length();
    
    auto _p_normal = ((_prev_e.normal() + _e.normal()) / 2.0f).normalized();
    auto _n_normal = ((_next_e.normal() + _e.normal()) / 2.0f).normalized();
    
    if(almost_equal(_t, 0.5f)){
        cam_gravity = _e.normal();
    }
    else if(_t < 0.5f){
        cam_gravity = vec2::slerp(_t * 2, _p_normal, _e.normal());
    }
    else { //t > 0.5
        cam_gravity = vec2::slerp((_t - 0.5f) * 2, _e.normal(), _n_normal);
    }
    
    auto _n = _e.normal().normalized();
    return _n * (-_gravity);
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

uint32_t physics_shape::priority() const {
    return _priority;
}

uint32_t physics_shape::priority(const uint32_t value){
    return _priority = value;
}

bool physics_shape::invert_velocity() const {
    return _invert_velocity;
}

bool physics_shape::invert_velocity(const bool value){
    return _invert_velocity = value;
}
































