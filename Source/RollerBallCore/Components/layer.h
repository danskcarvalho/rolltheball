//
//  layer.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 07/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__layer__
#define __RollerBallCore__layer__

#include "components_base.h"
#include "node_container.h"
#include "color.h"
#include "transform_space.h"

//Layer transforms
#define LAYER_TRANSFORM         0
#define ADJUST_LAYER_TRANSFORM  1

namespace rb {
    class scene;
    class node;
    class extended_dynamic_mesh_batch;
    class extended_static_mesh_batch;
    class process;
    class mesh;
    class texture_atlas;
    class basic_process;
    class transform_space;
    class color;
    class texture_map;
    
    class layer : public node_container {
    private:
        //fields
        bool _playing;
        scene* _parent_scene;
        
        //rendering
        extended_dynamic_mesh_batch* _dynamic_batch;
        extended_static_mesh_batch* _static_batch;
        rb_string _primary_group_name;
        rb_string _secondary_group_name;
        std::vector<process*> _processes;
        std::map<rb_string, basic_process*> _tx_proc_map[EFFECT_TEXTURED_COUNT];
        basic_process* _textureless_process;
        std::map<rb_string, rb_string> _tx_group_map;
        bool _validated;
        bool _static_layer;
        //some parameters
        color _ambient_color;
        blend_mode _blend_mode;
        //constructor/destructors
    private:
        layer();
        layer(scene* parent_scene);
        virtual ~layer();
    private:
        //activation listeners
        void before_becoming_active();
        void after_becoming_active();
        void before_becoming_inactive();
        void after_becoming_inactive();
    public:
        //friend classes
        friend class scene;
        friend class node;
        friend class director;
        friend class node_container;
        
        scene* parent_scene();
        const scene* parent_scene() const;
        
        //Active Flag
        bool active() const;
        
        //Adding/Removing children
        virtual bool add_node_at(node* n, uint32_t at) override;
        virtual bool remove_node(node* n, bool cleanup) override;
        
        //Reordering nodes
        virtual bool bring_to_front(node* n) override;
        virtual bool send_to_back(node* n) override;
        virtual bool send_backward(node* n) override;
        virtual bool bring_forward(node* n) override;
        
        //Play/Pause
    public:
        bool playing_flag() const;
        bool playing() const;
        void playing(const bool value);
    private:
        void scene_playing();
        void scene_paused();
        
        //Rendering
        //Some rendering states
    public:
        const color& ambient_color() const;
        const color& ambient_color(const color& value);
        blend_mode blend_mode() const;
        enum blend_mode blend_mode(const enum blend_mode value);
        inline bool static_layer() const {
            return _static_layer;
        }
        bool static_layer(const bool value);
        const rb_string& primary_group_name() const;
        const rb_string& primary_group_name(const rb_string& value);
        const rb_string& secondary_group_name() const;
        const rb_string& secondary_group_name(const rb_string& value);
        
        //Rendering functions
    private:
        void setup_processes();
        void setup_processes_for_gizmo_layer();
        void create_textured_effect(const int current_effect, const std::vector<rb_string>& texture_names);
        void texture_atlas_changed();
        void create_texture_group_mapping();
        void invalidate_buffers();
        void add_mesh_for_rendering(mesh* m, const rb_string& texture_name, bool wrapping_in_shader);
        void set_texture_for_mesh(mesh* m, const rb_string& texture_name, bool wrapping_in_shader);
        void add_gizmo(mesh* m, const rb_string& texture_name, bool wrapping_in_shader);
        const texture_map* create_mapping(const rb_string& texture_name, const transform_space& transform, texture_map* refurbished = nullptr);
        //render calll
        void render();
        
        //Transform
    protected:
        virtual const node_container* parent_node_container() const override;
    public:
        transform_space from_layer_space_to(const space another) const;
        transform_space from_layer_space_to(const node_container* another) const;
    protected:
        //Typed Object
        virtual void describe_type() override;
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
    public:
        virtual std::vector<rb_string> transformables();
        virtual void start_transformation(long index);
    protected:
        virtual void set_children(const std::vector<typed_object*>& children) override;
    };
}

#endif /* defined(__RollerBallCore__layer__) */
