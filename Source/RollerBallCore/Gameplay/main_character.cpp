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
#define PHYS_CHARACTER_JUMP 5.5f
#define PHYS_CHARACTER_JUMP_COUNT 1
//Camera Constants
#define CAMERA_SCALE_X 6
#define CAMERA_SCALE_Y 6
//Camera Linear Constants
//Camera Linear Velocity
#define CAMERA_LINEAR_ACCELERATION 0.015f
#define CAMERA_LINEAR_DECELERATION 0.002f
#define CAMERA_MAX_LINEAR_ACCELERATION 0.15f
#define CAMERA_MIN_LINEAR_ACCELERATION 0.06f
//Camera Focus Linear Velocity
#define CAMERA_LINEAR_FOCUS_VELOCITY_RATIO 0.9f
#define CAMERA_LINEAR_FOCUS_MIN_VELOCITY 3.5f
//Camera Focus Linear Radius
#define CAMERA_LINEAR_FOCUS_RADIUS 0.2f

//Touch Events
#if defined(IOS_TARGET)
#define RESTING_TOUCH_DURATION 4 //in frames
#define ZERO_VELOCITY_LENGTH 0.025f
#define JUMP_TOUCHES 2
#define MOVE_TOUCHES 1
#else
#define RESTING_TOUCH_DURATION 2 //in frames
#define ZERO_VELOCITY_LENGTH 0.00005f
#define JUMP_TOUCHES 3
#define MOVE_TOUCHES 2
#endif

#define kTouchIndex 0
#define kTouchDuration 1
#define kTouchVelocity 2

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
    //Jumping
    _jumpButton = false;
    _didJump = false;
    _jumpCount = PHYS_CHARACTER_JUMP_COUNT;
    //Reverse Jumping
    _rev_jumpButton = false;
    _rev_didJump = false;
    //Camera
    _cam_focus_velocity = 0;
    _current_gZone = nullptr;
    //Frame
    _frame_count = 0;
    _clear_jump = nullptr;
    _clear_rev_jump = nullptr;
    _ended_touches = 0;
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
    register_for(registrable_event::touch_events, 0);
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

nullable<vec2> main_character::resting_touches(){
    auto _all_stopped = std::all_of(_touches.begin(), _touches.end(), [](const std::tuple<touch, uint32_t, vec2>& t){
        return std::get<kTouchVelocity>(t).length() <= ZERO_VELOCITY_LENGTH && std::get<kTouchDuration>(t) >= RESTING_TOUCH_DURATION;
    });
    if(_all_stopped && _touches.size() == MOVE_TOUCHES){
        _touches.clear();
        return vec2::zero;
    }
    return nullptr;
}

void main_character::update(float dt){
    vec2 _cam_g;
    vec2 _pt_s;
    update_character(_cam_g, _pt_s);
    
    this->transform(this->transform().moved(_body->GetPosition().x, _body->GetPosition().y).rotated(_body->GetAngle(), _body->GetAngle() + M_PI_2));
    if(_cam_g != vec2::zero)
        update_camera(_cam_g);
    
    _frame_count++;
    if(_clear_jump.has_value() && _clear_jump.value() <= _frame_count){
        _jumpButton = false;
        _didJump = false;
        _clear_jump = nullptr;
    }
    
    if(_clear_rev_jump.has_value() && _clear_rev_jump.value() <= _frame_count){
        _rev_jumpButton = false;
        _rev_didJump = false;
        _clear_rev_jump = nullptr;
    }
    
    //we update touches
    for (auto& _t : _touches){
        std::get<kTouchDuration>(_t)++;
    }
}

void main_character::update_camera(const rb::vec2 &cam_gravity){
    auto _char_velocity = vec2(_body->GetLinearVelocity().x, _body->GetLinearVelocity().y).length();
    auto _current_camera = parent_scene()->camera();
    auto _final_camera = transform_space::from_matrix(matrix3x3(cam_gravity.rotated90(rotation_direction::cw) * CAMERA_SCALE_X, cam_gravity * CAMERA_SCALE_Y, this->transform().origin()));
    
    if(!_cam_focus.has_value()){
        _cam_focus = circle(this->transform().origin(), CAMERA_LINEAR_FOCUS_RADIUS);
        _cam_focus_velocity = 0;
        parent_scene()->camera(_final_camera);
        return;
    }
    else {
        if(!_cam_focus.value().contains_point(this->transform().origin())){
            //we advance _cam_focus based on a speed...
            auto _focus_distance = vec2::distance(_cam_focus.value().origin(), this->transform().origin());
            auto _focus_velocity = std::max((CAMERA_LINEAR_FOCUS_VELOCITY_RATIO * _char_velocity), CAMERA_LINEAR_FOCUS_MIN_VELOCITY) / 30.0f;
            if (_focus_distance < _focus_velocity)
                _focus_velocity = _focus_distance;
            auto _final_focus_center = _cam_focus.value().origin() + (this->transform().origin() - _cam_focus.value().origin()) * _focus_velocity;
            _cam_focus = circle(_final_focus_center, CAMERA_LINEAR_FOCUS_RADIUS);
        }
    }
    
    //we then interpolate from the current camera to the focus...
    //radius
    auto _current_up = _current_camera.from_space_to_base().y_vector().normalized();
    auto _dest_up =_final_camera.from_space_to_base().y_vector().normalized();
    
    //position
    auto _focus_pos = _cam_focus.value().origin();
    auto _current_pos = _current_camera.origin();
    if(_cam_focus.value().contains_point(_current_pos))
        _cam_focus_velocity = std::max(_cam_focus_velocity - CAMERA_LINEAR_DECELERATION, CAMERA_MIN_LINEAR_ACCELERATION);
    else
        _cam_focus_velocity = std::min(_cam_focus_velocity + CAMERA_LINEAR_ACCELERATION, CAMERA_MAX_LINEAR_ACCELERATION);
    
    auto _final_pos = vec2::lerp(_cam_focus_velocity, _current_pos, _focus_pos);
    auto _final_up = vec2::slerp(_cam_focus_velocity, _current_up, _dest_up);
    
    
    _final_camera = transform_space::from_matrix(matrix3x3(_final_up.rotated90(rotation_direction::cw) * CAMERA_SCALE_X, _final_up * CAMERA_SCALE_Y, _final_pos));
    
    parent_scene()->camera(_final_camera);
}

void main_character::update_character(vec2& cam_gravity, vec2& point_on_surface){
    auto _g = vec2::zero;
    cam_gravity = vec2::zero;
    point_on_surface = vec2::zero;
    
    std::multimap<uint32_t, physics_shape*> _shapes_by_priority;
    for (b2ContactEdge* ce = _body->GetContactList(); ce; ce = ce->next) {
        auto _other = dynamic_cast<physics_shape*>((node*)ce->other->GetUserData());
        if(_other && _other->shape_type() == physics_shape::kStaticGravityZone && ce->contact->IsTouching() && ce->contact->IsEnabled()){
            //is in contact with gravity zone
            if(_other->_active_gravity)
                _shapes_by_priority.insert({_other->priority(), _other});
        }
    }
    
    std::list<physics_shape*> _possible_shapes;
    nullable<uint32_t> _priority = nullptr;
    for (auto _p : _shapes_by_priority){
        if(_priority.has_value() && _priority.value() != _p.first)
            break;
        else {
            _priority = _p.first;
            _possible_shapes.push_back(_p.second);
        }
    }
    
    if(_possible_shapes.size() > 1 && _current_gZone != nullptr){
        bool _removed = false;
        _possible_shapes.erase(std::remove_if(_possible_shapes.begin(), _possible_shapes.end(), [&](const physics_shape* ps){
            auto _r = (ps == _current_gZone);
            _removed |= _r;
            return _r;
        }), _possible_shapes.end());
        if(_removed){
            _current_gZone->_active_gravity = false;
            _current_gZone = nullptr;
        }
    }
    
    if(_possible_shapes.size() != 0){
        _current_gZone = _possible_shapes.front();
        _current_gZone->_active_gravity = true;
        _possible_shapes.pop_front();
        for (auto _ps : _possible_shapes)
            _ps->_active_gravity = false;
    }
    else {
        _current_gZone = nullptr;
    }
    
    if (_current_gZone)
        _g = _current_gZone->planet()->gravity_vector(transform().origin(), cam_gravity, point_on_surface);
    
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
        
        if (_previous_g.has_value() && _g.angle_between(_previous_g.value()) < TO_RADIANS(90)){
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
            
            //TODO: Also, invert the controls (when using touch controls...)
            if(!_current_gZone->invert_velocity()){
                if(_previous_g.has_value() && _g.angle_between(_previous_g.value()) > TO_RADIANS(90)){
                    _body->SetAngularVelocity(-_body->GetAngularVelocity());
                }
            }
            else {
                if(_previous_g.has_value() && _g.angle_between(_previous_g.value()) > TO_RADIANS(90)){
                    _vx = -_vx;
                    _v = _vx + _vy;
                }
            }
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
        auto _rt = resting_touches();
        if(_rt.has_value() && _rt.value() == vec2::zero){
            _direction = 0;
        }
        else if(_rt.has_value() && _rt.value() == vec2::up && !this->_clear_jump.has_value()){
            _jumpButton = true;
            this->_clear_jump = _frame_count + (uint64_t)(RESTING_TOUCH_DURATION * 1.5);
        }
        
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
        
        if(_jumpButton && !_didJump){
            _didJump = true;
            if(_jumpCount > 0){
                _vy = -_gy * PHYS_CHARACTER_JUMP;
                _v = _vx + _vy;
                _jumpCount--;
            }
        }
        
        if(_rev_jumpButton && !_rev_didJump){
            _rev_didJump = true;
            _vy = _gy * PHYS_CHARACTER_JUMP;
            _vx = vec2::zero;
            _v = _vx + _vy;
            _body->SetAngularVelocity(0);
            
        }
    }
    
    _body->SetLinearVelocity(b2Vec2(_v.x(), _v.y()));
    auto _fg = _g * _body->GetMass();
    _body->ApplyForceToCenter(b2Vec2(_fg.x(), _fg.y()));
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
        _world->SetContactListener(this);
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
    if(keycode == KEY_LEFT)
        _direction = -1;
    else if(keycode == KEY_RIGHT)
        _direction = 1;
    else if(keycode == KEY_UP){
        _jumpButton = true;
    }
    else if(keycode == KEY_DOWN){
        _rev_jumpButton = true;
    }
}

void main_character::keyup(const uint32_t keycode, const rb::keyboard_modifier modifier, bool &swallow){
    swallow = false;
    if(keycode == KEY_LEFT || keycode == KEY_RIGHT)
        _direction = 0;
    else if(keycode == KEY_UP) {
        _jumpButton = false;
        _didJump = false;
    }
    else if(keycode == KEY_DOWN){
        _rev_jumpButton = false;
        _rev_didJump = false;
    }
}

void main_character::touches_began(const std::vector<touch> &touches, bool &swallow){
    swallow = false;
    for (auto& _t : touches){
        _touches.push_back(std::make_tuple(_t, (uint32_t)0, vec2::zero));
    }
}

void main_character::touches_moved(const std::vector<touch> &touches, bool &swallow){
    swallow = false;
    //we update velocities
    for (auto& _t : _touches){
        for (auto& t : touches){
            if(!std::get<kTouchIndex>(_t).compare_identity(t))
                continue;
            
            auto _velocity = std::get<kTouchVelocity>(_t) + (t.normalized_position() - std::get<kTouchIndex>(_t).normalized_position());
            std::get<kTouchIndex>(_t) = t;
            std::get<kTouchVelocity>(_t) = _velocity;
            break; //only one matches, so we can break...
        }
    }
    auto _right_count = 0;
    auto _left_count = 0;
    auto _up_count = 0;
    auto _down_count = 0;
    
    //we check velocities
    for (auto& _t : _touches){
        vec2 _d = vec2::zero;
        if(std::get<kTouchVelocity>(_t).length() > ZERO_VELOCITY_LENGTH){
            auto _v = std::get<kTouchVelocity>(_t);
            if(fabsf(_v.x()) > fabsf(_v.y())){
                _d = _v.x() > 0 ? vec2::right : vec2::left;
            }
            else {
                _d = _v.y() > 0 ? vec2::up : vec2::down;
            }
            
            if(_d != vec2::zero){
                if(_d == vec2::right)
                    _right_count++;
                else if(_d == vec2::left)
                    _left_count++;
                else if(_d == vec2::up)
                    _up_count++;
                else if(_d == vec2::down)
                    _down_count++;
            }
        }
    }
    
    nullable<vec2> _direction_taken = nullptr;
    
    if(_touches.size() == MOVE_TOUCHES){
        if(_right_count == MOVE_TOUCHES)
            _direction_taken = vec2::right;
        else if(_left_count == MOVE_TOUCHES)
            _direction_taken = vec2::left;
        else if(_up_count == MOVE_TOUCHES)
            _direction_taken = vec2::up;
        else if(_down_count == MOVE_TOUCHES)
            _direction_taken = vec2::down;
    }
    
    if(_direction_taken.has_value()){
        _touches.clear();
    }
    
    if(_direction_taken.has_value()){
        if(_direction_taken.value() == vec2::right){
            this->_direction = 1;
        }
        else if(_direction_taken.value() == vec2::left){
            this->_direction = -1;
        }
        else if(_direction_taken.value() == vec2::up && !this->_clear_jump.has_value()){
            this->_jumpButton = true;
            this->_clear_jump = _frame_count + (uint64_t)(RESTING_TOUCH_DURATION * 1.5);
        }
        else if(_direction_taken.value() == vec2::down && !this->_clear_rev_jump.has_value()){
            this->_rev_jumpButton = true;
            this->_clear_rev_jump = _frame_count + (uint64_t)(RESTING_TOUCH_DURATION * 1.5);
            this->_direction = 0;
        }
        else { //zero
#if !defined(IOS_TARGET)
            this->_direction = 0;
#endif
        }
    }
}

void main_character::touches_ended(const std::vector<touch> &touches, bool &swallow){
    swallow = false;
    _touches.erase(std::remove_if(_touches.begin(), _touches.end(), [=](const std::tuple<touch, uint32_t, vec2>& t){
        for (auto& _t : touches){
            if(_t.compare_identity(std::get<kTouchIndex>(t))){
                if(std::get<kTouchVelocity>(t).length() <= ZERO_VELOCITY_LENGTH){
                    _ended_touches++;
                }
                return true;
            }
        }
        return false;
    }), _touches.end());
    
#if defined(IOS_TARGET)
    if(_ended_touches >= JUMP_TOUCHES && !this->_clear_jump.has_value()){
        _ended_touches = 0;
        this->_jumpButton = true;
        this->_clear_jump = _frame_count + (uint64_t)(RESTING_TOUCH_DURATION * 1.5);
    }
    
    if(_ended_touches >= MOVE_TOUCHES && _touches.size() == 0){
        this->_direction = 0;
        _ended_touches = 0;
    }
#else
    if(_ended_touches == MOVE_TOUCHES){
        this->_direction = 0;
        _ended_touches = 0;
    }
#endif
}

void main_character::touches_cancelled(const std::vector<touch> &touches, bool &swallow){
    swallow = false;
    _touches.erase(std::remove_if(_touches.begin(), _touches.end(), [=](const std::tuple<touch, uint32_t, vec2>& t){
        for (auto& _t : touches){
            if(_t.compare_identity(std::get<kTouchIndex>(t)))
                return true;
        }
        return false;
    }), _touches.end());
    _ended_touches = 0;
}

float main_character::damping() const {
    return _damping;
}

float main_character::damping(const float value) {
    return _damping = value;
}

void main_character::BeginContact(b2Contact *contact){
    auto _bA = contact->GetFixtureA()->GetBody();
    auto _bB = contact->GetFixtureB()->GetBody();
    
    //renew jumps
    auto _nA = (node*)_bA->GetUserData();
    auto _nB = (node*)_bB->GetUserData();
    
    if(dynamic_cast<main_character*>(_nA) && dynamic_cast<physics_shape*>(_nB)){
        auto _pB = dynamic_cast<physics_shape*>(_nB);
        if(_pB->shape_type() == physics_shape::kStaticPlanet){
            auto _ma = dynamic_cast<main_character*>(_nA);
            if(!_ma->_didJump)
                _ma->_jumpCount = PHYS_CHARACTER_JUMP_COUNT;
        }
    }
    
    if(dynamic_cast<main_character*>(_nB) && dynamic_cast<physics_shape*>(_nA)){
        auto _pA = dynamic_cast<physics_shape*>(_nA);
        if(_pA->shape_type() == physics_shape::kStaticPlanet){
            auto _mB = dynamic_cast<main_character*>(_nB);
            if(!_mB->_didJump)
                _mB->_jumpCount = PHYS_CHARACTER_JUMP_COUNT;
        }
    }
}

void main_character::EndContact(b2Contact *contact){
    auto _bA = contact->GetFixtureA()->GetBody();
    auto _bB = contact->GetFixtureB()->GetBody();

    //renew gravity
    auto _nA = (node*)_bA->GetUserData();
    auto _nB = (node*)_bB->GetUserData();
    
    if(dynamic_cast<main_character*>(_nA) && dynamic_cast<physics_shape*>(_nB)){
        auto _pB = dynamic_cast<physics_shape*>(_nB);
        if(_pB->shape_type() == physics_shape::kStaticGravityZone){
            _pB->_active_gravity = true;
        }
    }
    
    if(dynamic_cast<main_character*>(_nB) && dynamic_cast<physics_shape*>(_nA)){
        auto _pA = dynamic_cast<physics_shape*>(_nA);
        if(_pA->shape_type() == physics_shape::kStaticGravityZone){
            _pA->_active_gravity = true;
        }
    }
}

void main_character::PreSolve(b2Contact *contact, const b2Manifold *oldManifold){
    
}

void main_character::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse){
    
}








































