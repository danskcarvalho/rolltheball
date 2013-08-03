//
//  state_manager.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 21/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__state_manager__
#define __RollerBallCore__state_manager__

#include "base.h"
#include "nullable.h"
#include "color.h"

namespace rb {
    class vec2;
    class rectangle;
    class state_manager {
    private:
        static nullable<blend_mode> _previous_blending;
        static nullable<uint32_t> _program_id;
        static nullable<float> _line_width;
        static nullable<color> _background_color;
    public:
        static void set_line_width(const float line_width);
        static void set_blending_mode(const blend_mode mode);
        static void use_program(uint32_t program_id);
        static void clear_framebuffer(const color& background_color);
        static vec2 viewport_size();
        static rectangle viewport_bounds();
    };
}

#endif /* defined(__RollerBallCore__state_manager__) */
