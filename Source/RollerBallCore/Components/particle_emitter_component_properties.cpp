//
//  particle_emitter_component_properties.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 30/12/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#include "particle_emitter_component.h"
#include "vec2.h"
#include "color.h"

using namespace rb;

#define VEC2_PROPERTY(name, desc) \
vec2_property<particle_emitter_component>(u###name, u##desc, true, { \
[](const particle_emitter_component* site){ \
    return site->name(); \
}, \
[](particle_emitter_component* site, const vec2& value){ \
    site->name(value); \
} \
})
#define COLOR_PROPERTY(name, desc) \
color_property<particle_emitter_component>(u###name, u##desc, true, true, { \
[](const particle_emitter_component* site){ \
    return site->name(); \
}, \
[](particle_emitter_component* site, const color& value){ \
    site->name(value); \
} \
})
#define FLOAT_PROPERTY(name, desc) \
single_property<particle_emitter_component>(u###name, u##desc, true, { \
[](const particle_emitter_component* site){ \
    return site->name(); \
}, \
[](particle_emitter_component* site, float value){ \
    site->name(value); \
} \
})
#define ANGLE_PROPERTY(name, desc) \
single_property<particle_emitter_component>(u###name, u##desc, true, { \
[](const particle_emitter_component* site){ \
    return TO_DEGREES(site->name()); \
}, \
[](particle_emitter_component* site, float value){ \
    site->name(TO_RADIANS(value)); \
} \
})
#define BOOL_PROPERTY(name, desc) \
boolean_property<particle_emitter_component>(u###name, u##desc, true, { \
[](const particle_emitter_component* site){ \
return site->name(); \
}, \
[](particle_emitter_component* site, bool value){ \
site->name(value); \
} \
})

void particle_emitter_component::describe_type(){
    node::describe_type();
    start_type<particle_emitter_component>([](){ return new particle_emitter_component(); });
    action<particle_emitter_component>(u"reset", u"Action", action_flags::multi_dispatch, { u"Reset" },
        [](particle_emitter_component* site, const rb_string& action_name){
            site->reset_emitter();
        });
    enumeration_property<particle_emitter_component, particle_state>(u"state", u"State", {{u"Stopped", particle_state::stopped}, {u"Hidden", particle_state::hidden}, {u"Paused", particle_state::paused}, {u"Showing", particle_state::showing}}, true, {
        [](const particle_emitter_component* site){
            return site->state();
        },
        [](particle_emitter_component* site, particle_state value){
            site->state(value);
        }
    });
    image_property<particle_emitter_component>(u"image_name", u"Image Name", true, {
        [](const particle_emitter_component* site){
            return site->image_name();
        },
        [](particle_emitter_component* site, const rb_string& value){
            site->image_name(value);
        }
    });
    ANGLE_PROPERTY(velocity_angle_01, "Vel Angle 01");
    ANGLE_PROPERTY(velocity_angle_02, "Vel Angle 02");
    FLOAT_PROPERTY(velocity_01, "Vel 01");
    FLOAT_PROPERTY(velocity_02, "Vel 02");
    FLOAT_PROPERTY(velocity_rate_01, "Vel Rate 01");
    FLOAT_PROPERTY(velocity_rate_02, "Vel Rate 02");
    ANGLE_PROPERTY(acceleration_angle_01, "Accel Angle 01");
    ANGLE_PROPERTY(acceleration_angle_02, "Accel Angle 02");
    FLOAT_PROPERTY(acceleration_01, "Accel 01");
    FLOAT_PROPERTY(acceleration_02, "Accel 02");
    FLOAT_PROPERTY(tangential_velocity_01, "Tg Vel 01");
    FLOAT_PROPERTY(tangential_velocity_02, "Tg Vel 02");
    FLOAT_PROPERTY(tangential_acceleration_01, "Tg Accel 01");
    FLOAT_PROPERTY(tangential_acceleration_02, "Tg Accel 02");
    FLOAT_PROPERTY(perpendicular_velocity_01, "Perp Vel 01");
    FLOAT_PROPERTY(perpendicular_velocity_02, "Perp Vel 02");
    FLOAT_PROPERTY(perpendicular_acceleration_01, "Perp Accel 01");
    FLOAT_PROPERTY(perpendicular_acceleration_02, "Perp Accel 02");
    VEC2_PROPERTY(gravity_acceleration, "Grv Accel");
    ANGLE_PROPERTY(angle_01, "Angle 01");
    ANGLE_PROPERTY(angle_02, "Angle 02");
    ANGLE_PROPERTY(radial_velocity_01, "Rad Vel 01");
    ANGLE_PROPERTY(radial_velocity_02, "Rad Vel 02");
    ANGLE_PROPERTY(radial_acceleration_01, "Rad Accel 01");
    ANGLE_PROPERTY(radial_acceleration_02, "Rad Accel 02");
    BOOL_PROPERTY(aspect_correction, "Aspect Correction");
    VEC2_PROPERTY(size_01, "Size 01");
    VEC2_PROPERTY(size_02, "Size 02");
    VEC2_PROPERTY(size_rate_01, "Size Rate 01");
    VEC2_PROPERTY(size_rate_02, "Size Rate 02");
    BOOL_PROPERTY(uniform_size, "Uniform Size");
    VEC2_PROPERTY(max_size, "Max Size");
    FLOAT_PROPERTY(opacity_01, "Opacity 01");
    FLOAT_PROPERTY(opacity_02, "Opacity 02");
    FLOAT_PROPERTY(opacity_rate_01, "Opacity Rate 01");
    FLOAT_PROPERTY(opacity_rate_02, "Opacity Rate 02");
    FLOAT_PROPERTY(blend_01, "Blend 01");
    FLOAT_PROPERTY(blend_02, "Blend 02");
    FLOAT_PROPERTY(blend_rate_01, "Blend Rate 01");
    FLOAT_PROPERTY(blend_rate_02, "Blend Rate 02");
    COLOR_PROPERTY(start_color_01, "St Color 01");
    COLOR_PROPERTY(start_color_02, "St Color 02");
    COLOR_PROPERTY(end_color_01, "End Color 01");
    COLOR_PROPERTY(end_color_02, "End Color 02");
    FLOAT_PROPERTY(start_color_mix_01, "St Color Mix 01");
    FLOAT_PROPERTY(start_color_mix_02, "St Color Mix 02");
    FLOAT_PROPERTY(color_mixing_velocity_01, "Color Mix Vel 01");
    FLOAT_PROPERTY(color_mixing_velocity_02, "Color Mix Vel 02");
    FLOAT_PROPERTY(life_01, "Life 01");
    FLOAT_PROPERTY(life_02, "Life 02");
    FLOAT_PROPERTY(inv_emission_rate, "Inv Em Rate");
    FLOAT_PROPERTY(emission_radius_01, "Em Radius 01");
    FLOAT_PROPERTY(emission_radius_02, "Em Radius 02");
    FLOAT_PROPERTY(duration, "Duration");
    FLOAT_PROPERTY(editor_duration, "Ed Duration");
    FLOAT_PROPERTY(delay, "Delay");
    FLOAT_PROPERTY(editor_delay, "Ed Delay");
    BOOL_PROPERTY(loop, "Loop");
    BOOL_PROPERTY(editor_loop, "Ed Loop");
    end_type();
}






























