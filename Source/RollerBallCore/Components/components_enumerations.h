//
//  components_enumerations.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 07/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef RollerBallCore_components_enumerations_h
#define RollerBallCore_components_enumerations_h

#include "stringification.h"

namespace rb {
    enum class keyboard_modifier {
        alpha_shift = 1 << 16,
        shift = 1 << 17,
        control = 1 << 18,
        alternate = 1 << 19,
        command = 1 << 20,
        numeric_pad = 1 << 21,
        help = 1 << 22,
        function = 1 << 23,
    };
    
    enum class property_type {
        number = 1,
        text = 2,
        boolean = 3,
        enumeration = 4,
        texture_name = 5,
        color = 6
    };
    
    enum class registrable_event {
        update = 0,
        in_editor_update = 1,
        space_changed = 2,
        touch_events = 3,
        keyboard_events = 4,
        mouse_events = 5,
        live_edit = 6,
        transform_changed = 7,
        count = 8
    };
    
    enum class space {
        layer = 0,
        scene = 1,
        camera = 2,
        normalized_screen = 3,
        screen = 4
    };
    
    enum class node_capability {
        rendering = 0,
        children_rendering = 1,
        gizmo_rendering = 2,
        can_become_current = 3,
        selection_rectangle = 4,
        count = 5
    };
    
    enum class live_edit {
        transformation = 0,
        count = 1
    };
    
    enum class navigation_mode {
        scroll = 0,
        rotate = 1,
        zoom = 2
    };
    
    enum class general_type_descriptor {
        string,
        integer,
        single,
        vec2,
        boolean,
        ranged,
        angle,
        color,
        image,
        enumeration,
        object,
        branch,
        vector,
        string_with_autocompletion,
        flags,
        action,
        buffer
    };
    
    enum class action_flags {
        single_dispatch = 0,
        multi_dispatch = 1,
        dispatch_mask = 1,
        cant_undo = 1 << 2
    };
    
    enum class generic_object_type {
        null = 0,
        string = 1,
        integer = 2,
        single = 3,
        vec2 = 4,
        boolean = 5,
        color = 6,
        object = 7,
        vector = 8,
        buffer = 9
    };
    
    enum class node_filter {
        renderable = 1,
        non_renderable = 2,
        all = renderable | non_renderable
    };
    
    enum class camera_invariant {
        none = 0,
        position = 1 << 0,
        scale = 1 << 1,
        rotation = 1 << 2
    };
    
    enum class particle_state {
        showing = 3,
        hidden = 1,
        paused = 2,
        stopped = 0
    };
    
    enum class animation_state {
        stopped = 0,
        playing = 1
    };
    
    enum class distribution_type {
        horizontally = 0,
        vertically = 1,
        along_path = 2
    };
    
    enum class action_type {
        select = 0,
        add_to_selection = 1,
        unselect = 2,
        block = 3,
        unblock = 4,
        hide = 5,
        unhide = 6,
        remove = 7
    };
}

BEGIN_STRINGIFY_ENUM(node_filter)
    STRINGIFY_ENUM_VALUE(node_filter::all)
    STRINGIFY_ENUM_VALUE(node_filter::renderable)
    STRINGIFY_ENUM_VALUE(node_filter::non_renderable)
END_STRINGIFY_ENUM()

BEGIN_STRINGIFY_ENUM(generic_object_type)
    STRINGIFY_ENUM_VALUE(generic_object_type::string)
    STRINGIFY_ENUM_VALUE(generic_object_type::integer)
    STRINGIFY_ENUM_VALUE(generic_object_type::single)
    STRINGIFY_ENUM_VALUE(generic_object_type::vec2)
    STRINGIFY_ENUM_VALUE(generic_object_type::boolean)
    STRINGIFY_ENUM_VALUE(generic_object_type::color)
    STRINGIFY_ENUM_VALUE(generic_object_type::object)
    STRINGIFY_ENUM_VALUE(generic_object_type::vector)
    STRINGIFY_ENUM_VALUE(generic_object_type::buffer)
END_STRINGIFY_ENUM()


BEGIN_STRINGIFY_ENUM(general_type_descriptor)
    STRINGIFY_ENUM_VALUE(general_type_descriptor::string)
    STRINGIFY_ENUM_VALUE(general_type_descriptor::integer)
    STRINGIFY_ENUM_VALUE(general_type_descriptor::single)
    STRINGIFY_ENUM_VALUE(general_type_descriptor::vec2)
    STRINGIFY_ENUM_VALUE(general_type_descriptor::boolean)
    STRINGIFY_ENUM_VALUE(general_type_descriptor::ranged)
    STRINGIFY_ENUM_VALUE(general_type_descriptor::angle)
    STRINGIFY_ENUM_VALUE(general_type_descriptor::color)
    STRINGIFY_ENUM_VALUE(general_type_descriptor::image)
    STRINGIFY_ENUM_VALUE(general_type_descriptor::enumeration)
    STRINGIFY_ENUM_VALUE(general_type_descriptor::object)
    STRINGIFY_ENUM_VALUE(general_type_descriptor::branch)
    STRINGIFY_ENUM_VALUE(general_type_descriptor::vector)
    STRINGIFY_ENUM_VALUE(general_type_descriptor::string_with_autocompletion)
    STRINGIFY_ENUM_VALUE(general_type_descriptor::flags)
    STRINGIFY_ENUM_VALUE(general_type_descriptor::action)
    STRINGIFY_ENUM_VALUE(general_type_descriptor::buffer)
END_STRINGIFY_ENUM()

BEGIN_STRINGIFY_ENUM(navigation_mode)
    STRINGIFY_ENUM_VALUE(navigation_mode::scroll)
    STRINGIFY_ENUM_VALUE(navigation_mode::rotate)
    STRINGIFY_ENUM_VALUE(navigation_mode::zoom)
END_STRINGIFY_ENUM()

BEGIN_STRINGIFY_ENUM(rb::live_edit)
    STRINGIFY_ENUM_VALUE(rb::live_edit::transformation)
    STRINGIFY_ENUM_VALUE(rb::live_edit::count)
END_STRINGIFY_ENUM()

BEGIN_STRINGIFY_ENUM(rb::space)
    STRINGIFY_ENUM_VALUE(rb::space::layer)
    STRINGIFY_ENUM_VALUE(rb::space::scene)
    STRINGIFY_ENUM_VALUE(rb::space::camera)
    STRINGIFY_ENUM_VALUE(rb::space::normalized_screen)
    STRINGIFY_ENUM_VALUE(rb::space::screen)
END_STRINGIFY_ENUM()

BEGIN_STRINGIFY_ENUM(rb::node_capability)
    STRINGIFY_ENUM_VALUE(rb::node_capability::rendering)
    STRINGIFY_ENUM_VALUE(rb::node_capability::children_rendering)
    STRINGIFY_ENUM_VALUE(rb::node_capability::can_become_current)
    STRINGIFY_ENUM_VALUE(rb::node_capability::count)
END_STRINGIFY_ENUM()

BEGIN_STRINGIFY_ENUM(rb::registrable_event)
    STRINGIFY_ENUM_VALUE(rb::registrable_event::update)
    STRINGIFY_ENUM_VALUE(rb::registrable_event::in_editor_update)
    STRINGIFY_ENUM_VALUE(rb::registrable_event::space_changed)
    STRINGIFY_ENUM_VALUE(rb::registrable_event::touch_events)
    STRINGIFY_ENUM_VALUE(rb::registrable_event::keyboard_events)
    STRINGIFY_ENUM_VALUE(rb::registrable_event::mouse_events)
    STRINGIFY_ENUM_VALUE(rb::registrable_event::live_edit)
    STRINGIFY_ENUM_VALUE(rb::registrable_event::transform_changed)
    STRINGIFY_ENUM_VALUE(rb::registrable_event::count)
END_STRINGIFY_ENUM()

BEGIN_STRINGIFY_ENUM(rb::property_type)
    STRINGIFY_ENUM_VALUE(rb::property_type::number)
    STRINGIFY_ENUM_VALUE(rb::property_type::text)
    STRINGIFY_ENUM_VALUE(rb::property_type::boolean)
    STRINGIFY_ENUM_VALUE(rb::property_type::enumeration)
    STRINGIFY_ENUM_VALUE(rb::property_type::texture_name)
    STRINGIFY_ENUM_VALUE(rb::property_type::color)
END_STRINGIFY_ENUM()

BEGIN_STRINGIFY_FLAGS(rb::keyboard_modifier)
    STRINGIFY_FLAG_VALUE(rb::keyboard_modifier::alpha_shift)
    STRINGIFY_FLAG_VALUE(rb::keyboard_modifier::shift)
    STRINGIFY_FLAG_VALUE(rb::keyboard_modifier::control)
    STRINGIFY_FLAG_VALUE(rb::keyboard_modifier::alternate)
    STRINGIFY_FLAG_VALUE(rb::keyboard_modifier::command)
    STRINGIFY_FLAG_VALUE(rb::keyboard_modifier::numeric_pad)
    STRINGIFY_FLAG_VALUE(rb::keyboard_modifier::help)
    STRINGIFY_FLAG_VALUE(rb::keyboard_modifier::function)
END_STRINGIFY_FLAGS()

#endif
