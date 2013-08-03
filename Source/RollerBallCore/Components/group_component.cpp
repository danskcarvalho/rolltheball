//
//  group_component.cpp
//  RollerBallCore
//
//  Created by Danilo Santos de Carvalho on 28/05/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#include "transform_gizmo.h"
#include "group_component.h"
#include "layer.h"

using namespace rb;

group_component::group_component(){
    _copy_node = false;
}

group_component::~group_component(){
    
}

void group_component::describe_type(){
    node::describe_type();
    start_type<group_component>([]() { return new group_component(); });
    begin_private_properties();
    
    boolean_property<group_component>(u"copy_node", u"Copy Node", false, {
        [](const group_component* site){
            return site->_copy_node;
        },
        [](group_component* site, bool value){
            site->_copy_node = value;
        }
    });
    
    end_private_properties();
    end_type();
}

rb_string group_component::type_name() const {
    return u"rb::group_component";
}

rb_string group_component::displayable_type_name() const {
    return u"Group";
}

void group_component::ungroup(bool select_ungrouped, bool delete_self){
    std::vector<node*> _children;
    copy_nodes_to_vector(_children);
    
    if (parent_node())
        parent_node()->clear_selection();
    else
        parent_layer()->clear_selection();
    
    for (auto _n : _children){
        auto _new_t = this->transform() * _n->transform();
        this->remove_node(_n, false);
        if (parent_node())
            parent_node()->add_node(_n);
        else
            parent_layer()->add_node(_n);
        _n->transform(_new_t);
        if(select_ungrouped)
            _n->add_to_selection();
    }
    
    if (parent_node())
        parent_node()->remove_node(this, delete_self);
    else
        parent_layer()->remove_node(this, delete_self);
}

std::vector<rb_string> group_component::transformables() {
    return { u"Adjust Group's Transform" };
}

void group_component::start_transformation(long index){
    transform_gizmo::start_transformation(this->parent(), this->transform(), rectangle(0, 0, 1, 1), false, [=](transform_gizmo* g, const transform_space& t){
        this->adjust_transformation(t, false);
    });
}

