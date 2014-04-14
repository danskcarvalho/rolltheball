//
//  keyframe_animation_component.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 2014-04-13.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__keyframe_animation_component__
#define __RollerBallCore__keyframe_animation_component__

#include "components_base.h"
#include "nvnode.h"
#include "vec2.h"
#include "resettable_component.h"
#include "transform_space.h"

namespace rb {
    enum class easing_function {
        linear,
        ease_in,
        ease_out,
        ease_in_out,
        ease_elastic_in,
        ease_elastic_out,
        ease_elastic_in_out,
        ease_bounce_in,
        ease_bounce_out,
        ease_bounce_in_out,
        ease_back_in,
        ease_back_out,
        ease_back_in_out,
        ease_exponential_in,
        ease_exponential_out,
        ease_exponential_in_out,
    };
    struct keyframe {
        struct easing_info {
            easing_function position_x_easing;
            float position_x_easing_factor;
            easing_function position_y_easing;
            float position_y_easing_factor;
            easing_function rotation_easing;
            float rotation_easing_factor;
        };
        uint32_t index;
        std::unordered_set<node*> animated;
        std::unordered_map<node*, transform_space> transforms;
        float delay;
        uint32_t n_frames;
        //Easing
        std::unordered_map<node*, easing_info> easings;
        //placeholder
        bool is_placeholder;
        
        //constructor
        keyframe();
    };
    class polygon_component;
    class keyframe_animation_component : public nvnode, public resettable_component {
    public:
        typedef std::list<keyframe>::iterator iterator;
    private:
        std::list<keyframe> _keyframes; //list of keyframes...
        iterator _current_pos; //keyframe current pos...
        std::vector<rb_string> _anim_names;
        std::vector<node*> _anim_nodes; //this includes the polygon points and attached nodes...
        std::unordered_map<node*, transform_space> _anim_nodes_saved_transforms;
        uint32_t _n_frames; //number of frames...
        std::vector<vec2> _anim_positions; //positions of animated elements... //each [n_frames] corresponds to one node...
        std::vector<float> _anim_rotations; //animated rotations
        bool _dirty_anim; //if _anim_positions and _anim_rotations needs to be updated...
        //attachment to path
        struct attach_info {
            polygon_component* attached;
            float at_length;
        };
        std::unordered_map<node*, attach_info> _attachments;
        //play control
        bool _playing_anim;
        uint32_t _current_frame_an;
    private:
        void generate_animation(node* n, size_t start_index);
        void generate_animation_for_attached(std::vector<node*>& nodes, polygon_component* attachable);
        void set_internal_animation_if_dirty();
        //ease functions
        static float ease(easing_function func, float t, float f);
        static float ease_in(float t, float f);
        static float ease_out(float t, float f);
        static float ease_in_out(float t, float f);
        static float ease_elastic_in(float t, float f);
        static float ease_elastic_out(float t, float f);
        static float ease_elastic_in_out(float t, float f);
        static float ease_bounce_in(float t, float f);
        static float ease_bounce_out(float t, float f);
        static float ease_bounce_in_out(float t, float f);
        static float ease_back_in(float t, float f);
        static float ease_back_out(float t, float f);
        static float ease_back_in_out(float t, float f);
        static float ease_exponential_in(float t, float f);
        static float ease_exponential_out(float t, float f);
        static float ease_exponential_in_out(float t, float f);
    public:
        //constructor
        keyframe_animation_component();
        //values and actions
        long current_index() const;
        void reselect_animated();
        void preview_current_keyframe();
        void update_transforms();
        void record_keyframe();
        void record_add_to_keyframe(); //record previously unrecorded...
        float current_delay() const;
        float current_delay(float value);
        easing_function current_position_x_easing() const;
        easing_function current_position_x_easing(easing_function value);
        float current_position_x_factor() const;
        float current_position_x_factor(float value);
        easing_function current_position_y_easing() const;
        easing_function current_position_y_easing(easing_function value);
        float current_position_y_factor() const;
        float current_position_y_factor(float value);
        easing_function current_rotation_easing() const;
        easing_function current_rotation_easing(easing_function value);
        float current_rotation_factor() const;
        float current_rotation_factor(float value);
        bool current_is_placeholder() const;
        //actions outside of the current selected
        //moving
        void goto_first();
        void goto_last();
        void goto_placeholder();
        void goto_previous();
        void goto_next();
        //deletion
        void delete_current();
        //start frame
        void record_start(); //also reset keyframes...
        void reset_transforms(); //reset transforms to the start...
        void clear_animation(); //reset all...
        //attachments
        //attachments must be setupped
        void setup_attachment(const rb_string& objects_class, const rb_string& polygon_id);
        void remove_attachment_for(const rb_string& polygon_id);
        void remove_attachment_for_all();
        //play
        void play_animation();
        void pause_animation();
    };
}

#endif /* defined(__RollerBallCore__keyframe_animation_component__) */







































