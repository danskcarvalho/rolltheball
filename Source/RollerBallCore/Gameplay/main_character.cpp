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
#include <random>
#include <Box2D/Box2D.h>

//Some constants
#define PHYS_CHARACTER_VELOCITY 5.5f
#define PHYS_CHARACTER_ACCEL (PHYS_CHARACTER_VELOCITY / (0.25f * 30.0f))
#define PHYS_CHARACTER_JUMP 5.5f
#define PHYS_CHARACTER_JUMP_COUNT 1
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

#define DESTRUCTION_MATRIX 6
#define DESTRUCTION_ANIM_DURATION 1.0f
#define MAX_DESTRUCTION_VELOCITY 6
#define MIN_DESTRUCTION_VELOCITY 4

#define EXPLOSION_GRAVITY_MAGNITUDE 25

using namespace rb;

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
    string_property<main_character>(u"camera_class", u"Cam Class Pol", true, false, {
        [](const main_character* site){
            return site->_camera_class;
        },
        [](main_character* site, const rb_string& value){
            site->_camera_class = value;
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

inline float sgn(float v){
    if(v == 0)
        return 0;
    return v / fabsf(v);
}

inline float signed_length(const vec2& v, const vec2& axis){
    return v.length() * sgn(vec2::dot(v, axis));
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

void main_character::update(float dt){
    if(_died){
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
        return;
    }

    if(check_die())
        return;
    
    vec2 _cam_g;
    update_character(_cam_g);
    
    this->transform(this->transform().moved(_body->GetPosition().x, _body->GetPosition().y).rotated(_body->GetAngle(), _body->GetAngle() + M_PI_2));
    if(_cam_g != vec2::zero)
        update_camera(_cam_g);
    
    _frame_count++;
    if(_clear_jump.has_value() && _clear_jump.value() <= _frame_count){
        _jumpButton = false;
        _didJump = false;
        _clear_jump = nullptr;
    }
}

nullable<vec2> main_character::getClosestPointFromCameraTrack(const rb::vec2& charPos) const{
    nullable<vec2> _closestPoint = nullptr;
    float _distance = std::numeric_limits<float>::max();
    for (auto _p : _camera_polygons){
        uint32_t _index;
        auto _e = _p.closest_edge(charPos, _index);
        auto _d = _e.distance_vector(charPos);
        if(_d.length() < _distance)
            _closestPoint = charPos - _d;
    }
    
    return _closestPoint;
}

void main_character::update_camera(const rb::vec2 &cam_gravity){
    auto _thisPosition = this->transform().origin();
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

void main_character::update_character(vec2& cam_gravity){
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
    
    if (_current_gZone)
        _g = _current_gZone->planet()->gravity_vector(transform().origin(), cam_gravity);
    
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
//        auto _rt = resting_touches();
//        if(_rt.has_value() && _rt.value() == vec2::zero){
//            _direction = 0;
//        }
        
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
            //_vx *= _damping;
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
    }
    
    if(_direction == 0)
        _body->SetAngularVelocity(0);
    
    _body->SetLinearVelocity(b2Vec2(_v.x(), _v.y()));
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
    std::mt19937 _generator;
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
    _an_manager->reset_animation(_shake_camera_an);
    _an_manager->animation(_shake_camera_an)->state = animation_state::stopped;
    _camera_x_shake = 0;
    _died = false;
    _gravity_died = vec2::zero;
    _camera_died = transform_space();
//    _an_manager->reset_animation(_die_an);
//    _an_manager->animation(_die_an)->state = animation_state::stopped;
    _die_emitter->reset_emitter();
    _die_emitter->state(particle_state::stopped);
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
        _fDef.filter.maskBits = PHYS_MASK_GRAVITY_REGION | PHYS_MASK_SHAPE | PHYS_MASK_ENEMY;
        b2CircleShape _c;
        _c.m_radius = 0.5;
        _fDef.shape = &_c;
        _body->CreateFixture(&_fDef);
        
        parent_scene()->camera(parent_scene()->camera().scaled(_cam_scale.x(), _cam_scale.y()).moved(this->transform().origin()));
        if(_camera_constrained){
            auto _closest = getClosestPointFromCameraTrack(this->transform().origin());
            if(_closest.has_value())
                parent_scene()->camera(parent_scene()->camera().scaled(_cam_scale.x(), _cam_scale.y()).moved(_closest.value()));
        }
        
        _saved_camera = parent_scene()->camera();
        _saved_transform = transform();
        
        //constrained camera
        _saved_camera_constrained = _camera_constrained;
        auto _polygons = parent_scene()->node_with_one_class(u"cameraPath");
        for (auto _p : _polygons){
            auto _actualP = dynamic_cast<polygon_component*>(_p);
            auto _untransformedP = _actualP->to_smooth_polygon();
            _actualP->from_node_space_to(space::layer).from_space_to_base().transform_polygon(_untransformedP);
            _camera_polygons.push_back(_untransformedP);
        }
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
    if(keycode == KEY_LEFT || keycode == KEY_RIGHT)
        _direction = 0;
    else if(keycode == KEY_UP) {
        _jumpButton = false;
        _didJump = false;
    }
}

void main_character::touches_began(const std::vector<touch> &touches, bool &swallow){
    swallow = false;
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
    
    if(std::get<0>(_groundContact) && std::get<1>(_groundContact)){
        auto _character = std::get<0>(_groundContact);
        auto _ground = std::get<1>(_groundContact);
        
        //get manifold
        b2WorldManifold manifold;
        contact->GetWorldManifold(&manifold);
        
        if(_ground->shape_type() == physics_shape::kStaticPlanet && testSlopeAngle(&manifold, _previous_g)){
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
    auto _maxAngle = cosf(TO_RADIANS(45));
    
    return (!_angle.has_value() || _angle.value() >= _maxAngle);
}

void main_character::EndContact(b2Contact *contact){
    auto _groundContact = get_objects<main_character, physics_shape>(contact);
    
    if(std::get<0>(_groundContact) && std::get<1>(_groundContact)){
        auto _ground = std::get<1>(_groundContact);
        
        if(_ground->shape_type() == physics_shape::kStaticGravityZone){
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








































