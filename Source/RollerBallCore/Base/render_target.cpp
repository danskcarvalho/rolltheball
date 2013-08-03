//
//  render_target.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 06/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "render_target.h"
#include "state_manager.h"

using namespace rb;

render_target::render_target(const vec2& size, const nullable<color>& background_color){
    assert(size.x() > 0);
    assert(size.y() > 0);
    
    vec2 _size = size;
    _size.x(roundf(size.x()));
    _size.y(roundf(size.y()));
    
    if(_size.x() < 1)
        _size.x(1);
    
    if(_size.y() < 1)
        _size.x(1);
    
    this->_border = texture_border::none;
    this->_bounds = rectangle(0.5, 0.5, 1, 1);
    this->_original_size = _size;
    this->_texture_size = _size;
    
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _size.x(), _size.y(), 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    GLuint fboId;
    glGenFramebuffers(1, &fboId);
    GLint old_fbo;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &old_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);
    
    // attach the texture to FBO color attachment point
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, textureId, 0);
    
    // check FBO status
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE)
    {
        this->_fbo_id = this->_texture_id = 0;
        glDeleteFramebuffers(1, &fboId);
        glDeleteTextures(1, &textureId);
        return;
    }
    
    this->_texture_id = textureId;
    this->_fbo_id = fboId;
    
    if(background_color.has_value())
        state_manager::clear_framebuffer(background_color.value());
    
    glBindFramebuffer(GL_FRAMEBUFFER, old_fbo);
}
void render_target::bind(){
    assert(loaded());
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_old_fbo);
    glGetFloatv(GL_VIEWPORT, _old_viewport);
    glBindFramebuffer(GL_FRAMEBUFFER, this->_fbo_id);
    glViewport(0, 0, _texture_size.x(), _texture_size.y());
    assert(glGetError() == GL_NO_ERROR);
}
void render_target::unbind(){
    assert(loaded());
    glViewport(_old_viewport[0], _old_viewport[1], _old_viewport[2], _old_viewport[3]);
    glBindFramebuffer(GL_FRAMEBUFFER, _old_fbo);
    assert(glGetError() == GL_NO_ERROR);
}
render_target::~render_target(){
    glDeleteFramebuffers(1, &_fbo_id);
}