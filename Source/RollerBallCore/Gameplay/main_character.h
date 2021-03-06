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
#include "polygon.h"
#include "action_target.h"
#include <Box2D/Box2D.h>

class b2World;
class b2Body;

namespace rb {
    class destructible_sprite_component;
    class physics_shape;
    class animation_manager_component;
    class particle_emitter_component;
    class base_enemy;
    class sprite_component;
    class physics_engine;
    typedef void* animation_id;
    class main_character : public node, public b2ContactListener, public resettable_component, public action_target {
    private:
        //touches
        std::vector<std::tuple<touch, vec2>> _touches; //current touches (touch, Initial position)
        bool _invert_xaxis;
        float _saved_direction;
        float _previous_direction;
        //others
        destructible_sprite_component* _sprite;
        physics_shape* _current_gZone;
        b2World* _world;
        b2Body* _body;
        physics_engine* _engine;
        bool _phys_initialized;
        float _direction;
        nullable<vec2> _previous_g;
        float _damping;
        std::unordered_map<node*, b2WorldManifold> _contacts;
        //Jumping
        bool _jumpButton;
        bool _didJump;
        uint32_t _jumpCount;
        float _gravity_mult;
        float _jump_velocity;
        //Camera
        nullable<vec2> _cam_offset;
        nullable<vec2> _saved_cam_offset;
        nullable<circle> _cam_focus;
        float _cam_focus_velocity;
        vec2 _cam_scale;
        //Frame
        uint64_t _frame_count;
        nullable<uint64_t> _clear_jump;
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
        //constrained camera
        bool _camera_constrained;
        bool _saved_camera_constrained;
        std::vector<polygon> _camera_polygons;
        //bounce balls
        //relates bouncing balls to its direction...
        std::unordered_map<node*, node*> _bouncing_balls;
        struct BouncingBallInfo {
            vec2 original_size;
            animation_id anim_id;
        };
        std::unordered_map<node*, BouncingBallInfo> _bouncing_anims;
        node* _current_bounceball;
        node* _is_bouncing;
        vec2 _local_ballPos;
        bool _current_bouncehasdir; //has direction?
        bool _current_autobounce; //is autobounce?
        float _bounce_velocity;
        //breaking block
        node* _block_to_break;
        //ground
        nullable<vec2> _normal;
        b2Body* _current_ground;
        //moving platform
        bool _moving_first;
        physics_shape* _moving_platform;
        physics_shape* _last_moving_platform;
        nullable<uint64_t> _clear_last_moving_platform;
        float _moving_t;
        float _moving_max_t;
        vec2 _up_vector;
        float _moving_vel;
        float _rotation_vel;
        //coins and hearts
        float _fixed_coins;
        float _nonfixed_coins;
        float _hearts;
        float _saved_coin_scale;
        std::unordered_set<sprite_component*> _coins;
        std::unordered_set<sprite_component*> _taken_coins;
        std::unordered_set<sprite_component*> _fixed_taken_coins;
        //free jump zone
        bool _inside_jump_zone;
        //debug mode
        bool _debug_mode;
        //win-zone
        node* _win_zone;
        bool _won;
        vec2 _saved_position_at_win;
        bool _full_win_an;
        //win animation
        animation_id _win_an;
        //force zones
        vec2 _velocity_force_zones;
        bool _zero_gravity;
        //deaths
        float _deaths;
        //time
        float _fixed_time;
        float _time;
    private:
        void check_win();
        void win_animation(float t);
    public:
        bool full_win_animation() const;
        bool full_win_animation(bool value);
    private:
        bool testSlopeAngle(b2WorldManifold* manifold, const nullable<vec2>& gravity) const;
        //get closest point from camera track...
        nullable<vec2> getClosestPointFromCameraTrack(const vec2& charPos) const;
    protected:
        virtual void reset_physics() override;
        virtual void reset_component() override;
    private:
        void check_new_moving_platform(vec2& v, nullable<float>& rot_vel);
        void check_bouncing();
        static void bounce_animation(float t, node* current_bounceball, const vec2& original_scale);
        void do_bounce_animation(node* current_bounceball);
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
        void update_movingplatform(vec2& v, nullable<float>& rot_vel, float dt, float direction);
        void update_died(float dt);
        void update_bounceball(float dt);
        void update_character(vec2& cam_gravity, float dt);
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
    protected:
        virtual void touches_began(const std::vector<touch>& touches, bool& swallow) override;
        virtual void touches_moved(const std::vector<touch>& touches, bool& swallow) override;
        virtual void touches_ended(const std::vector<touch>& touches, bool& swallow) override;
        virtual void touches_cancelled(const std::vector<touch>& touches, bool& swallow) override;
        //die
    public:
        void die();
        bool check_die(float dt);
        //shake camera
    public:
        void shake_camera();
        const vec2& camera_scale() const;
        const vec2& camera_scale(const vec2& value);
    public:
        virtual void do_action(const rb_string& action_name, const rb_string& arg) override;
        //coins and hearts
    public:
        float fixed_coins() const;
        float fixed_coins(float value);
        float nonfixed_coins() const;
        float nonfixed_coins(float value);
        float hearts() const;
        float hearts(float value);
        //methods for coins
    private:
        void check_for_coins();
    };
}

#endif /* defined(__RollerBallCore__main_character__) */
