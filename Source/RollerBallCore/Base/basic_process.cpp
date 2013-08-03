//
//  basic_process.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 22/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "basic_process.h"
#include "program_manager.h"
#include "program.h"
#include "matrix3x3.h"
#include "texture.h"
#include "state_manager.h"

using namespace rb;

basic_process::basic_process(const program_manager* manager, const bool can_transform_texture){
    if(!can_transform_texture)
        _gl_program = manager->get_basic_program()->program_id();
    else
        _gl_program = manager->get_basic_program_with_wrapping()->program_id();
    
    _gl_uniforms[(uint32_t)uniforms::position_transform] = glGetUniformLocation(_gl_program, "position_transform");
    _gl_uniforms[(uint32_t)uniforms::texture_sampler] = glGetUniformLocation(_gl_program, "texture_sampler");
    _gl_uniforms[(uint32_t)uniforms::texture_transform] = glGetUniformLocation(_gl_program, "texture_transform");
    _gl_uniforms[(uint32_t)uniforms::ambient_color] = glGetUniformLocation(_gl_program, "ambient_color");
    _position_transform = transform_space(vec2::zero, 1, 0);
    _texture_transform = transform_space(vec2::zero, 1, 0);
    _ambient_color = color::from_rgba(1, 1, 1, 1);
    _texture_sampler = nullptr;
}
basic_process::~basic_process(){
    
}
void basic_process::begin_draw(){
    _position_transform.from_space_to_base().set_matrix3x3(this->_m_position_transform);
    _texture_transform.from_space_to_base().set_matrix3x3(this->_m_texture_transform);
    state_manager::use_program(this->_gl_program);
    
    glUniformMatrix3fv(_gl_uniforms[(uint32_t)uniforms::position_transform], 1, GL_FALSE, _m_position_transform.m);
    glUniformMatrix3fv(_gl_uniforms[(uint32_t)uniforms::texture_transform], 1, GL_FALSE, _m_texture_transform.m);
    glUniform4f(_gl_uniforms[(uint32_t)uniforms::ambient_color],
                _ambient_color.r(), _ambient_color.g(), _ambient_color.b(), _ambient_color.a());
    if(_texture_sampler){
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _texture_sampler->texture_id());
        glUniform1i(_gl_uniforms[(uint32_t)uniforms::texture_sampler], 0);
    }
    else {
        //bind to default texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture::blank()->texture_id());
        glUniform1i(_gl_uniforms[(uint32_t)uniforms::texture_sampler], 0);
    }
}
void basic_process::end_draw(){
    
}

bool basic_process::equals(const process& other) const{
    auto _other = dynamic_cast<const basic_process*>(&other);
    if(_other == nullptr)
        return false;
    return _other->_gl_program == _gl_program && _other->_position_transform == _position_transform &&
    _other->_texture_transform == _texture_transform && _other->_texture_sampler == _texture_sampler &&
    _other->_ambient_color == _ambient_color;
}