//
//  dynamic_mesh_batch.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 21/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__dynamic_mesh_batch__
#define __RollerBallCore__dynamic_mesh_batch__

#include "base.h"
#include "mesh_batch.h"
#include "mesh.h"
#define BOOST_DISABLE_THREADS
#include <boost/pool/pool_alloc.hpp>

namespace rb {
    class process;
    class vertex;
    class static_mesh_batch;
    class dynamic_mesh_batch : public mesh_batch {
    public:
        typedef std::list<mesh*>::const_iterator const_iterator;
    private:
        //we don't track meshes... we imagine that a class is the owner of each mesh and is responsible for adding and removing meshes
        //from this class...
        std::list<mesh*, boost::fast_pool_allocator<mesh*>> _meshes;
        std::unordered_map<mesh*, const_iterator, std::hash<mesh*>, std::equal_to<mesh*>, boost::pool_allocator<std::pair<mesh*, const_iterator>>> _mesh_pos_map;
        //initially we won't care about clipping because we won't have big worlds
        //blend mode
        blend_mode _blend_mode;
        //a program to use for rendering...
        process* _process;
        //line width
        float _line_width;
        
        //dirty flag
        bool _dirty; //a polygon has been added or removed...
        
        //buffers
        vertex* _vb;
        uint32_t _vb_count;
        uint32_t _vb_used;
        uint16_t* _ib16;
        uint32_t _ib_count;
        uint32_t _ib_used;
        
        //opengl buffers
        GLuint _gl_vertex_array;
        GLuint _gl_vertex_buffer;
        GLuint _gl_index_buffer;
        
        geometry_type _geom_type;
        
        void realloc_buffers();
    public:
        class mesh_range {
        public:
            friend class dynamic_mesh_batch;
        private:
            dynamic_mesh_batch* _parent;
            const_iterator _begin;
            const_iterator _end;
        public:
            mesh_range();
            const_iterator begin() const;
            const_iterator end() const;
            
            void move_before(const mesh_range& other);
            void move_after(const mesh_range& other);
        };
    public:
        //constructors and destructors
        dynamic_mesh_batch();
        virtual ~dynamic_mesh_batch();
        
        //mesh management
        uint32_t mesh_count() const;
        const mesh* first_mesh() const;
        mesh* first_mesh();
        const mesh* last_mesh() const;
        mesh* last_mesh();
        bool contains_mesh(const mesh* m) const;
        void remove_meshes(const mesh_range& r);
        void add_mesh_before(mesh* m, const mesh_range& r);
        void add_mesh_after(mesh* m, const mesh_range& r);
        void add_mesh(mesh* m);
        void clear_meshes();
        void range(const mesh* start, const mesh* end, mesh_range& r);
        void range(const mesh* m, mesh_range& r);
        void entire_range(mesh_range& r);
        
        inline uint32_t vertex_count() const {
            return _vb_used;
        }
        
        inline uint32_t index_count() const {
            return _ib_used;
        }
        
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
        
        //draw
        virtual void draw() override;
        
        //compile
        static_mesh_batch* compile() const;
    };
}

#endif /* defined(__RollerBallCore__dynamic_mesh_batch__) */
