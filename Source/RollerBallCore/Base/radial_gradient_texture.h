//
//  radial_gradient_texture.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 05/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__radial_gradient_texture__
#define __RollerBallCore__radial_gradient_texture__

#include "texture.h"
#include "color.h"

namespace rb {
    class dynamic_texture;
    class texture_atlas;
    class radial_gradient_texture : public texture {
    private:
        dynamic_texture* _texture;
        color _color;
        static void draw_radial_gradient(void* ctx, const float size, const class color& color);
    public:
        friend class texture_atlas;
        inline const color& color() const{
            return _color;
        }
        radial_gradient_texture(const float size, const class color& color, const bool avoid_gl = false); //color is not pre-multiplied...
        virtual ~radial_gradient_texture();
        virtual void* to_cg_image() const override;
    };
}

#endif /* defined(__RollerBallCore__radial_gradient_texture__) */
