//
//  null_texture_map.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 13/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "null_texture_map.h"
#include "vec2.h"
#include "vertex.h"

using namespace rb;

null_texture_map::null_texture_map(){
    _r = rectangle(0.5, 0.5, 1, 1);
    _t = transform_space();
}

void null_texture_map::set_texture_coords(vertex& v) const {
    v.set_texture_coords(vec2(0, 0));
}

null_texture_map::~null_texture_map(){
    
}

const transform_space& null_texture_map::get_texture_space() const{
    return _t;
}
const rectangle& null_texture_map::bounds() const{
    return _r;
}