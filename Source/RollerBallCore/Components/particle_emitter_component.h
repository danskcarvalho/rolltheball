//
//  particle_emitter.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 28/12/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__particle_emitter__
#define __RollerBallCore__particle_emitter__

#include "components_base.h"
#include "node.h"
#include "particle_layer.h"

namespace rb {
    class vec2;
    class color;
    class particle_emitter_component : public node {
    private:
        emitter_info _ei;
        emitter_id _id;
        particle_state _state;
        float _editor_duration;
        float _editor_delay;
        float _editor_loop;
    protected:
        //returns the bounds in self space...
        virtual rectangle bounds() const override;
    protected:
        virtual void after_becoming_active(bool node_was_moved) override;
        virtual void before_becoming_inactive(bool node_was_moved) override;
        //Playing/Pause
    protected:
        virtual void playing() override;
        virtual void paused() override;
    protected:
        //Typed Object
        virtual void describe_type() override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
        virtual void render(const bool refill_buffers) override;
        //Constructor and Destructors
    public:
        particle_emitter_component();
        ~particle_emitter_component();
    public:
        //Actions
        void reset_emitter();
        //Properties
    public:
        float velocity_angle_01() const;
        float velocity_angle_01(float value);
        float velocity_angle_02() const;
        float velocity_angle_02(float value);
        float velocity_01() const;
        float velocity_01(float value);
        float velocity_02() const;
        float velocity_02(float value);
        float velocity_rate_01() const;
        float velocity_rate_01(float value);
        float velocity_rate_02() const;
        float velocity_rate_02(float value);
        float acceleration_angle_01() const;
        float acceleration_angle_01(float value);
        float acceleration_angle_02() const;
        float acceleration_angle_02(float value);
        float acceleration_01() const;
        float acceleration_01(float value);
        float acceleration_02() const;
        float acceleration_02(float value);
        float tangential_velocity_01() const;
        float tangential_velocity_01(float value);
        float tangential_velocity_02() const;
        float tangential_velocity_02(float value);
        float tangential_acceleration_01() const;
        float tangential_acceleration_01(float value);
        float tangential_acceleration_02() const;
        float tangential_acceleration_02(float value);
        float perpendicular_velocity_01() const;
        float perpendicular_velocity_01(float value);
        float perpendicular_velocity_02() const;
        float perpendicular_velocity_02(float value);
        float perpendicular_acceleration_01() const;
        float perpendicular_acceleration_01(float value);
        float perpendicular_acceleration_02() const;
        float perpendicular_acceleration_02(float value);
        vec2 gravity_acceleration() const;
        vec2 gravity_acceleration(const vec2& value);
        float angle_01() const;
        float angle_01(float value);
        float angle_02() const;
        float angle_02(float value);
        float radial_velocity_01() const;
        float radial_velocity_01(float value);
        float radial_velocity_02() const;
        float radial_velocity_02(float value);
        float radial_acceleration_01() const;
        float radial_acceleration_01(float value);
        float radial_acceleration_02() const;
        float radial_acceleration_02(float value);
        bool aspect_correction() const;
        bool aspect_correction(bool value);
        vec2 size_01() const;
        vec2 size_01(const vec2& value);
        vec2 size_02() const;
        vec2 size_02(const vec2& value);
        vec2 size_rate_01() const;
        vec2 size_rate_01(const vec2& value);
        vec2 size_rate_02() const;
        vec2 size_rate_02(const vec2& value);
        bool uniform_size() const;
        bool uniform_size(bool value);
        vec2 max_size() const;
        vec2 max_size(const vec2& value);
        const rb_string& image_name() const;
        const rb_string& image_name(const rb_string& value);
        float opacity_01() const;
        float opacity_01(float value);
        float opacity_02() const;
        float opacity_02(float value);
        float opacity_rate_01() const;
        float opacity_rate_01(float value);
        float opacity_rate_02() const;
        float opacity_rate_02(float value);
        float blend_01() const;
        float blend_01(float value);
        float blend_02() const;
        float blend_02(float value);
        float blend_rate_01() const;
        float blend_rate_01(float value);
        float blend_rate_02() const;
        float blend_rate_02(float value);
        color start_color_01() const;
        color start_color_01(const color& value);
        color start_color_02() const;
        color start_color_02(const color& value);
        color end_color_01() const;
        color end_color_01(const color& value);
        color end_color_02() const;
        color end_color_02(const color& value);
        float start_color_mix_01() const;
        float start_color_mix_01(float value);
        float start_color_mix_02() const;
        float start_color_mix_02(float value);
        float color_mixing_velocity_01() const;
        float color_mixing_velocity_01(float value);
        float color_mixing_velocity_02() const;
        float color_mixing_velocity_02(float value);
        float life_01() const;
        float life_01(float value);
        float life_02() const;
        float life_02(float value);
        float inv_emission_rate() const;
        float inv_emission_rate(float value);
        float duration() const;
        float duration(float value);
        float delay() const;
        float delay(float value);
        float editor_duration() const;
        float editor_duration(float value);
        float editor_delay() const;
        float editor_delay(float value);
        float emission_radius_01() const;
        float emission_radius_01(float value);
        float emission_radius_02() const;
        float emission_radius_02(float value);
        bool loop() const;
        bool loop(bool value);
        bool editor_loop() const;
        bool editor_loop(bool value);
        particle_state state() const;
        particle_state state(particle_state value);
    };
}

#endif /* defined(__RollerBallCore__particle_emitter__) */
