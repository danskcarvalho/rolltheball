//
//  node.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 07/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "node.h"
#include "layer.h"
#include "scene.h"
#include "mesh.h"
#include "polygon.h"
#include "null_texture_map.h"
#include "misc_classes_2.h"
#include "director.h"
#include "editor_delegate.h"

using namespace rb;

node::node(){
    _blocked = false;
    _move_flag = 0;
    _new_template = false;
    _parent_layer = nullptr;
    _parent_node = nullptr;
    _added = false;
    _active = false;
    _selection_marquee = nullptr;
    _id = new component_id();
    _id->named_node = this;
    _transformation = new transformation_values();
    _transformation->transformed_node = this;
    _transformation->translation(transform().origin());
    _transformation->scale(transform().scale());
    _transformation->rotation(vec2(TO_DEGREES(transform().rotation().x()), TO_DEGREES(transform().rotation().y())));
    for (int i = 0; i < (int)registrable_event::count; i++)
        _registration_ids[i] = nullptr;
    _enabled_transformation_notifications = true;
    _capabilities[(size_t)node_capability::rendering] = false;
    _capabilities[(size_t)node_capability::children_rendering] = true;
    _capabilities[(size_t)node_capability::can_become_current] = true;
    _capabilities[(size_t)node_capability::selection_rectangle] = true;
    _capabilities[(size_t)node_capability::gizmo_rendering] = false;
}

void node::alert(const rb_string &message) const {
    if(director::editor_delegate())
        director::editor_delegate()->alert(message);
}

void node::confirm(const rb_string &message, std::function<void (bool)> result) const {
    if(director::editor_delegate())
        director::editor_delegate()->confirm(message, result);
}

void node::push_itself_in_property_inspector(const rb_string& title) {
    rb_string _t = title;
    if(title == u"" & name() != u"")
        _t = u"Edit " + name();
    else
        _t = title;
    director::edit_object_properties(this, _t);
}

bool node::template_for_new() const {
    return _new_template;
}

node::~node(){
    if(_selection_marquee)
        delete _selection_marquee;
    if(_id)
        delete _id;
    if(_transformation)
        delete _transformation;
}

void node::describe_type(){
    node_container::describe_type();
    start_type<node>([](){ return nullptr; });
    object_property<node, component_id>(u"rb::node_id", u"Identification", true, {
        [](const node* site){
            return site->_id;
        },
        [](node* site, component_id* value){
            free_old_object(site->_id, value);
            site->_id = value;
            if(site->_id)
                site->_id->named_node = site;
        }
    });
    if(renderable()){
        object_property<node, transformation_values>(u"rb::transformation", u"Transformation", true, {
            [](const node* site){
                return site->_transformation;
            },
            [](node* site, transformation_values* value){
                free_old_object(site->_transformation, value);
                site->_transformation = value;
                if(site->_transformation){
                    site->_transformation->transformed_node = site;
                    site->_transformation->sync_values();
                }
            }
        });
        begin_private_properties();
        boolean_property<node>(u"rb::blocked", u"Blocked", false, {
            [](const node* site){
                return site->blocked();
            },
            [](node* site, const bool value){
                site->blocked(value);
            }
        });
        end_private_properties();
    }
    end_type();
}

rb_string node::type_name() const {
    return u"rb::node";
}

rb_string node::displayable_type_name() const {
    return u"Node";
}

//Transform
const transform_space& node::transform() const {
    return node_container::transform();
}

const transform_space& node::transform(const rb::transform_space &value){
    auto _t = node_container::transform(value);
    _transformation->translation(_t.origin());
    _transformation->rotation(vec2(TO_DEGREES(_t.rotation().x()), TO_DEGREES(_t.rotation().y())));
    _transformation->scale(_t.scale());
    if(_registration_ids[(size_t)registrable_event::transform_changed].has_value() && _enabled_transformation_notifications)
        transform_changed();
    return node_container::transform();
}

void node::shutdown_transform_notifications(){
    _enabled_transformation_notifications = false;
}

void node::restore_transform_notification(){
    _enabled_transformation_notifications = true;
}

//Scene
scene* node::parent_scene(){
    if(_parent_layer)
        return _parent_layer->parent_scene();
    else
        return nullptr;
}

const scene* node::parent_scene() const {
    if(_parent_layer)
        return _parent_layer->parent_scene();
    else
        return nullptr;
}

layer* node::parent_layer(){
    return _parent_layer;
}

const layer* node::parent_layer() const {
    return _parent_layer;
}

void node::set_layer_recursively(rb::layer *value){
    if(this->_parent_layer && this->_parent_layer->parent_scene()){
        this->_parent_layer->parent_scene()->remove_named_node(this);
        this->_parent_layer->parent_scene()->remove_node_with_class(this);
    }
    if(value && value->parent_scene()){
        value->parent_scene()->add_named_node(this->name(), this);
        value->parent_scene()->add_node_with_class(this->classes(), this);
    }
    this->_parent_layer = value;
    for (auto _child : *this){
        _child->set_layer_recursively(value);
    }
}

//Active flag
bool node::active() const {
    return _active;
}

//Internal Activation Listeners
void node::internal_before_becoming_active(bool node_was_moved){
    for (auto _child : *this){
        _child->internal_before_becoming_active(node_was_moved);
    }
    this->before_becoming_active(node_was_moved);
    _active = true;
}

void node::internal_after_becoming_active(bool node_was_moved){
    this->after_becoming_active(node_was_moved);
    for (auto _child : *this){
        _child->internal_after_becoming_active(node_was_moved);
    }
}

void node::internal_before_becoming_inactive(bool node_was_moved){
    this->before_becoming_inactive(node_was_moved);
    for (auto _child : *this){
        _child->internal_before_becoming_inactive(node_was_moved);
    }
    
    unregister_for_all_events();
    
    _active = false;
}

void node::internal_after_becoming_inactive(bool node_was_moved){
    for (auto _child : *this){
        _child->internal_after_becoming_inactive(node_was_moved);
    }
    this->after_becoming_inactive(node_was_moved);
}

bool node::move_node_to(rb::node_container *new_parent){
    auto _has_parent = this->parent_node_container() != nullptr;
    if(_has_parent && new_parent)
        _move_flag++;
    
    bool _r = true;
    
    if (this->parent_node())
        _r = this->parent_node()->remove_node(this, false);
    else if(this->parent_layer())
        _r = this->parent_layer()->remove_node(this, false);
    
    if(!_r){
        if(_has_parent && new_parent)
            _move_flag--;
        return false;
    }
    
    if(new_parent)
        _r = new_parent->add_node(this);
    
    if(_has_parent && new_parent)
        _move_flag--;
    
    return _r;
}

//Add/Remove Nodes
bool node::add_node_at(rb::node *n, uint32_t at){
    assert(n != this);
    assert(n);
    assert(!n->_added);
    bool _n_added = this->node_container::add_node_at(n, at);
    if(_n_added)
    {
        n->_added = true;
        n->_parent_layer = this->_parent_layer;
        n->_parent_node = this;
        n->set_layer_recursively(this->_parent_layer);
        
        if(_parent_layer && _parent_layer->parent_scene()->active()){
            n->internal_before_becoming_active(n->_move_flag != 0);
            n->internal_after_becoming_active(n->_move_flag != 0);
        }
    }
    if(_n_added && active())
        invalidate_buffers();
    if(director::editor_delegate())
        director::editor_delegate()->hierarchy_changed(this);
    return _n_added;
}

bool node::remove_node(rb::node *n, bool cleanup){
    assert(n);
    assert(n->_added);
    bool _n_removed = this->node_container::remove_node(n, false);
    if(_n_removed){
        if(_parent_layer && _parent_layer->parent_scene()->active()){
            n->internal_before_becoming_inactive(n->_move_flag != 0);
        }
        
        n->_added = false;
        n->_parent_layer = nullptr;
        n->_parent_node = nullptr;
        n->set_layer_recursively(nullptr);
        
        if(_parent_layer && _parent_layer->parent_scene()->active()){
            n->internal_after_becoming_inactive(n->_move_flag != 0);
        }
        if(cleanup)
            n->commit_suicide();
    }
    
    if(_n_removed && active())
        invalidate_buffers();
    if(director::editor_delegate())
        director::editor_delegate()->hierarchy_changed(this);
    return _n_removed;
}

bool node::bring_to_front(node* n){
    bool _r = this->node_container::bring_to_front(n);
    if(_r)
        invalidate_buffers();
    if(director::editor_delegate())
        director::editor_delegate()->hierarchy_changed(this);
    return _r;
}
bool node::send_to_back(node* n){
    bool _r = this->node_container::send_to_back(n);
    if(_r)
        invalidate_buffers();
    if(director::editor_delegate())
        director::editor_delegate()->hierarchy_changed(this);
    return _r;
}
bool node::send_backward(node* n){
    bool _r = this->node_container::send_backward(n);
    if(_r)
        invalidate_buffers();
    if(director::editor_delegate())
        director::editor_delegate()->hierarchy_changed(this);
    return _r;
}
bool node::bring_forward(node* n){
    bool _r = this->node_container::bring_forward(n);
    if(_r)
        invalidate_buffers();
    if(director::editor_delegate())
        director::editor_delegate()->hierarchy_changed(this);
    return _r;
}

//Playing/Pause
void node::scene_playing(){
    playing();
    for (auto _n : *this)
    {
        _n->scene_playing();
    }
}

void node::scene_paused(){
    paused();
    for (auto _n : *this)
    {
        _n->scene_paused();
    }
}

void node::playing(){
    
}

void node::paused(){
    
}

nullable<node::ordered_nodes::iterator>& node::registration_id(const registrable_event event) {
    return _registration_ids[(size_t)event];
}

void node::register_for(const registrable_event event, int32_t priority){
    assert(active());
    if(registration_id(event).has_value())
        parent_scene()->unregister_for(event, registration_id(event).value());
    registration_id(event) = parent_scene()->register_for(event, priority, this);
}

void node::unregister_for(const registrable_event event) {
    assert(active());
    if(!registration_id(event).has_value())
        return;
    parent_scene()->unregister_for(event, registration_id(event).value());
    registration_id(event) = nullptr;
}

void node::unregister_for_all_events(){
    assert(active());
    for (int i = 0; i < (int)registrable_event::count; i++) {
        auto event = (registrable_event)i;
        if(!registration_id(event).has_value())
            continue;
        parent_scene()->unregister_for(event, registration_id(event).value());
        registration_id(event) = nullptr;
    }
}

node* node::parent_node() {
    return _parent_node;
}

const node* node::parent_node() const {
    return _parent_node;
}

void node::enabled(const rb::node_capability capability, bool value){
    auto _previous = _capabilities[(size_t)capability];
    if(_previous == value)
        return;
    
    if(capability == node_capability::rendering)
        invalidate_buffers();
    
    if(capability == node_capability::children_rendering)
        invalidate_buffers();
    
    _capabilities[(size_t)capability] = value;
}

bool node::enabled(const rb::node_capability capability) const {
    return _capabilities[(size_t)capability];
}

void node::invalidate_buffers(){
    assert(active());
    parent_layer()->invalidate_buffers();
}

void node::add_mesh_for_rendering(rb::mesh *m, const rb_string &texture_name, bool wrapping_in_shader){
    assert(active());
    parent_layer()->add_mesh_for_rendering(m, texture_name, wrapping_in_shader);
}
void node::set_texture_for_mesh(rb::mesh *m, const rb_string &texture_name, bool wrapping_in_shader){
    assert(active());
    parent_layer()->set_texture_for_mesh(m, texture_name, wrapping_in_shader);
}
void node::add_gizmo(rb::mesh *m, const rb_string &texture_name, bool wrapping_in_shader){
    assert(active());
    parent_layer()->add_gizmo(m, texture_name, wrapping_in_shader);
}

const texture_map* node::create_mapping(const rb_string& texture_name, const transform_space& transform, texture_map* refurbished){
    assert(active());
    return parent_layer()->create_mapping(texture_name, transform, refurbished);
}

void node::render(const bool refill_buffers){
    //do nothing
}

void node::internal_render(const bool refill_buffers){
    if (enabled(node_capability::rendering) && !is_hidden_in_editor()) {
        render(refill_buffers);
    }
    
    if(enabled(node_capability::children_rendering)){
        for (auto _children : *this){
            if(!_children->is_hidden_in_editor())
                _children->internal_render(refill_buffers);
        }
    }
}
void node::internal_render_gizmo(){
    if(enabled(node_capability::gizmo_rendering))
        render_gizmo();
    else
        render_selection_marquee();
    
    for (auto _children : *this){
        _children->internal_render_gizmo();
    }
}

rb_string node::no_texture = u"";

void node::render_selection_marquee(){
    if(is_currently_selected()){
        if(!_selection_marquee)
            _selection_marquee = new mesh();
        auto _b = bounds();
        polygon _p;
        _b.to_polygon(_p);
        from_node_space_to(space::screen).from_space_to_base().transform_polygon(_p);
        if(!_p.is_simple())
            return;
        _p.optimize();
        if(_p.point_count() < 2)
            return;
        if(!_p.area().has_value() || almost_equal(_p.area().value(), 0))
            return;
        if(_p.point_count() == 2)
            _p.open_polygon();
        null_texture_map _n_map;
        _p.to_outline_mesh(*_selection_marquee, _n_map, NODE_SELECTION_SIZE, corner_type::miter, true);
        _selection_marquee->set_color(parent_scene()->selection_color());
        _selection_marquee->set_blend(0);
        add_gizmo(_selection_marquee, no_texture, false);
    }
}

void node::render_gizmo(){
    render_selection_marquee();
}

void node::space_changed(){
    //do nothing
}

bool node::in_editor() const {
    if(parent_scene())
        return parent_scene()->in_editor();
    else
        return false;
}

bool node::is_playing() const {
    if(parent_scene())
        return parent_scene()->playing();
    else
        return false;
}

//update
void node::update(float dt){
    //do nothing
}

void node::in_editor_update(float dt){
    //do nothing
}

void node::before_becoming_active(bool node_was_moved){
    //do nothing
}

void node::before_becoming_inactive(bool node_was_moved){
    //do nothing
}

void node::after_becoming_active(bool node_was_moved){
    //do nothing
}

void node::after_becoming_inactive(bool node_was_moved){
    //do nothing
}

const node_container* node::parent_node_container() const {
    if(parent_node())
        return parent_node();
    else
        return parent_layer();
}
transform_space node::from_node_space_to(const space another) const {
    assert(active());
    return from_space_to(another);
}

transform_space node::from_node_space_to(const rb::node_container *another) const {
    assert(active());
    assert(parent_scene() == another->parent_scene());
    assert(another);
    if(this == another)
        return transform_space(); //identity
    auto _to_scene_1 = another->from_space_to(space::scene);
    auto _to_scene_2 = from_space_to(space::scene);
    return _to_scene_1.inverse() * _to_scene_2;
}

void node::transform_changed(){
    
}

bool node::fast_hit_test(const vec2& pt) const {
    auto _t = from_node_space_to(space::screen);
    if(!_t.test_direction(transform_direction::from_base_to_space))
        return false;
    auto _pt_in_self_space = _t.from_base_to_space().transformed_point(pt);
    return bounds().intersects(_pt_in_self_space);
}

bool node::fast_hit_test(const rb::rectangle &rc) const {
    auto _t = from_node_space_to(space::screen);
    if(!_t.test_direction(transform_direction::from_base_to_space))
        return false;
    auto _rc_in_self_space = _t.from_base_to_space().transformed_rectangle(rc);
    return bounds().intersects(_rc_in_self_space);
}

bool node::hit_test(const rb::vec2 &pt) const {
    return true;
}

bool node::hit_test(const rb::rectangle &rc) const {
    polygon _rc;
    rc.to_polygon(_rc);
    auto _t = from_node_space_to(space::screen);
    if(!_t.test_direction(transform_direction::from_base_to_space))
        return false;
    _t.from_base_to_space()
    .transform_polygon(_rc);
    polygon _this_rc;
    bounds().to_polygon(_this_rc);
    
    return _this_rc.test_intersection(_rc);
}

rectangle node::bounds() const {
    rectangle _current_rc = rectangle(vec2::zero, vec2::zero);
    for(auto _child : *(node*)this){
        auto _b = _child->transform().from_space_to_base().transformed_rectangle(_child->bounds());
        _current_rc = _current_rc.compute_union(_b);
    }
    return _current_rc;
}

//selection
bool node::is_selected() const {
    if(!parent_node_container())
        return false;
    
    return parent_node_container()->is_selected(this);
}

bool node::is_currently_selected() const {
    if(!parent_node_container())
        return false;
    return is_selected() && parent_node_container()->is_current();
}

void node::add_to_selection(){
    if(!parent_node_container())
        return;
    
    auto n = (node_container*)parent_node_container();
    n->add_to_selection(this);
}

void node::remove_from_selection(){
    if(!parent_node_container())
        return;
    
    auto n = (node_container*)parent_node_container();
    n->remove_from_selection(this);
}



void node::keydown(const uint32_t keycode, const keyboard_modifier modifier, bool& swallow){
    
}
void node::keyup(const uint32_t keycode, const keyboard_modifier modifier, bool& swallow){
    
}
void node::touches_began(const std::vector<touch>& touches, bool& swallow){
    
}
void node::touches_moved(const std::vector<touch>& touches, bool& swallow){
    
}
void node::touches_ended(const std::vector<touch>& touches, bool& swallow){
    
}
void node::touches_cancelled(const std::vector<touch>& touches, bool& swallow){
    
}
void node::mouse_down(const vec2& normalized_position, bool& swallow){
    
}
void node::mouse_up(const vec2& normalized_position, bool& swallow){
    
}
void node::mouse_dragged(const vec2& normalized_position, bool& swallow){
    
}
void node::begin_live_edit(rb::live_edit kind){
    
}
void node::end_live_edit(){
    
}

const rb_string& node::name() const {
    return _id->name();
}

const rb_string& node::name(const rb_string &value) {
    _id->name(value);
    return _id->name();
}

const rb_string& node::classes() const {
    return _id->classes();
}

const rb_string& node::classes(const rb_string &value){
    _id->classes(value);
    return _id->classes();
}

bool node::has_class(const rb_string &cls) const {
    auto _classes = rb::tokenize(classes());
    for(auto& _cls : _classes){
        if(_cls == cls)
            return true;
    }
    
    return false;
}

void node::set_children(const std::vector<typed_object *> &children){
    node_container::set_children(children);
    for (auto _c : children){
        auto _n = dynamic_cast<node*>(_c);
        _n->_added = true;
        _n->_parent_node = this;
    }
}

bool node::blocked() const {
    return _blocked;
}

bool node::blocked(const bool value) {
    assert(renderable());
    if(value == _blocked)
        return _blocked;
    _blocked = value;
    if(_blocked)
        this->remove_from_selection();
    if(_blocked && parent_scene() && parent_scene()->current() == this){
        const_cast<scene*>(parent_scene())->current(const_cast<node_container*>(parent()));
    }
    return _blocked;
}

bool rb::is_node(const typed_object* obj){
    return dynamic_cast<const node*>(obj) != nullptr;
}



































