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
#include <Box2D/Box2D.h>

class b2World;
class b2Body;

namespace rb {
    class sprite_component;
    class physics_shape;
    class main_character : public node, public b2ContactListener {
    private:
        //touches
        std::vector<std::tuple<touch, uint32_t, vec2>> _touches; //current touches (touch, duration in frames, last position, velocity)
        uint32_t _ended_touches;
        //others
        sprite_component* _sprite;
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
        //Frame
        uint64_t _frame_count;
        nullable<uint64_t> _clear_jump;
        nullable<uint64_t> _clear_rev_jump;
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
    };
}

#endif /* defined(__RollerBallCore__main_character__) */