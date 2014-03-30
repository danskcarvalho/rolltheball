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
    private:
        //we don't track meshes... we imagine that a class is the owner of each mesh and is responsible for adding and removing meshes
        //from this class...
        std::list<mesh*> _meshes;
        std::unordered_map<mesh*, process*> _mesh_proc_map;
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
                                         const std::vector<mesh*>& meshes);
    public:
        //constructors and destructors
        extended_dynamic_mesh_batch();
        virtual ~extended_dynamic_mesh_batch();
        
        //mesh management
        uint32_t mesh_count() const;
        bool contains_mesh(const mesh* m) const;
        void add_mesh(mesh* m, const process* p);
        void clear_meshes();
        
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
