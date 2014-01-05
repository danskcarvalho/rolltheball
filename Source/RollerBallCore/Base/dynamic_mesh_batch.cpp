//
//  dynamic_mesh_batch.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 21/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "dynamic_mesh_batch.h"
#include "vertex.h"
#include "state_manager.h"
#include "process.h"
#include "static_mesh_batch.h"

using namespace rb;

dynamic_mesh_batch::dynamic_mesh_batch(){
    _blend_mode = blend_mode::normal;
    _process = nullptr;
    _dirty = false;
    _vb = nullptr;
    _vb_count = 0;
    _vb_used = 0;
    _ib16 = nullptr;
    _ib_count = 0;
    _ib_used = 0;
    _dirty = true;
    _line_width = 1;
    _geom_type = geometry_type::triangle;
    
    //opengl buffers
    _gl_vertex_array = 0;
    _gl_index_buffer = 0;
    _gl_vertex_buffer = 0;
}
dynamic_mesh_batch::~dynamic_mesh_batch(){
    if(_vb)
        free(_vb);
    if(_ib16)
        free(_ib16);
    
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
uint32_t dynamic_mesh_batch::mesh_count() const{
    return (uint32_t)_meshes.size();
}
void dynamic_mesh_batch::add_mesh(mesh* m){
    _meshes.push_back(m);
    _dirty = true;
}
void dynamic_mesh_batch::clear_meshes(){
    _meshes.clear();
    _dirty = true;
}

bool dynamic_mesh_batch::contains_mesh(const mesh* m) const{
    for (auto _item : _meshes){
        if(_item == m)
            return true;
    }
    return false;
}

void move_indexes(uint16_t* buffer, const uint16* source, size_t size, uint16_t vb_offset){
    for (size_t i = 0; i < size; i++) {
        buffer[i] = source[i] + vb_offset;
    }
}

void dynamic_mesh_batch::realloc_buffers(){
    if(_meshes.size() == 0)
        return;
    
    if (_dirty || !_gl_vertex_buffer){
        //check the needed vbsize
        uint32_t _n_vb_count = 0;
        uint32_t _n_ib_count = 0;
        for (auto _m : _meshes){
            _n_vb_count+= _m->vertex_count();
            _n_ib_count+= _m->index_count();
        }
        
        //i am gonna let it run if the number of vertexes goes >= 2^16
        //bool _use_32ib = _n_vb_count >= std::numeric_limits<uint16_t>::max();
        //assert(!_use_32ib);
        bool _realloc = false;
        
        //we don't have the memory zeroed... uncomment the bzero lines if needed...
        //we alloc/realloc the vertex buffer
        if(!_vb){
            _vb = (vertex*)malloc(sizeof(vertex) * (size_t)(_n_vb_count * 1.33f));
            assert(_vb);
            _vb_count = (uint32_t)(_n_vb_count * 1.33f);
            _realloc = true;
            //bzero(_vb, _vb_count * sizeof(vertex));
        }
        else {
            if (_n_vb_count > _vb_count){
                _vb = (vertex*)realloc(_vb, sizeof(vertex) * (size_t)(_n_vb_count * 1.33f)); //we apply a 1.33 factor to avoid reallocating _vb everytime
                assert(_vb);
                //auto _previous_vb_count = _vb_count;
                _vb_count = (uint32_t)(_n_vb_count * 1.33f);
                _realloc = true;
                //bzero(_vb + _previous_vb_count, sizeof(vertex) * (_vb_count - _previous_vb_count));
            }
            //we maintain _vb if _vb is bigger than the number of vertices needed...
        }
        _vb_used = _n_vb_count; //thats the vertices we will render...
        
        //we alloc/realloc the index buffer
        if(!_ib16){
            _ib16 = (uint16_t*)malloc(sizeof(uint16_t) * (size_t)(_n_ib_count * 1.33f));
            assert(_ib16);
            _ib_count = (uint32_t)(_n_ib_count * 1.33f);
            _realloc = true;
            //bzero(_ib16, _ib_count * sizeof(uint16_t));
        }
        else {
            if (_n_ib_count > _ib_count){
                _ib16 = (uint16_t*)realloc(_ib16, sizeof(uint16_t) * (size_t)(_n_ib_count * 1.33f)); //we apply a 1.33 factor to avoid reallocating _ib16 everytime
                assert(_ib16);
                //auto _previous_ib_count = _ib_count;
                _ib_count = (uint32_t)(_n_ib_count * 1.33f);
                _realloc = true;
                //bzero(_ib16 + _previous_ib_count, sizeof(uint16_t) * (_ib_count - _previous_ib_count));
            }
        }
        _ib_used = _n_ib_count;
        
        //we copy mesh data into the buffers
        uint32_t _vb_offset = 0;
        uint32_t _ib_offset = 0;
        for (auto _m : _meshes){
            move_indexes(_ib16 + _ib_offset, _m->index_buffer(), _m->index_count(), _vb_offset);
            _ib_offset += _m->index_count();
            memcpy(_vb + _vb_offset, _m->vertex_buffer(), sizeof(vertex) * _m->vertex_count());
            _vb_offset += _m->vertex_count();
            _m->clear_dirty_flag(); //not dirty anymore...
        }
        
        //we then alloc buffers
#ifdef IOS_TARGET
        if(!_gl_vertex_array)
            glGenVertexArraysOES(1, &_gl_vertex_array);
        glBindVertexArrayOES(_gl_vertex_array);
#else
        if(!_gl_vertex_array)
            glGenVertexArraysAPPLE(1, &_gl_vertex_array);
        glBindVertexArrayAPPLE(_gl_vertex_array);
#endif
        assert(_gl_vertex_array);
        
        bool _created_vb = false;
        if(!_gl_vertex_buffer){
            glGenBuffers(1, &_gl_vertex_buffer);
            _created_vb = true;
        }
        assert(_gl_vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, _gl_vertex_buffer);
        if(_realloc)
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * _vb_count, _vb, GL_DYNAMIC_DRAW);
        else
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex) * _vb_count, _vb);
        
        if(!_gl_index_buffer)
            glGenBuffers(1, &_gl_index_buffer);
        assert(_gl_index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _gl_index_buffer);
        if(_realloc)
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * _ib_count, _ib16, GL_DYNAMIC_DRAW);
        else
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint16_t) * _ib_count, _ib16);
        
        if(_created_vb) //this may cause problems...
            vertex::setup_attribute_descriptors();
        
        //we undirty everyone
        _dirty = false;
        
#ifdef IOS_TARGET
        glBindVertexArrayOES(0);
#else
        glBindVertexArrayAPPLE(0);
#endif
    }
    else {
        //in this case we only copy the dirty meshes
        uint32_t _vb_offset = 0;
        uint32_t _ib_offset = 0;
        bool _anyone_dirty_vb  = false;
        bool _anyone_dirty_ib  = false;
        nullable<size_t> _start_vb_offset;
        nullable<size_t> _start_ib_offset;
        size_t _last_vb_offset = 0;
        size_t _last_ib_offset = 0;
        for (auto _m : _meshes){
            if(_m->dirty_index_data()){
                move_indexes(_ib16 + _ib_offset, _m->index_buffer(), _m->index_count(), _vb_offset);
                _anyone_dirty_ib = true;
                if(!_start_ib_offset.has_value())
                    _start_ib_offset = _ib_offset;
                _last_ib_offset = _ib_offset + _m->index_count() - 1;
            }
            if(_m->dirty_vertex_data()){
                memcpy(_vb + _vb_offset, _m->vertex_buffer(), sizeof(vertex) * _m->vertex_count());
                _anyone_dirty_vb = true;
                if(!_start_vb_offset.has_value())
                    _start_vb_offset = _vb_offset;
                _last_vb_offset = _vb_offset + _m->vertex_count() - 1;
            }
            _ib_offset += _m->index_count();
            _vb_offset += _m->vertex_count();
            _m->clear_dirty_flag(); //not dirty anymore...
        }
        
        if(_anyone_dirty_vb || _anyone_dirty_ib){
#ifdef IOS_TARGET
            glBindVertexArrayOES(_gl_vertex_array);
#else
            glBindVertexArrayAPPLE(_gl_vertex_array);
#endif
            if(_anyone_dirty_vb){
                //vertex buffer...
                size_t _mod_vb_count = (_last_vb_offset - _start_vb_offset.value()) + 1;
                glBindBuffer(GL_ARRAY_BUFFER, _gl_vertex_buffer);
                glBufferSubData(GL_ARRAY_BUFFER, _start_vb_offset.value() * sizeof(vertex), sizeof(vertex) * _mod_vb_count, _vb + _start_vb_offset.value());
            }
            if(_anyone_dirty_ib){
                //index buffer...
                size_t _mod_ib_count = (_last_ib_offset - _start_ib_offset.value()) + 1;
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _gl_index_buffer);
                glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, _start_ib_offset.value() * sizeof(uint16_t), sizeof(uint16_t) * _mod_ib_count, _ib16 + _start_ib_offset.value());
            }
            
#ifdef IOS_TARGET
            glBindVertexArrayOES(0);
#else
            glBindVertexArrayAPPLE(0);
#endif
        }
    }
}

void dynamic_mesh_batch::draw() {
    if(_meshes.size() == 0) //no mesh to draw
        return;
    //we realloc buffer data if needed...
    realloc_buffers();
    
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
    
    //I will mantain this assertion... No gl error should occur...
#if !defined(NDEBUG)
    auto _gl_error = glGetError();
    assert(_gl_error == GL_NO_ERROR);
#endif
}

static_mesh_batch* dynamic_mesh_batch::compile() const{
    if(_meshes.size() == 0)
        return nullptr;
    
    //we realloc buffer data if needed...
    const_cast<dynamic_mesh_batch*>(this)->realloc_buffers();
    GLuint  _s_gl_vertex_array = 0,
            _s_gl_vertex_buffer = 0,
            _s_gl_index_buffer = 0;
    //we then alloc buffers
#ifdef IOS_TARGET
    glGenVertexArraysOES(1, &_s_gl_vertex_array);
    glBindVertexArrayOES(_s_gl_vertex_array);
#else
    glGenVertexArraysAPPLE(1, &_s_gl_vertex_array);
    glBindVertexArrayAPPLE(_s_gl_vertex_array);
#endif
    assert(_s_gl_vertex_array);

    glGenBuffers(1, &_s_gl_vertex_buffer);
    assert(_s_gl_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, _s_gl_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * _vb_used, _vb, GL_STATIC_DRAW);
    vertex::setup_attribute_descriptors();
    
    glGenBuffers(1, &_s_gl_index_buffer);
    assert(_s_gl_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _s_gl_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * _ib_used, _ib16, GL_STATIC_DRAW);
    
    static_mesh_batch* sb = new static_mesh_batch();
    sb->_blend_mode = _blend_mode;
    sb->_process = _process;
    sb->_gl_index_buffer = _s_gl_index_buffer;
    sb->_gl_vertex_array = _s_gl_vertex_array;
    sb->_gl_vertex_buffer = _s_gl_vertex_buffer;
    sb->_ib_used = _ib_used;
    sb->_geom_type = _geom_type;
    sb->_line_width = _line_width;
    
#ifdef IOS_TARGET
    glBindVertexArrayOES(0);
#else
    glBindVertexArrayAPPLE(0);
#endif
    return sb;
}
