//
//  main_character.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 13/08/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#include "main_character.h"
#include "destructible_sprite_component.h"
#include "physics_mask.h"
#include "physics_engine.h"
#include "scene.h"
#include "physics_shape.h"
#include "animation_manager_component.h"
#include "animation_function.h"
#include "particle_emitter_component.h"
#include "base_enemy.h"
#include "breakable_block.h"
#include "ray.h"
#include <random>
#include <Box2D/Box2D.h>

//Some constants
#define PHYS_CHARACTER_VELOCITY 5.5f
#define PHYS_CHARACTER_ACCEL (PHYS_CHARACTER_VELOCITY / (0.25f * 30.0f))
#define PHYS_CHARACTER_JUMP 4.5f
#define PHYS_CHARACTER_JUMP_COUNT 2
//Camera Constants
#define CAMERA_SCALE_X 8
#define CAMERA_SCALE_Y 8
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
//Touch Control only implemented for iOS
#define JUMP_TOUCH_DURATION 6 //in frames
#define ZERO_VELOCITY_LENGTH 0.1f

#define kTouchIndex 0
#define kTouchInitialPos 1

#define SHAKE_CAMERA_ANIM_DURATION 0.5f
#define SHAKE_FREQUENCY 10.0f
#define SHAKE_MAGNITUDE 0.15f

#define BOUNCE_ANIM_DURATION 1.0f
#define BOUNCE_FREQUENCY 5.0f
#define BOUNCE_MAGNITUDE 0.75f
#define BOUNCE_ANGLE_VELOCITY (TO_RADIANS(720.0f))

#define DESTRUCTION_MATRIX 6
#define DESTRUCTION_ANIM_DURATION 1.0f
#define MAX_DESTRUCTION_VELOCITY 6
#define MIN_DESTRUCTION_VELOCITY 4

#define EXPLOSION_GRAVITY_MAGNITUDE 25

using namespace rb;

inline float sgn(float v){
    if(v == 0)
        return 0;
    return v / fabsf(v);
}

inline float signed_length(const vec2& v, const vec2& axis){
    return v.length() * sgn(vec2::dot(v, axis));
}

main_character::main_character(){
    _invert_xaxis = false;
    _body = nullptr;
    _world = nullptr;
    _phys_initialized = false;
    _sprite = new destructible_sprite_component();
    _sprite->matrix(vec2(DESTRUCTION_MATRIX, DESTRUCTION_MATRIX));
    add_node(_sprite);
    name(u"Main Character");
    _direction = 0;
    _previous_direction = 0;
    _saved_direction = 0;
    _previous_g = nullptr;
    _damping = 0.9;
    //Jumping
    _jumpButton = false;
    _didJump = false;
    _jumpCount = PHYS_CHARACTER_JUMP_COUNT;
    //Camera
    _cam_focus_velocity = 0;
    _current_gZone = nullptr;
    _cam_scale = vec2(CAMERA_SCALE_X, CAMERA_SCALE_Y);
    //Frame
    _frame_count = 0;
    _clear_jump = nullptr;
    _clear_last_moving_platform = nullptr;
    //animation
    _an_manager = nullptr;
    //shake camera animation
    _shake_camera_an = nullptr;
    _camera_x_shake = 0;
    //die animation
    _died = false;
    _die_an = nullptr;
    _die_emitter = new particle_emitter_component();
    _die_emitter->perpendicular_velocity_01(MAX_DESTRUCTION_VELOCITY);
    _die_emitter->perpendicular_velocity_02(MIN_DESTRUCTION_VELOCITY);
    _die_emitter->angle_02(M_PI * 2);
    _die_emitter->radial_velocity_02(M_PI_2);
    _die_emitter->size_01(vec2(0.1, 0.1));
    _die_emitter->size_02(vec2(0.2, 0.2));
    _die_emitter->size_rate_01(vec2(0.05, 0.05));
    _die_emitter->size_rate_02(vec2(0.125, 0.125));
    _die_emitter->start_color_01(color::from_rgba(1, 0, 0, 1));
    _die_emitter->start_color_02(color::from_rgba(1, 0, 0, 1));
    _die_emitter->life_01(1);
    _die_emitter->life_02(1);
    _die_emitter->blend_01(0.1);
    _die_emitter->blend_02(0.1);
    _die_emitter->inv_emission_rate(0.005);
    _die_emitter->emission_radius_01(0);
    _die_emitter->emission_radius_02(0.4);
    _die_emitter->duration(0.15);
    _die_emitter->delay(0);
    _die_emitter->in_editor_hidden(true);
    _die_emitter->loop(false);
    _die_emitter->state(particle_state::stopped);
    add_node(_die_emitter);
    //resetting
    _resetted = false;
    //constrained camera
    _camera_constrained = false;
    _saved_camera_constrained = false;
    _cam_offset = _saved_cam_offset = nullptr;
    //bouncing ball
    _current_bounceball = nullptr;
    _is_bouncing = nullptr;
    _bounce_velocity = 1;
    //block to break
    _block_to_break = nullptr;
    //ground
    _normal = nullptr;
    _current_ground = nullptr;
    _moving_platform = nullptr;
    _last_moving_platform = nullptr;
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
    vec2_property<main_character>(u"camera_scale", u"Cam Scale", true, {
        [](const main_character* site){
            return site->_cam_scale;
        },
        [](main_character* site, const vec2& value){
            site->_cam_scale = value;
        }
    });
    single_property<main_character>(u"direction", u"Direction", true, {
        [](const main_character* site){
            return site->_saved_direction;
        },
        [](main_character* site, float value){
            site->_saved_direction = value;
        }
    });
    boolean_property<main_character>(u"invert_xaxis", u"Inv X Axis", true, {
        [](const main_character* site){
            return site->_invert_xaxis;
        },
        [](main_character* site, bool value){
            site->_invert_xaxis = value;
        }
    });
    boolean_property<main_character>(u"camera_constrained", u"Constrain Cam", true, {
        [](const main_character* site){
            return site->_camera_constrained;
        },
        [](main_character* site, bool value){
            site->_camera_constrained = value;
        }
    });
    nullable_vec2_property<main_character>(u"cam_offset", u"Cam Offset", true, {
        [](const main_character* site){
            return site->_cam_offset;
        },
        [](main_character* site, const nullable<vec2>& value){
            site->_cam_offset = value;
        }
    });
    single_property<main_character>(u"bounce_velocity", u"Bc Vel", true, {
        [](const main_character* site){
            return site->_bounce_velocity;
        },
        [](main_character* site, float value){
            site->_bounce_velocity = value;
        }
    });
    end_type();
}

const vec2& main_character::camera_scale() const {
    return _cam_scale;
}

const vec2& main_character::camera_scale(const vec2& value){
    _cam_scale = value;
    return _cam_scale;
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

bool main_character::check_die(){
    for (b2ContactEdge* ce = _body->GetContactList(); ce; ce = ce->next) {
        auto _other = dynamic_cast<base_enemy*>((node*)ce->other->GetUserData());
        if(_other && ce->contact->IsTouching() && ce->contact->IsEnabled()){
            die();
            return true;
        }
    }
    return false;
}

void main_character::update_died(float dt){
    _current_bounceball = nullptr;
    _is_bouncing = nullptr;
    this->transform(this->transform().rotated(0, M_PI_2));
    for (size_t i = 0; i < (DESTRUCTION_MATRIX * DESTRUCTION_MATRIX); i++) {
        _parts_velocities[i] += _gravity_died * dt;
    }
    for (uint32_t i = 0; i < DESTRUCTION_MATRIX; i++) {
        for (uint32_t j = 0; j < DESTRUCTION_MATRIX; j++) {
            auto _t = _sprite->transform(i, j);
            auto _v = _parts_velocities[i + j * DESTRUCTION_MATRIX];
            _t.origin(_t.origin() + _v * dt);
            _sprite->transform(i, j, _t);
        }
    }
    auto _final_camera = _camera_died;
    _final_camera.origin(vec2(_final_camera.origin().x() + _camera_x_shake, _final_camera.origin().y() - _camera_x_shake));
    
    parent_scene()->camera(_final_camera);
}

void main_character::check_bouncing(){
    if(_current_bounceball)
        return;
    
    for (auto _bb : _bouncing_balls){
        nullable<transform_space> _saved_space = nullptr;
        if(_bouncing_anims.count(_bb.first)){
            _saved_space = _bb.first->transform();
            _bb.first->transform(_bb.first->transform().scaled(_bouncing_anims[_bb.first].original_size));
        }
        
        auto _t = _bb.first->from_node_space_to(space::layer);
        auto _r = _t.scale().x() / 2.0f;
        auto _sumR = _r + 0.5f;
        auto _distance = vec2::distance(_t.origin(), transform().origin());
        if(_distance <= _sumR){ //intersects!
            if(_bb.first == _is_bouncing)
                continue; //can't go to the same ball again!
            
            //adjust
            auto _v = transform().origin() - _t.origin();
            _v.normalize();
            _v *= _sumR;
            auto _newPos = _t.origin() + _v;
            _body->SetAngularVelocity(0);
            _body->SetLinearVelocity(b2Vec2(0, 0));
            _body->SetTransform(b2Vec2(_newPos.x(), _newPos.y()), _body->GetAngle()); //we adjust the body
            this->transform(transform().moved(_newPos));
            _current_bounceball = _bb.first;
            _is_bouncing = nullptr;
            _local_ballPos = this->from_node_space_to(_bb.first).origin();
            _current_bouncehasdir = _bb.second != nullptr;
            _current_autobounce = _bb.first->has_class(u"autoBounce");
            if(_saved_space.has_value())
                _bb.first->transform(_saved_space.value());
            break;
        }
        if(_saved_space.has_value())
            _bb.first->transform(_saved_space.value());
    }
}

void main_character::update_bounceball(float dt){
    if(!_current_bounceball)
        return;
    
    bool _launch = false;
    if(_current_bouncehasdir){
        auto _vChar = _local_ballPos;
        auto _vDir = vec2::right;
        _bouncing_balls[_current_bounceball]->from_node_space_to(_current_bounceball).from_space_to_base().transform_vector(_vDir).normalize();
        auto _a = _vDir.angle_between(_vChar, rotation_direction::ccw);
        //we try to reduce the angle to 0
        float _reduced = _a;
        _a -= BOUNCE_ANGLE_VELOCITY * dt;
        bool _reached = false;
        if(_a <= 0){
            _a = 0;
            _reached = true;
        }
        else
            _reduced = BOUNCE_ANGLE_VELOCITY * dt;
        
        auto _bbt = _current_bounceball->transform();
        _bbt.rotate_by(-_reduced);
        _current_bounceball->transform(_bbt);
        
        //Transform to the direction
        auto _currentPoint = _current_bounceball->from_node_space_to(space::layer).from_space_to_base().transformed_point(_local_ballPos);
        _body->SetTransform(b2Vec2(_currentPoint.x(), _currentPoint.y()), _body->GetAngle() - _reduced);
        
        if(_reached && (_current_autobounce || _jumpButton))
            _launch = true;
    }
    else {
        auto _currentPoint = _current_bounceball->from_node_space_to(space::layer).from_space_to_base().transformed_point(_local_ballPos);
        _body->SetTransform(b2Vec2(_currentPoint.x(), _currentPoint.y()), _body->GetAngle());
        
        if(_jumpButton)
            _launch = true;
    }
    
    if(_launch){
        auto _vel = _current_bounceball->from_node_space_to(space::layer).from_space_to_base().transformed_vector(_local_ballPos).normalized() * _bounce_velocity;
        _is_bouncing = _current_bounceball;
        do_bounce_animation(_current_bounceball);
        _current_bounceball = nullptr;
        _body->SetLinearVelocity(b2Vec2(_vel.x(), _vel.y()));
    }
}

void main_character::update(float dt){
    if(_died){
        update_died(dt);
        return;
    }

    if(check_die())
        return;
    
    if(_block_to_break){
        dynamic_cast<breakable_block*>(_block_to_break)->break_block(true);
        _block_to_break = nullptr;
    }
    
    check_bouncing();
    if(_current_bounceball || _is_bouncing)
        update_bounceball(dt);
    vec2 _cam_g;
    update_character(_cam_g, dt);
    
    this->transform(this->transform().moved(_body->GetPosition().x, _body->GetPosition().y).rotated(_body->GetAngle(), _body->GetAngle() + M_PI_2));
    if(_cam_g != vec2::zero)
        update_camera(_cam_g);
    
    _frame_count++;
    if(_clear_jump.has_value() && _clear_jump.value() <= _frame_count){
        _jumpButton = false;
        _didJump = false;
        _clear_jump = nullptr;
    }
    
    if(_clear_last_moving_platform.has_value() && _clear_last_moving_platform.value() <= _frame_count){
        _last_moving_platform = nullptr;
        _clear_last_moving_platform = nullptr;
    }
    
    //jump count may be renovated
    if(!_didJump && _jumpCount != PHYS_CHARACTER_JUMP_COUNT){
        for(auto& _kp : _contacts){
            auto _physShape = dynamic_cast<physics_shape*>(_kp.first);
            auto _breakBlock = dynamic_cast<breakable_block*>(_kp.first);
            auto _cond1 = _physShape && _physShape->shape_type() == physics_shape::kStaticPlanet;
            auto _cond2 = _breakBlock != nullptr;
            if((_cond1 || _cond2) && testSlopeAngle(&_kp.second, _previous_g))
                this->_jumpCount = PHYS_CHARACTER_JUMP_COUNT;
        }
    }
}

nullable<vec2> main_character::getClosestPointFromCameraTrack(const rb::vec2& charPos) const{
    nullable<vec2> _closestPoint = nullptr;
    float _distance = std::numeric_limits<float>::max();
    for (auto _p : _camera_polygons){
        uint32_t _index;
        auto _e = _p.closest_edge(charPos, _index);
        auto _d = _e.distance_vector(charPos);
        if(_d.length() < _distance){
            _closestPoint = charPos - _d;
            _distance = _d.length();
        }
    }
    
    return _closestPoint;
}

void main_character::update_camera(const rb::vec2 &cam_gravity){
    auto _thisPosition = this->transform().origin();
    if(_current_bounceball && !_is_bouncing) //get the bouncing ball position
        _thisPosition = _current_bounceball->from_node_space_to(space::layer).origin();
    
    if(_cam_offset.has_value())
        _thisPosition += _cam_offset.value();
    if(_camera_constrained){
        auto _closest = getClosestPointFromCameraTrack(_thisPosition);
        if(_closest.has_value())
            _thisPosition = _closest.value();
    }
    auto _char_velocity = vec2(_body->GetLinearVelocity().x, _body->GetLinearVelocity().y).length();
    auto _current_camera = parent_scene()->camera();
    auto _final_camera = transform_space::from_matrix(matrix3x3(cam_gravity.rotated90(rotation_direction::cw) * _cam_scale.x(), cam_gravity * _cam_scale.y(), _thisPosition));
    
    if(!_cam_focus.has_value()){
        _cam_focus = circle(_thisPosition, CAMERA_LINEAR_FOCUS_RADIUS);
    }
    else {
        if(!_cam_focus.value().contains_point(_thisPosition)){
            //we advance _cam_focus based on a speed...
            auto _focus_distance = vec2::distance(_cam_focus.value().origin(), _thisPosition);
            auto _focus_velocity = std::max((CAMERA_LINEAR_FOCUS_VELOCITY_RATIO * _char_velocity), CAMERA_LINEAR_FOCUS_MIN_VELOCITY) / 30.0f;
            if (_focus_distance < _focus_velocity)
                _focus_velocity = _focus_distance;
            auto _final_focus_center = _cam_focus.value().origin() + (_thisPosition - _cam_focus.value().origin()) * _focus_velocity;
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
    
    
    _final_camera = transform_space::from_matrix(matrix3x3(_final_up.rotated90(rotation_direction::cw) * _cam_scale.x(), _final_up * _cam_scale.y(), _final_pos));
    _final_camera.origin(vec2(_final_camera.origin().x() + _camera_x_shake, _final_camera.origin().y() - _camera_x_shake));
    
    parent_scene()->camera(_final_camera);
}

void main_character::check_new_moving_platform(vec2& v, nullable<float>& rot_vel){
MvPlatform:
    rot_vel = nullptr;
    if(_moving_platform){
        auto _tangent = _moving_platform->get_normal().rotated(TO_RADIANS(-90));
        //check if already..
        if(_current_ground && _current_ground != _moving_platform->get_body()){
            //check if we can make a new moving platform
            _last_moving_platform = _moving_platform;
            _moving_platform = nullptr;
            _clear_last_moving_platform = _frame_count + (uint64_t)(JUMP_TOUCH_DURATION * 2);
            v = _tangent * _moving_vel;
            rot_vel = _rotation_vel;
            goto MvPlatform;
        }
        else if(_didJump){
            _last_moving_platform = _moving_platform;
            _moving_platform = nullptr;
            _clear_last_moving_platform = _frame_count + (uint64_t)(JUMP_TOUCH_DURATION * 2);
            v = _tangent * _moving_vel;
            rot_vel = _rotation_vel;
        }
//        else{
//            auto _rMoving = _moving_platform->get_ray();
//            auto _max_len = _rMoving.get_parameter(_moving_platform->get_pt1());
//            auto _tparam = _rMoving.get_parameter(vec2(_body->GetPosition().x, _body->GetPosition().y));
//            if(_tparam < -0.05 || _tparam > (_max_len + 0.05)){
//                _last_moving_platform = _moving_platform;
//                _moving_platform = nullptr;
//                _clear_last_moving_platform = _frame_count + (uint64_t)(JUMP_TOUCH_DURATION * 2);
//                v = _tangent * _moving_vel;
//            }
//        }
    }
    else {
        //if not...
        if(_current_ground){
            auto _pshape = dynamic_cast<physics_shape*>((node*)_current_ground->GetUserData());
            if(_pshape && _pshape->is_moving_platform()){
                if(_last_moving_platform && _last_moving_platform == _pshape)
                    return;
                _moving_platform = _pshape;
                auto _rMoving = _moving_platform->get_ray();
                _moving_max_t = _rMoving.get_parameter(_moving_platform->get_pt1());
                _moving_t = _rMoving.get_parameter(vec2(_body->GetPosition().x, _body->GetPosition().y));
                if(_moving_t < 0 || _moving_t > _moving_max_t){
                    _moving_platform = nullptr;
                    return;
                }
                //save up vector in local body space
                _up_vector = transform().from_space_to_base().transformed_vector(vec2::up);
                auto _b2_up = b2Vec2(_up_vector.x(), _up_vector.y());
                _b2_up = _moving_platform->get_body()->GetLocalVector(_b2_up);
                _up_vector = vec2(_b2_up.x, _b2_up.y); //save up vector
                //velocity
                auto _tangent = _moving_platform->get_normal().rotated(TO_RADIANS(-90));
                auto _proj_v = v.projection(_tangent);
                auto _vx = signed_length(_proj_v, _tangent);
                _moving_vel = _vx;
                _moving_first = true;
                //limit moving_vel
                if(_moving_vel > PHYS_CHARACTER_VELOCITY)
                    _moving_vel = PHYS_CHARACTER_VELOCITY;
                else if(_moving_vel < (-PHYS_CHARACTER_VELOCITY))
                    _moving_vel = -PHYS_CHARACTER_VELOCITY;
            }
        }
    }
}


void main_character::update_movingplatform(vec2& vel, nullable<float>& rot_vel, float dt, float direction){
    rot_vel = nullptr;
    if(_direction > 0) { //to right
        if(_moving_vel < PHYS_CHARACTER_VELOCITY)
        {
            _moving_vel += PHYS_CHARACTER_ACCEL;
        }
    }
    else if(_direction < 0) { //to left
        if(_moving_vel > (-PHYS_CHARACTER_VELOCITY)){
            _moving_vel -= PHYS_CHARACTER_ACCEL;
        }
    }
    else {
        _moving_vel = 0;
    }
    
    auto _to_move = _moving_vel * dt;
    auto _angle_obj = -_to_move / 0.5f;
    _rotation_vel = _angle_obj * 30.0f;
    
    if(!_moving_first){
        _moving_t += _to_move;
        this->_up_vector.rotate(_angle_obj);
    }
    _moving_first = false;
    bool _set_to_null = false;
    
    auto _ray = _moving_platform->get_ray();
    auto _pt = _ray.sample(_moving_t);
    _pt = _pt + _moving_platform->get_normal() * 0.5f;
    
    if(_moving_t < -0.05 || _moving_t > (_moving_max_t + 0.05)){
        _last_moving_platform = _moving_platform;
        _clear_last_moving_platform = _frame_count + (uint64_t)(JUMP_TOUCH_DURATION * 2);
        auto _tangent = _moving_platform->get_normal().rotated(TO_RADIANS(-90));
        vel = _tangent * _moving_vel;
        _set_to_null = true;
        rot_vel = _rotation_vel;
    }
    else {
        if(_jumpButton && !_didJump){
            _didJump = true;
            if(_jumpCount > 0){
                auto _vy = PHYS_CHARACTER_JUMP;
                auto _vAt = _moving_platform->get_velocity_at_pt(_pt);
                if(_vAt.y() > 0)
                    _vy += _vAt.y() / 3;
                _jumpCount--;
                
                _last_moving_platform = _moving_platform;
                _clear_last_moving_platform = _frame_count + (uint64_t)(JUMP_TOUCH_DURATION * 2);
                auto _tangent = _moving_platform->get_normal().rotated(TO_RADIANS(-90));
                vel = _tangent * _moving_vel;
                _set_to_null = true;
                rot_vel = _rotation_vel;
                if(!_previous_g.has_value())
                    vel.y(_vy + vel.y() / 3);
                else{
                    vel.y(vel.y() / 3);
                    vel += -_vy * _previous_g.value().normalized();
                }
            }
        }
    }
    
    //get up vector in world space
    auto _b2_upWorld = _moving_platform->get_body()->GetWorldVector(b2Vec2(_up_vector.x(), _up_vector.y()));
    auto _up_world = vec2(_b2_upWorld.x, _b2_upWorld.y);
    auto _right_world = _up_world.rotated(TO_RADIANS(-90));
    auto _angle = vec2::right.angle_between(_right_world, rotation_direction::ccw);

    _body->SetTransform(b2Vec2(_pt.x(), _pt.y()), _angle);
    if(_set_to_null)
        _moving_platform = nullptr;
}

void main_character::update_character(vec2& cam_gravity, float dt){
    auto _g = vec2::zero;
    cam_gravity = vec2::zero;
    
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
    
    if (_current_gZone && _current_gZone->planet())
        _g = _current_gZone->planet()->gravity_vector(transform().origin(), cam_gravity);
    else if(_current_gZone)
        _g = _current_gZone->gravity_vector(transform().origin(), cam_gravity);
    
    if(_g == vec2::zero){
        _g = _default_gravity;
        if(_g != vec2::zero)
            cam_gravity = -_g.normalized();
    }
    if(!_died)
        _gravity_died = _g.normalized() * EXPLOSION_GRAVITY_MAGNITUDE;
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
            if(_current_gZone && !_current_gZone->invert_velocity()){
                if(_previous_g.has_value() && _g.angle_between(_previous_g.value()) > TO_RADIANS(90)){
                    _body->SetAngularVelocity(-_body->GetAngularVelocity());
                    _direction = -_direction;
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
        auto _true_gy = _gy;
        if(_normal.has_value()) { //we move using the normal
            auto _sv_g = _g;
            _g = -_normal.value() * _sv_g.length();
            _gx = _normal.value().rotated(TO_RADIANS(-90));
            _gy = -_normal.value();
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
            _vx = vec2::zero;
            if(_current_ground)
                _vy = vec2::zero;
            //_vx *= _damping;
            _v = _vx + _vy;
        }
        
        if(_jumpButton && !_didJump && !_moving_platform){
            _didJump = true;
            if(_jumpCount > 0){
                _vy = -_true_gy * PHYS_CHARACTER_JUMP;
                _v = _vx + _vy;
                _jumpCount--;
            }
        }
        
        //check for moving platform...
        //1. decide for moving platform...
        nullable<float> _rot_vel;
        check_new_moving_platform(_v, _rot_vel);
        //2. if moving platform...
        if(_moving_platform){
            _g = vec2::zero;
            update_movingplatform(_v, _rot_vel, dt, _direction);
        }
        
        if(_rot_vel.has_value())
            _body->SetAngularVelocity(_rot_vel.value());
    }
    
    if(_current_bounceball || _is_bouncing)
        return;
    
    if(_direction == 0 || _moving_platform)
        _body->SetAngularVelocity(0);
    
    if(!_moving_platform)
        _body->SetLinearVelocity(b2Vec2(_v.x(), _v.y()));
    else
        _body->SetLinearVelocity(b2Vec2(0, 0));
    auto _fg = _g * _body->GetMass();
    _body->ApplyForceToCenter(b2Vec2(_fg.x(), _fg.y()));
}

bool main_character::should_serialize_children() const {
    return false;
}

void main_character::shake_camera(){
    _an_manager->animation(_shake_camera_an)->state = animation_state::playing;
    _an_manager->reset_animation(_shake_camera_an);
}

void main_character::shake_camera(float t){
    float _angle = t * M_PI * 2;
    _camera_x_shake = (1 - t) * SHAKE_MAGNITUDE * sinf(SHAKE_FREQUENCY * _angle);
}

void main_character::bounce_animation(float t, node* current_bounceball, const vec2& original_scale){
    float _angle = t * M_PI * 2;
    float _scale = (1 - t) * BOUNCE_MAGNITUDE * sinf(BOUNCE_FREQUENCY * _angle);
    current_bounceball->transform(current_bounceball->transform().scaled(_scale * vec2(1, 1) + original_scale));
    if(t >= 1)
        current_bounceball->transform(current_bounceball->transform().scaled(original_scale));
}

void main_character::do_bounce_animation(rb::node *current_bounceball){
    if(_bouncing_anims.count(current_bounceball)){
        current_bounceball->transform(current_bounceball->transform().scaled(_bouncing_anims[current_bounceball].original_size));
        if(_bouncing_anims[current_bounceball].anim_id)
            _an_manager->destroy_animation(_bouncing_anims[current_bounceball].anim_id);
        _bouncing_anims.erase(current_bounceball);
    }
    
    animation_info _ai;
    _ai.auto_destroy = true;
    _ai.duration = BOUNCE_ANIM_DURATION;
    _ai.state = animation_state::playing;
    auto _original_scale = current_bounceball->transform().scale();
    _ai.update_function = [current_bounceball, _original_scale](float t, animation_info* ai){
        bounce_animation(t, current_bounceball, _original_scale);
    };
    auto _aid = _an_manager->add_animation(&_ai);
    _bouncing_anims.insert({current_bounceball, {_original_scale, _aid}});
}

void main_character::die_animation(float t){
    if(t <= 0.5f)
        parent_scene()->fade_color(color::from_rgba(1, 0, 0, t * 2));
    else
        parent_scene()->fade_color(color::from_rgba(1, 0, 0, 2 - t * 2));
    if(t >= 0.5f && t < 1.0f && !_resetted){
        resettable_component::reset_components(parent_scene());
        _resetted = true;
    }
    if(t >= 1.0f)
        _resetted = false;
}

void main_character::die(){
    if(_died)
        return;
    
    shake_camera();
    _camera_died = parent_scene()->camera();
    _die_emitter->reset_emitter();
    _die_emitter->state(particle_state::showing);
    _die_emitter->gravity_acceleration(_gravity_died);
    _an_manager->animation(_die_an)->state = animation_state::playing;
    _an_manager->reset_animation(_die_an);
    parent_scene()->fade_color(color::from_rgba(1, 0, 0, 0));
    _parts_velocities.clear();
    auto _parts_count = DESTRUCTION_MATRIX * DESTRUCTION_MATRIX;
    std::mt19937 _generator((uint32_t)clock());
    std::uniform_real_distribution<float> _distribution(0.0f, 1.0f);
    
    for (size_t i = 0; i < _parts_count; i++) {
        auto _angle = (float)(_distribution(_generator) * 2 * M_PI);
        auto _v = _distribution(_generator);
        _v = (1 - _v) * MIN_DESTRUCTION_VELOCITY + _v * MAX_DESTRUCTION_VELOCITY;
        auto _vv = vec2::right.rotated(_angle) * _v;
        _parts_velocities.push_back(_vv);
    }
    _died = true;
}

void main_character::reset_component(){
    _block_to_break = nullptr;
    _sprite->opacity(1);
    for (uint32_t i = 0; i < DESTRUCTION_MATRIX; i++) {
        for (uint32_t j = 0; j < DESTRUCTION_MATRIX; j++) {
            _sprite->transform(i, j, transform_space());
        }
    }
    _previous_direction = 0;
    _current_gZone = nullptr;
    _previous_g = nullptr;
    _direction = _saved_direction;
    _jumpButton = false;
    _didJump = false;
    _jumpCount = 0;
    _cam_focus = nullptr;
    _cam_focus_velocity = 0;
    _frame_count = 0;
    _clear_jump = nullptr;
    _clear_last_moving_platform = nullptr;
    _an_manager->reset_animation(_shake_camera_an);
    _an_manager->animation(_shake_camera_an)->state = animation_state::stopped;
    _camera_x_shake = 0;
    _died = false;
    _gravity_died = vec2::zero;
    _camera_died = transform_space();
    _cam_offset = _saved_cam_offset;
//    _an_manager->reset_animation(_die_an);
//    _an_manager->animation(_die_an)->state = animation_state::stopped;
    _die_emitter->reset_emitter();
    _die_emitter->state(particle_state::hidden);
    _parts_velocities.clear();
    parent_scene()->camera(_saved_camera);
    transform(_saved_transform);
    //reset physics
    auto _t = transform();
    _body->SetTransform(b2Vec2(_t.origin().x(), _t.origin().y()), _t.rotation().x());
    _body->SetLinearVelocity(b2Vec2(0, 0));
    _body->SetAngularVelocity(0);
    //camera constrained
    _camera_constrained = _saved_camera_constrained;
    //bouncing ball
    _is_bouncing = nullptr;
    _current_bounceball = nullptr;
    //ground
    _current_ground = nullptr;
    _normal = nullptr;
    //moving platform
    _moving_platform = nullptr;
    _last_moving_platform = nullptr;
    _block_to_break = nullptr;
}

void main_character::playing(){
    if(!_phys_initialized){
        _direction = _saved_direction;
        auto _t = transform();
        _phys_initialized = true;
        _an_manager = dynamic_cast<animation_manager_component*>(parent_scene()->node_with_name(u"Animation Manager"));
        //shake camera
        animation_info _ai;
        _ai.auto_destroy = false;
        _ai.duration = SHAKE_CAMERA_ANIM_DURATION;
        _ai.state = animation_state::stopped;
        _ai.update_function = [this](float t, animation_info* ai){
            this->shake_camera(t);
        };
        _shake_camera_an = _an_manager->add_animation(&_ai);
        //die animation
        _ai.auto_destroy = false;
        _ai.duration = DESTRUCTION_ANIM_DURATION;
        _ai.state = animation_state::stopped;
        _ai.update_function = [this](float t, animation_info* ai){
            this->die_animation(t);
        };
        _die_an = _an_manager->add_animation(&_ai);
        
        auto _engine = dynamic_cast<physics_engine*>(parent_scene()->node_with_name(u"Physic's Engine"));
        _world = _engine->world();
        _world->SetContactListener(this);
        _default_gravity = _engine->default_gravity();
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
        _fDef.filter.maskBits = PHYS_MASK_GRAVITY_REGION | PHYS_MASK_SHAPE | PHYS_MASK_ENEMY | PHYS_BREAKABLE_BLOCK;
        b2CircleShape _c;
        _c.m_radius = 0.5;
        _fDef.shape = &_c;
        _body->CreateFixture(&_fDef);
        
        //constrained camera
        _saved_cam_offset = _cam_offset;
        _saved_camera_constrained = _camera_constrained;
        auto _polygons = parent_scene()->node_with_one_class(u"cameraPath");
        for (auto _p : _polygons){
            auto _actualP = dynamic_cast<polygon_component*>(_p);
            auto _untransformedP = _actualP->to_smooth_polygon();
            _actualP->from_node_space_to(space::layer).from_space_to_base().transform_polygon(_untransformedP);
            _camera_polygons.push_back(_untransformedP);
        }
        
        auto _initialCamPos = this->transform().origin();
        if(_cam_offset.has_value())
            _initialCamPos += _cam_offset.value();
        parent_scene()->camera(parent_scene()->camera().scaled(_cam_scale.x(), _cam_scale.y()).moved(_initialCamPos));
        if(_camera_constrained){
            auto _closest = getClosestPointFromCameraTrack(_initialCamPos);
            if(_closest.has_value())
                parent_scene()->camera(parent_scene()->camera().scaled(_cam_scale.x(), _cam_scale.y()).moved(_closest.value()));
        }
        
        _saved_camera = parent_scene()->camera();
        _saved_transform = transform();
        
        //bouncing ball
        _is_bouncing = nullptr;
        _current_bounceball = nullptr;
        auto _temp_bounceballs = parent_scene()->node_with_one_class(u"bounceBall");
        for (auto _bb : _temp_bounceballs){
            auto _id = _bb->name();
            auto _bb_dir = parent_scene()->node_with_one_class(_id);
            if(_bb_dir.size() == 0)
                _bouncing_balls.insert({_bb, nullptr});
            else
                _bouncing_balls.insert({_bb, _bb_dir[0]});
        }
        //moving platform
        _moving_platform = nullptr;
        _last_moving_platform = nullptr;
        _normal = nullptr;
        _current_ground = nullptr;
        _block_to_break = nullptr;
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
}

void main_character::keyup(const uint32_t keycode, const rb::keyboard_modifier modifier, bool &swallow){
    swallow = false;
    if(keycode == KEY_LEFT){
        if(_direction == -1)
            _direction = 0;
    }
    else if(keycode == KEY_RIGHT){
        if(_direction == 1)
            _direction = 0;
    }
    else if(keycode == KEY_UP) {
        _jumpButton = false;
        _didJump = false;
    }
}

void main_character::touches_began(const std::vector<touch> &touches, bool &swallow){
    swallow = false;
#if !IOS_TARGET
    return;
#endif
    for (auto& _t : touches){
        auto _np = _invert_xaxis ? -_t.normalized_position() : _t.normalized_position();
        _touches.push_back(std::make_tuple(_t, _np));

        if(_np.x() < 0) //movement control
        {
            _previous_direction = _direction;
            _direction = 0;
        }
        else { //jumping
            if(!this->_clear_jump.has_value()){
                this->_jumpButton = true;
                this->_clear_jump = _frame_count + (uint64_t)(JUMP_TOUCH_DURATION);
            }
        }
    }
}

void main_character::touches_moved(const std::vector<touch> &touches, bool &swallow){
#if !IOS_TARGET
    return;
#endif
    swallow = false;
    //we update velocities
    for (auto& _t : _touches){
        for (auto& t : touches){
            if(!std::get<kTouchIndex>(_t).compare_identity(t))
                continue;
            
            if(std::get<kTouchInitialPos>(_t).x() >= 0)
                continue; //it's a jump touch...
            
            auto _np = _invert_xaxis ? - t.normalized_position() : t.normalized_position();
            auto _movement = _np - std::get<kTouchInitialPos>(_t);
            if(fabsf(_movement.x()) > ZERO_VELOCITY_LENGTH){
                if(_movement.x() > 0){ //right
                    this->_direction = _invert_xaxis ? -1 : 1;
                    this->_previous_direction = _direction;
                }
                else { //left
                    this->_direction = _invert_xaxis ? 1 : -1;
                    this->_previous_direction = _direction;
                }
            }
        }
    }
}

void main_character::touches_ended(const std::vector<touch> &touches, bool &swallow){
#if !IOS_TARGET
    return;
#endif
    swallow = false;
    _touches.erase(std::remove_if(_touches.begin(), _touches.end(), [=](const std::tuple<touch, vec2>& t){
        for (auto& _t : touches){
            if(_t.compare_identity(std::get<kTouchIndex>(t))){
                if(std::get<kTouchInitialPos>(t).x() >= 0)
                    return true; //it's a jump touch...
                this->_direction = this->_previous_direction;
                return true;
            }
        }
        return false;
    }), _touches.end());
}

void main_character::touches_cancelled(const std::vector<touch> &touches, bool &swallow){
#if !IOS_TARGET
    return;
#endif
    touches_ended(touches, swallow);
}

float main_character::damping() const {
    return _damping;
}

float main_character::damping(const float value) {
    return _damping = value;
}

template<class A, class B>
std::tuple<A*, B*> get_objects(b2Contact* contact){
    auto _bA = contact->GetFixtureA()->GetBody();
    auto _bB = contact->GetFixtureB()->GetBody();

    auto _nA = (node*)_bA->GetUserData();
    auto _nB = (node*)_bB->GetUserData();
    
    auto _a = dynamic_cast<A*>(_nA);
    auto _b = dynamic_cast<B*>(_nB);
    
    if(_a && _b)
        return std::make_tuple(_a, _b);
    
    _a = dynamic_cast<A*>(_nB);
    _b = dynamic_cast<B*>(_nA);
    
    if(_a && _b)
        return std::make_tuple(_a, _b);
    
    return std::make_tuple<A*, B*>(nullptr, nullptr);
}

void main_character::BeginContact(b2Contact *contact){
    auto _groundContact = get_objects<main_character, physics_shape>(contact);
    auto _groundContact2 = get_objects<main_character, breakable_block>(contact);
    
    if((std::get<0>(_groundContact) || std::get<0>(_groundContact2)) &&
       (std::get<1>(_groundContact) || std::get<1>(_groundContact2)) &&
        contact->IsEnabled() && contact->IsTouching()){
        
        auto _character = std::get<0>(_groundContact);
        if(!_character)
            _character = std::get<0>(_groundContact2);
        auto _ground = std::get<1>(_groundContact);
        auto _block = std::get<1>(_groundContact2);
        
        if(_ground)
            _ground->main_character_entered();
        
        if(_block && _block->explode_character_contact())
            this->_block_to_break = _block;
        
        //get manifold
        b2WorldManifold manifold;
        contact->GetWorldManifold(&manifold);
        if(((node*)contact->GetFixtureA()->GetBody()->GetUserData()) == (node*)this){
            manifold.normal = -manifold.normal;
        }
        
        if(_ground)
            _contacts[_ground] = manifold;
        else
            _contacts[_block] = manifold;
        
        auto _cond1 = _ground && _ground->shape_type() == physics_shape::kStaticPlanet;
        auto _cond2 = _block != nullptr;
        if(_is_bouncing && (_cond1 || _cond2)){
            _is_bouncing = nullptr;
            _direction = 0;
        }
        
        if((_cond1 || _cond2) && testSlopeAngle(&manifold, _previous_g)){
            //normal pos + current ground
            _normal = vec2(manifold.normal.x, manifold.normal.y);
            if(((node*)contact->GetFixtureA()->GetBody()->GetUserData()) == (node*)this)
                _current_ground = contact->GetFixtureB()->GetBody();
            else
                _current_ground = contact->GetFixtureA()->GetBody();
            //////
            
            if(!_character->_didJump)
                _character->_jumpCount = PHYS_CHARACTER_JUMP_COUNT;
        }
    }
}

bool main_character::testSlopeAngle(b2WorldManifold *manifold, const nullable<rb::vec2> &gravity) const{
    if(!manifold)
        return true;
    
    auto _normal = vec2(manifold->normal.x, manifold->normal.y).normalized();
    auto _angle = gravity.has_value() ?
    (nullable<float>)vec2::dot(gravity.value().normalized().rotated90().rotated90(), _normal) : (nullable<float>)nullptr; //angle between gravity(rotated 180 degrees) and normal.
    auto _maxAngle = cosf(TO_RADIANS(60));
    
    return (!_angle.has_value() || _angle.value() >= _maxAngle);
}

void main_character::EndContact(b2Contact *contact){
    auto _groundContact = get_objects<main_character, physics_shape>(contact);
    auto _groundContact2 = get_objects<main_character, breakable_block>(contact);
    
    if(((std::get<0>(_groundContact) && std::get<1>(_groundContact)) ||
        (std::get<0>(_groundContact2) && std::get<1>(_groundContact2)))
        && contact->IsEnabled()){
        
        if(std::get<1>(_groundContact))
            std::get<1>(_groundContact)->main_character_exitted();
        
        if(_current_ground == contact->GetFixtureA()->GetBody() ||
           _current_ground == contact->GetFixtureB()->GetBody()){
            _normal = nullptr;
            _current_ground = nullptr;
        }
        
        auto _ground = std::get<1>(_groundContact);
        auto _block = std::get<1>(_groundContact2);
        if(_ground)
            _contacts.erase(_ground);
        else
            _contacts.erase(_block);
        
        if(_ground && _ground->shape_type() == physics_shape::kStaticGravityZone){
            _ground->_active_gravity = true;
        }
    }
}

void main_character::PreSolve(b2Contact *contact, const b2Manifold *oldManifold){
//    BeginContact(contact);
}

void main_character::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse){
//    EndContact(contact);
}

void main_character::do_action(const rb_string& action_name, const rb_string& arg){
    
}








































