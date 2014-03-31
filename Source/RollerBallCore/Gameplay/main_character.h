//
//  main_character.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 13/08/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__main_character__
#define __RollerBallCore__main_character__

#include "components_base.h"
#include "node.h"
#include "circle.h"
#include "touch.h"
#include "resettable_component.h"
#include <Box2D/Box2D.h>

class b2World;
class b2Body;

namespace rb {
    class destructible_sprite_component;
    class physics_shape;
    class animation_manager_component;
    class particle_emitter_component;
    class base_enemy;
    typedef void* animation_id;
    class main_character : public node, public b2ContactListener, public resettable_component {
    private:
        //touches
        std::vector<std::tuple<touch, uint32_t, vec2>> _touches; //current touches (touch, duration in frames, last position, velocity)
        uint32_t _ended_touches;
        //others
        destructible_sprite_component* _sprite;
        physics_shape* _current_gZone;
        b2World* _world;
        b2Body* _body;
        vec2 _default_gravity;
        bool _phys_initialized;
        float _direction;
        nullable<vec2> _previous_g;
        float _damping;
        //Jumping
        bool _jumpButton;
        bool _didJump;
        uint32_t _jumpCount;
        //Reverse Jumping
        bool _rev_jumpButton;
        bool _rev_didJump;
        //Camera
        nullable<circle> _cam_focus;
        float _cam_focus_velocity;
        vec2 _cam_scale;
        //Frame
        uint64_t _frame_count;
        nullable<uint64_t> _clear_jump;
        nullable<uint64_t> _clear_rev_jump;
        //shake animation
        animation_manager_component* _an_manager;
        animation_id _shake_camera_an;
        float _camera_x_shake;
        //dying
        bool _died;
        vec2 _gravity_died;
        transform_space _camera_died;
        animation_id _die_an;
        particle_emitter_component* _die_emitter;
        std::vector<vec2> _parts_velocities;
        //resetting
        bool _resetted;
        transform_space _saved_camera;
        transform_space _saved_transform;
    private:
        bool testSlopeAngle(b2WorldManifold* manifold, const nullable<vec2>& gravity) const;
    protected:
        virtual void reset_component() override;
    private:
        void shake_camera(float t);
        void die_animation(float t);
    public:
        main_character();
        ~main_character();
    protected:
        //Typed Object
        virtual void describe_type() override;
        virtual void after_becoming_active(bool node_was_moved) override;
        virtual void before_becoming_inactive(bool node_was_moved) override;
    protected:
        virtual bool should_serialize_children() const override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
    private:
        void update_character(vec2& cam_gravity);
        void update_camera(const vec2& cam_gravity);
    protected:
        //Update
        virtual void update(float dt) override;
        virtual void playing() override;
    public:
        const rb_string& image_name() const;
        const rb_string& image_name(const rb_string& value);
        float damping() const;
        float damping(const float value);
        //keys
    protected:
        virtual void keydown(const uint32_t keycode, const keyboard_modifier modifier, bool& swallow);
        virtual void keyup(const uint32_t keycode, const keyboard_modifier modifier, bool& swallow);
    public:
        virtual void BeginContact(b2Contact* contact) override;
        virtual void EndContact(b2Contact* contact) override;
        virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
        virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;
        //touches
    private:
        nullable<vec2> resting_touches();
    protected:
        virtual void touches_began(const std::vector<touch>& touches, bool& swallow) override;
        virtual void touches_moved(const std::vector<touch>& touches, bool& swallow) override;
        virtual void touches_ended(const std::vector<touch>& touches, bool& swallow) override;
        virtual void touches_cancelled(const std::vector<touch>& touches, bool& swallow) override;
        //die
    public:
        void die();
        bool check_die();
        //shake camera
    public:
        void shake_camera();
        const vec2& camera_scale() const;
        const vec2& camera_scale(const vec2& value);
    };
}

#endif /* defined(__RollerBallCore__main_character__) */
