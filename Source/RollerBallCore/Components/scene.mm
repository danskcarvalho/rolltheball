//
//  scene.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 07/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "scene.h"
#include "layer.h"
#include "director.h"
#include "node.h"
#include "texture_atlas.h"
#include "program_manager.h"
#include "apple_program_manager.h"
#include "state_manager.h"
#include "extended_dynamic_mesh_batch.h"
#include "null_texture_map.h"
#include "polygon.h"
#include "editor_delegate.h"
#include "misc_classes_3.h"
#include "group_component.h"
#include "texture_atlas_loader.h"
#include "scene_loader.h"
#include "particle_layer.h"
#include "dynamic_mesh_batch.h"
#include "basic_process.h"

using namespace rb;

inline NSString* to_platform_string(const rb::rb_string& str){
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> _str_converter;
    auto _u8Str = _str_converter.to_bytes(str);
    NSString* _platform_str = [NSString stringWithUTF8String:_u8Str.c_str()];
    return _platform_str;
}

inline rb::rb_string from_platform_string(NSString* str){
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> _str_converter;
    return _str_converter.from_bytes([str UTF8String]);
}

scene::scene(){
    _n_seed = 0;
    bool in_editor = director::in_editor();
    _active = false;
    _playing = false;
    _in_editor = in_editor;
    _navigation_mode = navigation_mode::scroll;
    _background_color = color::from_rgba(0, 0, 0, 1); //opaque black
    _gizmo_color = color::from_rgba(0, 0.7, 0, 1); //opaque green
    _alternate_gizmo_color = color::from_rgba(1, 0.64, 0, 1); //opaque orange
    _selection_color = color::from_rgba(1, 1, 1, 1); //opaque white
    for (int i = 0; i < (MAX_LAYERS + 1); i++) {
        _layers[i] = new class layer(this);
    }
    _program_manager = nullptr;
    _texture_atlas = nullptr;
    _viewport_size = state_manager::viewport_size();
    _dirty_transform = true;
    _current_node = nullptr;
    _gizmo_layer = nullptr;
    if(in_editor)
        _gizmo_layer = new extended_dynamic_mesh_batch();
    _selection_rectangle = nullptr;
    _selection_marquee = nullptr;
    _in_transformation = false;
    _selection_indicator = nullptr;
    for (int i = 0; i < HANDLE_COUNT; i++) {
        _handles[i].display_mesh = nullptr;
        _handles[i].display_mesh_copy = nullptr;
    }
    _m_x_axis = nullptr;
    _m_y_axis = nullptr;
    _enabled_aspect_correction = true;
    _new_template = nullptr;
    _current_new = nullptr;
    _delta_transform = transform_space(vec2::zero, vec2::zero, vec2::zero);
    _fade_mesh = nullptr;
    _fade_batch = nullptr;
    _textureless_process = nullptr;
    _fade_color = color::from_rgba(0, 0, 0, 0);
    _locked_selection = nullptr;
    _move10x = false;
    _alignToGrid = false;
    //goto next level
    _unfaded_at_start = false;
    _faded_time_passed = 0;
    _faded_time_total = 0;
}

node* scene::locked_selection() const {
    return _locked_selection;
}

void scene::lock_selection() {
    std::vector<node*> _selection;
    if(!current()){
        alert(u"No current node.");
        return;
    }
    
    current()->fill_with_selection(_selection);
    
    if(_selection.size() != 1){
        alert(u"Need to select just one node.");
        return;
    }
    
    _locked_selection = _selection[0];
    current()->clear_selection();
}

void scene::unlock_selection(){
    _locked_selection = nullptr;
}

void scene::create_fading_machinery(){
    if(!_fade_mesh){
        _fade_mesh = new mesh();
        rectangle _rc(0, 0, 2, 2);
        _rc.to_mesh(*_fade_mesh, rectangle(0.5, 0.5, 1, 1));
    }
    
    if(!_textureless_process){
        _textureless_process = new basic_process(_program_manager);
        _textureless_process->texture_sampler(nullptr);
        _textureless_process->ambient_color(color::from_rgba(1, 1, 1, 1));
        _textureless_process->position_transform(transform_space());
        _textureless_process->texture_sampler(nullptr);
    }
    
    if(!_fade_batch){
        _fade_batch = new dynamic_mesh_batch();
        _fade_batch->add_mesh(_fade_mesh);
        _fade_batch->process(_textureless_process);
    }
}

void scene::enter_new_mode(const class type_descriptor *td){
    assert(active());
    assert(current());
    assert(!_new_template);
    assert(td);
    auto _nt = dynamic_cast<node*>(td->new_object());
    
    if(!_nt->renderable()){
        _nt->_new_template = false;
        if(!current()->add_node(_nt)){
            delete _nt;
            alert(u"Can't add " + _nt->displayable_type_name() + u" to current node.");
        }
        _new_template = nullptr;
    }
    else {
        _new_template = new node_without_transform(_nt);
        _nt->_new_template = true;
        _current_new = nullptr;
    }
}

void scene::exit_new_mode(){
    if(!_new_template)
        return;
    
    delete _new_template;
    _new_template = nullptr;
}

typed_object* scene::template_for_new() const{
    return _new_template;
}

class layer* scene::layer(uint32_t index){
    assert(index >= 0);
    assert(index < (MAX_LAYERS + 1));
    return _layers[index];
}
const class layer* scene::layer(uint32_t index) const{
    assert(index >= 0);
    assert(index < (MAX_LAYERS + 1));
    return _layers[index];
}

class layer* scene::hidden_layer() {
    return _layers[HIDDEN_LAYER];
}

const class layer* scene::hidden_layer() const {
    return _layers[HIDDEN_LAYER];
}

scene::~scene(){
    for (int i = 0; i < (MAX_LAYERS + 1); i++) {
        delete _layers[i];
    }
    if(_program_manager)
        delete _program_manager;
    if(_gizmo_layer)
        delete _gizmo_layer;
    if(_selection_marquee)
        delete _selection_marquee;
    for (int i = 0; i < HANDLE_COUNT; i++) {
        if(_handles[i].display_mesh)
            delete _handles[i].display_mesh;
        if(_handles[i].display_mesh_copy)
            delete _handles[i].display_mesh_copy;
    }
    if(_selection_indicator)
        delete _selection_indicator;
    if(_m_x_axis)
        delete _m_x_axis;
    if(_m_y_axis)
        delete _m_y_axis;
    if(_texture_atlas)
        texture_atlas_loader::release_atlas(&_texture_atlas);
    if(_fade_batch)
        delete _fade_batch;
    if(_textureless_process)
        delete _textureless_process;
    if(_fade_mesh)
        delete _fade_mesh;
}

bool scene::in_editor() const {
    return _in_editor;
}

//activation methods
void scene::before_becoming_active(){
    for (int i = 0; i < (MAX_LAYERS + 1); i++) {
        _layers[i]->before_becoming_active();
    }
}

void scene::after_becoming_active(){
    _active = true;
    _program_manager = new apple_program_manager();
    for (int i = 0; i < (MAX_LAYERS + 1); i++) {
        _layers[i]->after_becoming_active();
    }
}

void scene::before_becoming_inactive(){
    for (int i = 0; i < (MAX_LAYERS + 1); i++) {
        _layers[i]->before_becoming_inactive();
    }
}

void scene::after_becoming_inactive(){
    _active = false;
    if(_program_manager){
        delete _program_manager;
        _program_manager = nullptr;
    }
    //the texture atlas is only destroyed at scene destructor...
    for (int i = 0; i < (MAX_LAYERS + 1); i++) {
        _layers[i]->after_becoming_inactive();
    }
}

bool scene::active() const{
    return _active;
}

bool scene::playing() const {
    return _playing;
}

void scene::playing(const bool value){
    if(value == _playing)
        return;
    _playing = value;
    cancel_user_selection();
    if(_playing){
        if(in_editor()){
            bool _was_in_transformation = _in_transformation;
            start_transformation();
            _handler_transformation = nullptr;
            _in_transformation = false;
            _dragged_mouse = false;
            
            if(_was_in_transformation){
                for (auto& _kvp : registered_nodes(registrable_event::live_edit)){
                    if(!unregistered(registrable_event::live_edit, _kvp.second) && _kvp.second->is_currently_selected())
                        _kvp.second->end_live_edit();
                }
            }
        }
    }
    if(!_playing){ //we reset if the scene is paused...
        if(in_editor()){
            start_transformation();
            _handler_transformation = nullptr;
            _in_transformation = false;
            _dragged_mouse = false;
        }
    }
    
    if(_playing && in_editor()){
        _saved_scene = scene_loader::serialize_to_string(this);
    }
    
    for (int i = 0; i < (MAX_LAYERS + 1); i++) {
        _layers[i]->invalidate_buffers(); //only when the full scene pauses...
        //this invalidation is useful inside the editor...
        if(_playing)
            _layers[i]->scene_playing();
        else
            _layers[i]->scene_paused();
    }
    
    if(!_playing && in_editor() && _saved_scene != u""){
        //HACK: Manage ghost data
        auto _ghosts = this->node_with_one_class(u"ghostData");
        struct transform_layer {
            node* ghost;
            int32_t layer;
        };
        std::vector<transform_layer> _ghost_layers;
        for (auto _g : _ghosts){
            if(!_g->parent_layer())
                continue;
            transform_layer tl;
            tl.ghost = _g;
            tl.layer = -1;
            for (uint32_t i = 0; i < MAX_LAYERS; i++){
                if(this->layer(i) == _g->parent_layer()){
                    tl.layer = i;
                    break;
                }
            }
            if(tl.layer != -1){
                _g->parent_layer()->remove_node(_g, false);
                _ghost_layers.push_back(tl);
            }
        }
        
        rb_string _ss_scene = _saved_scene;
        director::active_scene(nullptr, true);
        auto _s = dynamic_cast<scene*>(scene_loader::deserialize_from_string(_ss_scene));
        director::active_scene(_s, true);
        
        if(director::editor_delegate()){
            director::editor_delegate()->current_changed();
            director::editor_delegate()->hierarchy_changed(nullptr);
        }
        
        //HACK: Manage ghost data
        for (auto _g : _ghost_layers){
            _s->layer(_g.layer)->add_node_at(_g.ghost, _s->layer(_g.layer)->node_count());
            _g.ghost->blocked(true);
        }
    }
}

void scene::update() {
    unregister_nodes();
    if(_in_editor && !_playing){ //only if paused
        in_editor_update(DESIRED_FPS);
    }
    
    if(!_playing)
        return;
    
    fixed_update(DESIRED_FPS);
}

void scene::unregister_nodes(){
    if(_to_be_unregistrated.size() != 0){
        for(auto& _p : _to_be_unregistrated)
            registered_nodes(_p.first).erase(_p.second); //we unregister the nodes scheduled to unregister...
        _to_be_unregistrated.clear();
        _to_be_unregistrated2.clear();
    }
}

bool scene::unregistered(const rb::registrable_event event, rb::node *n){
    return _to_be_unregistrated2.count({event, n});
}

scene::ordered_nodes& scene::registered_nodes(const rb::registrable_event event){
    return _registered_nodes[(size_t)event];
}

void scene::cancel_user_selection(){
    _selection_rectangle = nullptr;
    _saved_selection.clear();
    _dragged_mouse = false;
    _in_transformation = false;
    _handler_transformation = nullptr;
    _original_transforms.clear();
}

void scene::fixed_update(float dt){
    for (int i = 0; i < (MAX_LAYERS + 1); i++) {
        if(_layers[i]->_particle_layer)
            _layers[i]->_particle_layer->update(dt);
    }
    for (auto& _kvp : registered_nodes(registrable_event::update)){
        if(unregistered(registrable_event::update, _kvp.second))
            continue;
        if(!_kvp.second->parent_layer()->playing() || _kvp.second->parent_layer()->_static_batch)
            continue;
        _kvp.second->update(dt);
    }
    
    for (auto& _kvp : registered_nodes(registrable_event::space_changed)){
        if(unregistered(registrable_event::space_changed, _kvp.second))
            continue;
        if(!_kvp.second->parent_layer()->playing() || _kvp.second->parent_layer()->_static_batch)
            continue;
        if(!_kvp.second->has_space_changed())
            continue;
        _kvp.second->space_changed();
    }
}

void scene::in_editor_update(float dt){
    for (int i = 0; i < (MAX_LAYERS + 1); i++) {
        if(_layers[i]->_particle_layer)
            _layers[i]->_particle_layer->update(dt);
    }
    for (auto& _kvp : registered_nodes(registrable_event::in_editor_update)){
        if(unregistered(registrable_event::in_editor_update, _kvp.second))
            continue;
        _kvp.second->in_editor_update(dt);
    }
    
    for (auto& _kvp : registered_nodes(registrable_event::space_changed)){
        if(unregistered(registrable_event::space_changed, _kvp.second))
            continue;
        if(!_kvp.second->has_space_changed())
            continue;
        _kvp.second->space_changed();
    }
}

scene::ordered_nodes::iterator scene::register_for(const registrable_event event, int32_t priority, rb::node *n){
    return registered_nodes(event).insert({priority, n});
}

void scene::unregister_for(const registrable_event event, ordered_nodes::iterator it){
    _to_be_unregistrated.push_back({event, it});
    _to_be_unregistrated2.insert({event, it->second});
    //registered_nodes(event).erase(it);
}

//rendering
const texture_atlas* scene::atlas() const {
    return _texture_atlas;
}

const texture_atlas* scene::atlas(const rb::texture_atlas *value){
    if(_texture_atlas == value)
        return _texture_atlas;
    if(value)
        assert(!value->dirty());
    _texture_atlas = const_cast<rb::texture_atlas*>(value);
    if(_active){
        for (int i = 0; i < (MAX_LAYERS + 1); i++)
        {
            _layers[i]->texture_atlas_changed();
        }
    }
    return _texture_atlas;
}

void scene::load_atlas(const rb_string& url){
    if(_texture_atlas){
        texture_atlas_loader::release_atlas(&_texture_atlas);
    }
    auto _a = (texture_atlas*)texture_atlas_loader::load_atlas(url);
    atlas(_a);
    
    NSString* urlStr = to_platform_string(url);
    NSURL* originalUrl = [NSURL fileURLWithPath: [urlStr stringByExpandingTildeInPath]];
    urlStr = [originalUrl path];
    originalUrl = [NSURL fileURLWithPath:urlStr isDirectory:YES];
    NSURL* urlAtlas = [NSURL fileURLWithPath: urlStr];
    if([[[urlAtlas pathComponents] objectAtIndex:1] isEqualToString:[originalUrl host]])
    {
        NSMutableArray* array = [NSMutableArray arrayWithArray:[urlAtlas pathComponents]];
        [array removeObjectAtIndex:1];
        urlStr = [NSString pathWithComponents:array];
        urlAtlas = [NSURL fileURLWithPath: urlStr];
    }
    
    _atlas_path = from_platform_string([urlStr stringByAbbreviatingWithTildeInPath]);
    
    
    force_notify_property_changed(u"atlas_path");
    force_notify_property_changed(u"atlas_name");
}

typed_object* scene::clone() const {
    scene* s = dynamic_cast<scene*>(typed_object::clone());
    return clone_children(s);
}

typed_object* scene::clone_children(scene* cloned) const {
    for (size_t i = 0; i < MAX_LAYERS; i++){
        if(cloned->_layers[i]){
            delete cloned->_layers[i];
            cloned->_layers[i] = nullptr;
        }
        cloned->_layers[i] = dynamic_cast<class layer*>(this->_layers[i]->clone());
    }
    cloned->was_deserialized();
    return cloned;
}

void scene::was_deserialized(){
    if(_atlas_path != u"")
        load_atlas(_atlas_path);
    
    if(director::in_editor())
        set_current_from_string(_saved_current);
    
    _by_name.clear();
    _by_class.clear();
}

const rb_string& scene::atlas_path() const {
    return _atlas_path;
}

const transform_space& scene::camera() const {
    return _camera;
}

const transform_space& scene::camera(const rb::transform_space &value){
    if(_camera == value)
        return _camera;
    _dirty_transform = true;
    _camera = value;
    return _camera;
}

bool scene::camera_aspect_correction() const {
    return _enabled_aspect_correction;
}

bool scene::camera_aspect_correction(bool enabled){
    if(enabled == _enabled_aspect_correction)
        return enabled;
    _enabled_aspect_correction = enabled;
    _dirty_transform = true;
    return _enabled_aspect_correction;
}

enum navigation_mode scene::navigation_mode() const {
    return _navigation_mode;
}

enum navigation_mode scene::navigation_mode(enum navigation_mode value) {
    _navigation_mode = value;
    return _navigation_mode;
}

//background color
const color& scene::background_color() const {
    return _background_color;
}

const color& scene::background_color(const rb::color &value) {
    _background_color = value;
    return _background_color;
}

//gizmo color
const color& scene::gizmo_color() const {
    return _gizmo_color;
}

const color& scene::gizmo_color(const rb::color &value) {
    _gizmo_color = value;
    return _gizmo_color;
}

//alternate gizmo color
const color& scene::alternate_gizmo_color() const {
    return _alternate_gizmo_color;
}

const color& scene::alternate_gizmo_color(const rb::color &value) {
    _alternate_gizmo_color = value;
    return _alternate_gizmo_color;
}

//selection color
const color& scene::selection_color() const {
    return _selection_color;
}

const color& scene::selection_color(const rb::color &value) {
    _selection_color = value;
    return _selection_color;
}

void scene::cancel_auto_fading(){
    _unfaded_at_start = true;
    _fade_color.a(0);
}

void scene::render() {
    if(!_unfaded_at_start && _fade_color.a() >= 0){
        _fade_color.a(_fade_color.a() - 0.12f);
        if(_fade_color.a() <= 0){
            _unfaded_at_start = true;
            _fade_color.a(0);
        }
    }
    if(_unfaded_at_start && _faded_time_total > 0){
        _faded_time_passed += (1.0f / 30.0f);
        if(_faded_time_passed >= _faded_time_total)
            _faded_time_passed = _faded_time_total;
        
        auto _t = _faded_time_passed / _faded_time_total;
        color _res = color::lerp(_t, _source_fd_color, _target_fd_color);
        _fade_color = _res;
        if(_faded_time_passed == _faded_time_total){
            _faded_time_total = 0;
            _faded_time_passed = 0;
            if(_completion_fn)
                _completion_fn();
        }
    }
    //we clear the gizmo layer
    if(_gizmo_layer)
        _gizmo_layer->clear_meshes();
    //we clear with the background color
    state_manager::clear_framebuffer(_background_color.pre_multiplied());
    auto _gl_error = glGetError();
    assert(_gl_error == GL_NO_ERROR);
    _dirty_transform = false;
    for (int i = 0; i < (MAX_LAYERS + 1); i++) {
        _layers[i]->render();
    }
    if(_gizmo_layer && !playing()){
        if(current() && current()->selection_count(node_filter::renderable) != 0){
            std::vector<node*> _selected;
            current()->fill_with_selection(_selected);
            if(std::any_of(_selected.begin(), _selected.end(), [](node* nn){ return nn->enabled(node_capability::selection_rectangle); }))
                render_selection_indicator();
            render_handles();
        }
        if(_selection_rectangle.has_value()){
            render_selection_rectangle();
        }
        
        _gizmo_layer->blend_mode(blend_mode::normal);
        _gizmo_layer->geometry_type(geometry_type::triangle);
        _gizmo_layer->draw();
    }
    
    if(_fade_color.a() > 0){
        create_fading_machinery();
        _fade_mesh->set_color(_fade_color);
        _fade_mesh->set_blend(0);
        _fade_batch->draw();
    }
}

void scene::render_selection_indicator(){
    std::vector<node*> _selection;
    current()->fill_with_selection(_selection, node_filter::renderable);
    rectangle _bounds;
    if(_selection.size() != 1)
        _bounds = compute_selection_bounds(_selection, true);
    else
        _bounds = _selection[0]->bounds();
    polygon _p;
    _bounds.to_polygon(_p);
    if(_selection.size() != 1)
        current()->from_space_to(space::screen).transform_polygon(_p);
    else {
        _selection[0]->from_space_to(space::screen).transform_polygon(_p);
    }
    if(!_selection_indicator)
        _selection_indicator = new mesh();
    null_texture_map _n_map;
    _p.optimize();
    if(_p.point_count() < 2 || (_p.area().has_value() && almost_equal(_p.area().value(), 0)))
        return;
    if(_p.point_count() == 2)
        _p.open_polygon();
    _p.to_outline_mesh(*_selection_indicator, _n_map, SCENE_SELECTION_SIZE, corner_type::miter, true);
    _selection_indicator->set_color(selection_color());
    _selection_indicator->set_blend(0);
    //any layer will do...
    _layers[0]->add_gizmo(_selection_indicator, node::no_texture, false);
}

void scene::center_camera_on_selection(){
    if(!current() || current()->selection_count() == 0)
        return;
    auto _current_transform = get_current_transform(true);
    if(current()->selection_count() == 1)
        _current_transform = (*current()->_selection.begin())->old_transform();
    
    auto _center = _current_transform.origin();
    current()->from_space_to(space::scene).transform_point(_center);
    camera(camera().moved(_center));
}

void scene::render_handles(){
    if(_in_transformation)
        return;
    for (int i = 0; i < HANDLE_COUNT; i++) {
        if(!_handles[i].display_mesh){
            _handles[i].display_mesh = new mesh();
            rectangle _rc(0, 0, 1, 1);
            _rc.to_mesh(*_handles[i].display_mesh, rectangle(0.5, 0.5, 1, 1));
            matrix3x3::build_scale(HANDLE_SIZE, HANDLE_SIZE).transform_mesh(*_handles[i].display_mesh);
            _handles[i].display_mesh->set_color(gizmo_color());
            _handles[i].display_mesh->set_blend(0);
            _handles[i].display_mesh_copy = new mesh();
            *_handles[i].display_mesh_copy = *_handles[i].display_mesh;
        }
    }
    
    if(!_m_x_axis){
        _m_x_axis = new mesh();
    }
    if(!_m_y_axis){
        _m_y_axis = new mesh();
    }
    
    vec2 _center, _x, _y;
    auto _current_transform = get_current_transform(true);
    if(current() && current()->selection_count() == 1)
        _current_transform = (*current()->_selection.begin())->old_transform();
    
    _center = _current_transform.origin();
    _x = _current_transform.origin() + _current_transform.from_space_to_base().x_vector();
    _y = _current_transform.origin() + _current_transform.from_space_to_base().y_vector();
    
    current()->from_space_to(space::screen).transform_point(_center);
    current()->from_space_to(space::screen).transform_point(_x);
    current()->from_space_to(space::screen).transform_point(_y);
    
    _x = _center + (_x - _center).normalized() * HANDLE_STEM_SIZE;
    _y = _center + (_y - _center).normalized() * HANDLE_STEM_SIZE;
    
    //x axis
    polygon _x_axis;
    polygon::build_open_polygon({_center, _x}, _x_axis);
    polygon _y_axis;
    polygon::build_open_polygon({_center, _y}, _y_axis);
    null_texture_map _null_map;
    if(_x_axis.perimeter().has_value() && !almost_equal(_x_axis.perimeter().value(), 0)){
        _x_axis.to_outline_mesh(*_m_x_axis, _null_map, SCENE_SELECTION_SIZE, corner_type::miter, true);
        _m_x_axis->set_blend(0);
        _m_x_axis->set_color(gizmo_color());
        
         _layers[0]->add_gizmo(_m_x_axis, node::no_texture, false);
    }
    if(_y_axis.perimeter().has_value() && !almost_equal(_y_axis.perimeter().value(), 0)){
        _y_axis.to_outline_mesh(*_m_y_axis, _null_map, SCENE_SELECTION_SIZE, corner_type::miter, true);
        _m_y_axis->set_blend(0);
        _m_y_axis->set_color(gizmo_color());
        
        _layers[0]->add_gizmo(_m_y_axis, node::no_texture, false);
    }
    
    *_handles[0].display_mesh = *_handles[0].display_mesh_copy;
    matrix3x3::build_translation(_x).transform_mesh(*_handles[0].display_mesh);
    
    *_handles[1].display_mesh = *_handles[1].display_mesh_copy;
    matrix3x3::build_translation(_y).transform_mesh(*_handles[1].display_mesh);
    
    *_handles[2].display_mesh = *_handles[2].display_mesh_copy;
    matrix3x3::build_translation(_center).transform_mesh(*_handles[2].display_mesh);
    
    for (int i = 0; i < HANDLE_COUNT; i++) {
        _layers[0]->add_gizmo(_handles[i].display_mesh, node::no_texture, false);
    }
}

nullable<vec2> scene::hit_test_handler(const vec2& normalized_position){
    if(!current() || current()->selection_count() == 0)
        return nullptr;
    rectangle _x_handle(0, 0, 1, 1);
    rectangle _y_handle(0, 0, 1, 1);
    rectangle _center_handle(0, 0, 1, 1);
    matrix3x3::build_scale(HANDLE_SIZE, HANDLE_SIZE).transform_rectangle(_x_handle);
    matrix3x3::build_scale(HANDLE_SIZE, HANDLE_SIZE).transform_rectangle(_y_handle);
    matrix3x3::build_scale(HANDLE_SIZE, HANDLE_SIZE).transform_rectangle(_center_handle);
    
    auto _current_transform = get_current_transform();
    vec2 _center, _x, _y;
    _center = _current_transform.origin();
    _x = _current_transform.origin() + _current_transform.from_space_to_base().x_vector();
    _y = _current_transform.origin() + _current_transform.from_space_to_base().y_vector();
    
    current()->from_space_to(space::screen).transform_point(_center);
    current()->from_space_to(space::screen).transform_point(_x);
    current()->from_space_to(space::screen).transform_point(_y);
    
    _x = _center + (_x - _center).normalized() * HANDLE_STEM_SIZE;
    _y = _center + (_y - _center).normalized() * HANDLE_STEM_SIZE;
    
    matrix3x3::build_translation(_x).transform_rectangle(_x_handle);
    matrix3x3::build_translation(_y).transform_rectangle(_y_handle);
    matrix3x3::build_translation(_center).transform_rectangle(_center_handle);
    
    vec2 _pt_screen = from_space_to_another(space::normalized_screen, space::screen).from_space_to_base().transformed_point(normalized_position);
    if(_x_handle.intersects(_pt_screen))
        return vec2(1, 0);
    else if(_y_handle.intersects(_pt_screen))
        return vec2(0, 1);
    else if(_center_handle.intersects(_pt_screen))
        return vec2(0, 0);
    else
        return nullptr;
}

void scene::render_selection_rectangle(){
    auto _screen_rc = _selection_rectangle.value();
    if(!_selection_marquee)
        _selection_marquee = new mesh();
    polygon _p;
    _screen_rc.to_polygon(_p);
    null_texture_map _n_map;
    _p.optimize();
    if(_p.point_count() < 2)
        return;
    if(_p.point_count() == 2)
        _p.open_polygon();
    _p.to_outline_mesh(*_selection_marquee, _n_map, 2, corner_type::miter, true);
    _selection_marquee->set_color(selection_color());
    _selection_marquee->set_blend(0);
    //any layer will do...
    _layers[0]->add_gizmo(_selection_marquee, node::no_texture, false);
}

const vec2& scene::viewport_size() const {
    return _viewport_size;
}

void scene::viewport_resized() {
    _viewport_size = state_manager::viewport_size();
    _dirty_transform = true;
}

transform_space to_screen(const space from, const transform_space& camera, const bool aspect_correction, const vec2& viewport_size){
    assert(from != space::layer);
    if(from == space::camera){
        float _aspect = viewport_size.y() / viewport_size.x();
        if(!aspect_correction)
            _aspect = 1;
        transform_space _correction = transform_space(vec2::zero, vec2(_aspect, 1), 0);
        transform_space _translation = transform_space(vec2(1, 1));
        transform_space _scale = transform_space(vec2::zero, vec2(viewport_size.x() / 2.0, viewport_size.y() / 2.0));
        return _scale * _translation *  _correction;
    }
    else if(from == space::normalized_screen){
        transform_space _translation = transform_space(vec2(1, 1));
        transform_space _scale = transform_space(vec2::zero, vec2(viewport_size.x() / 2.0, viewport_size.y() / 2.0));
        return _scale * _translation;
    }
    else if(from == space::scene){
        transform_space _to_camera = camera.inverse();
        float _aspect = viewport_size.y() / viewport_size.x();
        if(!aspect_correction)
            _aspect = 1;
        transform_space _correction = transform_space(vec2::zero, vec2(_aspect, 1), 0);
        transform_space _translation = transform_space(vec2(1, 1));
        transform_space _scale = transform_space(vec2::zero, vec2(viewport_size.x() / 2.0, viewport_size.y() / 2.0));
        return _scale * _translation *  _correction * _to_camera;
    }
    else { //screen
        return transform_space();
    }
}

bool scene::in_live_selection() const {
    return _selection_rectangle.has_value();
}

transform_space scene::from_space_to_another(const space from, const space to){
    assert(active());
    assert(from != space::layer && to != space::layer);
    transform_space _to_screen1 = to_screen(from, camera(), camera_aspect_correction(), viewport_size());
    transform_space _to_screen2 = to_screen(to, camera(), camera_aspect_correction(), viewport_size());
    return _to_screen2.inverse() * _to_screen1;
}

node_container* scene::current() const {
    return _current_node;
}

node_container* scene::current(rb::node_container *value) {
    if(value){
        assert(value->renderable());
        if(dynamic_cast<node*>(value))
            assert(dynamic_cast<node*>(value)->enabled(node_capability::can_become_current));
        if(in_editor()){
            assert(!value->in_editor_hidden());
            if(dynamic_cast<node*>(value))
                assert(!dynamic_cast<node*>(value)->blocked());
        }
    }
    if (_current_node == value)
        return _current_node;
    cancel_user_selection();
    auto _s_current_node = _current_node;
    _current_node = value;
    if(_s_current_node)
        _s_current_node->internal_resign_current();
    if(_current_node && _current_node == value)
        _current_node->internal_became_current();
    if(director::editor_delegate())
        director::editor_delegate()->current_changed();
    exit_new_mode();
    return _current_node;
}

//Type Descriptor
rb_string scene::type_name() const {
    return u"rb::scene";
}

rb_string scene::displayable_type_name() const {
    return u"Scene";
}

float scene::align_to_grid(float value){
    bool _lessThan0 = value < 0;
    if(_lessThan0)
        value = -value;
    auto _ceill = ceilf(value);
    auto _floor = floorf(value);
    auto _mid = (_ceill + _floor) / 2.0f;
    auto _dc = fabsf(value - _ceill);
    auto _df = fabsf(value - _floor);
    auto _dm = fabsf(value - _mid);
    auto _min = std::min(_dc, std::min(_df, _dm));
    if(_min == _dc)
        return _lessThan0 ? -_ceill : _ceill;
    else if(_min == _df)
        return _lessThan0 ? -_floor : _floor;
    else
        return _lessThan0 ? -_mid : _mid;
}

vec2 scene::align_to_grid(rb::vec2 value){
    if(!_alignToGrid)
        return value;
    else
        return vec2(align_to_grid(value.x()), align_to_grid(value.y()));
}

void scene::describe_type() {
    start_type<scene>([]() { return new scene(); });
    string_property<scene>(u"atlas_path", u"Atlas Path", false, false, {
        [](const scene* site){
            return site->_atlas_path;
        },
        [](scene* site, const rb_string& value){
            site->_atlas_path = value;
        }
    });
    string_property<scene>(u"atlas_name", u"Atlas Name", false, false, {
        [](const scene* site){
            NSString* path = to_platform_string(site->_atlas_path);
            return from_platform_string(path.lastPathComponent);
        },
        [](scene* site, const rb_string& value){
        }
    });
    boolean_property<scene>(u"aspect_correction", u"Aspect", true, {
        [](const scene* site){
            return site->camera_aspect_correction();
        },
        [](scene* site, bool value){
            site->camera_aspect_correction(value);
        }
    });
    boolean_property<scene>(u"move_10x", u"Move 10x", true, {
        [](const scene* site){
            return site->_move10x;
        },
        [](scene* site, bool value){
            site->_move10x = value;
        }
    });
    boolean_property<scene>(u"align_grid", u"Align To Grid", true, {
        [](const scene* site){
            return site->_alignToGrid;
        },
        [](scene* site, bool value){
            site->_alignToGrid = value;
        }
    });
    color_property<scene>(u"background_color", u"Background", true, true, {
        [](const scene* site){
            return site->background_color();
        },
        [](scene* site, const color& value){
            site->background_color(value);
        }
    });
    color_property<scene>(u"gizmo_color", u"Gizmo", true, true, {
        [](const scene* site){
            return site->gizmo_color();
        },
        [](scene* site, const color& value){
            site->gizmo_color(value);
        }
    });
    color_property<scene>(u"alternate_gizmo", u"Alt Gizmo", true, true, {
        [](const scene* site){
            return site->alternate_gizmo_color();
        },
        [](scene* site, const color& value){
            site->alternate_gizmo_color(value);
        }
    });
    color_property<scene>(u"selection_color", u"Sel Color", true, true, {
        [](const scene* site){
            return site->selection_color();
        },
        [](scene* site, const color& value){
            site->selection_color(value);
        }
    });
    color_property<scene>(u"fade_color", u"Fade Color", true, true, {
        [](const scene* site){
            return site->fade_color();
        },
        [](scene* site, const color& value){
            site->fade_color(value);
        }
    });
    
    begin_private_properties();
    integer_property<scene>(u"name_seed", u"Name Seed", false, {
        [](const scene* site){
            return site->_n_seed;
        },
        [](scene* site, long value){
            if(director::in_editor()){
                site->_n_seed = value;
            }
        }
    });
    nullable_string_property<scene>(u"current", u"Current", false, false, {
        [](const scene* site){
            return site->serialize_current();
        },
        [](scene* site, const nullable<rb_string>& value){
            if(director::in_editor()){
                site->_saved_current = value;
            }
        }
    });
    buffer_property<scene>(u"camera", u"Camera", {
        [](const scene* site){
            return site->camera().to_buffer();
        },
        [](scene* site, const buffer value){
            site->camera(transform_space(value));
        }
    });
    end_private_properties();
    end_type();
}

void scene::alert(const rb_string &message) const {
    assert(in_editor() && !playing());
    if(director::editor_delegate())
        director::editor_delegate()->alert(message);
}

void scene::confirm(const rb_string &message, std::function<void (bool)> result) const {
    assert(in_editor() && !playing());
    if(director::editor_delegate())
        director::editor_delegate()->confirm(message, result);
}

void scene::fill_vector_with_children(std::vector<typed_object *> &children) const {
    children.clear();
    for (size_t i = 0; i < MAX_LAYERS; i++) {
        children.push_back(_layers[i]);
    }
}

void scene::set_children(const std::vector<typed_object *> &children) {
    for (size_t i = 0; i < MAX_LAYERS; i++) {
        if(_layers[i])
            delete _layers[i];
        
        _layers[i] = dynamic_cast<class layer*>(children[i]);
        _layers[i]->_parent_scene = this;
    }
}

rb_string scene::copy_selected_nodes(){
    assert(current());
    auto _grp = current()->group_selected();
    _grp->copy_node(true);
    rb_string _str = _grp->serialize_to_string();
    _grp->ungroup(true, true); //delete self...
    return _str;
}

void scene::rename_nodes_recursively(node* n){
    if(node_with_name(n->name()) || n->name() == u""){
        n->name(u"n" + rb::to_string(_n_seed));
        _n_seed++;
    }
    for(auto _child : *n){
        rename_nodes_recursively(_child);
    }
}

void scene::paste_nodes(rb_string copied){
    assert(current());
    auto _obj = dynamic_cast<node*>(typed_object::deserialize_from_string(copied));
    assert(_obj);
    vec2 _center = current()->from_space_to(space::normalized_screen).inverse().transformed_point(vec2::zero);
    _obj->old_transform(_obj->old_transform().moved(_center));
    rename_nodes_recursively(_obj);
    bool _added = current()->add_node(_obj);
    if(!_added){
        delete _obj;
        return;
    }
    auto _grp = dynamic_cast<group_component*>(_obj);
    if(_grp)
        _grp->ungroup(true, true);
}

void scene::camera_to_match_current_rotation(){
    if(current()){
        auto _t = transform_space::from_matrix(current()->from_space_to(space::scene));
        camera(camera().rotated(_t.rotation()));
    }
}

void scene::unblock_all(){
    for (size_t i = 0; i < MAX_LAYERS; i++) {
        _layers[i]->unblock_all_children();
    }
}

void scene::unhide_all(){
    for (size_t i = 0; i < MAX_LAYERS; i++) {
        _layers[i]->in_editor_hidden(false);
        _layers[i]->unhide_all_children();
    }
}

void scene::bring_to_front(uint32_t layer){
    assert(layer >= 0 && layer < MAX_LAYERS);
    if(layer == 0)
        return;
    auto _temp = _layers[0];
    _layers[0] = _layers[layer];
    _layers[layer] = _temp;
    if(director::editor_delegate())
        director::editor_delegate()->hierarchy_changed(nullptr);
}

void scene::send_to_back(uint32_t layer){
    assert(layer >= 0 && layer < MAX_LAYERS);
    if(layer == (MAX_LAYERS - 1))
        return;
    
    auto _temp = _layers[MAX_LAYERS - 1];
    _layers[MAX_LAYERS - 1] = _layers[layer];
    _layers[layer] = _temp;
    if(director::editor_delegate())
        director::editor_delegate()->hierarchy_changed(nullptr);
}

void scene::send_backward(uint32_t layer){
    assert(layer >= 0 && layer < MAX_LAYERS);
    if(layer == (MAX_LAYERS - 1))
        return;
    
    auto _nl = layer + 1;
    if(_nl >= MAX_LAYERS)
        _nl = MAX_LAYERS - 1;
    
    auto _temp = _layers[_nl];
    _layers[_nl] = _layers[layer];
    _layers[layer] = _temp;
    if(director::editor_delegate())
        director::editor_delegate()->hierarchy_changed(nullptr);
}

void scene::bring_forward(uint32_t layer){
    assert(layer >= 0 && layer < MAX_LAYERS);
    if(layer == 0)
        return;
    
    auto _nl = (int)layer - 1;
    if(_nl < 0)
        _nl = 0;
    
    auto _temp = _layers[_nl];
    _layers[_nl] = _layers[layer];
    _layers[layer] = _temp;
    if(director::editor_delegate())
        director::editor_delegate()->hierarchy_changed(nullptr);
}

nullable<rb_string> scene::serialize_current() const {
    if (!current())
        return nullptr;
    
    std::vector<uint32_t> _indexes;
    
    auto _current = current();
    
    while(_current){
        if(_current->parent()){
            auto _i = _current->parent()->search_node(dynamic_cast<node*>(_current));
            assert(_i.has_value());
            _indexes.push_back(_i.value());
            _current = _current->parent();
        }
        else {
            for (size_t i = 0; i < MAX_LAYERS; i++) {
                if(_layers[i] == _current)
                {
                    _indexes.push_back((uint32_t)i);
                    break;
                }
            }
            _current = nullptr;
        }
    }
    
    std::reverse(_indexes.begin(), _indexes.end());
    
    rb_string _str = u"";
    
    for (auto _i : _indexes){
        _str += rb::to_string(_i);
        _str += u"|";
    }
    
    return _str;
}

void scene::set_current_from_string(const nullable<rb_string> &str){
    if(!str.has_value()){
        current(nullptr);
        return;
    }
    
    std::vector<uint32_t> _indexes;
    rb_string _num = u"";
    for (size_t i = 0; i < str.value().length(); i++) {
        if(str.value()[i] != u'|'){
            _num += str.value()[i];
        }
        else {
            uint32_t _i = (uint32_t)[to_platform_string(_num) integerValue];
            _indexes.push_back(_i);
            _num = u"";
        }
    }
    
    if(_indexes.size() == 0){
        current(nullptr);
        return;
    }
    
    node_container* _current = layer(_indexes[0]);
    _indexes.erase(_indexes.begin());
    
    while (_indexes.size() != 0) {
        _current = _current->node_at(_indexes[0]);
        _indexes.erase(_indexes.begin());
    }
    
    current(_current);
}

void scene::add_named_node(const rb_string &name, const rb::node *n){
    if(name == u"")
        return;
    _by_name[name].insert(const_cast<node*>(n));
}

void scene::add_node_with_class(const rb_string &classes, const rb::node *n){
    auto _classes = rb::tokenize(classes);
    for (auto& _c : _classes){
        _by_class[_c].insert(const_cast<node*>(n));
    }
}

void scene::remove_named_node(const rb::node *n){
    if(_by_name.count(n->name()) != 0){
        _by_name[n->name()].erase(const_cast<node*>(n));
        if(_by_name[n->name()].size() == 0)
            _by_name.erase(n->name());
    }
}

void scene::remove_node_with_class(const rb::node *n){
    auto _classes = rb::tokenize(n->classes());
    for (auto& _c : _classes){
        if(_by_class.count(_c) != 0){
            _by_class[_c].erase(const_cast<node*>(n));
            if(_by_class[_c].size() == 0)
                _by_class.erase(_c);
        }
    }
}

const node* scene::node_with_name(const rb_string& name) const{
    if(_by_name.count(name) == 0)
        return nullptr;
    
    if(_by_name.at(name).size() == 0)
        return nullptr;
    
    return *_by_name.at(name).begin();
}

node* scene::node_with_name(const rb_string &name){
    if(_by_name.count(name) == 0)
        return nullptr;
    
    if(_by_name.at(name).size() == 0)
        return nullptr;
    
    return *_by_name.at(name).begin();
}

std::vector<const node*> scene::node_with_all_classes(const rb_string& classes) const{
    std::unordered_set<node*> _nodes;
    std::vector<const node*> _result;
    auto _classes = tokenize(classes);
    
    for (auto _c : _classes){
        if(_by_class.count(_c)){
            auto& _cns = _by_class.at(_c);
            if(_cns.size() == 0)
                return _result;
            
            if(_nodes.size() == 0)
                _nodes.insert(_cns.begin(), _cns.end());
            else {
                auto _it = _nodes.begin();
                auto _end = _nodes.end();
                while (_it != _end){
                    if(!_cns.count(*_it))
                        _it = _nodes.erase(_it);
                    else
                        _it++;
                }
                if(_nodes.size() == 0)
                    return _result;
            }
        }
        else
            return _result;
    }
    
    for (auto _n : _nodes)
        _result.push_back(_n);
    
    return _result;
}
std::vector<node*> scene::node_with_all_classes(const rb_string& classes){
    std::unordered_set<node*> _nodes;
    std::vector<node*> _result;
    auto _classes = tokenize(classes);
    
    for (auto _c : _classes){
        if(_by_class.count(_c)){
            auto& _cns = _by_class.at(_c);
            if(_cns.size() == 0)
                return _result;
            
            if(_nodes.size() == 0)
                _nodes.insert(_cns.begin(), _cns.end());
            else {
                auto _it = _nodes.begin();
                auto _end = _nodes.end();
                while (_it != _end){
                    if(!_cns.count(*_it))
                        _it = _nodes.erase(_it);
                    else
                        _it++;
                }
                if(_nodes.size() == 0)
                    return _result;
            }
        }
        else
            return _result;
    }
    
    for (auto _n : _nodes)
        _result.push_back(_n);
    
    return _result;
}
std::vector<const node*> scene::node_with_one_class(const rb_string& classes) const{
    std::unordered_set<node*> _nodes;
    std::vector<const node*> _result;
    auto _classes = tokenize(classes);
    
    for (auto _c : _classes){
        if(_by_class.count(_c)){
            auto& _cns = _by_class.at(_c);
            for (auto _n : _cns){
                _nodes.insert(_n);
            }
        }
    }
    
    for (auto _n : _nodes)
        _result.push_back(_n);
    
    return _result;
}
std::vector<node*> scene::node_with_one_class(const rb_string& classes){
    std::unordered_set<node*> _nodes;
    std::vector<node*> _result;
    auto _classes = tokenize(classes);
    
    for (auto _c : _classes){
        if(_by_class.count(_c)){
            auto& _cns = _by_class.at(_c);
            for (auto _n : _cns){
                _nodes.insert(_n);
            }
        }
    }
    
    for (auto _n : _nodes)
        _result.push_back(_n);
    
    return _result;
}

uint32_t scene::remove_degenerated(){
    std::vector<node*> _nodes;
    for (size_t i = 0; i < MAX_LAYERS; i++) {
        _layers[i]->test_degeneration(_nodes);
    }
    
    for (size_t i = 0; i < _nodes.size(); i++) {
        _nodes[i]->parent()->remove_node(_nodes[i], true);
    }
    
    return (uint32_t)_nodes.size();
}

const color& scene::fade_color() const {
    return _fade_color;
}

const color& scene::fade_color(const color& value){
    _fade_color = value;
    return _fade_color;
}

void scene::animated_fade(const rb::color &target_color, float time, std::function<void ()> completionFn){
    _target_fd_color = target_color;
    _source_fd_color = _fade_color;
    _faded_time_total = time;
    _faded_time_passed = 0;
    _completion_fn = completionFn;
}































