//
//  base_enumerations.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 02/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef RollerBallCore_base_enumerations__
#define RollerBallCore_base_enumerations__

#include "stringification.h"

namespace rb {
    enum class rotation_direction {
        cw = 1,
        ccw = 2,
        shortest = 4
    };

    enum class transform_direction {
        from_space_to_base = 1,
        from_base_to_space = 2
    };
    
    enum class corner_type {
        bevel = 1,
        miter = 2
    };
    
    enum class point_ordering {
        cw = 1,
        ccw = 2,
        unknown = 4
    };
    
    enum class blend_mode {
        normal = 0,
        multiply = 1,
        screen = 2,
        copy_source = 3,
        preserve_destination = 4,
        add = 5
    };
    
    enum class geometry_type {
        triangle = 0,
        line = 1
    };
    
    enum class texture_border {
        none = 0,
        empty = 1,
        clamp = 2,
        repeat = 3
    };
    
    enum class texture_layer_blend_mode {
        normal = 1,
        multiply = 2,
        screen = 3,
        overlay = 4,
        darken = 5,
        lighten = 6,
        hue = 7,
        saturation = 8,
        color = 9,
        luminosity = 10
    };
    
    enum class mesh_group_order {
        front = 0,
        back = 1
    };
    
    enum class texture_mapping_type {
        transformable,
        untransformable
    };
}

BEGIN_STRINGIFY_ENUM(rb::texture_border)
    STRINGIFY_ENUM_VALUE(rb::texture_border::none)
    STRINGIFY_ENUM_VALUE(rb::texture_border::empty)
    STRINGIFY_ENUM_VALUE(rb::texture_border::clamp)
    STRINGIFY_ENUM_VALUE(rb::texture_border::repeat)
END_STRINGIFY_ENUM()

BEGIN_STRINGIFY_ENUM(rb::texture_mapping_type)
    STRINGIFY_ENUM_VALUE(rb::texture_mapping_type::transformable)
    STRINGIFY_ENUM_VALUE(rb::texture_mapping_type::untransformable)
END_STRINGIFY_ENUM()

BEGIN_STRINGIFY_ENUM(rb::mesh_group_order)
    STRINGIFY_ENUM_VALUE(rb::mesh_group_order::front)
    STRINGIFY_ENUM_VALUE(rb::mesh_group_order::back)
END_STRINGIFY_ENUM()

BEGIN_STRINGIFY_ENUM(rb::texture_layer_blend_mode)
    STRINGIFY_ENUM_VALUE(rb::texture_layer_blend_mode::normal)
    STRINGIFY_ENUM_VALUE(rb::texture_layer_blend_mode::multiply)
    STRINGIFY_ENUM_VALUE(rb::texture_layer_blend_mode::screen)
    STRINGIFY_ENUM_VALUE(rb::texture_layer_blend_mode::overlay)
    STRINGIFY_ENUM_VALUE(rb::texture_layer_blend_mode::darken)
    STRINGIFY_ENUM_VALUE(rb::texture_layer_blend_mode::lighten)
    STRINGIFY_ENUM_VALUE(rb::texture_layer_blend_mode::hue)
    STRINGIFY_ENUM_VALUE(rb::texture_layer_blend_mode::saturation)
    STRINGIFY_ENUM_VALUE(rb::texture_layer_blend_mode::color)
    STRINGIFY_ENUM_VALUE(rb::texture_layer_blend_mode::luminosity)
END_STRINGIFY_ENUM()

BEGIN_STRINGIFY_ENUM(rb::rotation_direction)
    STRINGIFY_ENUM_VALUE(rb::rotation_direction::cw)
    STRINGIFY_ENUM_VALUE(rb::rotation_direction::ccw)
    STRINGIFY_ENUM_VALUE(rb::rotation_direction::shortest)
END_STRINGIFY_ENUM()

BEGIN_STRINGIFY_ENUM(rb::transform_direction)
    STRINGIFY_ENUM_VALUE(rb::transform_direction::from_space_to_base)
    STRINGIFY_ENUM_VALUE(rb::transform_direction::from_base_to_space)
END_STRINGIFY_ENUM()

BEGIN_STRINGIFY_ENUM(rb::corner_type)
    STRINGIFY_ENUM_VALUE(rb::corner_type::bevel)
    STRINGIFY_ENUM_VALUE(rb::corner_type::miter)
END_STRINGIFY_ENUM()

BEGIN_STRINGIFY_ENUM(rb::geometry_type)
    STRINGIFY_ENUM_VALUE(rb::geometry_type::triangle)
    STRINGIFY_ENUM_VALUE(rb::geometry_type::line)
END_STRINGIFY_ENUM()

BEGIN_STRINGIFY_ENUM(rb::point_ordering)
    STRINGIFY_ENUM_VALUE(rb::point_ordering::cw)
    STRINGIFY_ENUM_VALUE(rb::point_ordering::ccw)
    STRINGIFY_ENUM_VALUE(rb::point_ordering::unknown)
END_STRINGIFY_ENUM()

BEGIN_STRINGIFY_ENUM(rb::blend_mode)
    STRINGIFY_ENUM_VALUE(rb::blend_mode::normal)
    STRINGIFY_ENUM_VALUE(rb::blend_mode::multiply)
    STRINGIFY_ENUM_VALUE(rb::blend_mode::screen)
    STRINGIFY_ENUM_VALUE(rb::blend_mode::copy_source)
    STRINGIFY_ENUM_VALUE(rb::blend_mode::preserve_destination)
    STRINGIFY_ENUM_VALUE(rb::blend_mode::add)
END_STRINGIFY_ENUM()

#endif
