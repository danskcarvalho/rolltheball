//
//  texture.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 15/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__texture__
#define __RollerBallCore__texture__

#include "base.h"
#include "vec2.h"
#include "texture_map.h"
#include "transform_space.h"
#include "rectangle.h"

namespace rb {
    class texture_source;
    class texture_atlas;
    class texture {
    protected:
        texture();
        GLuint _texture_id;
        vec2 _texture_size;
        vec2 _original_size;
        texture_border _border;
        rectangle _bounds;
    private:
        void* _cg_texture;
        static texture* _blank;
    
        class simple_texture_map : public texture_map {
        private:
            transform_space _texture_transform;
            rectangle _bounds;
        public:
            simple_texture_map(const transform_space& texture_transform, const rectangle& _bounds);
            virtual void set_texture_coords(vertex& v) const;
            virtual ~simple_texture_map();
            virtual const rectangle& bounds() const;
            virtual const transform_space& get_texture_space() const;
        };
    public:
        friend class texture_atlas;
        virtual const texture_map* create_mapping(const transform_space& transform);
        inline const vec2& original_size() const {
            return _original_size;
        }
        inline const vec2& texture_size() const {
            return _texture_size;
        }
        inline bool loaded() const {
            return _texture_id != 0;
        }
        inline uint32_t texture_id() const {
            return _texture_id;
        }
        inline texture_border border() const {
            return _border;
        }
        inline const rectangle& bounds() const {
            return _bounds;
        }
        
        texture(const texture_source& source, const texture_border border = texture_border::none, const float scale = 1);
        static texture* from_cg_image(const void* image);
        static const texture* blank();
        virtual ~texture();
        virtual void* to_cg_image() const;
        virtual void* to_cg_image(const rectangle& subarea) const;
        virtual void save_to_file(const URL_TYPE url) const;
    };
}

#endif /* defined(__RollerBallCore__texture__) */
