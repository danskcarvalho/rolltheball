//
//  vertex.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 09/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__vertex__
#define __RollerBallCore__vertex__

#include "base.h"
#include "vec2.h"
#include "color.h"
#include "rectangle.h"

#define VERTEX_PACKED_COLOR

namespace rb {
    struct vertex {
        float x, y;
        float s, t;
        float a;
        float blend;
        float tx, ty, tw, th; //texture rectangle in atlas
#if defined(VERTEX_PACKED_COLOR)
        uint32_t rgba;
#else
        float redColor;
        float greenColor;
        float blueColor;
        float alphaColor;
#endif
        
        inline vertex& set_position(const vec2& v){
            x = v.x();
            y = v.y();
            return *this;
        }
        
        inline rectangle get_texture_bounds() const{
            return rectangle(vec2(tx + (tw / 2), ty + (th / 2)), vec2(tw, th));
        }
        
        inline vertex& set_texture_bounds(const rectangle& value){
            tx = value.bottom_left().x();
            ty = value.bottom_left().y();
            tw = value.size().x();
            th = value.size().y();
            return *this;
        }
        
        inline vec2 get_position() const {
            return vec2(x, y);
        }
        
        inline vertex& set_texture_coords(const vec2& v){
            s = v.x();
            t = v.y();
            return *this;
        }
        
        inline vec2 get_texture_coords() const {
            return vec2(s, t);
        }
        
        inline vertex& set_color(const color& c){
#if defined(VERTEX_PACKED_COLOR)
            rgba = c.pre_multiplied().packed_rgba();
            return *this;
#else
            auto _new_c = c.pre_multiplied();
            redColor = _new_c.r();
            greenColor = _new_c.g();
            blueColor = _new_c.b();
            alphaColor = _new_c.a();
            return *this;
#endif
        }
        
        inline color get_color() const{
#if defined(VERTEX_PACKED_COLOR)
            return color::from_packed_rgba(rgba);
#else
            return color::from_rgba(redColor, greenColor, blueColor, alphaColor);
#endif
        }
        
        static void setup_attribute_descriptors();
        static void setup_attribute_bindings(uint32_t program);
    } __attribute__((packed));
}

#endif /* defined(__RollerBallCore__vertex__) */
