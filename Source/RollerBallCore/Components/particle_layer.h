//
//  particle_layer.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 23/12/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__particle_layer__
#define __RollerBallCore__particle_layer__

#include "components_base.h"
#include "vec2.h"
#include "color.h"
#include "transform_space.h"
#include <random>

#define INFINITE_DURATION -1
#define PARTICLE_BLOCK_SIZE 1500
#define MAX_FRAGMENTATION_ALLOWED 0.42f

namespace rb {
    class mesh;
    class particle_allocator;
    struct emitter_info;
    class particle_layer;
    struct particle_block;
    struct particle_info;
    class layer;
    class texture_map;
    class process;
    struct ex_emitter_info;
    typedef void* emitter_id;
    typedef std::function<void (particle_info*, float)> particle_updater_function;
    typedef std::function<void (emitter_id, particle_layer*, float)> emitter_function;
    typedef std::function<void (particle_info*)> particle_destroy_function;
    typedef std::function<void (emitter_id)> emitter_destroy_function;
    struct particle_info {
        matrix3x3 base_transform;
        vec2 size;
        vec2 max_size;
        vec2 translation;
        float angle = 0;
        vec2 velocity;
        float tangential_velocity = 0;
        float perpendicular_velocity = 0;
        vec2 acceleration;
        float tangential_acceleration = 0;
        float perpendicular_acceleration = 0;
        float velocity_exp_rate = 0;
        float radial_velocity = 0;
        float radial_acceleration = 0;
        vec2 size_rate;
        float opacity = 0;
        float opacity_rate = 0;
        float blend = 0;
        float blend_rate = 0;
        color start_color;
        color end_color;
        float current_color_mix;
        float color_mixing_velocity;
        float life_remaining = 0;
        void* user_data = nullptr;
    };
    struct ex_particle_info {
        particle_info pinfo;
        mesh* assigned_mesh;
        ex_emitter_info* emitter;
        particle_block* block;
        particle_state state;
        bool collapsed;
    };
    
    struct emitter_info {
        bool initialized = false;
        float velocity_angle_01 = 0;
        float velocity_angle_02 = 0;
        float velocity_01 = 0;
        float velocity_02 = 0;
        float velocity_rate_01 = 0;
        float velocity_rate_02 = 0;
        float acceleration_angle_01 = 0;
        float acceleration_angle_02 = 0;
        float acceleration_01 = 0;
        float acceleration_02 = 0;
        float tangential_velocity_01 = 0;
        float tangential_velocity_02 = 0;
        float tangential_acceleration_01 = 0;
        float tangential_acceleration_02 = 0;
        float perpendicular_velocity_01 = 0;
        float perpendicular_velocity_02 = 0;
        float perpendicular_acceleration_01 = 0;
        float perpendicular_acceleration_02 = 0;
        vec2 gravity_acceleration;
        float angle_01 = 0;
        float angle_02 = 0;
        float radial_velocity_01 = 0;
        float radial_velocity_02 = 0;
        float radial_acceleration_01 = 0;
        float radial_acceleration_02 = 0;
        bool aspect_correction = true;
        vec2 aspect_correction_factor;
        vec2 size_01;
        vec2 size_02;
        vec2 size_rate_01;
        vec2 size_rate_02;
        bool uniform_size = true;
        vec2 max_size;
        rb_string image_name;
        float opacity_01 = 0;
        float opacity_02 = 0;
        float opacity_rate_01 = 0;
        float opacity_rate_02 = 0;
        float blend_01 = 0;
        float blend_02 = 0;
        float blend_rate_01 = 0;
        float blend_rate_02 = 0;
        color start_color_01;
        color start_color_02;
        color end_color_01;
        color end_color_02;
        float start_color_mix_01 = 0;
        float start_color_mix_02 = 0;
        float color_mixing_velocity_01 = 0;
        float color_mixing_velocity_02 = 0;
        float life_01 = 0;
        float life_02 = 0;
        float inv_emission_rate = 0; //its 1/er where er is particles/second
        float duration = 0;
        float delay = 0;
        float emission_radius_01 = 0;
        float emission_radius_02 = 0;
        bool loop = false;
        matrix3x3 transform;
        void* user_data = nullptr;
        particle_updater_function updater_func;
        emitter_function emitter_func;
        particle_destroy_function particle_destroy_func;
        emitter_destroy_function emitter_destroy_func;
    };
    struct ex_emitter_info {
        emitter_info einfo;
        particle_state state = particle_state::stopped;
        float duration_remaining = 0;
        float delay_remaining = 0;
        float emission_acc = 0;
    };
    class mesh;
    class extended_dynamic_mesh_batch;
    
    class quad_mesh_allocator {
    private:
        std::vector<void*> _blocks;
        std::vector<mesh*> _meshes;
    public:
        void alloc_meshes(size_t n_meshes, std::vector<mesh*>& meshes);
        ~quad_mesh_allocator();
    };
    
    struct particle_block {
        particle_block* previous;
        particle_block* next;
        size_t allocated_blocks;
        size_t allocation_index;
        ex_particle_info particles[PARTICLE_BLOCK_SIZE];
    };
    
    class particle_iterator {
    private:
        particle_block* _current;
        particle_allocator* _parent;
        size_t _index;
        bool _include_freed;
        particle_iterator();
    public:
        particle_iterator(const particle_iterator& it);
        particle_iterator(particle_iterator&& it);
        particle_iterator& operator=(const particle_iterator& other);
        particle_iterator& operator=(particle_iterator&& other);
        
        friend class particle_allocator;
        ex_particle_info* next();
        void reset();
        ~particle_iterator();
    };
    
    class particle_allocator {
    private:
        particle_block* _first_block;
        particle_block* _spare_block;
        particle_block* _current_block;
        quad_mesh_allocator* _m_allocator;
        size_t _iteration_count;
        particle_block* compress_block(particle_block* block);
        bool needs_compresssion(particle_block* block);
    public:
        friend class particle_iterator;
        particle_allocator();
        ex_particle_info* alloc(size_t n_particles, size_t& allocated, extended_dynamic_mesh_batch* edmb, process* p);
        void free(ex_particle_info* particle);
        ~particle_allocator();
        particle_iterator iterate(bool include_freed);
    };
    
    class particle_layer {
    private:
        typedef std::vector<ex_emitter_info*> emitter_list;
        emitter_list _emitters;
        particle_allocator* _p_allocator;
        extended_dynamic_mesh_batch* _batch;
        particle_state _state;
        layer* _parent_layer;
        bool _invalidated;
        mesh* _quad_ref;
        texture_map* _map;
        const matrix3x3 _zero_matrix;
        std::mt19937 _generator;
        std::uniform_real_distribution<float> _distribution;
    private:
        void update_emitter(ex_emitter_info* einfo, float dt);
        void update_particle(ex_particle_info* pinfo, float dt);
        void update_particle_mesh(ex_particle_info* pinfo);
        float random_0_1();
        vec2 size_of_tex(const rb_string& img_name) const;
        vec2 aspect_correction_factor(const rb_string& img_name) const;
    public:
        particle_layer(class layer* layer);
        ~particle_layer();
        emitter_id register_emitter(emitter_info* einfo);
        void unregister_emitter(emitter_id eid);
        emitter_info* emitter(emitter_id eid);
        particle_state state() const;
        particle_state state(particle_state value);
        const layer* parent_layer() const;
        class layer* parent_layer();
        
        particle_info* alloc_particle(particle_info* pinfo, emitter_id eid, particle_state initial_state);
        void free_particle(particle_info* pinfo);
        emitter_id get_emitter(particle_info* pinfo);
        particle_state particle_state(particle_info* pinfo) const;
        enum particle_state particle_state(particle_info* pinfo, enum particle_state value);
        enum particle_state emitter_state(emitter_id eid) const;
        enum particle_state emitter_state(emitter_id eid, enum particle_state value);
        void unregister_all_emitters();
        void reset_emitter(emitter_id eid);
        void seed(uint32_t value);
        
        void update(float dt);
        void render();
        void invalidate_buffers();
        blend_mode blend_mode() const;
        enum blend_mode blend_mode(enum blend_mode value);
    };
}

#endif /* defined(__RollerBallCore__particle_layer__) */





















