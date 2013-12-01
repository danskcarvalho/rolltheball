//
//  director.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 07/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "director.h"
#include "scene.h"
#include "editor_delegate.h"
#include "sprite_component.h"
#include "misc_classes_2.h"
#include "group_component.h"
#include "layer.h"
#include "polygon_component.h"
#include "physics_engine.h"
#include "main_character.h"
#include "physics_shape.h"
#include "destructible_sprite_component.h"
#import <Foundation/Foundation.h>

using namespace rb;

scene* director::_active_scene = nullptr;
editor_delegate* director::_delegate = nullptr;
std::unordered_set<type_descriptor*> director::_registered_creatable_types;
bool director::_in_editor = false;

bool director::in_editor() {
    return _in_editor;
}

bool director::in_editor(bool value){
    _in_editor = value;
    return _in_editor;
}

scene* director::active_scene() {
    return _active_scene;
}

void director::register_creatable_types(){
    register_creatable_type(type_descriptor::get<sprite_component>());
    register_creatable_type(type_descriptor::get<destructible_sprite_component>());
    register_creatable_type(type_descriptor::get<polygon_component>());
    
    //gameplay
    register_creatable_type(type_descriptor::get<physics_engine>());
    register_creatable_type(type_descriptor::get<main_character>());
    register_creatable_type(type_descriptor::get<physics_shape>());
    
    //register serializable types
    register_serializable_type<scene>();
    register_serializable_type<layer>();
    register_serializable_type<sprite_component>();
    register_serializable_type<destructible_sprite_component>();
    register_serializable_type<component_id>();
    register_serializable_type<transformation_values>();
    register_serializable_type<group_component>();
    register_serializable_type<polygon_point_component>();
}

void director::active_scene(rb::scene *scene, bool cleanup){
    register_creatable_types();
    
    if(scene == _active_scene)
        return;
    
    if(scene)
        assert(!scene->_playing);
    
    if(_active_scene){
        assert(!_active_scene->_playing);
        _active_scene->before_becoming_inactive();
    }
    
    rb::scene* _previous = _active_scene;
    
    if(scene)
        scene->before_becoming_active();
    
    _active_scene = scene;
    
    if(scene)
        scene->after_becoming_active();
    
    if(_previous)
        _previous->after_becoming_inactive();
    
    if(cleanup)
        delete _previous;
}

responder* director::active_responder(){
    if(!_active_scene)
        return nullptr;
    return _active_scene->get_responder();
}

double director::ticks(){
    return [NSDate timeIntervalSinceReferenceDate];
}

void director::editor_delegate(class editor_delegate *delegate){
    _delegate = delegate;
}

class editor_delegate* director::editor_delegate() {
    return _delegate;
}

void director::register_creatable_type(const rb::type_descriptor *td){
    if(_registered_creatable_types.count(const_cast<type_descriptor*>(td)))
        return;
    
    _registered_creatable_types.insert(const_cast<type_descriptor*>(td));
    if(_delegate)
        _delegate->creatable_type_was_registered(td);
}

void director::fill_with_registered_creatable_types(std::vector<type_descriptor *> &list){
    list.clear();
    for (auto _td : _registered_creatable_types){
        list.push_back(const_cast<type_descriptor*>(_td));
    }
}

bool director::is_registered(const rb::type_descriptor *td){
    return _registered_creatable_types.count(const_cast<type_descriptor*>(td)) != 0;
}

void director::edit_object_properties(rb::typed_object *obj, const rb_string &title){
    assert(obj);
    if(_delegate)
        _delegate->push_object_in_property_inspector(obj, title);
}








