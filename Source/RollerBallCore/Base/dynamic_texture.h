//
//  dynamic_texture.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 06/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__dynamic_texture__
#define __RollerBallCore__dynamic_texture__

#include "texture.h"

namespace rb {
    class vec2;
    class dynamic_texture : public texture {
    private:
        uint8_t* create_image_data(const std::function<void (void* ctx)>& f, bool avoid_gl);
        void setup_texture(uint8_t* image);
        dynamic_texture();
        void* _cg_image;
    public:
        dynamic_texture(const vec2& size, const texture_border border, const std::function<void (void* ctx)>& f, const bool avoid_gl = false);
        virtual ~dynamic_texture();
        virtual void* to_cg_image() const override;
    };
}

#endif /* defined(__RollerBallCore__dynamic_texture__) */
