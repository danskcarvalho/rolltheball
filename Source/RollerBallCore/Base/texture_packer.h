//
//  texture_packer.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 11/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__texture_packer__
#define __RollerBallCore__texture_packer__

#include "base.h"
#include "vec2.h"
#include "rectangle.h"

namespace rb {
    class texture_packer {
    private:
        std::vector<nullable<rectangle>> _tex_destinations;
        vec2 _full_filled_area;
        texture_packer(); //private default constructor...
    public:
        static texture_packer pack_textures(const std::vector<vec2>& texture_sizes, const vec2& space_available, bool may_rotate = true);
        const nullable<rectangle>& texture_rectangle(uint32_t at) const;
        uint32_t texture_count() const;
        bool has_been_packed(uint32_t at) const;
        const vec2& filled_area() const;
    };
}

#endif /* defined(__RollerBallCore__texture_packer__) */
