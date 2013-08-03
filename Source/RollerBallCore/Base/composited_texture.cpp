//
//  composited_texture.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 09/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//
#define ROUND_UP(N, S) ((((N) + (S) - 1) / (S)) * (S))

#include "composited_texture.h"
#include "texture_composition.h"
#include "vec2.h"
#include "cg_resource.h"
#include "color.h"
#include "texture_source.h"
#include <CoreGraphics/CoreGraphics.h>
#include <ImageIO/ImageIO.h>
#include <boost/shared_array.hpp>

using namespace rb;

void* composited_texture::create_image(const texture_composition* composition){
    CGImageRef _blank_image = (CGImageRef)create_blank_image();
    
    if(!_blank_image){
        ERROR("failed to create 10x10 image...");
        return nullptr;
    }
    
    cg_resource myImageSourceRef(CGImageSourceCreateWithURL((__bridge CFURLRef)composition->base_texture()->image_url(), NULL));
    if (!myImageSourceRef.resource()) {
        ERROR("failed loading texture reference: ", composition->base_texture());
        CGImageRelease(_blank_image);
        return false;
    }
    cg_resource myImageRef(CGImageSourceCreateImageAtIndex((CGImageSourceRef)myImageSourceRef.resource(), 0, NULL));
    if(!myImageRef.resource()){
        ERROR("failed loading image:", composition->base_texture());
        CGImageRelease(_blank_image);
        return false;
    }
    
    size_t width = CGImageGetWidth((CGImageRef)myImageRef.resource());
    size_t height = CGImageGetHeight((CGImageRef)myImageRef.resource());
    vec2 _original_size;
    vec2 _texture_size;
    _original_size = vec2(width, height);
    _texture_size = _original_size * composition->scale();
    _texture_size.x(roundf(_texture_size.x()));
    _texture_size.y(roundf(_texture_size.y()));
    if(_texture_size.x() < 3)
        _texture_size.x(3);
    if(_texture_size.y() < 3)
        _texture_size.y(3);
    
    int alphaStrideLength = (int)ROUND_UP(width * 1, 4);
    int destStrideLength = _texture_size.x() * 4;
    boost::shared_array<uint8_t> alphaData(new uint8_t[alphaStrideLength * (size_t)_texture_size.y()]);
    boost::shared_array<uint8_t> imageData(new uint8_t[destStrideLength * (size_t)_texture_size.y()]);
    
    cg_resource alphaOnlyContext(CGBitmapContextCreate(alphaData.get(),
                                                          _texture_size.x(),
                                                          _texture_size.y(),
                                                          8,
                                                          alphaStrideLength,
                                                          NULL,
                                                          kCGImageAlphaOnly));
    
    cg_resource space(CGColorSpaceCreateDeviceRGB());
    cg_resource myBitmapContext(CGBitmapContextCreate (imageData.get(),
                                                       _texture_size.x(), _texture_size.y(), 8,
                                                       destStrideLength, (CGColorSpaceRef)space.resource(),
                                                       kCGBitmapByteOrder32Big |
                                                       kCGImageAlphaPremultipliedLast));
    
    if(!myBitmapContext.resource())
    {
        ERROR("failed to create bitmap context...");
        CGImageRelease(_blank_image);
        return nullptr;
    }
    
    CGContextSetBlendMode((CGContextRef)myBitmapContext.resource(), kCGBlendModeCopy);
    CGContextSetInterpolationQuality((CGContextRef)myBitmapContext.resource(), kCGInterpolationHigh);
    CGContextScaleCTM((CGContextRef)myBitmapContext.resource(), _texture_size.x() / 2, _texture_size.y() / 2);
    CGContextTranslateCTM((CGContextRef)myBitmapContext.resource(), 1, 1);
    //do the same with alpha
    CGContextScaleCTM((CGContextRef)alphaOnlyContext.resource(), _texture_size.x() / 2, _texture_size.y() / 2);
    CGContextTranslateCTM((CGContextRef)alphaOnlyContext.resource(), 1, 1);
    
    //CGRect rect = {{0, 0}, {(CGFloat)_texture_size.x(), (CGFloat)_texture_size.y()}};
    CGRect rect = {{-1, -1}, {2, 2}};
    CGContextDrawImage((CGContextRef)myBitmapContext.resource(), rect, (CGImageRef)myImageRef.resource());
    CGContextFlush((CGContextRef)myBitmapContext.resource());
    
    for (size_t j = 0; j < _texture_size.y(); j++) {
        for (size_t i = 0; i < _texture_size.x(); i++) {
            auto _alpha_value = imageData[j * destStrideLength + i * 4 + 3];
            if(composition->mask_aggressively()){
                if(_alpha_value == 255)
                    _alpha_value = 255;
                else
                    _alpha_value = 0;
            }
            alphaData[j * alphaStrideLength + i] = _alpha_value;
        }
    }
    
    cg_resource _alpha_image(CGBitmapContextCreateImage((CGContextRef)alphaOnlyContext.resource()));
    //clear context
    if(composition->mask_aggressively()){
        CGContextSetRGBFillColor((CGContextRef)myBitmapContext.resource(), 0, 0, 0, 0);
        CGContextFillRect((CGContextRef)myBitmapContext.resource(), rect);
    }
    CGContextClipToMask((CGContextRef)myBitmapContext.resource(), rect, (CGImageRef)_alpha_image.resource());
    //redraw image
    if(composition->mask_aggressively())
        CGContextDrawImage((CGContextRef)myBitmapContext.resource(), rect, (CGImageRef)myImageRef.resource());
    
    for(auto& _l : composition->layers()){
        CGContextSaveGState((CGContextRef)myBitmapContext.resource());
        if(!draw_layer((CGContextRef)myBitmapContext.resource(), _l, _blank_image)){
            CGContextRestoreGState((CGContextRef)myBitmapContext.resource());
            CGImageRelease(_blank_image);
            return nullptr;
        }
        CGContextRestoreGState((CGContextRef)myBitmapContext.resource());
    }
    
    CGImageRelease(_blank_image);
    
    return CGBitmapContextCreateImage((CGContextRef)myBitmapContext.resource());;
}

void* composited_texture::create_blank_image(){
    cg_resource space(CGColorSpaceCreateDeviceRGB());
    return CGBitmapContextCreate (NULL,
                                   10, 10, 8,
                                   10 * 4, (CGColorSpaceRef)space.resource(),
                                   kCGBitmapByteOrder32Big |
                                   kCGImageAlphaPremultipliedLast);
}

float fsignf(float n){
    return n / fabsf(n);
}

bool composited_texture::draw_layer(const void* ctx, const texture_layer* layer, const void* blank_image){
    CGContextRef _ctx = (CGContextRef)ctx;
    CGContextSetAlpha(_ctx, layer->opacity());
    if(layer->blend_mode() == texture_layer_blend_mode::color)
        CGContextSetBlendMode(_ctx, kCGBlendModeColor);
    if(layer->blend_mode() == texture_layer_blend_mode::darken)
        CGContextSetBlendMode(_ctx, kCGBlendModeDarken);
    if(layer->blend_mode() == texture_layer_blend_mode::hue)
        CGContextSetBlendMode(_ctx, kCGBlendModeHue);
    if(layer->blend_mode() == texture_layer_blend_mode::lighten)
        CGContextSetBlendMode(_ctx, kCGBlendModeLighten);
    if(layer->blend_mode() == texture_layer_blend_mode::luminosity)
        CGContextSetBlendMode(_ctx, kCGBlendModeLuminosity);
    if(layer->blend_mode() == texture_layer_blend_mode::multiply)
        CGContextSetBlendMode(_ctx, kCGBlendModeMultiply);
    if(layer->blend_mode() == texture_layer_blend_mode::normal)
        CGContextSetBlendMode(_ctx, kCGBlendModeNormal);
    if(layer->blend_mode() == texture_layer_blend_mode::overlay)
        CGContextSetBlendMode(_ctx, kCGBlendModeOverlay);
    if(layer->blend_mode() == texture_layer_blend_mode::saturation)
        CGContextSetBlendMode(_ctx, kCGBlendModeSaturation);
    if(layer->blend_mode() == texture_layer_blend_mode::screen)
        CGContextSetBlendMode(_ctx, kCGBlendModeScreen);
    
    //CGRect _dest_size = {{0, 0}, {(CGFloat)CGBitmapContextGetWidth(_ctx), (CGFloat)CGBitmapContextGetHeight(_ctx)}};
    CGRect _dest_size = {{-1, -1}, {2, 2}};
    if(layer->uses_color()){
        CGContextRef _context_ref = (CGContextRef)blank_image;
        CGRect _size = {{0, 0}, {10, 10}};
        CGContextSetRGBFillColor(_context_ref, layer->solid_color().r(), layer->solid_color().g(), layer->solid_color().b(), layer->solid_color().a());
        CGContextFillRect(_context_ref, _size);
        CGImageRef _image = CGBitmapContextCreateImage(_context_ref);
        if(!_image)
            return false;
        CGContextDrawImage(_ctx, _dest_size, _image);
        CGImageRelease(_image);
        return true;
    }
    else {
        cg_resource myImageSourceRef(CGImageSourceCreateWithURL((__bridge CFURLRef)layer->texture_source()->image_url(), NULL));
        if (!myImageSourceRef.resource()) {
            ERROR("failed loading texture reference: ", layer->texture_source());
            return false;
        }
        cg_resource myImageRef(CGImageSourceCreateImageAtIndex((CGImageSourceRef)myImageSourceRef.resource(), 0, NULL));
        if(!myImageRef.resource()){
            ERROR("failed loading image:", layer->texture_source());
            return false;
        }
        
        float _ctx_width = CGBitmapContextGetWidth(_ctx);
        float _ctx_height = CGBitmapContextGetHeight(_ctx);
        float _img_width = CGImageGetWidth((CGImageRef)myImageRef.resource());
        float _img_height = CGImageGetHeight((CGImageRef)myImageRef.resource());
        float _aspect = _ctx_width / _ctx_height;
        float _scale_x = layer->texture_space().scale().x();
        float _scale_y = layer->texture_space().scale().y();
        
        //we correct aspect
        CGContextScaleCTM(_ctx, 1 / _aspect, 1);
        CGContextTranslateCTM(_ctx, layer->texture_space().origin().x(), layer->texture_space().origin().y());
        CGContextRotateCTM(_ctx, layer->texture_space().rotation().x());
        CGContextScaleCTM(_ctx, _scale_x, _scale_y);
        
        double _height_mult = 2 * (_img_height / _img_width);
        
        
        rectangle imgRect(0, 0, 2, _height_mult);
        CGRect _img_size = {{imgRect.bottom_left().x(), imgRect.bottom_left().y()}, { imgRect.size().x(), imgRect.size().y()}};
        
        if(layer->repeat()){
            CGContextDrawTiledImage(_ctx, _img_size, (CGImageRef)myImageRef.resource());
        }
        else {
            CGContextDrawImage(_ctx, _img_size, (CGImageRef)myImageRef.resource());
        }
        return true;
    }
}

void composited_texture::setup_texture(void* image, const texture_border border){
    cg_resource myImageRef((CFTypeRef)image);
    GLuint myTextureName;
    
    size_t width = CGImageGetWidth((CGImageRef)myImageRef.resource());
    size_t height = CGImageGetHeight((CGImageRef)myImageRef.resource());
    this->_original_size = vec2(width, height);
    this->_texture_size = this->_original_size * this->_composition->scale();
    this->_texture_size.x(roundf(this->_texture_size.x()));
    this->_texture_size.y(roundf(this->_texture_size.y()));
    if(_texture_size.x() < 3)
        _texture_size.x(3);
    if(_texture_size.y() < 3)
        _texture_size.y(3);
    this->_border = border;
    
    CGRect rect;
    if(border == texture_border::none)
        rect = {{0, 0}, {(CGFloat)_texture_size.x(), (CGFloat)_texture_size.y()}};
    else
        rect = {{0, 0}, {(CGFloat)(_texture_size.x() + 2 * TEXTURE_BORDER), (CGFloat)(_texture_size.y() + 2 * TEXTURE_BORDER)}};
    
    if(border == texture_border::none)
        _bounds = rectangle(0.5, 0.5, 1, 1);
    else
        _bounds = rectangle(0.5, 0.5, (rect.size.width - 2.0f * TEXTURE_BORDER) / rect.size.width, (rect.size.height - 2.0f * TEXTURE_BORDER) / rect.size.height);
    
    boost::shared_array<uint8_t> myData(new uint8_t[(uint32_t)rect.size.width * 4 * (uint32_t)rect.size.height]);
    
    if(!myData.get()){
        ERROR("failed to alloc memory...");
        return;
    }
    
    cg_resource space(CGColorSpaceCreateDeviceRGB());
    cg_resource myBitmapContext(CGBitmapContextCreate (myData.get(),
                                                       rect.size.width, rect.size.height, 8,
                                                       rect.size.width * 4, (CGColorSpaceRef)space.resource(),
                                                       kCGBitmapByteOrder32Big |
                                                       kCGImageAlphaPremultipliedLast));
    if(!myBitmapContext.resource())
    {
        ERROR("failed to create bitmap context...");
        return;
    }
    
    CGContextSetBlendMode((CGContextRef)myBitmapContext.resource(), kCGBlendModeCopy);
    CGContextSetInterpolationQuality((CGContextRef)myBitmapContext.resource(), kCGInterpolationHigh);
    CGContextScaleCTM((CGContextRef)myBitmapContext.resource(), 1, -1);
    CGContextTranslateCTM((CGContextRef)myBitmapContext.resource(), 0, -rect.size.height);
    
    //border rects
    //corners
    //CGRect low_left_corner_r = {{0, 0}, {1, 1}};
    CGRect low_left_corner_r = {{0, 0}, {TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect low_right_corner_r = {{rect.size.width - TEXTURE_BORDER, 0}, {TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect up_left_corner_r = {{0, rect.size.height - TEXTURE_BORDER}, {TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect up_right_corner_r = {{rect.size.width - TEXTURE_BORDER, rect.size.height - TEXTURE_BORDER}, {TEXTURE_BORDER, TEXTURE_BORDER}};
    //edges
    CGRect low_edge_r = {{TEXTURE_BORDER, 0}, {rect.size.width - 2 * TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect up_edge_r = {{TEXTURE_BORDER, rect.size.height - TEXTURE_BORDER}, {rect.size.width - 2 * TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect left_edge_r = {{0, TEXTURE_BORDER}, {TEXTURE_BORDER, rect.size.height - 2 * TEXTURE_BORDER}};
    CGRect right_edge_r = {{rect.size.width - TEXTURE_BORDER, TEXTURE_BORDER}, {TEXTURE_BORDER, rect.size.height - 2 * TEXTURE_BORDER}};
    
    //border rects of image...
    //corners
    CGRect i_low_left_corner_r = {{0, 0}, {TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect i_low_right_corner_r = {{_original_size.x() - TEXTURE_BORDER, 0}, {TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect i_up_left_corner_r = {{0, _original_size.y() - TEXTURE_BORDER}, {TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect i_up_right_corner_r = {{_original_size.x() - TEXTURE_BORDER, _original_size.y() - TEXTURE_BORDER}, {TEXTURE_BORDER, TEXTURE_BORDER}};
    //edges
    CGRect i_low_edge_r = {{TEXTURE_BORDER, 0}, {_original_size.x() - 2 * TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect i_up_edge_r = {{TEXTURE_BORDER, _original_size.y() - TEXTURE_BORDER}, {_original_size.x() - 2 * TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect i_left_edge_r = {{0, TEXTURE_BORDER}, {TEXTURE_BORDER, _original_size.y() - 2 * TEXTURE_BORDER}};
    CGRect i_right_edge_r = {{_original_size.x() - TEXTURE_BORDER, TEXTURE_BORDER}, {TEXTURE_BORDER, _original_size.y() - 2 * TEXTURE_BORDER}};
    
    if(border == texture_border::empty){
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
    else if(border == texture_border::clamp){
        cg_resource ii_low_left_corner(CGImageCreateWithImageInRect((CGImageRef)myImageRef.resource(), i_low_left_corner_r));
        cg_resource ii_low_right_corner(CGImageCreateWithImageInRect((CGImageRef)myImageRef.resource(), i_low_right_corner_r));
        cg_resource ii_up_left_corner(CGImageCreateWithImageInRect((CGImageRef)myImageRef.resource(), i_up_left_corner_r));
        cg_resource ii_up_right_corner(CGImageCreateWithImageInRect((CGImageRef)myImageRef.resource(), i_up_right_corner_r));
        
        cg_resource ii_low_edge(CGImageCreateWithImageInRect((CGImageRef)myImageRef.resource(), i_low_edge_r));
        cg_resource ii_up_edge(CGImageCreateWithImageInRect((CGImageRef)myImageRef.resource(), i_up_edge_r));
        cg_resource ii_left_edge(CGImageCreateWithImageInRect((CGImageRef)myImageRef.resource(), i_left_edge_r));
        cg_resource ii_right_edge(CGImageCreateWithImageInRect((CGImageRef)myImageRef.resource(), i_right_edge_r));
        
        CGContextDrawImage((CGContextRef)myBitmapContext.resource(), low_left_corner_r, (CGImageRef)ii_low_left_corner.resource());
        CGContextDrawImage((CGContextRef)myBitmapContext.resource(), low_right_corner_r, (CGImageRef)ii_low_left_corner.resource());
        CGContextDrawImage((CGContextRef)myBitmapContext.resource(), up_left_corner_r, (CGImageRef)ii_up_left_corner.resource());
        CGContextDrawImage((CGContextRef)myBitmapContext.resource(), up_right_corner_r, (CGImageRef)ii_up_right_corner.resource());
        
        CGContextDrawImage((CGContextRef)myBitmapContext.resource(), low_edge_r, (CGImageRef)ii_low_edge.resource());
        CGContextDrawImage((CGContextRef)myBitmapContext.resource(), up_edge_r, (CGImageRef)ii_up_edge.resource());
        CGContextDrawImage((CGContextRef)myBitmapContext.resource(), left_edge_r, (CGImageRef)ii_left_edge.resource());
        CGContextDrawImage((CGContextRef)myBitmapContext.resource(), right_edge_r, (CGImageRef)ii_right_edge.resource());
    }
    else if(border == texture_border::repeat){
        cg_resource ii_low_left_corner(CGImageCreateWithImageInRect((CGImageRef)myImageRef.resource(), i_low_left_corner_r));
        cg_resource ii_low_right_corner(CGImageCreateWithImageInRect((CGImageRef)myImageRef.resource(), i_low_right_corner_r));
        cg_resource ii_up_left_corner(CGImageCreateWithImageInRect((CGImageRef)myImageRef.resource(), i_up_left_corner_r));
        cg_resource ii_up_right_corner(CGImageCreateWithImageInRect((CGImageRef)myImageRef.resource(), i_up_right_corner_r));
        
        cg_resource ii_low_edge(CGImageCreateWithImageInRect((CGImageRef)myImageRef.resource(), i_low_edge_r));
        cg_resource ii_up_edge(CGImageCreateWithImageInRect((CGImageRef)myImageRef.resource(), i_up_edge_r));
        cg_resource ii_left_edge(CGImageCreateWithImageInRect((CGImageRef)myImageRef.resource(), i_left_edge_r));
        cg_resource ii_right_edge(CGImageCreateWithImageInRect((CGImageRef)myImageRef.resource(), i_right_edge_r));
        
        CGContextDrawImage((CGContextRef)myBitmapContext.resource(), low_left_corner_r, (CGImageRef)ii_up_right_corner.resource());
        CGContextDrawImage((CGContextRef)myBitmapContext.resource(), low_right_corner_r, (CGImageRef)ii_up_left_corner.resource());
        CGContextDrawImage((CGContextRef)myBitmapContext.resource(), up_left_corner_r, (CGImageRef)ii_low_right_corner.resource());
        CGContextDrawImage((CGContextRef)myBitmapContext.resource(), up_right_corner_r, (CGImageRef)ii_low_left_corner.resource());
        
        CGContextDrawImage((CGContextRef)myBitmapContext.resource(), low_edge_r, (CGImageRef)ii_up_edge.resource());
        CGContextDrawImage((CGContextRef)myBitmapContext.resource(), up_edge_r, (CGImageRef)ii_low_edge.resource());
        CGContextDrawImage((CGContextRef)myBitmapContext.resource(), left_edge_r, (CGImageRef)ii_right_edge.resource());
        CGContextDrawImage((CGContextRef)myBitmapContext.resource(), right_edge_r, (CGImageRef)ii_left_edge.resource());
    }
    
    if (border == texture_border::none)
        CGContextDrawImage((CGContextRef)myBitmapContext.resource(), rect, (CGImageRef)myImageRef.resource());
    else
    {
        CGRect newRect;
        newRect = {{TEXTURE_BORDER, TEXTURE_BORDER}, {rect.size.width - 2 * TEXTURE_BORDER, rect.size.height - 2 * TEXTURE_BORDER}};
        CGContextDrawImage((CGContextRef)myBitmapContext.resource(), newRect, (CGImageRef)myImageRef.resource());
    }
    
    while(glGetError() != GL_NO_ERROR); //clear error flags
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &myTextureName);
    if(!myTextureName){
        ERROR("failed to create texture...");
        return;
    }
    glBindTexture(GL_TEXTURE_2D, myTextureName);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    if(glGetError() != GL_NO_ERROR){
        ERROR("error setting texture min filter to linear...");
        glDeleteTextures(1, &myTextureName);
        return;
    }
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if(glGetError() != GL_NO_ERROR){
        ERROR("error setting texture mag filter to linear...");
        glDeleteTextures(1, &myTextureName);
        return;
    }
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    if(glGetError() != GL_NO_ERROR){
        ERROR("error setting texture wrap s filter to clamp to edge...");
        glDeleteTextures(1, &myTextureName);
        return;
    }
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    if(glGetError() != GL_NO_ERROR){
        ERROR("error setting texture wrap t filter to clamp to edge...");
        glDeleteTextures(1, &myTextureName);
        return;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)rect.size.width, (GLsizei)rect.size.height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, myData.get());
    if(glGetError() != GL_NO_ERROR){
        ERROR("error uploading texture data...");
        glDeleteTextures(1, &myTextureName);
        return;
    }
    this->_texture_id = myTextureName;
}

composited_texture::composited_texture(const texture_composition* composition, texture_border border, const bool avoid_gl) : _composition(composition) {
    assert(composition);
    this->_composition = composition;
    _border = border;
    auto _image = create_image(composition);
    if(!_image){
        this->_texture_id = 0;
        this->_cg_image = nullptr;
    }
    else {
        if(!avoid_gl){
            this->_cg_image = nullptr;
            setup_texture(_image, border);
        }
        else {
            this->_cg_image = _image;
        }
    }
}
composited_texture::~composited_texture(){
    if(_composition)
        delete _composition;
    if(this->_cg_image)
        CGImageRelease((CGImageRef)this->_cg_image);
}

void* composited_texture::to_cg_image() const{
    if(this->_cg_image){
        CGImageRetain((CGImageRef)this->_cg_image);
        return this->_cg_image;
    }
    else
        return this->texture::to_cg_image();
}