//
//  texture_layer.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 09/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__texture_layer__
#define __RollerBallCore__texture_layer__

#include "base.h"
#include "color.h"
#include "transform_space.h"

namespace rb {
    class texture_source;
    class texture_layer {
    private:
        float _opacity;
        color _solid_color;
        texture_source* _texture_source;
        texture_layer_blend_mode _blend_mode;
        texture_layer();
        bool _repeat;
        transform_space _texture_space;
    public:
        ~texture_layer();
        static texture_layer* from_solid_color(const float opacity, const texture_layer_blend_mode blend_mode, const color& color);
        static texture_layer* from_texture(const float opacity, const texture_layer_blend_mode blend_mode,
                                           const bool repeat, const transform_space& texture_space,
                                           const texture_source& source);
        
        inline float opacity() const {
            return _opacity;
        }
    
        inline const color& solid_color() const {
            return _solid_color;
        }
    
        inline const texture_source* texture_source() const {
            return _texture_source;
        }
        
        inline texture_layer_blend_mode blend_mode() const{
            return _blend_mode;
        }
        
        inline bool uses_color() const {
            return _texture_source == nullptr;
        }
        
        inline bool uses_texture() const {
            return _texture_source != nullptr;
        }
        
        inline bool repeat() const {
            return _repeat;
        }

        inline const transform_space& texture_space() const {
            return _texture_space;
        }
    };
}

#endif /* defined(__RollerBallCore__texture_layer__) */
