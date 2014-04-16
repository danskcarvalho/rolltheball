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
#include "scene.h"
#include "buffer.h"

using namespace rb;

#define ANIMATOR_UPDATE_PRIORITY -30000

keyframe::keyframe(){
    index = 0;
    delay = 0;
    n_frames = 0;
    is_placeholder = false;
}

keyframe_animation_component::keyframe_animation_component(){
    init();
}

void keyframe_animation_component::init(){
    _anim_nodes.clear();
    _anim_nodes_set.clear();
    _anim_nodes_saved_transforms.clear();
    _anim_positions.clear();
    _anim_rotations.clear();
    _attachments.clear();
    _keyframes.clear();
    _keyframes.push_back(keyframe());
    _keyframes.front().is_placeholder = true;
    _current_pos = _keyframes.begin();
    _n_frames = 0;
    _dirty_anim = false;
    _playing_anim = false;
    _playing_mirror = false;
    _current_frame_an = 0;
    _current_position_x_easing = easing_function::linear;
    _current_position_x_factor = 0;
    _current_position_y_easing = easing_function::linear;
    _current_position_y_factor = 0;
    _current_rotation_easing = easing_function::linear;
    _current_rotation_factor = 0;
    _ed_playing_anim = false;
    _ed_current_frame_an = 0;
    _ed_playing_mirror = false;
    _saved_playing_anim = false;
    _loop = false;
    _mirror = false;
    _initialized = false;
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
        if(_k.is_placeholder)
            continue;
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
        if(_k.is_placeholder)
            continue;
        
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
    _dirty_anim = false;
}

float keyframe_animation_component::ease(rb::easing_function func, float t, float f){
    if(func == easing_function::linear)
        return t;
    else if(func == easing_function::ease_back_in)
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

void keyframe_animation_component::reselect_animated(){
    if(!in_editor())
        return;
    
    parent_scene()->current()->clear_selection();
    for (auto _n : _current_pos->animated)
        parent_scene()->current()->add_to_selection(_n);
}

void keyframe_animation_component::preview_current_keyframe(){
    if(!in_editor())
        return;
    for (auto _n : _current_pos->animated)
        _n->transform(_current_pos->transforms[_n]);
}


void keyframe_animation_component::update_transforms(){
    if(!in_editor())
        return;
    for (auto _n : _current_pos->animated){
        if(!_n->is_selected())
            continue;
        if(_current_pos->transforms.count(_n))
            _current_pos->transforms[_n] = _n->transform();
    }
    placeholder_updated();
    _dirty_anim = true;
}

void keyframe_animation_component::placeholder_updated(){
    if(_current_pos->is_placeholder){
        _current_pos->is_placeholder = false;
        _keyframes.push_back(keyframe());
        _keyframes.back().is_placeholder = true;
        _keyframes.back().index = std::prev(_keyframes.end(), 2)->index + 1;
        notify_property_changed(u"current_index");
    }
}

void keyframe_animation_component::record_keyframe(){
    if(!in_editor())
        return;
    _current_pos->animated.clear();
    _current_pos->transforms.clear();
    _current_pos->easings.clear();
    
    
    std::vector<node*> _sel;
    parent_scene()->current()->fill_with_selection(_sel, node_filter::renderable);
    //checkings...
    for (auto _n : _sel){
        if(_anim_nodes_set.count(_n) == 0){
            parent_scene()->alert(u"One of the nodes hadn't it's transform state recorded at start...");
            return;
        }
    }
    //do it...
    for (auto _n : _sel){
        _current_pos->animated.insert(_n);
        _current_pos->transforms.insert({_n, _n->transform()});
        keyframe::easing_info _ei;
        _ei.position_x_easing = _current_position_x_easing;
        _ei.position_x_easing_factor = _current_position_x_factor;
        _ei.position_y_easing = _current_position_y_easing;
        _ei.position_y_easing_factor = _current_position_y_factor;
        _ei.rotation_easing = _current_rotation_easing;
        _ei.rotation_easing_factor = _current_rotation_factor;
        _current_pos->easings.insert({_n, _ei});
    }
    placeholder_updated();
    _dirty_anim = true;
}

void keyframe_animation_component::record_add_to_keyframe(){
    if(!in_editor())
        return;
    
    std::vector<node*> _sel;
    parent_scene()->current()->fill_with_selection(_sel, node_filter::renderable);
    //checkings...
    for (auto _n : _sel){
        if(_anim_nodes_set.count(_n) == 0){
            parent_scene()->alert(u"One of the nodes hadn't it's transform state recorded at start...");
            return;
        }
    }
    //do it...
    for (auto _n : _sel){
        if(_current_pos->animated.count(_n) == 0)
            _current_pos->animated.insert(_n);
        if(_current_pos->animated.count(_n) == 0)
            _current_pos->transforms.insert({_n, _n->transform()});
        else
            _current_pos->transforms[_n] = _n->transform();
        keyframe::easing_info _ei;
        _ei.position_x_easing = _current_position_x_easing;
        _ei.position_x_easing_factor = _current_position_x_factor;
        _ei.position_y_easing = _current_position_y_easing;
        _ei.position_y_easing_factor = _current_position_y_factor;
        _ei.rotation_easing = _current_rotation_easing;
        _ei.rotation_easing_factor = _current_rotation_factor;
        if(_current_pos->animated.count(_n) == 0)
            _current_pos->easings.insert({_n, _ei});
        else
            _current_pos->easings[_n] = _ei;
    }
    placeholder_updated();
    _dirty_anim = true;
}

bool keyframe_animation_component::loop() const {
    return _loop;
}

bool keyframe_animation_component::loop(bool value){
    return _loop = value;
}

bool keyframe_animation_component::mirror() const {
    return _mirror;
}

bool keyframe_animation_component::mirror(bool value){
    return _mirror = value;
}

void keyframe_animation_component::set_delay() {
    _current_pos->delay = _current_delay;
    placeholder_updated();
    _dirty_anim = true;
}

float keyframe_animation_component::current_delay() const {
    return _current_delay;
}

float keyframe_animation_component::current_delay(float value){
    return _current_delay = value;
}

easing_function keyframe_animation_component::current_position_x_easing() const {
    return _current_position_x_easing;
}

easing_function keyframe_animation_component::current_position_x_easing(rb::easing_function value){
    return _current_position_x_easing = value;
}

float keyframe_animation_component::current_position_x_factor() const {
    return _current_position_x_factor;
}

float keyframe_animation_component::current_position_x_factor(float value){
    return _current_position_x_factor = value;
}

easing_function keyframe_animation_component::current_position_y_easing() const {
    return _current_position_y_easing;
}

easing_function keyframe_animation_component::current_position_y_easing(rb::easing_function value){
    return _current_position_y_easing = value;
}

float keyframe_animation_component::current_position_y_factor() const {
    return _current_position_y_factor;
}

float keyframe_animation_component::current_position_y_factor(float value){
    return _current_position_y_factor = value;
}

easing_function keyframe_animation_component::current_rotation_easing() const {
    return _current_rotation_easing;
}

easing_function keyframe_animation_component::current_rotation_easing(rb::easing_function value){
    return _current_rotation_easing = value;
}

float keyframe_animation_component::current_rotation_factor() const {
    return _current_rotation_factor;
}

float keyframe_animation_component::current_rotation_factor(float value){
    return _current_rotation_factor = value;
}

bool keyframe_animation_component::current_is_placeholder() const {
    return _current_pos->is_placeholder;
}

void keyframe_animation_component::goto_first(){
    _current_pos = _keyframes.begin();
    _current_delay = _current_pos->delay;
    notify_property_changed(u"current_index");
    notify_property_changed(u"current_delay");
}

void keyframe_animation_component::goto_last(){
    _current_pos = _keyframes.size() == 1 ? std::prev(_keyframes.end(), 1) : std::prev(_keyframes.end(), 2);
    _current_delay = _current_pos->delay;
    notify_property_changed(u"current_index");
    notify_property_changed(u"current_delay");
}

void keyframe_animation_component::goto_placeholder(){
    _current_pos = std::prev(_keyframes.end(), 1);
    _current_delay = _current_pos->delay;
    notify_property_changed(u"current_index");
    notify_property_changed(u"current_delay");
}

void keyframe_animation_component::goto_previous(){
    if(_current_pos != _keyframes.begin())
        _current_pos = std::prev(_current_pos);
    _current_delay = _current_pos->delay;
    notify_property_changed(u"current_index");
    notify_property_changed(u"current_delay");
}

void keyframe_animation_component::goto_next(){
    if(_current_pos != std::prev(_keyframes.end()))
        _current_pos = std::next(_current_pos);
    _current_delay = _current_pos->delay;
    notify_property_changed(u"current_index");
    notify_property_changed(u"current_delay");
}

void keyframe_animation_component::delete_current(){
    if(_current_pos->is_placeholder)
        return;
    
    _current_pos = _keyframes.erase(_current_pos);
    auto _index = 0;
    for (auto _k : _keyframes){
        _k.index = _index;
        _index++;
    }
    
    _current_delay = _current_pos->delay;
    notify_property_changed(u"current_index");
    notify_property_changed(u"current_delay");
    _dirty_anim = true;
}

void keyframe_animation_component::record_start(){
    if(_keyframes.size() > 1 && in_editor()){
        parent_scene()->confirm(u"Would you like to remove all the keyframes?", [this](bool result){
            continue_record_start();
        });
        return;
    }
    continue_record_start();
}

bool is_valid_id(const rb_string& str){
    if(str == u"")
        return false;
    for (size_t i = 0; i < str.size(); i++) {
        if(str[i] == u' ')
            return false;
    }
    return true;
}

void keyframe_animation_component::continue_record_start(){
    init();
    
    std::vector<node*> _sel;
    parent_scene()->current()->fill_with_selection(_sel, node_filter::renderable);
    //checkings...
    for (auto _n : _sel){
        if(!is_valid_id(_n->name()))
        {
            parent_scene()->alert(u"Invalid name: '" + _n->name() + u"'");
            return;
        }
    }
    //do it...
    for (auto _n : _sel){
        _anim_nodes_set.insert(_n);
        _anim_nodes.push_back(_n);
        _anim_nodes_saved_transforms.insert({_n, _n->transform()});
    }
    notify_property_changed(u"current_index");
    notify_property_changed(u"current_delay");
    _dirty_anim = true;
}

void keyframe_animation_component::reset_transforms(){
    for (auto _n : _anim_nodes){
        if(parent_scene()->current()->selection_count() != 0){
            if(_n->is_selected())
                _n->transform(_anim_nodes_saved_transforms[_n]);
        }
        else
            _n->transform(_anim_nodes_saved_transforms[_n]);
    }
}

polygon_path get_path(const polygon_component* pol){
    auto _pol = pol->to_smooth_polygon();
    pol->transform().from_space_to_base().transform_polygon(_pol);
    
    polygon_path _path(_pol);
    return _path;
}

float get_length(const node* n, const polygon_path& path){
    auto _pt = n->transform().origin();
    
    uint32_t _index;
    auto _e = path.polygon().closest_edge(_pt, _index);
    auto _d = _e.distance_vector(_pt);
    _pt = _pt - _d; //we're on surface...
    return path.length(_pt);
}

void keyframe_animation_component::setup_attachment(const rb_string &objects_class, const rb_string &polygon_id){
    auto _nodes = parent_scene()->node_with_one_class(objects_class);
    auto _p = dynamic_cast<polygon_component*>(parent_scene()->node_with_name(polygon_id));
    if(_nodes.size() == 0 || !_p){
        parent_scene()->alert(u"No nodes or polygons...");
        return;
    }
    if(!_anim_nodes_set.count(_p)){
        parent_scene()->alert(u"Polygon is not animatable!");
        return;
    }
    //checkings
    for (auto _n : _nodes){
        if (!_anim_nodes_set.count(_n)){
            parent_scene()->alert(u"One of the nodes wasn't registrated at the start...");
            return;
        }
    }
    
    auto _path = get_path(_p);
    
    for (auto _n : _nodes){
        _attachments[_n] = {_p, get_length(_n, _path)};
    }
    _dirty_anim = true;
}

void keyframe_animation_component::remove_attachment_for(const rb_string &polygon_id){
    auto _p = dynamic_cast<polygon_component*>(parent_scene()->node_with_name(polygon_id));
    if(!_p)
        return;
    
    auto _it = _attachments.begin();
    auto _end = _attachments.end();
    while (_it != _end) {
        if(_it->second.attached == _p)
            _it = _attachments.erase(_it);
        else
            _it++;
    }
    _dirty_anim = true;
}

void keyframe_animation_component::remove_attachment_for_all(){
    _attachments.clear();
    _dirty_anim = true;
}

bool keyframe_animation_component::is_playing_animation() const {
    return _playing_anim;
}

void keyframe_animation_component::play_animation() {
    _playing_anim = true;
    _playing_mirror = false;
    _current_frame_an = 0;
}

void keyframe_animation_component::resume_animation(){
    _playing_anim = true;
}

void keyframe_animation_component::pause_animation(){
    _playing_anim = false;
}

void keyframe_animation_component::editor_play_animation() {
    _ed_playing_anim = true;
    _ed_playing_mirror = false;
    _ed_current_frame_an = 0;
}

void keyframe_animation_component::editor_resume_animation(){
    _ed_playing_anim = true;
}

void keyframe_animation_component::editor_pause_animation(){
    _ed_playing_anim = false;
}

void align4(size_t* s){
    if((*s) == 0)
        return;
    auto _mod = (*s) % 4;
    if(_mod == 0)
        return;
    
    *s = ((*s) / 4) * 4 + 4;
}

template<class T>
void * tto_buffer(const std::vector<T>& v, uint32_t* size){
    auto _size = 8 + v.size() * sizeof(T);
    align4(&_size);
    auto _mem = (uint32_t*)malloc(_size);
    _mem[0] = (uint32_t)_size;
    _mem[1] = (uint32_t)v.size();
    auto _vals = (T*)(_mem + 2);
    for (size_t i = 0; i < v.size(); i++) {
        _vals[i] = v[i];
    }
    *size = (uint32_t)_size;
    return _mem;
}

template<class T>
void tfrom_buffer(void* buffer, std::vector<T>& v, void** next){
    uint32_t* _u32mem = (uint32_t*)buffer;
    uint32_t _sizeBytes = _u32mem[0];
    uint32_t _countItems = _u32mem[1];
    T* _tmem = (T*)(_u32mem + 2);
    
    for (uint32_t i = 0; i < _countItems; i++) {
        T item;
        memcpy(&item, _tmem, sizeof(T));
        v.push_back(item);
        _tmem += 1;
    }
    
    *next = (((char*)buffer) + _sizeBytes);
}

void* to_buffer(const rb_string& str, uint32_t* size){
    auto _size = 8 + str.size() * sizeof(rb_string::value_type);
    align4(&_size);
    auto _mem = (uint32_t*)malloc(_size);
    _mem[0] = (uint32_t)_size;
    _mem[1] = (uint32_t)str.size();
    auto _chars = (char16_t*)(_mem + 2);
    for (size_t i = 0; i < str.size(); i++) {
        _chars[i] = str[i];
    }
    *size = (uint32_t)_size;
    return _mem;
}

rb_string sfrom_buffer(void* buffer, void** next){
    uint32_t* _u32mem = (uint32_t*)buffer;
    uint32_t _sizeBytes = _u32mem[0];
    uint32_t _countItems = _u32mem[1];
    rb_string _res = u"";
    char16_t* _tmem = (char16_t*)(_u32mem + 2);
    
    for (uint32_t i = 0; i < _countItems; i++) {
        _res.push_back(*_tmem);
        _tmem += 1;
    }
    *next = (((char*)buffer) + _sizeBytes);
    return _res;
}

struct simple_keyframe {
    uint32_t index;
    float delay;
    uint32_t n_frames;
    uint32_t is_placeholder;
};

size_t get_size_keyframe(const keyframe& kf){
    auto _size_tts = 8 + kf.transforms.size() * sizeof(transform_space);
    align4(&_size_tts);
    return sizeof(simple_keyframe) + (kf.animated.size() * 4 + 8) + _size_tts + (kf.easings.size() * sizeof(keyframe::easing_info) + 8);
}

void write_to_buffer(void* buffer, const keyframe& k, const std::unordered_map<node*, uint32_t>& indexes, void** cont){
    char* _cBuffer = (char*)buffer;
    
    std::vector<uint32_t> _mIndexes;
    std::vector<transform_space> _transfs;
    std::vector<keyframe::easing_info> _easings;
    for (auto _n : k.animated){
        _mIndexes.push_back(indexes.at(_n));
        _transfs.push_back(k.transforms.at(_n));
        _easings.push_back(k.easings.at(_n));
    }
    simple_keyframe sk;
    sk.delay = k.delay;
    sk.index = k.index;
    sk.is_placeholder = k.is_placeholder;
    sk.n_frames = k.n_frames;
    
    memcpy(_cBuffer, &sk, sizeof(simple_keyframe));
    _cBuffer += sizeof(simple_keyframe);
    
    uint32_t _ts;
    auto _t = tto_buffer(_mIndexes, &_ts);
    memcpy(_cBuffer, _t, _ts);
    _cBuffer += _ts;
    free(_t);
    
    _t = tto_buffer(_transfs, &_ts);
    memcpy(_cBuffer, _t, _ts);
    _cBuffer += _ts;
    free(_t);
    
    _t = tto_buffer(_easings, &_ts);
    memcpy(_cBuffer, _t, _ts);
    _cBuffer += _ts;
    free(_t);
    
    *cont = _cBuffer;
}

void* to_buffer(const std::list<keyframe>& kv, const std::unordered_map<node*, uint32_t>& indexes, uint32_t* size){
    size_t _total_size = 4;
    for (auto _k : kv)
        _total_size += get_size_keyframe(_k);
    
    auto _mem = (uint32_t*)malloc(_total_size);
    _mem[0] = (uint32_t)kv.size();
    
    auto _start = (void*)(_mem + 1);
    for (auto _k : kv){
        void* _next;
        write_to_buffer(_start, _k, indexes, &_next);
        _start = _next;
    }
    *size = (uint32_t)_total_size;
    return _mem;
}

keyframe kread_one_keyframe(const std::vector<node*>& nodes, void* buffer, void** cont){
    char* _cBuffer = (char*)buffer;
    simple_keyframe sk;
    memcpy(&sk, _cBuffer, sizeof(simple_keyframe));
    _cBuffer += sizeof(simple_keyframe);
    
    std::vector<uint32_t> _mIndexes;
    std::vector<transform_space> _transfs;
    std::vector<keyframe::easing_info> _easings;
    
    void* _contBuffer = _cBuffer;
    tfrom_buffer(_contBuffer, _mIndexes, &_contBuffer);
    tfrom_buffer(_contBuffer, _transfs, &_contBuffer);
    tfrom_buffer(_contBuffer, _easings, &_contBuffer);
    
    keyframe _k;
    _k.index = sk.index;
    _k.delay = sk.delay;
    _k.n_frames = sk.n_frames;
    _k.is_placeholder = (bool)sk.is_placeholder;
    
    for (auto _i : _mIndexes)
        _k.animated.insert(nodes[_i]);
    
    for (uint32_t i = 0; i < _mIndexes.size(); i++) {
        _k.transforms[nodes[_mIndexes[i]]] = _transfs[i];
        _k.easings[nodes[_mIndexes[i]]] = _easings[i];
    }
    
    *cont = _contBuffer;
    return _k;
}

void kfrom_buffer(std::list<keyframe>& kv, const std::vector<node*>& nodes, void* buffer){
    uint32_t* _itemCountBuffer = (uint32_t*)buffer;
    void* _contBuffer = _itemCountBuffer + 1;
    uint32_t _itemCount = *_itemCountBuffer;
    
    for (uint32_t i = 0; i < _itemCount; i++) {
        kv.push_back(kread_one_keyframe(nodes, _contBuffer, &_contBuffer));
    }
}

struct simple_attach_info {
    uint32_t node_index;
    uint32_t pol_index;
    float at_length;
    
    simple_attach_info(uint32_t node_index,
                       uint32_t pol_index,
                       float at_length){
        this->node_index = node_index;
        this->pol_index = pol_index;
        this->at_length = at_length;
    }
    simple_attach_info(){
        
    }
};

buffer keyframe_animation_component::save_state() const {
    //first we save the name of the nodes and create a index mapping
    std::unordered_map<node*, uint32_t> _indexes;
    rb_string _names = u"";
    for (size_t i = 0; i < _anim_nodes.size(); i++) {
        _indexes[_anim_nodes[i]] = (uint32_t)i;
        if(i != 0)
            _names += u" ";
        _names += _anim_nodes[i]->name();
    }
    
    uint32_t _nameBufferSize = 0;
    auto _nameBuffer = to_buffer(_names, &_nameBufferSize);
    
    //serialize transform spaces
    std::vector<transform_space> _tss;
    for(auto _n : _anim_nodes){
        auto _t = _anim_nodes_saved_transforms.at(_n);
        _tss.push_back(_t);
    }
    uint32_t _transfBufferSize = 0;
    auto _transBuffer = tto_buffer(_tss, &_transfBufferSize);
    
    uint32_t _posBufferSize = 0;
    auto _posBuffer = tto_buffer(_anim_positions, &_posBufferSize);
    
    uint32_t _rotBufferSize = 0;
    auto _rotBuffer = tto_buffer(_anim_rotations, &_rotBufferSize);
    
    std::vector<simple_attach_info> _att_info_vector;
    for (auto _kvp : _attachments)
        _att_info_vector.push_back(simple_attach_info(_indexes[_kvp.first], _indexes[_kvp.second.attached], _kvp.second.at_length));
    
    uint32_t _attBufferSize = 0;
    auto _attBuffer = tto_buffer(_att_info_vector, &_attBufferSize);
    
    uint32_t _kfBufferSize = 0;
    auto _kfBuffer = to_buffer(_keyframes, _indexes, &_kfBufferSize);
    
    auto _totalSize = _nameBufferSize + _transfBufferSize + _posBufferSize + _rotBufferSize + _attBufferSize + _kfBufferSize;
    auto _mem = (char*)malloc(_totalSize);
    auto _saved_mem = _mem;
    assert(_mem);
    
    memcpy(_mem, _nameBuffer, _nameBufferSize);
    _mem += _nameBufferSize;
    memcpy(_mem, _transBuffer, _transfBufferSize);
    _mem += _transfBufferSize;
    memcpy(_mem, _posBuffer, _posBufferSize);
    _mem += _posBufferSize;
    memcpy(_mem, _rotBuffer, _rotBufferSize);
    _mem += _rotBufferSize;
    memcpy(_mem, _attBuffer, _attBufferSize);
    _mem += _attBufferSize;
    memcpy(_mem, _kfBuffer, _kfBufferSize);
    
    auto _buff = buffer(_saved_mem, _totalSize);
    free(_saved_mem);
    free(_nameBuffer);
    free(_transBuffer);
    free(_posBuffer);
    free(_rotBuffer);
    free(_attBuffer);
    free(_kfBuffer);
    return _buff;
}

void keyframe_animation_component::restore_state(rb::buffer buff){
    auto _buff = const_cast<void*>(buff.internal_buffer());
    void* _cont = nullptr;
    auto _names = sfrom_buffer(_buff, &_cont);
    _buff = _cont;
    
    auto _nv = rb::tokenize(_names);
    _anim_nodes.clear();
    _anim_nodes_set.clear();
    for (auto _name : _nv){
        auto _n = parent_scene()->node_with_name(_name);
        assert(_n);
        _anim_nodes.push_back(_n);
        _anim_nodes_set.insert(_n);
    }
    
    _anim_nodes_saved_transforms.clear();
    std::vector<transform_space> _tts;
    tfrom_buffer(_buff, _tts, &_cont);
    _buff = _cont;
    for (size_t i = 0; i < _tts.size(); i++) {
        _anim_nodes_saved_transforms.insert({_anim_nodes[i], _tts[i]});
    }
    
    _anim_positions.clear();
    tfrom_buffer(_buff, _anim_positions, &_cont);
    _buff = _cont;
    
    _anim_rotations.clear();
    tfrom_buffer(_buff, _anim_rotations, &_cont);
    _buff = _cont;
    
    std::vector<simple_attach_info> _att_info_vector;
    _attachments.clear();
    tfrom_buffer(_buff, _att_info_vector, &_cont);
    _buff = _cont;
    for (auto _att : _att_info_vector){
        _attachments.insert({_anim_nodes[_att.node_index], {dynamic_cast<polygon_component*>(_anim_nodes[_att.pol_index]), _att.at_length}});
    }
    
    _keyframes.clear();
    kfrom_buffer(_keyframes, _anim_nodes, _buff);
    _current_pos = _keyframes.end();
    _current_pos--;
}

void keyframe_animation_component::restore_pending_buffer(){
    if(!_pending_buffer.has_value())
        return;
    
    restore_state(_pending_buffer.value());
    _pending_buffer = nullptr;
}

void keyframe_animation_component::reset_component(){
    restore_pending_buffer();
    
    for (size_t i = 0; i < _anim_nodes.size(); i++) {
        _anim_nodes[i]->transform(_anim_nodes_saved_transforms[_anim_nodes[i]]);
        
    }
    _playing_anim = _saved_playing_anim;
    _current_frame_an = 0;
    _playing_mirror = false;
}

void keyframe_animation_component::playing(){
    if(!_initialized){
        _saved_playing_anim = _playing_anim;
        _current_frame_an = 0;
        restore_pending_buffer();
    }
}

void keyframe_animation_component::was_deserialized(){
    restore_pending_buffer();
}

void keyframe_animation_component::update(float dt){
    restore_pending_buffer();
    set_internal_animation_if_dirty();
    
    if(!_playing_anim)
        return;
    
    if(_current_frame_an >= _n_frames){
        if(_loop){
            if(!_mirror){
                _current_frame_an = 0;
                _playing_mirror = false;
            }
            else {
                _current_frame_an = _n_frames - 1;
                _playing_mirror = true;
            }
        }
        else
            return;
    }
    else if(_current_frame_an < 0){
        _current_frame_an = 0;
        _playing_mirror = false;
    }
    
    for (size_t i = 0; i < _anim_nodes.size(); i++) {
        auto _pos = _anim_positions[_n_frames * i + _current_frame_an];
        auto _rot = _anim_rotations[_n_frames * i + _current_frame_an];
        _anim_nodes[i]->transform(_anim_nodes[i]->transform().moved(_pos).rotated(_rot, _rot + (float)M_PI_2));
    }
    
    if(!_playing_mirror)
        _current_frame_an++;
    else
        _current_frame_an--;
}

void keyframe_animation_component::in_editor_update(float dt){
    if(is_playing())
        return;
    
    restore_pending_buffer();
    set_internal_animation_if_dirty();
    
    if(!_ed_playing_anim)
        return;
    
    if(_ed_current_frame_an >= _n_frames){
        if(_loop){
            if(!_mirror){
                _ed_current_frame_an = 0;
                _ed_playing_mirror = false;
            }
            else {
                _ed_current_frame_an = _n_frames - 1;
                _ed_playing_mirror = true;
            }
        }
        else
            return;
    }
    else if(_ed_current_frame_an < 0){
        _ed_current_frame_an = 0;
        _ed_playing_mirror = false;
    }
    
    for (size_t i = 0; i < _anim_nodes.size(); i++) {
        auto _pos = _anim_positions[_n_frames * i + _ed_current_frame_an];
        auto _rot = _anim_rotations[_n_frames * i + _ed_current_frame_an];
        _anim_nodes[i]->transform(_anim_nodes[i]->transform().moved(_pos).rotated(_rot, _rot + (float)M_PI_2));
    }
    
    if(!_ed_playing_mirror)
        _ed_current_frame_an++;
    else
        _ed_current_frame_an--;
}

void keyframe_animation_component::after_becoming_active(bool node_was_moved){
    register_for(registrable_event::update, ANIMATOR_UPDATE_PRIORITY);
    register_for(registrable_event::in_editor_update, 0);
}

rb_string keyframe_animation_component::type_name() const {
    return u"rb::keyframe_animation_component";
}

rb_string keyframe_animation_component::displayable_type_name() const {
    return u"Keyframe Animator";
}







































