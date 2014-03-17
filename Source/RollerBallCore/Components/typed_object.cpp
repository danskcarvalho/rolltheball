//
//  typed_object.cpp
//  RollerBallCore
//
//  Created by Danilo Santos de Carvalho on 27/02/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#define _OBJC_BRIDGE_
#include "typed_object.h"
#include "observer.h"
#import "CppBridgeProtocol.h"
#include "generic_object.h"
#include <boost/lexical_cast.hpp>
#include "scene_loader.h"

inline NSString* to_platform_string(const rb::rb_string& str){
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> _str_converter;
    auto _u8Str = _str_converter.to_bytes(str);
    NSString* _platform_str = [NSString stringWithUTF8String:_u8Str.c_str()];
    return _platform_str;
}

using namespace rb;

std::set<rb_string> typed_object::_types_being_described;

typed_object::typed_object(){
    _cpp_bridge = nullptr;
    _current_type_constr = nullptr;
    _cached_type_descriptor = nullptr;
    _cached_nullable_type_descriptor = nullptr;
    _cached_vector_type_descriptor = nullptr;
    _update_silently = false;
}

typed_object::~typed_object(){
    auto _copy_observers(_observers); //i make a copy because i think this is not going to be a problem...
    for (auto _o : _copy_observers)
        _o->object_deleted(this);
    if(_cpp_bridge){
        [_cpp_bridge notifyObserversOfDeletedObject];
    }
    
    _cpp_bridge = nullptr;
    if(_current_type_constr)
        delete _current_type_constr;
    for(auto _td : _stack_inheritance){
        delete _td;
    }
    
    if(dynamic_type()){
        if(_cached_type_descriptor)
            delete _cached_type_descriptor;
        
        if(_cached_nullable_type_descriptor)
            delete _cached_nullable_type_descriptor;
        
        if(_cached_vector_type_descriptor)
            delete _cached_vector_type_descriptor;
        
        for(auto _at : _associated_types){
            delete _at;
        }
    }
}

bool typed_object::dynamic_type() const {
    return false;
}

void typed_object::start_branch(const rb_string &name){
    assert(_current_type_constr);
    assert(_current_type_constr->stage == tsStartedBranchType);
    class type_descriptor* _branch = new class type_descriptor();
    _branch->_display_name = displayable_type_name() + rb_string(u"::branch_") + rb::to_string(_current_type_constr->cached_type_descriptor->_union_map.size());
    _branch->_name = type_name() + rb_string(u"::branch_") + rb::to_string(_current_type_constr->cached_type_descriptor->_union_map.size());
    _branch->_gtd = general_type_descriptor::object;
    _current_type_constr->stage = tsStartedBranch;
    _current_type_constr->cached_type_descriptor->_union_map[name] = _branch;
    _current_type_constr->current_branch = _branch;
}

void typed_object::end_branch(){
    assert(_current_type_constr);
    assert(_current_type_constr->stage == tsStartedBranch);
    _current_type_constr->stage = tsStartedBranchType;
    _current_type_constr->current_branch = nullptr;
}

void typed_object::end_type(){
    assert(_current_type_constr);
    assert(_current_type_constr->stage == tsStartedType || _current_type_constr->stage == tsStartedBranchType);
    _stack_inheritance.push_back(_current_type_constr->cached_type_descriptor);
    delete _current_type_constr;
    _current_type_constr = nullptr;
    _types_being_described.erase(type_name());
}

void typed_object::create_type_descriptor(){
    describe_type();
    assert(_stack_inheritance.size() != 0);
    assert(std::all_of(_stack_inheritance.begin(), _stack_inheritance.end(), [](class type_descriptor* td){ return td->gtd() == general_type_descriptor::object; }) || _stack_inheritance.size() == 1);
    assert(!(_stack_inheritance.back()->_gtd == general_type_descriptor::branch) || (_stack_inheritance.back()->_properties.size() == 0));
    
    class type_descriptor   *_td = new class type_descriptor(),
                            *_n_td = new class type_descriptor(),
                            *_v_td = new class type_descriptor();
    _td->_constructor = _n_td->_constructor = _stack_inheritance.back()->_constructor;
    
    _td->_dynamic_type = _n_td->_dynamic_type = dynamic_type();
    _td->_display_name = _n_td->_display_name = _stack_inheritance.back()->_display_name;
    _v_td->_display_name = _td->_display_name + rb_string(u" Vector");
    
    _n_td->_element_type = _td->_element_type = nullptr;
    _v_td->_element_type = _td;
    
    _td->_gtd = _n_td->_gtd = _stack_inheritance.back()->_gtd;
    _v_td->_gtd = general_type_descriptor::vector;
    
    _td->_name = _stack_inheritance.back()->_name;
    _n_td->_name = rb_string(u"_%_nullable_") + _td->_name;
    _v_td->_name = rb_string(u"_%_vector_") + _td->_name;
    
    _td->_nullable = false;
    _n_td->_nullable = true;
    _v_td->_nullable = false;
    
    _td->_selector_name = _stack_inheritance.back()->_selector_name;
    _n_td->_selector_name = _td->_selector_name;
    
    _td->_union_map = _n_td->_union_map = _stack_inheritance.back()->_union_map;
    
    for (auto _t : _stack_inheritance){
        for (auto _p : _t->_properties_decl_order){
            _td->_properties_decl_order.push_back(_p);
            _n_td->_properties_decl_order.push_back(_p);
            _td->_properties[_p] = _t->_properties[_p];
            _n_td->_properties[_p] = _t->_properties[_p];
        }
    }
    if(!dynamic_type()){
        type_descriptor::register_type(_td);
        type_descriptor::register_type(_n_td);
        type_descriptor::register_type(_v_td);
    }
    _cached_type_descriptor = _td;
    _cached_nullable_type_descriptor = _n_td;
    _cached_vector_type_descriptor = _v_td;
    for (auto _t : _stack_inheritance)
        delete _t;
    _stack_inheritance.clear();
}

void typed_object::begin_private_properties(){
    assert(_current_type_constr);
    _current_type_constr->private_properties++;
}

void typed_object::end_private_properties(){
    assert(_current_type_constr);
    assert(_current_type_constr->private_properties > 0);
    _current_type_constr->private_properties--;
}

const class type_descriptor* typed_object::type_descriptor() const {
    if(_cached_type_descriptor)
        return _cached_type_descriptor;
    
    const_cast<typed_object*>(this)->_cached_type_descriptor = const_cast<class type_descriptor*>(type_descriptor::with_name(type_name()));
    
    if(_cached_type_descriptor)
        return _cached_type_descriptor;
    
    const_cast<typed_object*>(this)->create_type_descriptor();
    
    assert(_cached_type_descriptor);
    return _cached_type_descriptor;
}

const class type_descriptor* typed_object::nullable_type_descriptor() const {
    if(_cached_nullable_type_descriptor)
        return _cached_nullable_type_descriptor;
    
    const_cast<typed_object*>(this)->_cached_nullable_type_descriptor = const_cast<class type_descriptor*>(type_descriptor::with_name(rb_string(u"_%_nullable_") + type_name()));
    
    if(_cached_nullable_type_descriptor)
        return _cached_nullable_type_descriptor;
    
    const_cast<typed_object*>(this)->create_type_descriptor();
    
    assert(_cached_nullable_type_descriptor);
    return _cached_nullable_type_descriptor;
}

void typed_object::free_old_object(const rb::typed_object *old_obj, const rb::typed_object *new_obj){
    if(old_obj == new_obj)
        return;
    
    delete old_obj;
}

const class type_descriptor* typed_object::vector_type_descriptor() const {
    if(_cached_vector_type_descriptor)
        return _cached_vector_type_descriptor;
    
    const_cast<typed_object*>(this)->_cached_vector_type_descriptor = const_cast<class type_descriptor*>(type_descriptor::with_name(rb_string(u"_%_vector_") + type_name()));
    
    if(_cached_vector_type_descriptor)
        return _cached_vector_type_descriptor;
    
    const_cast<typed_object*>(this)->create_type_descriptor();
    
    assert(_cached_vector_type_descriptor);
    return _cached_vector_type_descriptor;
}

const rb_string& typed_object::branch_tag() const {
    if(type_descriptor()->gtd() != general_type_descriptor::branch){
        const_cast<typed_object*>(this)->_cbranch_tag = rb_string(u"");
        return _cbranch_tag;
    }
    if(_cbranch_tag == u""){
        const_cast<typed_object*>(this)->_cbranch_tag = type_descriptor()->branches().begin()->first;
    }
    return _cbranch_tag;
}

const rb_string& typed_object::branch_tag(const rb_string& value) {
    assert(type_descriptor()->branches().count(value) != 0);
    _cbranch_tag = value;
    branch_tag_changed();
    if(!_update_silently){
        auto _copy_observers(_observers); //i make a copy because i think this is not going to be a problem...
        for (auto _o : _copy_observers)
            _o->branch_tag_changed(this);
        
        if(_cpp_bridge){
            [_cpp_bridge notifyObservers:nil];
        }
    }
    return _cbranch_tag;
}

void typed_object::branch_tag_changed(){
    //do nothing
}

void typed_object::shutdown_notifications(std::function<void ()> region){
    auto _previous = _update_silently;
    _update_silently = true;
    region();
    _update_silently = _previous;
}

void typed_object::notify_property_changed(const rb_string &name){
    if(!_update_silently){
        auto _copy_observers(_observers);
        for (auto _o : _copy_observers)
            _o->property_changed(this, name);
        if(_cpp_bridge){
            NSString* _p_name = to_platform_string(name);
            [_cpp_bridge notifyObservers:_p_name];
        }
    }
}

void typed_object::force_notify_property_changed(const rb_string &name){
    auto _copy_observers(_observers);
    for (auto _o : _copy_observers)
        _o->property_changed(this, name);
    
    if(_cpp_bridge){
        NSString* _p_name = to_platform_string(name);
        [_cpp_bridge notifyObservers:_p_name];
    }
}

void typed_object::notify_atlas_changed(){
    auto _td = type_descriptor();
    std::vector<rb_string> _properties;
    _td->get_all_properties(_properties);
    for (auto _p : _properties){
        auto _p_td = _td->property_type(_p);
        
        if(_p_td->gtd() == general_type_descriptor::image)
            force_notify_property_changed(_p);
        
        if(_p_td->gtd() == general_type_descriptor::object){
            typed_object* _obj = _td->get_object_property<typed_object>(this, _p);
            _obj->notify_atlas_changed();
        }
    }
}

void typed_object::add_observer(rb::observer *o){
    _observers.insert(o);
}

void typed_object::remove_observer(rb::observer *o){
    _observers.erase(o);
}

void typed_object::set_bridge(BRIGE_TYPE bridge){
    if(bridge == _cpp_bridge)
        return;
    _cpp_bridge = bridge;
}

BRIGE_TYPE typed_object::get_bridge(){
    return _cpp_bridge;
}

bool typed_object::should_serialize_children() const {
    return true;
}

typed_object* typed_object::clone(std::unordered_map<typed_object*, typed_object*>& already_cloned) const {
    if(already_cloned.count(const_cast<typed_object*>(this)))
        return already_cloned[const_cast<typed_object*>(this)];
    
    auto newT = this->type_descriptor()->new_object();
    
    already_cloned[const_cast<typed_object*>(this)] = newT;
    
    if(newT->type_descriptor()->gtd() == general_type_descriptor::branch)
        newT->branch_tag(this->branch_tag());
    
    std::vector<rb_string> _properties;
    newT->type_descriptor()->get_all_properties(_properties);
    for (auto _p : _properties){
        auto _pgtd = this->type_descriptor()->property_type(_p)->gtd();
        if(_pgtd == general_type_descriptor::action)
            continue;
        auto _pv = this->type_descriptor()->get_property(this, _p);
        
        if(_pgtd == general_type_descriptor::branch || _pgtd == general_type_descriptor::object){
            if(!_pv.is_null())
                newT->type_descriptor()->set_property(newT, _p, _pv.object_value()->clone(already_cloned));
            else
                newT->type_descriptor()->set_property(newT, _p, nullptr);
        }
        else if(_pgtd == general_type_descriptor::vector){
            auto _pvv = _pv.vector_value();
            for (size_t i = 0; i < _pvv.size(); i++) {
                if(_pvv[i])
                    _pvv[i] = _pvv[i]->clone(already_cloned);
            }
            newT->type_descriptor()->set_property(newT, _p, _pvv);
        }
        else
            newT->type_descriptor()->set_property(newT, _p, _pv);
    }
    
    return newT;
}

typed_object* typed_object::clone() const {
    std::unordered_map<typed_object*, typed_object*> _already_cloned;
    return clone(_already_cloned);
}

std::vector<typed_object*> typed_object::removed_objects(const std::vector<typed_object *> &previous, const std::vector<typed_object *> &now){
    std::set<typed_object*> _previous_s, _now_s;
    _previous_s.insert(previous.begin(), previous.end());
    _now_s.insert(now.begin(), now.end());
    
    std::vector<typed_object *> _removed;
    std::set_intersection(_previous_s.begin(), _previous_s.end(),
                          _now_s.begin(), _now_s.end(),
                          std::back_inserter(_removed));
    return _removed;
}

std::vector<typed_object*> typed_object::added_objects(const std::vector<typed_object *> &previous, const std::vector<typed_object *> &now){
    std::set<typed_object*> _previous_s, _now_s;
    _previous_s.insert(previous.begin(), previous.end());
    _now_s.insert(now.begin(), now.end());
    
    std::vector<typed_object *> _removed;
    std::set_intersection(_now_s.begin(), _now_s.end(),
                          _previous_s.begin(), _previous_s.end(),
                          std::back_inserter(_removed));
    return _removed;
}

void typed_object::free_removed_objects(const std::vector<typed_object *> &previous, const std::vector<typed_object *> &now, std::function<void (typed_object*)> deleter){
    auto _rem = removed_objects(previous, now);
    for (auto _r : _rem){
        if(deleter)
            deleter(_r);
        else
            delete _r;
    }
}

void typed_object::begin_serialization(){
    
}

void typed_object::end_serialization(){
    
}

void typed_object::was_deserialized(){
    
}

void typed_object::internal_begin_serialization(){
    begin_serialization();
    if(should_serialize_children()){
        std::vector<typed_object*> _objs;
        fill_vector_with_children(_objs);
        for (auto _n : _objs)
            _n->internal_begin_serialization();
    }
}

void typed_object::internal_end_serialization(){
    std::vector<typed_object*> _objs;
    if(should_serialize_children())
        fill_vector_with_children(_objs);
    end_serialization();
    if(should_serialize_children()){
        for (auto _n : _objs)
            _n->internal_end_serialization();
    }
}

void typed_object::internal_was_deserialized(){
    std::vector<typed_object*> _objs;
//    if(should_serialize_children())
        fill_vector_with_children(_objs);
    was_deserialized();
//    if(should_serialize_children()){
        for (auto _n : _objs)
            _n->internal_was_deserialized();
//    }
}

void typed_object::serialize_to_url(const rb_string& url){
    scene_loader::serialize_to_url(this, url);
}
rb_string typed_object::serialize_to_string(){
    return scene_loader::serialize_to_string(this);
}
typed_object* typed_object::deserialize_from_url(const rb_string& url){
    return scene_loader::deserialize_from_url(url);
}
typed_object* typed_object::deserialize_from_string(const rb_string& str){
    return scene_loader::deserialize_from_string(str);
}

void typed_object::fill_vector_with_children(std::vector<typed_object *> &children) const {
    
}

void typed_object::set_children(const std::vector<typed_object *> &children){
    
}


















