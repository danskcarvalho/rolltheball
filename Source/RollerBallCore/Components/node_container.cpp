//
//  node_container.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 08/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "node_container.h"
#include "node.h"
#include "scene.h"
#include "director.h"
#include "editor_delegate.h"
#include "group_component.h"
#include "matrix3x3.h"
#include "layer.h"

using namespace rb;

node_container::node_container(){
    _dirty_transform = true;
    _in_editor_hidden = false;
    _selected = nullptr;
}

bool node_container::add_node(rb::node *n){
    return add_node_at(n, (uint32_t)_child_nodes.size());
}

bool node_container::add_node_at(rb::node *n, uint32_t at){
    _child_nodes.insert(_child_nodes.begin() + at, n);
    return true;
}

bool node_container::remove_node(rb::node *n, bool cleanup){
    auto _n_position = search_node(n);
    if(!_n_position.has_value())
        return false;
    if(parent_scene() && parent_scene()->current() == n)
        const_cast<scene*>(parent_scene())->current((node_container*)n->parent_node_container());
    if(parent_scene())
        const_cast<scene*>(parent_scene())->_saved_selection.erase(n);
    _child_nodes.erase(_child_nodes.begin() + _n_position.value());
    remove_from_selection(n);
    if(cleanup)
        n->commit_suicide();
    return true;
}

node* node_container::first() {
    return _child_nodes.front();
}

const node* node_container::first() const {
    return _child_nodes.front();
}

node* node_container::last(){
    return _child_nodes.back();
}

const node* node_container::last() const {
    return _child_nodes.back();
}

nullable<uint32_t> node_container::search_node(const rb::node *n) const {
    for (uint32_t i = 0; i < _child_nodes.size(); i++) {
        if(_child_nodes[i] == n)
            return i;
    }
    return nullptr;
}

node_container::node_list::iterator node_container::begin() {
    return _child_nodes.begin();
}

node_container::node_list::iterator node_container::end() {
    return _child_nodes.end();
}

node_container::node_list::const_iterator node_container::begin() const {
    return _child_nodes.cbegin();
}

node_container::node_list::const_iterator node_container::end() const {
    return _child_nodes.cend();
}

bool node_container::bring_to_front(rb::node *n){
    auto _index = search_node(n);
    if(!_index.has_value())
        return false;
    auto _position = _child_nodes.begin() + _index.value();
    if(_position == begin())
        return true;
    _child_nodes.erase(_position);
    _child_nodes.insert(begin(), n);
    return true;
}

bool node_container::send_to_back(rb::node *n){
    auto _index = search_node(n);
    if(!_index.has_value())
        return false;
    auto _position = _child_nodes.begin() + _index.value();
    if(_position == std::prev(end()))
        return true;
    _child_nodes.erase(_position);
    _child_nodes.insert(end(), n);
    return true;
}

uint32_t node_container::node_count() const {
    return (uint32_t)_child_nodes.size();
}

bool node_container::send_backward(rb::node *n){
    auto _index = search_node(n);
    if(!_index.has_value())
        return false;
    auto _position = _child_nodes.begin() + _index.value();
    if(_position == std::prev(end()))
        return true;
    _child_nodes.erase(_position);
    _position = _child_nodes.begin() + _index.value() + 1;
    _child_nodes.insert(_position, n);
    
    return true;
}

bool node_container::bring_forward(rb::node *n){
    auto _index = search_node(n);
    if(!_index.has_value())
        return false;
    auto _position = _child_nodes.begin() + _index.value();
    if(_position == begin())
        return true;
    _child_nodes.erase(_position);
    _position = _child_nodes.begin() + (_index.value() - 1);
    _child_nodes.insert(_position, n);
    
    return true;
}

node_container::~node_container(){
    for (auto _n : _child_nodes)
        _n->commit_suicide();
    _child_nodes.clear();
}

void node_container::copy_nodes_to_vector(std::vector<node *> &nodes, const node_filter filter) const {
    nodes.clear();
    for (auto _n : _child_nodes){
        if(_n->renderable() && !_n->_new_template && rb::has_flag(filter, node_filter::renderable))
            nodes.push_back(_n);
        else if((!_n->renderable() || _n->_new_template) && rb::has_flag(filter, node_filter::non_renderable))
            nodes.push_back(_n);
    }
}

void node_container::commit_suicide(){
    delete this;
}

bool node_container::remove_node_at(uint32_t at, bool cleanup){
    if(at >= _child_nodes.size())
        return false;
    return this->remove_node(_child_nodes[at], cleanup);
}

//transform
const transform_space& node_container::transform() const {
    return _transform;
}

const transform_space& node_container::transform(const rb::transform_space &value){
    if(_transform == value)
        return _transform;
    _dirty_transform = true;
    _transform = value;
    return _transform;
}

transform_space node_container::from_space_to(const rb::space another) const{
    assert(parent_scene()); //has to be active...
    if(another == space::camera){
        transform_space _to_scene = from_space_to(space::scene);
        auto _camera = parent_scene()->camera();
        auto _l = parent_layer();
        if(!_l)
            _l = dynamic_cast<const layer*>(this);
        assert(_l);
        if(rb::has_flag(_l->camera_invariant_flags(), camera_invariant::position))
            _camera.origin(vec2::zero);
        else
            _camera.origin(_camera.origin() * _l->camera_position_factor());
        if(rb::has_flag(_l->camera_invariant_flags(), camera_invariant::scale))
            _camera.scale(vec2(1, 1));
        if(rb::has_flag(_l->camera_invariant_flags(), camera_invariant::rotation))
            _camera.rotation(0);
        return _camera.inverse() * _to_scene;
    }
    else if(another == space::layer){
        const node_container* _current = this;
        transform_space _acc = transform_space();
        
        while (_current->parent_node_container()) {
            _acc = _current->transform() * _acc;
            _current = _current->parent_node_container();
        }
        return _acc;
    }
    else if(another == space::normalized_screen){
        transform_space _to_camera = from_space_to(space::camera);
        const vec2& _viewport_size = parent_scene()->viewport_size();
        float _aspect = _viewport_size.y() / _viewport_size.x();
        if(!parent_scene()->camera_aspect_correction())
            _aspect = 1;
        transform_space _correction = transform_space(vec2::zero, vec2(_aspect, 1), 0);
        return _correction * _to_camera;
    }
    else if(another == space::scene){
        const node_container* _current = this;
        transform_space _acc = transform_space();
        
        while (_current) {
            _acc = _current->transform() * _acc;
            _current = _current->parent_node_container();
        }
        return _acc;
    }
    else { //screen
        const vec2& _viewport_size = parent_scene()->viewport_size();
        transform_space _norm_screen = from_space_to(space::normalized_screen);
        transform_space _translation = transform_space(vec2(1, 1));
        transform_space _scale = transform_space(vec2::zero, vec2(_viewport_size.x() / 2.0, _viewport_size.y() / 2.0));
        return _scale * _translation * _norm_screen;
    }
}

const node_container* node_container::parent() const {
    return parent_node_container();
}

node_container* node_container::parent() {
    return const_cast<node_container*>(parent_node_container());
}

bool node_container::has_space_changed() const {
    assert(parent_scene());
    if (_dirty_transform)
        return true;
    if(parent_scene()){
        if(parent_scene()->_dirty_transform)
            return true;
    }
    if (parent_node_container())
        return parent_node_container()->has_space_changed();
    else { //in layer...
        return false;
    }
}

void node_container::clear_dirty_transform(){
    _dirty_transform = false;
    for(auto _c : *this){
        _c->clear_dirty_transform();
    }
}

void node_container::internal_became_current(){
    became_current();
}

void node_container::became_current(){
    
}

void node_container::internal_resign_current(){
    resign_current();
}

void node_container::resign_current(){
    
}

bool node_container::is_current() const{
    return parent_scene()->current() == this;
}

//selection
void node_container::add_to_selection(rb::node *n){
    if(!n)
        return;
    if(n->parent_node_container() != this)
        return;
    _selection.insert(n);
    if(parent_scene()->active())
        ((scene*)parent_scene())->start_transformation();
    if(director::editor_delegate())
        director::editor_delegate()->selection_changed(this);
}

void node_container::remove_from_selection(rb::node *n){
    if(!n)
        return;
    if(_selection.count(n) == 0)
        return;
    _selection.erase(n);
    if(parent_scene()->active())
        ((scene*)parent_scene())->start_transformation();
    if(director::editor_delegate())
        director::editor_delegate()->selection_changed(this);
}

std::unordered_set<node*>::iterator node_container::internal_remove_from_selection(std::unordered_set<node*>::iterator& pos){
    auto _ret = _selection.erase(pos);
    ((scene*)parent_scene())->start_transformation();
    if(director::editor_delegate())
        director::editor_delegate()->selection_changed(this);
    return _ret;
}

void node_container::clear_selection(){
    clear_selection_container();
}

void node_container::clear_selection_container(){
    _selection.clear();
    ((scene*)parent_scene())->start_transformation();
    if(director::editor_delegate())
        director::editor_delegate()->selection_changed(this);
}

void node_container::fill_with_selection(std::vector<node *>& selection, const node_filter filter, bool in_order) const {
    selection.clear();
    if(!in_order){
        for (auto _n : _selection){
            if(_n->renderable() && !_n->_new_template && rb::has_flag(filter, node_filter::renderable))
                selection.push_back(_n);
            else if((!_n->renderable() || _n->_new_template) && rb::has_flag(filter, node_filter::non_renderable))
                selection.push_back(_n);
        }
    }
    else {
        for (auto _n : *this){
            if(_n->is_selected()){
                if(_n->renderable() && !_n->_new_template && rb::has_flag(filter, node_filter::renderable))
                    selection.push_back(_n);
                else if((!_n->renderable() || _n->_new_template) && rb::has_flag(filter, node_filter::non_renderable))
                    selection.push_back(_n);
            }
        }
    }
}

uint32_t node_container::selection_count(const node_filter filter) const {
    if(filter == node_filter::all)
        return (uint32_t)_selection.size();
    else {
        uint32_t _count = 0;
        for (auto _n : _selection){
            if(_n->renderable() && !_n->_new_template && rb::has_flag(filter, node_filter::renderable))
                _count++;
            else if((!_n->renderable() || _n->_new_template) && rb::has_flag(filter, node_filter::non_renderable))
                _count++;
        }
        return _count;
    }
}

bool node_container::is_selected(const rb::node *n) const{
    if(!n)
        return false;
    return _selection.count((node*)n) != 0;
}

bool node_container::renderable() const {
    return true;
}

group_component* node_container::group_nodes(const std::vector<node *> &nodes){
    assert(nodes.size() != 0);
    assert(std::all_of(nodes.begin(), nodes.end(), [](node* n) { return n != nullptr; }));
    assert(std::all_of(nodes.begin(), nodes.end(), [=](node* n){ return n->parent_node_container() == this; }));
//    assert(std::all_of(nodes.begin(), nodes.end(), [](node* n) { return n->renderable(); }));
    
    nullable<rectangle> _rc;
    
    for (auto _n : nodes){
        if(!_n->renderable())
            continue;
        auto _b = _n->transform().from_space_to_base().transformed_rectangle(_n->bounds());
        if(!_rc.has_value())
            _rc = _b;
        else
            _rc = _rc.value().compute_union(_b);
    }
    
    transform_space _grp_space = transform_space(_rc.value().center(), 1, 0);
    auto _new_grp = new group_component();
    _new_grp->transform(_grp_space);
    bool _added_grp = this->add_node(_new_grp);
    if(!_added_grp){
        delete _new_grp;
        return nullptr;
    }
    
    for (auto _n : nodes){
        auto _new_t = _grp_space.inverse() * _n->transform();
//        this->remove_node(_n, false);
//        _new_grp->add_node(_n);
        _n->move_node_to(_new_grp);
        _n->transform(_new_t);
    }
    
    return _new_grp;
}

group_component* node_container::group_selected(){
    std::vector<node*> _selection;
    fill_with_selection(_selection, node_filter::all, true);
    assert(_selection.size() != 0);
    auto _grp = group_nodes(_selection);
    if(!_grp)
        return nullptr;
    clear_selection();
    add_to_selection(_grp);
    return _grp;
}

std::vector<rb_string> node_container::transformables(){
    return std::vector<rb_string>();
}

void node_container::start_transformation(long index){
}

bool node_container::adjust_transformation(const rb::transform_space &transform, bool only_if_all){
    auto _previous_t = this->transform();
    if(!transform.test_direction(transform_direction::from_base_to_space))
        return false;
    auto _i_t = transform.inverse();
    
    std::unordered_map<node*, transform_space> _inverses;
    
    for(auto _n : *this){
        if(only_if_all){
            if(!_n->transform().test_direction(transform_direction::from_base_to_space))
                return false;
        }
        
        _inverses.insert({_n, _n->transform().inverse()});
    }
    
    for(auto _n : *this){
        if(_inverses.count(_n) == 0)
            continue;
        
        _n->transform(_i_t * _previous_t * _n->transform());
    }
    
    this->transform(transform);
    
    return true;
}

typed_object* node_container::clone() const {
    auto _c = dynamic_cast<node_container*>(typed_object::clone());
    return clone_children(_c);
}

typed_object* node_container::clone_children(node_container* cloned) const {
    std::vector<node*> _current_children;
    cloned->copy_nodes_to_vector(_current_children);
    for (auto _n : _current_children)
        cloned->remove_node(_n, true);
        
    for (auto _n : _child_nodes){
        cloned->add_node(dynamic_cast<node*>(_n->clone()));
    }
    return cloned;
}

void node_container::fill_vector_with_children(std::vector<typed_object *> &children) const {
    children.clear();
    for (auto _n : *this){
        children.push_back(_n);
    }
}

void node_container::set_children(const std::vector<typed_object *> &children) {
    _child_nodes.clear();
    for (auto _n : children){
        auto _node = dynamic_cast<node*>(_n);
        _child_nodes.push_back(_node);
        _node->_added = true;
    }
}

std::vector<rectangle> node_container::calc_bounds(const std::vector<node *> &nodes) const{
    std::vector<rectangle> _rcs;
    for (auto _n : nodes){
        auto _b = _n->bounds();
        auto _t = _n->from_node_space_to(space::normalized_screen);
        _rcs.push_back(_t.from_space_to_base().transformed_rectangle(_b));
    }
    return _rcs;
}

void node_container::hor_align_by_center(const std::vector<node *> &nodes){
    assert(nodes.size() != 0);
    auto _bs = calc_bounds(nodes);
    auto _vs = vec2::zero;
    auto _sum = 0.0;
    for (auto& _rc : _bs){
        _vs += _rc.center();
        _sum += 1;
    }
    _vs /= _sum;
    for (size_t i = 0; i < nodes.size(); i++) {
        auto _v = _vs - _bs[i].center();
        auto _nv = this->from_space_to(space::normalized_screen).from_space_to_base().transformed_point(nodes[i]->transform().origin());
        auto _new_origin = vec2(_nv.x() + _v.x(), _nv.y());
        this->from_space_to(space::normalized_screen).from_base_to_space().transform_point(_new_origin);
        nodes[i]->transform(nodes[i]->transform().moved(_new_origin));
    }
}

void node_container::hor_align_by_left_edge(const std::vector<node *> &nodes){
    assert(nodes.size() != 0);
    auto _bs = calc_bounds(nodes);
    auto _vs = vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    for (auto& _rc : _bs){
        if(_rc.top_left().x() < _vs.x()){
            _vs.x(_rc.top_left().x());
            _vs.y(_rc.top_left().y());
        }
    }
    for (size_t i = 0; i < nodes.size(); i++) {
        auto _v = _vs - _bs[i].top_left();
        auto _nv = this->from_space_to(space::normalized_screen).from_space_to_base().transformed_point(nodes[i]->transform().origin());
        auto _new_origin = vec2(_nv.x() + _v.x(), _nv.y());
        this->from_space_to(space::normalized_screen).from_base_to_space().transform_point(_new_origin);
        nodes[i]->transform(nodes[i]->transform().moved(_new_origin));
    }
}

void node_container::hor_align_by_right_edge(const std::vector<node *> &nodes){
    assert(nodes.size() != 0);
    auto _bs = calc_bounds(nodes);
    auto _vs = vec2(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
    for (auto& _rc : _bs){
        if(_rc.top_right().x() > _vs.x()){
            _vs.x(_rc.top_right().x());
            _vs.y(_rc.top_right().y());
        }
    }
    for (size_t i = 0; i < nodes.size(); i++) {
        auto _v = _vs - _bs[i].top_right();
        auto _nv = this->from_space_to(space::normalized_screen).from_space_to_base().transformed_point(nodes[i]->transform().origin());
        auto _new_origin = vec2(_nv.x() + _v.x(), _nv.y());
        this->from_space_to(space::normalized_screen).from_base_to_space().transform_point(_new_origin);
        nodes[i]->transform(nodes[i]->transform().moved(_new_origin));
    }
}

void node_container::ver_align_by_center(const std::vector<node *> &nodes){
    assert(nodes.size() != 0);
    auto _bs = calc_bounds(nodes);
    auto _vs = vec2::zero;
    auto _sum = 0.0;
    for (auto& _rc : _bs){
        _vs += _rc.center();
        _sum += 1;
    }
    _vs /= _sum;
    for (size_t i = 0; i < nodes.size(); i++) {
        auto _v = _vs - _bs[i].center();
        auto _nv = this->from_space_to(space::normalized_screen).from_space_to_base().transformed_point(nodes[i]->transform().origin());
        auto _new_origin = vec2(_nv.x(), _nv.y() + _v.y());
        this->from_space_to(space::normalized_screen).from_base_to_space().transform_point(_new_origin);
        nodes[i]->transform(nodes[i]->transform().moved(_new_origin));
    }
}

void node_container::ver_align_by_top_edge(const std::vector<node *> &nodes){
    assert(nodes.size() != 0);
    auto _bs = calc_bounds(nodes);
    auto _vs = vec2(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
    for (auto& _rc : _bs){
        if(_rc.top_left().y() > _vs.y()){
            _vs.x(_rc.top_left().x());
            _vs.y(_rc.top_left().y());
        }
    }
    for (size_t i = 0; i < nodes.size(); i++) {
        auto _v = _vs - _bs[i].top_left();
        auto _nv = this->from_space_to(space::normalized_screen).from_space_to_base().transformed_point(nodes[i]->transform().origin());
        auto _new_origin = vec2(_nv.x(), _nv.y() + _v.y());
        this->from_space_to(space::normalized_screen).from_base_to_space().transform_point(_new_origin);
        nodes[i]->transform(nodes[i]->transform().moved(_new_origin));
    }
}

void node_container::ver_align_by_bottom_edge(const std::vector<node *> &nodes){
    assert(nodes.size() != 0);
    auto _bs = calc_bounds(nodes);
    auto _vs = vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    for (auto& _rc : _bs){
        if(_rc.bottom_left().y() < _vs.y()){
            _vs.x(_rc.bottom_left().x());
            _vs.y(_rc.bottom_left().y());
        }
    }
    for (size_t i = 0; i < nodes.size(); i++) {
        auto _v = _vs - _bs[i].bottom_left();
        auto _nv = this->from_space_to(space::normalized_screen).from_space_to_base().transformed_point(nodes[i]->transform().origin());
        auto _new_origin = vec2(_nv.x(), _nv.y() + _v.y());
        this->from_space_to(space::normalized_screen).from_base_to_space().transform_point(_new_origin);
        nodes[i]->transform(nodes[i]->transform().moved(_new_origin));
    }
}

void node_container::unblock_all_children(){
    for (auto _n : *this){
        _n->blocked(false);
        _n->unblock_all_children();
    }
}

void node_container::describe_type() {
    start_type<node_container>([]() { return nullptr; });
    if(renderable()){
        begin_private_properties();
        boolean_property<node_container>(u"rb::in_editor_hidden", u"In Editor Hidden", true, {
            [](const node_container* site){
                return site->in_editor_hidden();
            },
            [](node_container* site, bool value){
                site->in_editor_hidden(value);
            }
        });
        end_private_properties();
    }
    begin_private_properties();
    boolean_property<node_container>(u"rb::selected", u"Selected", false, {
        [](const node_container* site){
            if(site->parent() && dynamic_cast<const node*>(site))
                return site->parent()->is_selected(dynamic_cast<const node*>(site));
            else
                return false;
        },
        [](node_container* site, bool value){
            site->_selected = value;
        }
    });
    end_private_properties();
    end_type();
}

void node_container::was_deserialized(){
    if(this->parent() && director::in_editor() && _selected.has_value()){
        if(_selected.value())
            this->parent()->add_to_selection(dynamic_cast<node*>(this));
        else
            this->parent()->remove_from_selection(dynamic_cast<node*>(this));
    }
    
    if(dynamic_cast<node*>(this)){
        auto _n = dynamic_cast<node*>(this);
        if(_n->name() != u"")
            _n->parent_scene()->add_named_node(_n->name(), _n);
        
        if(_n->classes() != u"")
            _n->parent_scene()->add_node_with_class(_n->classes(), _n);
    }
}

bool node_container::in_editor_hidden() const {
    return _in_editor_hidden;
}

const node* node_container::node_at(uint32_t at) const {
    assert(at >= 0 && at < _child_nodes.size());
    return _child_nodes[at];
}

node* node_container::node_at(uint32_t at) {
    assert(at >= 0 && at < _child_nodes.size());
    return _child_nodes[at];
}

bool node_container::in_editor_hidden(const bool value){
    assert(renderable());
    if(_in_editor_hidden == value)
        return _in_editor_hidden;
    _in_editor_hidden = value;
    if(parent_scene() && parent_scene()->active() && !parent_scene()->playing() && parent_scene()->in_editor()){
        if (dynamic_cast<layer*>(this)){
            dynamic_cast<layer*>(this)->invalidate_buffers();
        }
        else {
            auto _current = const_cast<node_container*>(this->parent_node_container());
            while(!dynamic_cast<layer*>(_current)){
                _current = const_cast<node_container*>(_current->parent_node_container());
            }
            
            dynamic_cast<layer*>(_current)->invalidate_buffers();
        }
    }
    if(_in_editor_hidden && parent_node_container())
        const_cast<node_container*>(parent_node_container())->remove_from_selection(dynamic_cast<node*>(this));
    if(_in_editor_hidden && parent_scene() && parent_scene()->current() == this){
        const_cast<scene*>(parent_scene())->current(const_cast<node_container*>(parent_node_container()));
    }
    return _in_editor_hidden;
}

bool node_container::is_hidden_in_editor() const {
    if(parent_scene()) {
        if(!parent_scene()->playing() && parent_scene()->in_editor()){
            return in_editor_hidden();
        }
        else
            return false;
    }
    else
        return false;
}

void node_container::unhide_all_children() {
    for (auto _n : *this){
        _n->in_editor_hidden(false);
        _n->unhide_all_children();
    }
}

bool node_container::is_degenerated() const {
    auto _t = transform();
    return !_t.test_direction(transform_direction::from_base_to_space) || !_t.test_direction(transform_direction::from_space_to_base);
}

void node_container::test_degeneration(std::vector<node *> &nodes){
    if (dynamic_cast<node*>(this) && this->is_degenerated())
        nodes.push_back(dynamic_cast<node*>(this));
    
    for (auto _n : *this){
        _n->test_degeneration(nodes);
    }
}







































