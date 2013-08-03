//
//  texture_composition.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 09/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "texture_composition.h"
#include "texture_source.h"

using namespace rb; 

texture_composition::texture_composition(const texture_source& base_texture, const float scale, const bool mask_aggressively, const std::vector<texture_layer*>& layers){
    assert(scale > 0);
    
    this->_base_texture = base_texture.duplicate();
    this->_scale = scale;
    this->_layers = layers;
    this->_mask_aggressively = mask_aggressively;
}
texture_composition::~texture_composition(){
    if(this->_base_texture)
        delete this->_base_texture;
    
    for(auto _layer : _layers)
        delete _layer;
}
