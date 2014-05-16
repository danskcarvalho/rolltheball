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
#include "action_buffer.h"
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
    _fire_action_once = true;
    _fired_on_enter = false;
    _fired_on_exit = false;
    _phase_through = false;
    _free_jump_zone = false;
    _auto_move_dir = 0;
    _direction_on_jumping = 0;
    _force_zone = false;
    _force_one = vec2::zero;
    _force_two = vec2::zero;
}

physics_shape::~physics_shape(){
    
}

void physics_shape::describe_type(){
    polygon_component::describe_type();
    
    start_type<physics_shape>([](){ return new physics_shape(); });
    enumeration_property<physics_shape, type>(u"shape_type", u"Type", {{u"Static Planet", kStaticPlanet}, {u"Static Gravity Zone", kStaticGravityZone}, {u"Nothing", kNothing}}, true, {
        [](const physics_shape* site){
            return site->_type;
        },
        [](physics_shape* site, type value){
            site->_type = value;
        }
    });
    vec2_property<physics_shape>(u"texture_anim", u"Tx Anim", true, {
        [](const physics_shape* site){
            return site->_tex_transform_anim;
        },
        [](physics_shape* site, const vec2& value){
            site->_tex_transform_anim = value;
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
    boolean_property<physics_shape>(u"phase_through", u"Phasing", true, {
        [](const physics_shape* site){
            return site->phase_through();
        },
        [](physics_shape* site, const bool value){
            site->phase_through(value);
        }
    });
    single_property<physics_shape>(u"auto_move", u"Auto Move", true, {
        [](const physics_shape* site){
            return site->_auto_move_dir;
        },
        [](physics_shape* site, float value){
            site->_auto_move_dir = value;
        }
    });
    single_property<physics_shape>(u"direction_jump", u"Direction Jump", true, {
        [](const physics_shape* site){
            return site->_direction_on_jumping;
        },
        [](physics_shape* site, float value){
            site->_direction_on_jumping = value;
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
    boolean_property<physics_shape>(u"free_jump_zone", u"Free Jump", true, {
        [](const physics_shape* site){
            return site->_free_jump_zone;
        },
        [](physics_shape* site, const bool value){
            site->_free_jump_zone = value;
        }
    });
    boolean_property<physics_shape>(u"fire_action_once", u"Fire Act Once", true, {
        [](const physics_shape* site){
            return site->_fire_action_once;
        },
        [](physics_shape* site, const bool value){
            site->_fire_action_once = value;
        }
    });
    string_property<physics_shape>(u"on_enter_action_buffer", u"Enter Buffer", true, true, {
        [](const physics_shape* site){
            return site->_on_enter_action_buffer;
        },
        [](physics_shape* site, const rb_string& value){
            site->_on_enter_action_buffer = value;
        }
    });
    string_property<physics_shape>(u"on_enter_action_name", u"Enter Action", true, true, {
        [](const physics_shape* site){
            return site->_on_enter_action_name;
        },
        [](physics_shape* site, const rb_string& value){
            site->_on_enter_action_name = value;
        }
    });
    string_property<physics_shape>(u"on_exit_action_buffer", u"Exit Buffer", true, true, {
        [](const physics_shape* site){
            return site->_on_exit_action_buffer;
        },
        [](physics_shape* site, const rb_string& value){
            site->_on_exit_action_buffer = value;
        }
    });
    string_property<physics_shape>(u"on_exit_action_name", u"Exit Name", true, true, {
        [](const physics_shape* site){
            return site->_on_exit_action_name;
        },
        [](physics_shape* site, const rb_string& value){
            site->_on_exit_action_name = value;
        }
    });
    boolean_property<physics_shape>(u"force_zone", u"Force Zone", true, {
        [](const physics_shape* site){
            return site->_force_zone;
        },
        [](physics_shape* site, const bool value){
            site->_force_zone = value;
        }
    });
    vec2_property<physics_shape>(u"force_one", u"Force One", true, {
        [](const physics_shape* site){
            return site->_force_one;
        },
        [](physics_shape* site, const vec2& value){
            site->_force_one = value;
        }
    });
    vec2_property<physics_shape>(u"force_two", u"Force Two", true, {
        [](const physics_shape* site){
            return site->_force_two;
        },
        [](physics_shape* site, const vec2& value){
            site->_force_two = value;
        }
    });
    single_property<physics_shape>(u"max_velocity", u"Max Vel", true, {
        [](const physics_shape* site){
            return site->_max_velocity;
        },
        [](physics_shape* site, float value){
            site->_max_velocity = value;
        }
    });
    boolean_property<physics_shape>(u"zero_gravity", u"Zero Gravity", true, {
        [](const physics_shape* site){
            return site->_zero_gravity;
        },
        [](physics_shape* site, const bool value){
            site->_zero_gravity = value;
        }
    });
    end_type();
}

bool physics_shape::is_force_zone() const {
    if(!_force_zone)
        return false;
    
    std::vector<node*> _children;
    this->copy_nodes_to_vector(_children, node_filter::renderable);
    if(_children.size() != 4)
        return false;
    int _first = 0, _second = 0;
    for(size_t i = 0; i < _children.size(); i++){
        if(_children[i]->has_class(u"first"))
            _first++;
        if(_children[i]->has_class(u"second"))
            _second++;
    }
    return _first == 1 && _second == 1;
}

vec2 physics_shape::get_force(const rb::vec2 &worldPt) const{
    if(!is_force_zone())
        return vec2::zero;
    
    std::vector<node*> _children;
    std::vector<node*> _edgeOne;
    std::vector<node*> _edgeTwo;
    float _max_x = std::numeric_limits<float>::min();
    float _max_y = std::numeric_limits<float>::min();
    float _min_x = std::numeric_limits<float>::max();
    float _min_y = std::numeric_limits<float>::max();
    this->copy_nodes_to_vector(_children, node_filter::renderable);
    for(size_t i = 0; i < _children.size(); i++){
        if(_children[i]->has_class(u"first"))
            _edgeOne.push_back(_children[i]);
        else if(_children[i]->has_class(u"second"))
            _edgeOne.push_back(_children[i]);
        else
            _edgeTwo.push_back(_children[i]);
        
        if(_children[i]->transform().origin().x() > _max_x)
            _max_x = _children[i]->transform().origin().x();
        if(_children[i]->transform().origin().y() > _max_y)
            _max_y = _children[i]->transform().origin().y();
        if(_children[i]->transform().origin().x() < _min_x)
            _min_x = _children[i]->transform().origin().x();
        if(_children[i]->transform().origin().y() < _min_y)
            _min_y = _children[i]->transform().origin().y();
    }
    auto _pt0 = _edgeOne[0]->from_node_space_to(space::layer).origin();
    auto _pt1 = _edgeOne[1]->from_node_space_to(space::layer).origin();
    auto _pt2 = _edgeTwo[0]->from_node_space_to(space::layer).origin();
    auto _pt3 = _edgeTwo[1]->from_node_space_to(space::layer).origin();
    auto _r = rectangle(vec2((_min_x + _max_x) / 2.0f, (_min_y + _max_y) / 2.0f), vec2(fabsf(_max_x - _min_x), fabsf(_max_y - _min_y)));
    auto _localPt = from_node_space_to(space::layer).from_base_to_space().transformed_point(worldPt);
    if(!_r.intersects(_localPt))
        return vec2::zero;
    auto _pm0 = (_pt0 + _pt1) / 2.0f;
    auto _pm1 = (_pt2 + _pt3) / 2.0f;
    auto _maxDY = vec2::distance(_pm0, _pm1);
    
    auto _e1 = edge(_pt0, _pt1, vec2::down);
    auto _d1 = _e1.distance(worldPt);
    auto _t = _d1 / _maxDY;
    return _force_one * (1 - _t) + _force_two * _t;
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
}

void physics_shape::reset_physics(){
    transform(_saved_transform);
    
    auto _saved_scale = transform().scale();
    transform(transform().scaled(1, 1)); //no scale
    auto _t = from_node_space_to(space::layer);
    transform(transform().scaled(_saved_scale)); //restore scale
    
    _body->SetTransform(b2Vec2(_t.origin().x(), _t.origin().y()), _t.rotation().x());
    _body->SetLinearVelocity(b2Vec2(0, 0));
    _body->SetAngularVelocity(0);
    _fired_on_enter = false;
    _fired_on_exit = false;
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

bool physics_shape::free_jump_zone() const {
    return _free_jump_zone;
}

bool physics_shape::free_jump_zone(bool value){
    return _free_jump_zone = value;
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
        _fDef.isSensor = _type == kStaticGravityZone || _type == kNothing;
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

static float to_canonical_angle(float a){
    auto _2pi = 2 * M_PI;
    auto _d = (int)(a / _2pi);
    a -= _d * _2pi;
    
    if(a < 0)
        a = 2 * M_PI + a;

    return a;
}

static float diff_angles(float before, float after){
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
    
    _body->SetLinearVelocity(b2Vec2(0, 0));
    _body->SetAngularVelocity(0);
    auto _t = this->from_node_space_to(space::layer);
    _body->SetTransform(b2Vec2(_t.origin().x(), _t.origin().y()), _t.rotation().x());
    if(this->transformable())
        this->texture_space(this->texture_space().moved(this->texture_space().origin() + dt * _tex_transform_anim));
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

void physics_shape::main_character_entered(){
    if(_fired_on_enter && _fire_action_once)
        return;
    if(_on_enter_action_buffer == u"")
        return;
    auto _buffer = dynamic_cast<action_buffer*>(parent_scene()->node_with_name(_on_enter_action_buffer));
    if(_buffer){
        _buffer->perform_action(_on_enter_action_name);
        _fired_on_enter = true;
    }
    else {
        auto _at = dynamic_cast<action_target*>(parent_scene()->node_with_name(_on_enter_action_buffer));
        if(_at){
            _at->do_action(_on_enter_action_name, u"");
        }
    }
}

void physics_shape::main_character_exitted(){
    if(_fired_on_exit && _fire_action_once)
        return;
    if(_on_exit_action_buffer == u"")
        return;
    auto _buffer = dynamic_cast<action_buffer*>(parent_scene()->node_with_name(_on_exit_action_buffer));
    if(_buffer){
        _buffer->perform_action(_on_exit_action_name);
        _fired_on_exit = true;
    }
    else {
        auto _at = dynamic_cast<action_target*>(parent_scene()->node_with_name(_on_exit_action_buffer));
        if(_at){
            _at->do_action(_on_exit_action_name, u"");
        }
    }
}

void physics_shape::do_action(const rb_string& action_name, const rb_string& arg){
    
}

bool physics_shape::phase_through() const {
    return _phase_through;
}

bool physics_shape::phase_through(bool value){
    return _phase_through = value;
}






























