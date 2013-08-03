//
//  composited_texture.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 09/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__composited_texture__
#define __RollerBallCore__composited_texture__

#include "base.h"
#include "texture.h"

namespace rb {
    class texture_composition;
    class texture_layer;
    class vec2;
    class texture_atlas;
    class composited_texture : public texture {
    private:
        const texture_composition* _composition;
        texture_border _border;
        static void* create_image(const texture_composition* composition);
        static void* create_blank_image();
        static bool draw_layer(const void* ctx, const texture_layer* layer, const void* color_image);
        void setup_texture(void* image, const texture_border border);
        void* _cg_image;
    public:
        friend class texture_atlas;
        composited_texture(const texture_composition* composition, texture_border border, const bool avoid_gl = false);
        virtual ~composited_texture();
    
        inline const texture_composition* composition(){
            return _composition;
        }
        
        inline const texture_border border(){
            return _border;
        }
        virtual void* to_cg_image() const override;
    };
}

#endif /* defined(__RollerBallCore__composited_texture__) */
