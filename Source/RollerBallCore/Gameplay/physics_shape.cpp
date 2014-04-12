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
#include "ray.h"
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
    _gravity_ref_node = nullptr;
    _animatable = false;
    _moving_platform = false;
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
    boolean_property<physics_shape>(u"animatable", u"Animatable", true, {
        [](const physics_shape* site){
            return site->animatable();
        },
        [](physics_shape* site, const bool value){
            site->animatable(value);
        }
    });
    end_type();
}

void physics_shape::after_becoming_active(bool node_was_moved){
    polygon_component::after_becoming_active(node_was_moved);
    
    if(_animatable)
        register_for(registrable_event::update, PHYS_OBJECT_UPDATE_PRIORITY);
    else
        unregister_for(registrable_event::update);
}

void physics_shape::before_becoming_inactive(bool node_was_moved){
    polygon_component::before_becoming_inactive(node_was_moved);
}

void physics_shape::reset_component() {
    transform(_saved_transform);

    auto _saved_scale = transform().scale();
    transform(transform().scaled(1, 1)); //no scale
    auto _t = from_node_space_to(space::layer);
    transform(transform().scaled(_saved_scale)); //restore scale

    _body->SetTransform(b2Vec2(_t.origin().x(), _t.origin().y()), _t.rotation().x());
    _body->SetLinearVelocity(b2Vec2(0, 0));
    _body->SetAngularVelocity(0);
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
        auto _engine = dynamic_cast<physics_engine*>(parent_scene()->node_with_name(u"Physic's Engine"));
        _planet = dynamic_cast<physics_shape*>(parent_scene()->node_with_name(_planet_name));
        _world = _engine->world();
        if(_gravity_ref.has_value())
            _gravity_ref_node = parent_scene()->node_with_name(_gravity_ref.value());
        
        auto _saved_scale = transform().scale();
        transform(transform().scaled(1, 1)); //no scale
        auto _t = from_node_space_to(space::layer);
        transform(transform().scaled(_saved_scale)); //restore scale
        
        //Here we insert code so the object can be reset
        _saved_transform = transform();
        if(_animatable)
            register_for(registrable_event::update, PHYS_OBJECT_UPDATE_PRIORITY);
        else
            unregister_for(registrable_event::update);
        ///////////////////////////////////////////////
        
        b2BodyDef _bDef;
        _bDef.active = true;
        _bDef.angle = _t.rotation().x();
        _bDef.linearDamping = 0.0f;
        _bDef.angularDamping = 0.1f;
        _bDef.position = b2Vec2(_t.origin().x(), _t.origin().y());
        _bDef.type = _animatable ? b2_kinematicBody : b2_staticBody;
        _bDef.userData = (node*)this;
        _body = _world->CreateBody(&_bDef);
        
        b2FixtureDef _fDef;
        _fDef.isSensor = _type == kStaticGravityZone;
        _fDef.friction = 1;
        _fDef.restitution = 0;
        _fDef.filter.categoryBits = _type == kStaticPlanet ? PHYS_MASK_SHAPE : PHYS_MASK_GRAVITY_REGION;
        _fDef.filter.maskBits = PHYS_MASK_CHARACTER;
        
        
        _cached_pol = to_smooth_polygon();
        auto _cached_pol_copy = _cached_pol;
        //apply scaling
        matrix3x3::build_scale(_saved_scale).transform_polygon(_cached_pol); //scale applied!!!
        
        if(_type == kStaticPlanet){
            b2ChainShape _c;
            b2Vec2* _v = new b2Vec2[_cached_pol.point_count()];
            for (uint32_t i = 0; i < _cached_pol.point_count(); i++) {
                auto _p = _cached_pol.get_point(i);
                _v[i] = b2Vec2(_p.x(), _p.y());
            }
            _c.CreateLoop(_v, _cached_pol.point_count());
            delete [] _v;
            _fDef.shape = &_c;
            _body->CreateFixture(&_fDef);
        }
        else {
            b2PolygonShape _c;
            b2Vec2* _v = new b2Vec2[_cached_pol.point_count()];
            for (uint32_t i = 0; i < _cached_pol.point_count(); i++) {
                auto _p = _cached_pol.get_point(i);
                _v[i] = b2Vec2(_p.x(), _p.y());
            }
            _c.Set(_v, _cached_pol.point_count());
            delete [] _v;
            _fDef.shape = &_c;
            _body->CreateFixture(&_fDef);
        }
        
        //check if moving
        check_moving_platform();
        //save polygon
        _cached_pol = _cached_pol_copy;
        from_node_space_to(space::layer).from_space_to_base().transform_polygon(_cached_pol); //we transform to layer space
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
    if(_gravity_ref_node) //if gravity is fixed
    {
        auto _g = _gravity_ref_node->transform().from_space_to_base().y_vector().normalized();
        cam_gravity = -_g;
        return _g * _gravity;
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

bool physics_shape::animatable() const {
    return _animatable;
}

bool physics_shape::animatable(bool value){
    _animatable = value;
    if(active()){
        if(_animatable)
            register_for(registrable_event::update, PHYS_OBJECT_UPDATE_PRIORITY);
        else
            unregister_for(registrable_event::update);
    }
    return _animatable;
}

float to_canonical_angle(float a){
    auto _2pi = 2 * M_PI;
    auto _d = (int)(a / _2pi);
    a -= _d * _2pi;
    
    if(a < 0)
        a = 2 * M_PI + a;

    return a;
}

float diff_angles(float before, float after){
    float _source = to_canonical_angle(before);
    if(_source > M_PI)
        _source = _source - 2 * M_PI;
    float _target = to_canonical_angle(after);
    if(_target > M_PI)
        _target = _target - 2 * M_PI;
    float a = _target - _source;
    auto _2pi = 2 * M_PI;
    a += (a > M_PI) ? -_2pi : (a < -M_PI) ? _2pi : 0;
    return a;
}

void physics_shape::update(float dt){
    if(!_phys_initialized)
        return;
    
    auto _before = vec2(_body->GetPosition().x, _body->GetPosition().y);
    auto _t = this->from_node_space_to(space::layer);
    auto _v = _t.origin() - _before;
    auto _o = diff_angles(_body->GetAngle(), _t.rotation().x());
    
    _v *= 30.0f;
    _o *= 30.0f;
    
    _body->SetLinearVelocity(b2Vec2(_v.x(), _v.y()));
    _body->SetAngularVelocity(_o);
}

void physics_shape::check_moving_platform(){
    if(this->node_count() != 4){
        _moving_platform = false;
        return;
    }
    
    std::vector<node*> _nodes;
    this->copy_nodes_to_vector(_nodes);
    assert(_nodes.size() == 4);
    
    bool _has_first = false;
    bool _has_second = false;
    
    for(auto _n : _nodes){
        if(_n->has_class(u"first")){
            _has_first = true;
            _pt0 = _n->transform().origin() * this->transform().scale();
            continue;
        }
        if(_n->has_class(u"second")){
            _has_second = true;
            _pt1 = _n->transform().origin() * this->transform().scale();
            continue;
        }
    }
    
    if(_has_first && _has_second)
        _moving_platform = true;
    else
        _moving_platform = false;
}

bool physics_shape::is_moving_platform() const {
    return _moving_platform;
}

vec2 physics_shape::get_pt0() const {
    auto _bV = b2Vec2(_pt0.x(), _pt0.y());
    auto _bV2 = _body->GetWorldPoint(_bV);
    return vec2(_bV2.x, _bV2.y);
}

vec2 physics_shape::get_pt1() const {
    auto _bV = b2Vec2(_pt1.x(), _pt1.y());
    auto _bV2 = _body->GetWorldPoint(_bV);
    return vec2(_bV2.x, _bV2.y);
}

ray physics_shape::get_ray() const {
    auto _p0 = get_pt0();
    auto _p1 = get_pt1();

    return ray(_p0, _p1 - _p0);
}

vec2 physics_shape::get_normal() const {
    return get_ray().direction().rotated90();
}

vec2 physics_shape::get_velocity_at_pt(const rb::vec2 &pt) const{
    auto _v = _body->GetLinearVelocityFromWorldPoint(b2Vec2(pt.x(), pt.y()));
    return vec2(_v.x, _v.y);
}

b2Body* physics_shape::get_body() {
    return _body;
}































