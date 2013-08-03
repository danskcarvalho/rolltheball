//
//  texture_atlas.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 02/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__texture_atlas__
#define __RollerBallCore__texture_atlas__

#include "base.h"
#include "texture_source.h"
#include "texture_map.h"
#include "transform_space.h"
#include "texture_composition.h"
#include "rectangle.h"

namespace rb {
    class texture;
    class texture_source;
    class texture_map;
    class transform_space;
    class texture_composition;
    class xnode;
    class texture_atlas {
    private:
        enum texture_type {
            from_source,
            dynamic,
            composition,
            unknown
        };
        struct texture_info {
            texture_composition* composition;
            texture_source* source;
            texture_border border;
            float scale;
            void* user_data;
            size_t data_size;
            std::function<void (void*, void*)> content_creator;
            texture_type type;
            vec2 dynamic_image_size;
            
            texture_info(){
                source = nullptr;
                border = texture_border::none;
                scale = 0;
                user_data = nullptr;
                data_size = 0;
                dynamic_image_size = vec2::zero;
                composition = nullptr;
            }
            
            texture_info(texture_source* source, const texture_border border, const float scale)
            {
                this->composition = nullptr;
                this->source = source;
                this->border = border;
                this->scale = scale;
                this->user_data = nullptr;
                this->data_size = 0;
                this->type = from_source;
                dynamic_image_size = vec2::zero;
            }
            
            texture_info(const vec2& image_size, const std::function<void (void*, void*)>& f, void* user_data, size_t data_size, texture_border border){
                this->source = nullptr;
                this->border = border;
                this->scale = 1;
                if(!user_data)
                    this->user_data = nullptr;
                else {
                    this->user_data = malloc(data_size);
                    memcpy(this->user_data, user_data, data_size);
                }
                this->content_creator = f;
                this->data_size = data_size;
                this->type = dynamic;
                this->dynamic_image_size = image_size;
                this->composition = nullptr;
            }
            
            texture_info(const texture_composition* composition){
                source = nullptr;
                border = texture_border::none;
                scale = 0;
                user_data = nullptr;
                data_size = 0;
                dynamic_image_size = vec2::zero;
                this->composition = const_cast<texture_composition*>(composition);
                this->type = texture_type::composition;
            }
            
            texture_info& operator=(texture_info&& other){
                if(this == &other)
                    return *this;
                
                if(this->source)
                    delete this->source;
                
                this->source = other.source;
                this->border = other.border;
                this->scale = other.scale;
                other.source = nullptr;
                
                if(this->user_data)
                    free(this->user_data);
                
                this->user_data = other.user_data;
                other.user_data = nullptr;
                this->content_creator = other.content_creator;
                this->data_size = other.data_size;
                this->type = other.type;
                this->dynamic_image_size = other.dynamic_image_size;
                this->composition = other.composition;
                other.composition = nullptr;
                
                return *this;
            }
            
            texture_info(texture_info&& other){
                this->source = other.source;
                this->border = other.border;
                this->scale = other.scale;
                other.source = nullptr;
                this->user_data = other.user_data;
                other.user_data = nullptr;
                this->content_creator = other.content_creator;
                this->data_size = other.data_size;
                this->type = other.type;
                this->dynamic_image_size = other.dynamic_image_size;
                this->composition = other.composition;
                other.composition = nullptr;
            }
            
            texture_info& operator=(texture_info& other) = delete;
            texture_info(texture_info& other) = delete;
            
            ~texture_info(){
                if(source)
                    delete source; //we own the source...
                if(user_data)
                    free(user_data);
                if(composition)
                    delete composition;
            }
        };
        struct texture_bound {
            rb_string texture_name;
            rectangle bounds;
        };
        class simple_texture_map : public texture_map {
        private:
            transform_space _texture_transform;
            rectangle _bounds;
        public:
            simple_texture_map(const transform_space& texture_transform, const rectangle& bounds);
            virtual void set_texture_coords(vertex& v) const;
            virtual const rectangle& bounds() const;
            virtual const transform_space& get_texture_space() const;
            virtual ~simple_texture_map();
        };
        uint32_t _max_atlas_size;
        std::unordered_set<rb_string> _group_names;
        std::vector<texture*> _atlases;
        std::unordered_set<rb_string> _texture_names;
        std::unordered_map<rb_string, texture_info> _texture_infos;
        std::unordered_multimap<rb_string, rb_string> _group_textures;
        std::unordered_map<rb_string, std::vector<texture*>> _group_atlases;
        std::map<std::pair<rb_string, rb_string>, texture*> _texture_atlases;
        std::unordered_map<rb_string, std::vector<texture_bound>> _bounds;
        bool _dirty;
        bool _avoid_gl;
        
        //methods
        void free_textures();
        bool try_compile_group(uint32_t max_size, const rb_string& group_name, const std::vector<rb_string>& names, const std::vector<vec2>& sizes);
        bool try_compile(uint32_t max_size, const std::vector<rb_string>& names, const std::vector<vec2>& sizes);
        bool get_texture_sizes(const std::vector<rb_string>& names, std::vector<vec2>& sizes);
        bool draw_image(const void* data, const rb_string& texture_name, const rectangle& bounds);
        bool draw_dynamic_image(const void* data, const rb_string& texture_name, const rectangle& bounds);
        texture* create_texture(const void* data, const vec2& size);
        xnode serialize_to_xnode();
        void serialize_group_names(xnode& n);
        void serialize_texture_names(xnode& n);
    public:
        typedef std::unordered_set<rb_string>::const_iterator name_iterator;
        typedef std::vector<texture*>::const_iterator atlas_iterator;
        
        texture_atlas(bool avoid_gl);
        ~texture_atlas();
        
        bool add_texture(const rb_string& name, const std::vector<rb_string>& groups, const texture_source& source, const texture_border border = texture_border::none, const float scale = 1);
        bool add_dynamic_texture(const rb_string& name, const std::vector<rb_string>& groups, const vec2& size, const texture_border border, const std::function<void (void*, void*)>& f, void* user_data, uint32_t data_size);
        bool add_radial_gradient_texture(const rb_string & name, const std::vector<rb_string>& groups, const float size, const class color& color);
        bool add_composited_texture(const rb_string& name, const std::vector<rb_string>& groups, const texture_composition* composition, const texture_border border = texture_border::none);
        uint32_t texture_count() const;
        uint32_t texture_group_count() const;
        const name_iterator texture_names_begin() const;
        const name_iterator texture_names_end() const;
        const name_iterator texture_groups_begin() const;
        const name_iterator texture_groups_end() const;
        void get_texture_names(std::vector<rb_string>& names) const;
        void get_groups(std::vector<rb_string>& names) const;
        bool contains_texture(const rb_string& name) const;
        bool contains_group(const rb_string& name) const;
        void get_groups(const rb_string& name, std::vector<rb_string>& group_names) const;
        bool remove_texture(const rb_string& name);
        uint32_t max_atlas_size() const;
        uint32_t max_atlas_size(const uint32_t value);
        bool dirty() const;
        
        //compile the atlas...
        void compile();
    
        uint32_t atlas_count() const;
        const atlas_iterator atlases_begin() const;
        const atlas_iterator atlases_end() const;
        void get_atlases(std::vector<texture*>& atlases) const;
        //return null if no texture with this name...
        void get_atlas(const rb_string& group_name, std::vector<texture*>& atlases) const;
        void get_textures(const texture* atlas, std::vector<rb_string>& texture_names) const;
        const texture* get_atlas(const rb_string& group_name, const rb_string& texture_name) const;
        
        const texture_map* create_mapping(const rb_string& group_name, const rb_string& texture_name, const transform_space& transform, texture_map* refurbished = nullptr);
        rectangle get_bounds_in_pixels(const rb_string& group_name, const rb_string& texture_name) const;
        
        //save and load from file
        void save_to_directory(URL_TYPE directory_path);
        static texture_atlas* load_from_directory(URL_TYPE directory_path, const vec2& resolution);
    };
}

#endif /* defined(__RollerBallCore__texture_atlas__) */
