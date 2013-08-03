//
//  static_mesh_batch.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 21/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__static_mesh_batch__
#define __RollerBallCore__static_mesh_batch__

#include "base.h"
#include "mesh_batch.h"

namespace rb {
    class dynamic_mesh_batch;
    class process;
    class static_mesh_batch : public mesh_batch {
    private:
        //opengl buffers
        GLuint _gl_vertex_array;
        GLuint _gl_vertex_buffer;
        GLuint _gl_index_buffer;
        uint32_t _ib_used;
        
        //blend mode
        blend_mode _blend_mode;
        //a program to use for rendering...
        process* _process;
        //line width
        float _line_width;
        
        geometry_type _geom_type;
        
        static_mesh_batch();
    public:
        virtual ~static_mesh_batch();
        virtual void draw() override;
        
        //line width
        inline float line_width() const {
            return _line_width;
        }
        
        inline float line_width(const float value) {
            assert(value >= 0);
            _line_width = value;
            return _line_width;
        }
        
        //geometry type
        inline enum geometry_type geometry_type() const {
            return _geom_type;
        }
        inline enum geometry_type geometry_type(const enum geometry_type value){
            _geom_type = value;
            return _geom_type;
        }
        
        //blend mode
        inline blend_mode blend_mode() const {
            return _blend_mode;
        }
        inline enum blend_mode blend_mode(const enum blend_mode value) {
            _blend_mode = value;
            return _blend_mode;
        }
        
        //program
        inline const process* process() const {
            return _process;
        }
        inline class process* process() {
            return _process;
        }
        inline class process* process(const class process* value){
            _process = const_cast<class process*>(value);
            return _process;
        }
        
        friend class dynamic_mesh_batch;
    };
}

#endif /* defined(__RollerBallCore__static_mesh_batch__) */
