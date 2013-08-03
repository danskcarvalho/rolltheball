//
//  texture_layer.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 09/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "texture_layer.h"
#include "texture_source.h"

using namespace rb;

texture_layer::texture_layer(){
    
}

texture_layer::~texture_layer(){
    if(this->_texture_source)
        delete this->_texture_source;
}

texture_layer* texture_layer::from_solid_color(const float opacity, const texture_layer_blend_mode blend_mode, const color& color){
    texture_layer* _new_obj = new texture_layer();
    assert(opacity >= 0 && opacity <= 1.0f);
    _new_obj->_opacity = opacity;
    _new_obj->_blend_mode = blend_mode;
    _new_obj->_solid_color = color;
    _new_obj->_texture_source = nullptr;
    return _new_obj;
}
texture_layer* texture_layer::from_texture(const float opacity, const texture_layer_blend_mode blend_mode,
                                           const bool repeat, const transform_space& texture_space,
                                           const class texture_source& source){
    assert(opacity >= 0 && opacity <= 1.0f);
    texture_layer* _new_obj = new texture_layer();
    _new_obj->_opacity = opacity;
    _new_obj->_blend_mode = blend_mode;
    _new_obj->_texture_source = source.duplicate();
    _new_obj->_repeat = repeat;
    _new_obj->_texture_space = texture_space;
    return _new_obj;
}