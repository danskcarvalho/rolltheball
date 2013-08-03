//
//  static_mesh_batch.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 21/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "static_mesh_batch.h"
#include "state_manager.h"
#include "process.h"

using namespace rb;

static_mesh_batch::static_mesh_batch(){
    _process = nullptr;
    _blend_mode = blend_mode::normal;
    _gl_vertex_array = 0;
    _gl_index_buffer = 0;
    _gl_vertex_buffer = 0;
    _ib_used = 0;
    _line_width = 1;
    _geom_type = geometry_type::triangle;
}

static_mesh_batch::~static_mesh_batch(){
    if(_gl_vertex_buffer)
        glDeleteBuffers(1, &_gl_vertex_buffer);
    if(_gl_index_buffer)
        glDeleteBuffers(1, &_gl_index_buffer);
    if(_gl_vertex_array){
#ifdef IOS_TARGET
        glDeleteVertexArraysOES(1, &_gl_vertex_array);
#else
        glDeleteVertexArraysAPPLE(1, &_gl_vertex_array);
#endif
    }
}

void static_mesh_batch::draw(){
    //we set blending mode
    state_manager::set_blending_mode(_blend_mode);
    state_manager::set_line_width(_line_width);
    
#ifdef IOS_TARGET
    glBindVertexArrayOES(_gl_vertex_array);
#else
    glBindVertexArrayAPPLE(_gl_vertex_array);
#endif
    
    //we use the program...
    assert(_process);
    _process->begin_draw();
    if(_geom_type == geometry_type::triangle)
        glDrawElements(GL_TRIANGLES, _ib_used, GL_UNSIGNED_SHORT, 0);
    else
        glDrawElements(GL_LINES, _ib_used, GL_UNSIGNED_SHORT, 0);
    _process->end_draw();
#ifdef IOS_TARGET
    glBindVertexArrayOES(0);
#else
    glBindVertexArrayAPPLE(0);
#endif
    assert(glGetError() == GL_NO_ERROR);
}