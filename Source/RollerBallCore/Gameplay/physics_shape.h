//
//  physics_shape.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 16/08/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__physics_shape__
#define __RollerBallCore__physics_shape__

#include "components_base.h"
#include "polygon_component.h"
#include "resettable_component.h"
#include "action_target.h"

class b2World;
class b2Body;

namespace rb {
    class main_character;
    class ray;
//    class position_animator_component;
    class physics_shape : public polygon_component, public resettable_component, public action_target {
//    public:
//        friend class position_animator_component;
    protected:
        virtual void reset_component() override;
        virtual void reset_physics() override;
    public:
        enum type {
            kStaticGravityZone = 0,
            kStaticPlanet = 1,
            kNothing = 2
        };
    private:
        float _gravity;
        type _type;
        b2World* _world;
        b2Body* _body;
        bool _phys_initialized;
        rb_string _planet_name;
        physics_shape* _planet;
        polygon _cached_pol;
        uint32_t _priority;
        bool _active_gravity;
        bool _invert_velocity;
        nullable<rb_string> _gravity_ref;
        node* _gravity_ref_node;
        //saving and animating
        transform_space _saved_transform;
        bool _animatable;
        //moving platform
        bool _moving_platform;
        vec2 _pt0;
        vec2 _pt1;
        //action-fire
        bool _fire_action_once;
        bool _fired_on_enter;
        bool _fired_on_exit;
        rb_string _on_enter_action_buffer;
        rb_string _on_enter_action_name;
        rb_string _on_exit_action_buffer;
        rb_string _on_exit_action_name;
        //phase through
        bool _phase_through;
        //free jump zone
        bool _free_jump_zone;
        //auto-move_character
        float _auto_move_dir;
        float _direction_on_jumping;
        //texture animation
        vec2 _tex_transform_anim;
        //force zone
        bool _force_zone;
        vec2 _force_one;
        vec2 _force_two;
        float _max_velocity;
        bool _zero_gravity;
    public:
        friend class main_character;
        physics_shape();
        ~physics_shape();
    protected:
        //Typed Object
        virtual void describe_type() override;
        virtual void after_becoming_active(bool node_was_moved) override;
        virtual void before_becoming_inactive(bool node_was_moved) override;
    protected:
        //Update
        virtual void update(float dt);
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
    protected:
        virtual void playing() override;
    public:
        void main_character_entered();
        void main_character_exitted();
        bool animatable() const;
        bool animatable(bool value);
        type shape_type() const;
        type shape_type(const type value);
        float gravity() const;
        float gravity(const float value);
        vec2 gravity_vector(const vec2& position, vec2& cam_gravity);
        const rb_string& planet_name() const;
        const rb_string& planet_name(const rb_string& value);
        physics_shape* planet() const;
        uint32_t priority() const;
        uint32_t priority(const uint32_t value);
        bool invert_velocity() const;
        bool invert_velocity(const bool value);
        const nullable<rb_string>& gravity_reference() const;
        const nullable<rb_string>& gravity_reference(const nullable<rb_string>& value);
        bool phase_through() const;
        bool phase_through(bool value);
        bool free_jump_zone() const;
        bool free_jump_zone(bool value);
        //moving platform
    private:
        void check_moving_platform();
    public:
        bool is_moving_platform() const;
        vec2 get_normal() const;
        vec2 get_pt0() const;
        vec2 get_pt1() const;
        ray get_ray() const;
        vec2 get_velocity_at_pt(const vec2& pt) const;
        b2Body* get_body();
        //force zone
    public:
        bool is_force_zone() const;
        vec2 get_force(const vec2& worldPt) const;
    public:
        virtual void do_action(const rb_string& action_name, const rb_string& arg) override;
    };
}


#endif /* defined(__RollerBallCore__physics_shape__) */
