//
//  extended_static_mesh_batch.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 12/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "extended_static_mesh_batch.h"
#include "static_mesh_batch.h"

using namespace rb;

extended_static_mesh_batch::extended_static_mesh_batch(){
    _blend_mode = blend_mode::normal;
    _geom_type = geometry_type::triangle;
    _line_width = 1;
}

extended_static_mesh_batch::~extended_static_mesh_batch(){
    for (auto _b : _batches)
        delete _b;
}

void extended_static_mesh_batch::draw(){
    for (auto _b : _batches){
        _b->blend_mode(_blend_mode);
        _b->geometry_type(_geom_type);
        _b->line_width(_line_width);
        _b->draw();
    }
}