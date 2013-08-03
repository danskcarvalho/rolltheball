//
//  dynamic_texture.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 06/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "dynamic_texture.h"
#include "cg_resource.h"
#include <CoreGraphics/CoreGraphics.h>
#include <ImageIO/ImageIO.h>

using namespace rb;

dynamic_texture::dynamic_texture(){
    
}

uint8_t* dynamic_texture::create_image_data(const std::function<void (void* ctx)>& f, bool avoid_gl){
    vec2 _total_size = this->_texture_size + (_border == texture_border::none ? vec2::zero : vec2(2 * TEXTURE_BORDER, 2 * TEXTURE_BORDER));
    
    uint8_t* myData = new uint8_t[(uint32_t)_total_size.x() * 4 * (uint32_t)_total_size.y()];
    
    if(!myData){
        ERROR("failed to alloc memory...");
        return nullptr;
    }
    
    cg_resource space(CGColorSpaceCreateDeviceRGB());
    cg_resource myBitmapContext(CGBitmapContextCreate (myData,
                                                       _total_size.x(), _total_size.y(), 8,
                                                       _total_size.x() * 4, (CGColorSpaceRef)space.resource(),
                                                       kCGBitmapByteOrder32Big |
                                                       kCGImageAlphaPremultipliedLast));
    
    if(!myBitmapContext.resource())
    {
        ERROR("failed to create bitmap context...");
        delete [] myData;
        return nullptr;
    }
    
    CGContextSetBlendMode((CGContextRef)myBitmapContext.resource(), kCGBlendModeCopy);
    
    if(_border == texture_border::empty){
        CGContextSaveGState((CGContextRef)myBitmapContext.resource());
        CGContextScaleCTM((CGContextRef)myBitmapContext.resource(), 1, -1);
        CGContextTranslateCTM((CGContextRef)myBitmapContext.resource(), TEXTURE_BORDER, -_total_size.y() + TEXTURE_BORDER); //we shift by one to account for the border..
    }
    else {
        CGContextScaleCTM((CGContextRef)myBitmapContext.resource(), 1, -1);
        CGContextTranslateCTM((CGContextRef)myBitmapContext.resource(), 0, -_total_size.y());
    }
    
    CGContextSaveGState((CGContextRef)myBitmapContext.resource());
    f((CGContextRef)myBitmapContext.resource());
    CGContextRestoreGState((CGContextRef)myBitmapContext.resource());
    
    if(_border == texture_border::empty){
        CGContextRestoreGState((CGContextRef)myBitmapContext.resource());
        CGContextScaleCTM((CGContextRef)myBitmapContext.resource(), 1, -1);
        CGContextTranslateCTM((CGContextRef)myBitmapContext.resource(), 0, -_total_size.y());
    }
    
    CGRect low_left_corner_r = {{0, 0}, {TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect low_right_corner_r = {{_total_size.x() - TEXTURE_BORDER, 0}, {TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect up_left_corner_r = {{0, _total_size.y() - TEXTURE_BORDER}, {TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect up_right_corner_r = {{_total_size.x() - TEXTURE_BORDER, _total_size.y() - TEXTURE_BORDER}, {TEXTURE_BORDER, TEXTURE_BORDER}};
    //edges
    CGRect low_edge_r = {{TEXTURE_BORDER, 0}, {_total_size.x() - 2 * TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect up_edge_r = {{TEXTURE_BORDER, _total_size.y() - TEXTURE_BORDER}, {_total_size.x() - 2 * TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect left_edge_r = {{0, TEXTURE_BORDER}, {TEXTURE_BORDER, _total_size.y() - 2 * TEXTURE_BORDER}};
    CGRect right_edge_r = {{_total_size.x() - TEXTURE_BORDER, TEXTURE_BORDER}, {TEXTURE_BORDER, _total_size.y() - 2 * TEXTURE_BORDER}};
    
    if(_border == texture_border::empty){
        CGContextSetRGBFillColor((CGContextRef)myBitmapContext.resource(), 0, 0, 0, 0);
        CGContextFillRect((CGContextRef)myBitmapContext.resource(), low_left_corner_r);
        CGContextFillRect((CGContextRef)myBitmapContext.resource(), low_right_corner_r);
        CGContextFillRect((CGContextRef)myBitmapContext.resource(), up_left_corner_r);
        CGContextFillRect((CGContextRef)myBitmapContext.resource(), up_right_corner_r);
        
        CGContextFillRect((CGContextRef)myBitmapContext.resource(), low_edge_r);
        CGContextFillRect((CGContextRef)myBitmapContext.resource(), up_edge_r);
        CGContextFillRect((CGContextRef)myBitmapContext.resource(), left_edge_r);
        CGContextFillRect((CGContextRef)myBitmapContext.resource(), right_edge_r);
    }
    
    if(avoid_gl) {
        _cg_image = CGBitmapContextCreateImage((CGContextRef)myBitmapContext.resource());
        delete [] myData;
        return nullptr;
    }
    else {
        _cg_image = nullptr;
        return myData;
    }
}
void dynamic_texture::setup_texture(uint8_t* image){
    vec2 _total_size = this->_texture_size + (_border == texture_border::none ? vec2::zero : vec2(2 * TEXTURE_BORDER, 2 * TEXTURE_BORDER));
    
    GLuint myTextureName;
    while(glGetError() != GL_NO_ERROR); //clear error flags
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &myTextureName);
    if(!myTextureName){
        ERROR("failed to create texture...");
        delete [] image;
        return;
    }
    glBindTexture(GL_TEXTURE_2D, myTextureName);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    if(glGetError() != GL_NO_ERROR){
        ERROR("error setting texture min filter to linear...");
        glDeleteTextures(1, &myTextureName);
        delete [] image;
        return;
    }
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if(glGetError() != GL_NO_ERROR){
        ERROR("error setting texture mag filter to linear...");
        glDeleteTextures(1, &myTextureName);
        delete [] image;
        return;
    }
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    if(glGetError() != GL_NO_ERROR){
        ERROR("error setting texture wrap s filter to clamp to edge...");
        glDeleteTextures(1, &myTextureName);
        delete [] image;
        return;
    }
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    if(glGetError() != GL_NO_ERROR){
        ERROR("error setting texture wrap t filter to clamp to edge...");
        glDeleteTextures(1, &myTextureName);
        delete [] image;
        return;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)_total_size.x(), (GLsizei)_total_size.y(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    if(glGetError() != GL_NO_ERROR){
        ERROR("error uploading texture data...");
        glDeleteTextures(1, &myTextureName);
        delete [] image;
        return;
    }
    this->_texture_id = myTextureName;
    delete [] image;
}

dynamic_texture::dynamic_texture(const vec2& size, const texture_border border, const std::function<void (void* ctx)>& f, const bool avoid_gl) : texture() {
    assert(size.x() > 0);
    assert(size.y() > 0);
    assert(border == texture_border::empty || border == texture_border::none);
    
    vec2 _size = size;
    _size.x(roundf(_size.x()));
    _size.y(roundf(_size.y()));
    
    if(_size.x() < 1)
        _size.x(1);
    
    if(_size.y() < 1)
        _size.y(1);
    
    vec2 _total_size = _size + (_border == texture_border::none ? vec2::zero : vec2(2 * TEXTURE_BORDER, 2 * TEXTURE_BORDER));
    
    this->_border = border;
    this->_bounds = rectangle(0.5, 0.5, _size.x() / _total_size.x(), _size.y() / _total_size.y());
    this->_original_size = _size;
    this->_texture_size = _size;
    
    _cg_image = nullptr;
    auto image = create_image_data(f, avoid_gl);
    if(!avoid_gl)
        setup_texture(image);
}

void* dynamic_texture::to_cg_image() const{
    if(_cg_image){
        CGImageRetain((CGImageRef)this->_cg_image);
        return this->_cg_image;
    }
    else
        return this->texture::to_cg_image();
}

dynamic_texture::~dynamic_texture(){
    if(_cg_image)
        CGImageRelease((CGImageRef)_cg_image);
    _cg_image = nullptr;
}