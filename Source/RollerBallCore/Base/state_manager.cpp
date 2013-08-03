//
//  state_manager.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 21/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "state_manager.h"
#include "vec2.h"
#include "rectangle.h"

using namespace rb;

nullable<blend_mode> state_manager::_previous_blending = nullptr;
nullable<uint32_t> state_manager::_program_id = nullptr;
nullable<float> state_manager::_line_width = nullptr;
nullable<color> state_manager::_background_color = nullptr;

void state_manager::clear_framebuffer(const color& background_color){
    while (true) {
        if(!_background_color.has_value()){
            _background_color = background_color.pre_multiplied();
            glClearColor(_background_color.value().r(), _background_color.value().g(), _background_color.value().b(), _background_color.value().a());
        }
        else {
            if(_background_color.value() == background_color.pre_multiplied())
                break;
            
            _background_color = background_color;
            glClearColor(_background_color.value().r(), _background_color.value().g(), _background_color.value().b(), _background_color.value().a());
        }
        break;
    }
    glClear(GL_COLOR_BUFFER_BIT);
}

void state_manager::set_line_width(const float line_width){
    if(!_line_width.has_value()){
        _line_width = line_width;
        glLineWidth(line_width);
    }
    else {
        if(_line_width.value() == line_width)
            return;
        
        _line_width = line_width;
        glLineWidth(line_width);
    }
}

void state_manager::set_blending_mode(const blend_mode mode){
    if(!_previous_blending.has_value()){
        glBlendEquation(GL_FUNC_ADD);
        glEnable(GL_BLEND);
    }
    
    if(_previous_blending.has_value() && _previous_blending.value() == mode)
        return;
    
    if (mode == blend_mode::normal){
        //o alpha é pré-multiplicado em ambos os layers
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    }
    else if(mode == blend_mode::multiply){
        glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
    }
    else if(mode == blend_mode::screen){
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
    }
    else if(mode == blend_mode::copy_source){
        glBlendFunc(GL_ONE, GL_ZERO);
    }
    else if(mode == blend_mode::add){
        glBlendFunc(GL_ONE, GL_ONE);
    }
    else //preserve_destination
    {
        glBlendFunc(GL_ZERO, GL_ONE);
    }
    
    _previous_blending = mode;
}

void state_manager::use_program(uint32_t program_id){
    if(_program_id.has_value()){
        if(_program_id.value() == program_id)
            return;
        _program_id = program_id;
        glUseProgram(_program_id.value());
    }
    else {
        _program_id = program_id;
        glUseProgram(_program_id.value());
    }
}

vec2 state_manager::viewport_size(){
    float values[] = {0, 0, 0, 0};
    glGetFloatv(GL_VIEWPORT, values);
    return vec2(values[2], values[3]);
}
rectangle state_manager::viewport_bounds(){
    float values[] = {0, 0, 0, 0};
    glGetFloatv(GL_VIEWPORT, values);
    float center_x = values[0] + (values[2] / 2.0f);
    float center_y = values[1] + (values[3] / 2.0f);
    return rectangle(center_x, center_y, values[2], values[3]);
}