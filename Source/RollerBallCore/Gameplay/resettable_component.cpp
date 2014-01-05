//
//  resettable_component.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 05/01/14.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#include "resettable_component.h"
#include "scene.h"
#include "layer.h"
#include "node.h"

using namespace rb;

void resettable_component::reset_component(){
    
}

void resettable_component::reset_component(rb::node *n){
    auto _rc = dynamic_cast<resettable_component*>(n);
    if(_rc)
        _rc->reset_component();
    for (auto _cn : *n){
        reset_component(_cn);
    }
}

void resettable_component::reset_components(scene* s){
    assert(s);
    for (uint32_t i = 0; i < MAX_LAYERS; i++) {
        auto _l = s->layer(i);
        for (auto _n : *_l){
            reset_component(_n);
        }
    }
}

resettable_component::~resettable_component(){
    
}


















