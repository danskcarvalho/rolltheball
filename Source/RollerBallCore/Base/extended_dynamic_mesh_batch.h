//
//  extended_dynamic_mesh_batch.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 12/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__extended_dynamic_mesh_batch__
#define __RollerBallCore__extended_dynamic_mesh_batch__

#include "base.h"
#include "mesh_batch.h"
#include "mesh.h"
#define BOOST_DISABLE_THREADS
#include <boost/pool/pool_alloc.hpp>

namespace rb {
    class process;
    class vertex;
    class extended_static_mesh_batch;
    class dynamic_mesh_batch;
    class extended_dynamic_mesh_batch : public mesh_batch {
    public:
        typedef std::list<mesh*>::const_iterator const_iterator;
    private:
        //we don't track meshes... we imagine that a class is the owner of each mesh and is responsible for adding and removing meshes
        //from this class...
        std::list<mesh*, boost::fast_pool_allocator<mesh*>> _meshes;
        std::unordered_map<mesh*, const_iterator, std::hash<mesh*>, std::equal_to<mesh*>, boost::pool_allocator<std::pair<mesh*, const_iterator>>> _mesh_pos_map;
        std::unordered_map<mesh*, process*, std::hash<mesh*>, std::equal_to<mesh*>, boost::pool_allocator<std::pair<mesh*, process*>>>
        _mesh_proc_map;
        //initially we won't care about clipping because we won't have big worlds
        //blend mode
        blend_mode _blend_mode;
        //line width
        float _line_width;
        
        std::vector<dynamic_mesh_batch*> _batches;
        
        //dirty flag
        bool _dirty; //a polygon has been added or removed...
        geometry_type _geom_type;
        void realloc_batches();
        uint32_t realloc_batches_for_process(uint32_t start_index, class process* p,
                                         std::vector<mesh*, boost::pool_allocator<class mesh*>> meshes);
    public:
        class mesh_range {
        public:
            friend class extended_dynamic_mesh_batch;
        private:
            extended_dynamic_mesh_batch* _parent;
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
        extended_dynamic_mesh_batch();
        virtual ~extended_dynamic_mesh_batch();
        
        //mesh management
        uint32_t mesh_count() const;
        const mesh* first_mesh() const;
        mesh* first_mesh();
        const mesh* last_mesh() const;
        mesh* last_mesh();
        bool contains_mesh(const mesh* m) const;
        void remove_meshes(const mesh_range& r);
        void add_mesh_before(mesh* m, const process* p, const mesh_range& r);
        void add_mesh_after(mesh* m, const process* p, const mesh_range& r);
        void add_mesh(mesh* m, const process* p);
        void clear_meshes();
        void range(const mesh* start, const mesh* end, mesh_range& r);
        void range(const mesh* m, mesh_range& r);
        void entire_range(mesh_range& r);
        
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
        const class process* process(const mesh* m) const;
        class process* process(const mesh* m);
        class process* process(const mesh* m, class process* value);
        const class process* default_process() const;
        class process* default_process();
        class process* default_process(class process* value);
        
        //draw
        virtual void draw() override;
        
        //compile
        extended_static_mesh_batch* compile() const;
    };
}

#endif /* defined(__RollerBallCore__extended_dynamic_mesh_batch__) */
