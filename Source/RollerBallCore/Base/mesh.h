//
//  mesh.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 10/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__mesh__
#define __RollerBallCore__mesh__

#include "base.h"
#include <functional>

namespace rb {
    class vertex;
    class rectangle;
    class vec2;
    class brush;
    class texture_map;
    class color;
    class buffer;
    class mesh {
    private:
        vertex* _vb;
        uint32_t _vb_count;
        uint16_t* _ib;
        uint32_t _ib_count;
        bool _external;
        bool _dirty_vb;
        bool _dirty_ib;
    private:
        void load_from_buffer(const void* buffer, const void** next);
        void store_in_buffer(void* buffer, size_t* size, void** next) const;
    public:
        mesh();
        mesh(vertex* vb, uint32_t vb_count, uint16_t* ib, uint32_t ib_count);
        mesh(buffer buffer);
        mesh(const mesh& other);
        mesh(mesh&& other);
        const mesh& operator=(const mesh& other);
        const mesh& operator=(mesh&& other);
        bool is_empty() const;
        
        //inline properties
        inline bool dirty() const {
            return _dirty_vb || _dirty_ib;
        }
        
        inline bool dirty_vertex_data() const {
            return _dirty_vb;
        }
        
        inline bool dirty_index_data() const {
            return _dirty_ib;
        }
        
        inline void clear_dirty_flag() {
            _dirty_vb = false;
            _dirty_ib = false;
        }
        
        mesh& set_buffers(vertex* vb, uint32_t vb_count, uint16_t* ib, uint32_t ib_count, bool assume_ownership);
        
        const vertex* vertex_buffer() const;
        const uint32_t vertex_count() const;
        
        const uint16_t* index_buffer() const;
        const uint32_t index_count() const;
        
        rectangle compute_bounds() const;
        
        //alterations
        void lock_buffers(std::function<void(vertex* vb, uint16_t* ib)> f);
        void lock_vertex_buffer(std::function<void(vertex* vb)> f);
        void lock_index_buffer(std::function<void(uint16_t* ib)> f);
        void remap(const rectangle& texture_bounds, const texture_mapping_type mapping_type);
        void expand_from_atlas_bounds(const texture_map& map, bool reapply_mapping);
        static mesh* merge_meshes(const std::vector<mesh*>& meshes);
        //other alterations
        void set_alpha(const float alpha);
        void set_blend(const float blend);
        void set_color(const color& color);
        void set_texture_bounds(const rectangle& bounds);
        
        //operations
        void paint(const vec2& position, const brush& brush);
        mesh& to_line_mesh(mesh& storage);
        
        //to string
        rb_string to_string() const;
        
        //to_buffer
        buffer to_buffer() const;
        
        //vectors of mesh
        static buffer to_buffer(const std::vector<mesh*>& meshes);
        static std::vector<mesh*> from_buffer(buffer b);
        
        ~mesh();
    };
}

#endif /* defined(__RollerBallCore__mesh__) */
