//
//  misc_classes_2.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 04/08/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#include "misc_classes_2.h"
#include "node.h"
#include "director.h"
#include "editor_delegate.h"
#include "scene.h"

using namespace rb;

component_id::component_id(){
    _name = u"";
    _classes = u"";
    named_node = nullptr;
    _internal = nullptr;
}

const rb_string& component_id::name(const rb_string &value){
    if(named_node && named_node->parent_scene())
        named_node->parent_scene()->remove_named_node(named_node);
    _name = value;
    if(named_node && named_node->parent_scene())
        named_node->parent_scene()->add_named_node(_name, named_node);
    notify_property_changed(u"name");
    if(director::editor_delegate() && named_node)
        director::editor_delegate()->name_changed(named_node);
    return _name;
}

const rb_string& component_id::classes(const rb_string& value){
    if(named_node && named_node->parent_scene())
        named_node->parent_scene()->remove_node_with_class(named_node);
    _classes = value;
    if(named_node && named_node->parent_scene())
        named_node->parent_scene()->add_node_with_class(_classes, named_node);
    notify_property_changed(u"classes");
    return _classes;
}

void component_id::describe_type(){
    start_type<component_id>([](){ return new component_id(); });
    string_property<component_id>(u"name", u"Name", true, false, {
        [](const component_id* site){
            return site->_name;
        },
        [](component_id* site, const rb_string& value){
            site->name(value);
        }
    });
    string_property<component_id>(u"classes", u"Classes", true, false, {
        [](const component_id* site){
            return site->_classes;
        },
        [](component_id* site, const rb_string& value){
            site->classes(value);
        }
    });
    end_type();
}

rb_string component_id::type_name() const {
    return u"rb::component_id";
}

rb_string component_id::displayable_type_name() const {
    return u"Identification";
}

transformation_values::transformation_values(){
    _orthogonal = true;
    _uniform = false;
    transformed_node = nullptr;
}

void transformation_values::sync_values(){
    if(transformed_node){
        transformed_node->old_transform(transform_space(translation(), scale(), vec2(TO_RADIANS(rotation().x()), TO_RADIANS(rotation().y()))));
    }
}

void transformation_values::describe_type(){
    start_type<transformation_values>([](){ return new transformation_values(); });
    vec2_property<transformation_values>(u"translation", u"Translation", true, {
        [](const transformation_values* site){
            return site->_translation;
        },
        [](transformation_values* site, const vec2& value){
            site->translation(value);
            if(site->transformed_node)
                site->transformed_node->old_transform(site->transformed_node->old_transform().moved(value));
        }
    });
    vec2_property<transformation_values>(u"scale", u"Scale", true, {
        [](const transformation_values* site){
            return site->_scale;
        },
        [](transformation_values* site, const vec2& value){
            if(!site->_uniform){
                if(site->transformed_node)
                    site->transformed_node->old_transform(site->transformed_node->old_transform().scaled(value));
                site->scale(value);
            }
            else {
                if(site->transformed_node)
                    site->transformed_node->old_transform(site->transformed_node->old_transform().scaled(value.x(), value.x()));
                site->scale(vec2(value.x(), value.x()));
                site->force_notify_property_changed(u"scale");
            }
        }
    });
    boolean_property<transformation_values>(u"uniform", u"Uniform", true, {
        [](const transformation_values* site){
            return site->_uniform;
        },
        [](transformation_values* site, const bool& value){
            site->uniform(value);
            if(site->transformed_node){
                auto _t = site->transformed_node->old_transform();
                if(site->uniform())
                    site->transformed_node->old_transform(site->transformed_node->old_transform().scaled(_t.scale().x(), _t.scale().x()));
            }
        }
    });
    vec2_property<transformation_values>(u"rotation", u"Rotation", true, {
        [](const transformation_values* site){
            return site->_rotation;
        },
        [](transformation_values* site, const vec2& value){
            if(!site->_orthogonal){
                if(site->transformed_node)
                    site->transformed_node->old_transform(site->transformed_node->old_transform().rotated(TO_RADIANS(value.x()), TO_RADIANS(value.y())));
                site->rotation(value);
            }
            else {
                if(site->transformed_node)
                    site->transformed_node->old_transform(site->transformed_node->old_transform().rotated(TO_RADIANS(value.x()), TO_RADIANS(value.x() + 90)));
                site->rotation(vec2(value.x(), value.x() + 90));
            }
        }
    });
    boolean_property<transformation_values>(u"orthogonal", u"Orthogonal", true, {
        [](const transformation_values* site){
            return site->_orthogonal;
        },
        [](transformation_values* site, const bool& value){
            site->orthogonal(value);
            if(site->transformed_node){
                auto _t = site->transformed_node->old_transform();
                if(site->orthogonal())
                    site->transformed_node->old_transform(site->transformed_node->old_transform().rotated(_t.rotation().x(), _t.rotation().x() + (M_PI / 2.0f)));
            }
        }
    });
    end_type();
}

rb_string transformation_values::type_name() const {
    return u"rb::transformation_values";
}

rb_string transformation_values::displayable_type_name() const {
    return u"Transformation";
}
