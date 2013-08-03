//
//  nvnode.cpp
//  RollerBallCore
//
//  Created by Danilo Santos de Carvalho on 20/05/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#include "nvnode.h"

using namespace rb;

nvnode::~nvnode(){
}

bool nvnode::renderable() const {
    return false;
}

void nvnode::describe_type(){
    node::describe_type();
    start_type<nvnode>([]() { return new nvnode(); });
    end_type();
}

rb_string nvnode::type_name() const {
    return u"rb::nvnode";
}

rb_string nvnode::displayable_type_name() const {
    return u"Non Visual Node";
}

bool nvnode::add_node_at(rb::node *n, uint32_t at){
    return false;
}