//
//  radial_gradient_texture.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 05/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "radial_gradient_texture.h"
#include "dynamic_texture.h"
#include "cg_resource.h"
#include <CoreGraphics/CoreGraphics.h>
#include <ImageIO/ImageIO.h>

using namespace rb;

void radial_gradient_texture::draw_radial_gradient(void* ctx, const float size, const class color& color){
    CGContextRef _context = (CGContextRef)ctx;
    cg_resource _color_space(CGColorSpaceCreateDeviceRGB());
    CGFloat _cc_start[] = { color.r(), color.g(), color.b(), color.a() };
    CGFloat _cc_end[] = { color.r(), color.g(), color.b(), 0 };
    CGColorRef _c_start = CGColorCreate((CGColorSpaceRef)_color_space.resource(), _cc_start);
    CGColorRef _c_end = CGColorCreate((CGColorSpaceRef)_color_space.resource(), _cc_end);
    CGColorRef _colors[] = { _c_start, _c_end };
    CFArrayRef _color_array = CFArrayCreate(NULL, (const void**)_colors, 2, &kCFTypeArrayCallBacks);
    CGGradientRef _gradient = CGGradientCreateWithColors((CGColorSpaceRef)_color_space.resource(), _color_array, NULL);
    CFRelease(_color_array);
    CFRelease(_c_end);
    CFRelease(_c_start);
    
    CGPoint _center = CGPointMake(size / 2, size / 2);
    float _radius = size / 2;
    
    CGContextSetInterpolationQuality(_context, kCGInterpolationHigh);
    CGContextDrawRadialGradient(_context, _gradient, _center, 0, _center, _radius, kCGGradientDrawsAfterEndLocation);
    CFRelease(_gradient);
}

radial_gradient_texture::radial_gradient_texture(const float size, const class color& color, const bool avoid_gl) : texture() {
    float _size = size;
    assert(_size > 0);
    _size = roundf(_size);
    if(_size < 1)
        _size = 1;
    
    _texture = new dynamic_texture(vec2(_size, _size), texture_border::empty, [&](void* ctx) {
        draw_radial_gradient(ctx, _size, color);
    }, avoid_gl);
    
    this->_border = texture_border::empty;
    this->_bounds = _texture->bounds();
    this->_original_size = vec2(_size, _size);
    this->_texture_size = this->_original_size;
    this->_texture_id = _texture->texture_id();
}

radial_gradient_texture::~radial_gradient_texture(){
    delete _texture;
}

void* radial_gradient_texture::to_cg_image() const {
    return _texture->to_cg_image();
}