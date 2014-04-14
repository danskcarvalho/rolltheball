//
//  keyframe_animation_component.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 2014-04-13.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#include "keyframe_animation_component.h"
#include "polygon_component.h"
#include "smooth_curve.h"
#include "polygon_path.h"

using namespace rb;

keyframe::keyframe(){
    index = 0;
    delay = 0;
    n_frames = 0;
    is_placeholder = false;
}

keyframe_animation_component::keyframe_animation_component(){
    _keyframes.push_back(keyframe());
    _keyframes.front().is_placeholder = true;
    _current_pos = _keyframes.begin();
    _n_frames = 0;
    _dirty_anim = false;
    _playing_anim = false;
    _current_frame_an = 0;
}

static inline float to_canonical_angle(float a){
    auto _2pi = 2 * (float)M_PI;
    auto _d = (int)(a / _2pi);
    a -= _d * _2pi;
    
    if(a < 0)
        a = 2 * M_PI + a;
    
    return a;
}

static float interp_angle(float step, float from, float to)
{
    auto _2pi = (float)M_PI * 2;
    from = to_canonical_angle(from);
    to = to_canonical_angle(to);
    if(almost_equal(from, _2pi))
        from = 0;
    if(almost_equal(to, _2pi))
        to = 0;
    if(almost_equal(from, to))
        return from;
    
    if(from > to){
        std::swap(from, to);
        step = 1 - step;
    }

    float d1 = to - from;
    float d2 = (to - _2pi) - from;
    float d = fabsf(d1) < fabsf(d2) ? d1 : d2;
    return from + d * step;
}

void keyframe_animation_component::generate_animation(rb::node *n, size_t start_index){
    size_t _offset = 0;
    
    for (auto _k : _keyframes){
        bool _contains = _k.animated.count(n) != 0;
        auto _init_pos = _offset == 0 ? _anim_nodes_saved_transforms[n].origin() : _anim_positions[_offset + start_index - 1];
        auto _init_rot = _offset == 0 ? _anim_nodes_saved_transforms[n].rotation().x() : _anim_rotations[_offset + start_index - 1];
        
        for(uint32_t i = 0; i < _k.n_frames; i++){
            if(!_contains){
                auto _previous_pos = _offset == 0 ? _anim_nodes_saved_transforms[n].origin() : _anim_positions[_offset + start_index - 1];
                auto _previous_rot = _offset == 0 ? _anim_nodes_saved_transforms[n].rotation().x() : _anim_rotations[_offset + start_index - 1];
                
                _anim_positions[_offset + start_index] = _previous_pos;
                _anim_rotations[_offset + start_index] = _previous_rot;
            }
            else {
                auto _t = ((float)(i + 1)) / ((float)_k.n_frames);
                
                auto _vx = _init_pos.x() + (_k.transforms[n].origin().x() - _init_pos.x())* ease(_k.easings[n].position_x_easing, _t, _k.easings[n].position_x_easing_factor);
                auto _vy = _init_pos.y() + (_k.transforms[n].origin().y() - _init_pos.y())* ease(_k.easings[n].position_y_easing, _t, _k.easings[n].position_y_easing_factor);
                _anim_positions[_offset + start_index] = vec2(_vx, _vy);
                
                _anim_rotations[_offset + start_index] = interp_angle(ease(_k.easings[n].rotation_easing, _t, _k.easings[n].rotation_easing_factor), _init_rot, _k.transforms[n].rotation().x());
            }
            _offset++;
        }
    }
}

polygon reconstruct_polygon(rb::polygon_component *pol_component, std::vector<vec2>& pts, transform_space& ts){
    polygon _polygon;
    _polygon = polygon::build_open_polygon(pts, _polygon);
    ts.from_space_to_base().transform_polygon(_polygon);
    
    if (pol_component->smooth() && pts.size() >= 4){
        smooth_curve _sc = smooth_curve::build_open_curve(pts, _sc);
        
        polygon _f_pol;
        _sc.to_polygon(_f_pol, pol_component->smooth_quality(), pol_component->smooth_divisions());
        ts.from_space_to_base().transform_polygon(_f_pol);
        return _f_pol;
    }
    else
        return _polygon;
}

void keyframe_animation_component::generate_animation_for_attached(std::vector<node *> &nodes, rb::polygon_component *attachable){
    //1.get indexes for all nodes
    std::unordered_map<node*, size_t> _indexes; //indexes for nodes, points and polygon
    for (auto _n : nodes)
        _indexes[_n] = -1;
    _indexes[attachable] = -1;
    std::vector<node*> _points;
    for (auto _p : *attachable){
        _indexes[_p] = -1;
        _points.push_back(_p); //in order!
    }
    
    for(size_t i = 0; i < this->_anim_nodes.size(); i++){
        if(_indexes.count(this->_anim_nodes[i]))
            _indexes[_anim_nodes[i]] = i;
    }
    
    
    size_t _offset = 0;
    for (auto _k : _keyframes){
        for(uint32_t i = 0; i < _k.n_frames; i++){
            //2. reconstruct polygon...
            //get all the points current situations
            std::vector<vec2> _pt_pos;
            for(auto _p : _points)
                _pt_pos.push_back(_anim_positions[_indexes[_p] * _n_frames + _offset]);
            //get the current transformation of polygon
            auto _current_pol_pt = _anim_positions[_indexes[attachable] * _n_frames + _offset];
            auto _current_pol_rot = _anim_rotations[_indexes[attachable] * _n_frames + _offset];
            auto _current_pol_t = attachable->transform().moved(_current_pol_pt).rotated(_current_pol_rot, _current_pol_rot + M_PI_2);
            auto _polygon = reconstruct_polygon(attachable, _pt_pos, _current_pol_t);
            //3. do a length mapping
            polygon_path _path = polygon_path(_polygon);
            //4. infer positions and rotations
            for (size_t j = 0; j < nodes.size(); j++) {
                auto _p = _path.point_at(_attachments[nodes[j]].at_length, true);
                auto _pa = (j != (nodes.size() - 1)) ? _path.point_at(_attachments[nodes[j + 1]].at_length, true) : _path.point_at(_attachments[nodes[j - 1]].at_length, true);
                _pa = _pa - _p;
                if(j == (nodes.size() - 1))
                    _pa = -_pa;
                auto _r = vec2::right.angle_between(_pa, rotation_direction::ccw);
                
                //set position and r...
                _anim_positions[_indexes[nodes[j]] * _n_frames + _offset] = _p;
                if(nodes[j]->has_class(u"changeRot"))
                    _anim_rotations[_indexes[nodes[j]] * _n_frames + _offset] = _r;
                else
                    _anim_rotations[_indexes[nodes[j]] * _n_frames + _offset] = nodes[j]->transform().rotation().x();
            }
            
            _offset++;
        }
    }
}

void keyframe_animation_component::set_internal_animation_if_dirty(){
    if(!_dirty_anim)
        return;
    
    _n_frames = 0;
    for (auto _k : _keyframes){
        if(_k.is_placeholder)
            continue;
        _k.n_frames = roundf(_k.delay * 30.0f);
        _n_frames += _k.n_frames;
    }
    
    //1. generate dummy values
    auto _n_dummy = _n_frames * _anim_nodes.size();
    _anim_positions.clear();
    _anim_rotations.clear();
    for (size_t i = 0; i < _n_dummy; i++){
        _anim_positions.push_back(vec2::zero);
        _anim_rotations.push_back(0);
    }
    //1. generate animation for all non attached...
    size_t _offset = 0;
    for (auto _n : _anim_nodes){
        if(!_attachments.count(_n)) //not attached
            generate_animation(_n, _offset);
        _offset += _n_frames;
    }
    
    //2. generate animation for all attached...
    //2.1 we group by the attachable node...
    //2.1 first we gather all attachable polygons...
    std::unordered_set<polygon_component*> _attachable_set;
    std::unordered_multimap<polygon_component*, node*> _attachments_s;
    for (auto _kvp : _attachments){
        _attachable_set.insert(_kvp.second.attached);
        _attachments_s.insert({_kvp.second.attached, _kvp.first});
    }
    
    for (auto _p : _attachable_set){
        auto _r = _attachments_s.equal_range(_p);
        std::vector<node*> _att_nodes;
        for(auto _it = _r.first; _it != _r.second; _it++)
            _att_nodes.push_back(_it->second);
        std::sort(_att_nodes.begin(), _att_nodes.end(), [=](node* a, node* b){
            return _attachments.at(a).at_length < _attachments.at(b).at_length;
        });
        
        generate_animation_for_attached(_att_nodes, _p);
    }
}

float keyframe_animation_component::ease(rb::easing_function func, float t, float f){
    if(func == easing_function::ease_back_in)
        return ease_back_in(t, f);
    else if(func == easing_function::ease_back_in_out)
        return ease_back_in_out(t, f);
    else if(func == easing_function::ease_back_out)
        return ease_back_out(t, f);
    else if(func == easing_function::ease_bounce_in)
        return ease_bounce_in(t, f);
    else if(func == easing_function::ease_bounce_in_out)
        return ease_bounce_in_out(t, f);
    else if(func == easing_function::ease_bounce_out)
        return ease_bounce_out(t, f);
    else if(func == easing_function::ease_elastic_in)
        return ease_elastic_in(t, f);
    else if(func == easing_function::ease_elastic_in_out)
        return ease_elastic_in_out(t, f);
    else if(func == easing_function::ease_elastic_out)
        return ease_elastic_out(t, f);
    else if(func == easing_function::ease_exponential_in)
        return ease_exponential_in(t, f);
    else if(func == easing_function::ease_exponential_in_out)
        return ease_exponential_in_out(t, f);
    else if(func == easing_function::ease_exponential_out)
        return ease_exponential_out(t, f);
    else if(func == easing_function::ease_in)
        return ease_in(t, f);
    else if(func == easing_function::ease_in_out)
        return ease_in_out(t, f);
    else //ease_out
        return ease_out(t, f);
}

float keyframe_animation_component::ease_in(float t, float f){
    return powf(t, f);
}

float keyframe_animation_component::ease_out(float t, float f){
    return powf(t, 1 / f);
}

float keyframe_animation_component::ease_in_out(float t, float f){
    int sign = 1;
    int r = (int)f;
    
    if (r % 2 == 0)
    {
        sign = -1;
    }
    
    t *= 2;
    if (t < 1)
    {
        return 0.5f * powf(t, f);
    }
    else
    {
        return sign * 0.5f * (powf(t - 2, f) + sign * 2);
    }
}

#define M_PI_X_2 ((float)M_PI * 2.0f)

float keyframe_animation_component::ease_elastic_in(float t, float f){
    float newT = 0;
    if (t == 0 || t == 1)
    {
        newT = t;
    }
    else
    {
        float s = f / 4;
        t = t - 1;
        newT = -powf(2, 10 * t) * sinf((t - s) * M_PI_X_2 / f);
    }
    
    return newT;
}

float keyframe_animation_component::ease_elastic_out(float t, float f){
    float newT = 0;
    if (t == 0 || t == 1)
    {
        newT = t;
    }
    else
    {
        float s = f / 4;
        newT = powf(2, -10 * t) * sinf((t - s) * M_PI_X_2 / f) + 1;
    }
    
    return newT;
}

float keyframe_animation_component::ease_elastic_in_out(float t, float f){
    float newT = 0;
    if (t == 0 || t == 1)
    {
        newT = t;
    }
    else
    {
        float _period = f;
        t = t * 2;
        if (! _period)
        {
            _period = 0.3f * 1.5f;
        }
        
        float s = _period / 4;
        
        t = t - 1;
        if (time < 0)
        {
            newT = -0.5f * powf(2, 10 * t) * sinf((t - s) * M_PI_X_2 / _period);
        }
        else
        {
            newT = powf(2, -10 * t) * sinf((t - s) * M_PI_X_2 / _period) * 0.5f + 1;
        }
    }
    
    return newT;
}

float bounce_time(float time) {
    if (time < 1 / 2.75)
    {
        return 7.5625f * time * time;
    } else
		if (time < 2 / 2.75)
		{
			time -= 1.5f / 2.75f;
			return 7.5625f * time * time + 0.75f;
		} else
            if(time < 2.5 / 2.75)
            {
                time -= 2.25f / 2.75f;
                return 7.5625f * time * time + 0.9375f;
            }
    
    time -= 2.625f / 2.75f;
    return 7.5625f * time * time + 0.984375f;
}

float keyframe_animation_component::ease_bounce_in(float t, float f){
    float newT = 1 - bounce_time(1 - t);
    return newT;
}

float keyframe_animation_component::ease_bounce_out(float t, float f){
    return bounce_time(t);
}

float keyframe_animation_component::ease_bounce_in_out(float t, float f){
    float newT = 0;
    if (t < 0.5f)
    {
        t = t * 2;
        newT = (1 - bounce_time(1 - t)) * 0.5f;
    }
    else
    {
        newT = bounce_time(t * 2 - 1) * 0.5f + 0.5f;
    }
    return newT;
}

float keyframe_animation_component::ease_back_in(float t, float f){
    float overshoot = 1.70158f;
    return t * t * ((overshoot + 1) * t - overshoot);
}

float keyframe_animation_component::ease_back_out(float t, float f){
    float overshoot = 1.70158f;
    
    t = t - 1;
    return t * t * ((overshoot + 1) * t + overshoot) + 1;
}

float keyframe_animation_component::ease_back_in_out(float t, float f){
    float overshoot = 1.70158f * 1.525f;
    
    t = t * 2;
    if (t < 1)
    {
        return ((t * t * ((overshoot + 1) * t - overshoot)) / 2);
    }
    else
    {
        t = t - 2;
        return ((t * t * ((overshoot + 1) + overshoot)) / 2 + 1);
    }
}

float keyframe_animation_component::ease_exponential_in(float t, float f){
    return t == 0 ? 0 : powf(2, 10 * (t/1 - 1)) - 1 * 0.001f;
}

float keyframe_animation_component::ease_exponential_out(float t, float f){
    return t == 1 ? 1 : (-powf(2, -10 * t / 1) + 1);
}

float keyframe_animation_component::ease_exponential_in_out(float t, float f){
    t /= 0.5f;
    if (t < 1)
    {
        t = 0.5f * powf(2, 10 * (t - 1));
    }
    else
    {
        t = 0.5f * (-powf(2, 10 * (t - 1)) + 2);
    }
    
    return t;
}


long keyframe_animation_component::current_index() const{
    if(_current_pos->is_placeholder)
        return -1;
    else
        return _current_pos->index;
}



















