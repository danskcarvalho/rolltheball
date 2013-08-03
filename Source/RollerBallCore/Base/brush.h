//
//  brush.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 10/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__brush__
#define __RollerBallCore__brush__

#include "color.h"

namespace rb {
    class vec2;
    class transform_space;
    class brush {
    private:
        color _color;
        float _size;
        float _hardness;
        blend_mode _blend_mode;
    public:
        brush();
        brush(const class color& color, const float size, const float hardness, blend_mode blend_mode = blend_mode::normal);
        
        //inline properties
        const class color& color() const {
            return _color;
        }
        const class color& color(const class color& value) {
            assert(value.is_valid());
            _color = value.clamped();
            return _color;
        }
        
        float size() const {
            return _size;
        }
        
        float size(const float value) {
            assert(value >= 0);
            _size = value;
            return _size;
        }

        float hardness() const {
            return _hardness;
        }

        float hardness(const float value) {
            assert(value >= 0);
            if(value > 1.0f)
                _hardness = 1.0f;
            else
                _hardness = value;
            return _hardness;
        }
        
        blend_mode blend_mode() const {
            return _blend_mode;
        }
        
        enum blend_mode blend_mode(enum blend_mode value) {
            _blend_mode = value;
            return _blend_mode;
        }

        //to string
        rb_string to_string() const;

        //position is in base space
        class color sample_color(const transform_space& ts, const vec2& position) const;
    };
}

CLASS_NAME(rb::brush);

#endif /* defined(__RollerBallCore__brush__) */
