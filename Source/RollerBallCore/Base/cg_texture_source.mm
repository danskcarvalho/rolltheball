//
//  cg_texture_source.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 22/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "cg_texture_source.h"
using namespace rb;

cg_texture_source::cg_texture_source(NSURL* path){
    assert(path);
    this->_path_ref = path;
    auto _utf8_str = [[path path] cStringUsingEncoding:NSUTF8StringEncoding];
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> _str_converter;
    this->_path_str = _str_converter.from_bytes(_utf8_str);
}
const URL_TYPE cg_texture_source::image_url() const{
    return this->_path_ref;
}
rb_string cg_texture_source::to_string() const{
    return _path_str;
}

cg_texture_source::~cg_texture_source(){
}

cg_texture_source::cg_texture_source(){
    
}

texture_source* cg_texture_source::duplicate() const {
    cg_texture_source* _texture = new cg_texture_source();
    _texture->_path_ref = this->_path_ref;
    _texture->_path_str = this->_path_str;
    return _texture;
}
