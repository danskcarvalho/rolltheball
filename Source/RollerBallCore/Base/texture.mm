//
//  texture.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 15/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "texture.h"
#include "cg_resource.h"
#include "texture_source.h"
#include "matrix3x3.h"
#include "vertex.h"
#include "mesh.h"
#include "vertex.h"
#include "dynamic_texture.h"
#import <Foundation/Foundation.h>
#if defined(IOS_TARGET)
#import <MobileCoreServices/MobileCoreServices.h>
#endif
#include <CoreGraphics/CoreGraphics.h>
#include <ImageIO/ImageIO.h>
#include <boost/shared_array.hpp>

using namespace rb;

texture::simple_texture_map::simple_texture_map(const transform_space& texture_transform, const rectangle& bounds){
    _texture_transform = texture_transform;
    _bounds = bounds;
}
void texture::simple_texture_map::set_texture_coords(vertex& v) const {
    auto _coords = _texture_transform.from_base_to_space().transformed_point(v.get_position());
    v.set_texture_coords(_coords);
    v.set_texture_bounds(_bounds);
}

const rectangle& texture::simple_texture_map::bounds() const{
    return _bounds;
}

const transform_space& texture::simple_texture_map::get_texture_space() const {
    return _texture_transform;
}

texture::simple_texture_map::~simple_texture_map(){
    
}

const texture_map* texture::create_mapping(const transform_space& transform){
    return new simple_texture_map(transform, _bounds);
}

texture* texture::_blank = nullptr;

const texture* texture::blank(){
    if(!_blank){
        _blank = new dynamic_texture(vec2(10, 10), texture_border::none, [](void* ctx){
            
            CGContextRef _ctx = (CGContextRef)ctx;
            CGContextSetRGBFillColor(_ctx, 1, 1, 1, 1);
            CGContextFillRect(_ctx, CGRectMake(0, 0, 10, 10));
            });
    }
    return _blank;
}

texture::texture(){
    _cg_texture = nullptr;
    _texture_id = 0;
    _texture_size = vec2::zero;
    _original_size = vec2::zero;
    _border = texture_border::none;
    _bounds = rectangle(0, 0, 0, 0);
}

texture* texture::from_cg_image(const void* image){
    texture* _tex = new texture();
    _tex->_cg_texture = const_cast<void*>(image);
    CGImageRef _img = (CGImageRef)_tex->_cg_texture;
    CGImageRetain(_img);
    float width = CGImageGetWidth(_img);
    float height = CGImageGetHeight(_img);
    _tex->_texture_id = 0;
    _tex->_texture_size = vec2(width, height);
    _tex->_original_size = vec2(width, height);
    _tex->_border = texture_border::none;
    _tex->_bounds = rectangle(0.5, 0.5, 1, 1);
    return _tex;
}

texture::texture(const texture_source& source, const texture_border border, const float scale){
    assert(scale > 0);
    _cg_texture = nullptr;
    NSURL* imageURL = (NSURL*)source.image_url();
    if(imageURL.scheme == nil)
        imageURL = [NSURL fileURLWithPath:imageURL.path];
    cg_resource myImageSourceRef(CGImageSourceCreateWithURL((__bridge CFURLRef)imageURL, NULL));
    if (!myImageSourceRef.resource()) {
        ERROR("failed loading texture reference: ", source);
        return;
    }
    cg_resource myImageRef(CGImageSourceCreateImageAtIndex((CGImageSourceRef)myImageSourceRef.resource(), 0, NULL));
    if(!myImageRef.resource()){
        ERROR("failed loading image:", source);
        return;
    }
    GLuint myTextureName;
    
    size_t width = CGImageGetWidth((CGImageRef)myImageRef.resource());
    size_t height = CGImageGetHeight((CGImageRef)myImageRef.resource());
    this->_original_size = vec2(width, height);
    this->_texture_size = this->_original_size * scale;
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
        rect = {{0, 0}, {(CGFloat)(_texture_size.x() + (2 * TEXTURE_BORDER)), (CGFloat)(_texture_size.y() + (2 * TEXTURE_BORDER))}};
    
    if(border == texture_border::none)
        _bounds = rectangle(0.5, 0.5, 1, 1);
    else
        _bounds = rectangle(0.5, 0.5, (rect.size.width - (2.0f * TEXTURE_BORDER)) / rect.size.width, (rect.size.height - (2.0f * TEXTURE_BORDER)) / rect.size.height);
    
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

void* texture::to_cg_image(const rb::rectangle &subarea) const {
    CGRect rect;
    if(_border == texture_border::none)
        rect = {{0, 0}, {(CGFloat)_texture_size.x(), (CGFloat)_texture_size.y()}};
    else
        rect = {{0, 0}, {(CGFloat)(_texture_size.x() + 2 * TEXTURE_BORDER), (CGFloat)(_texture_size.y() + 2 * TEXTURE_BORDER)}};
    
    CGImageRef _cg_img = (CGImageRef)to_cg_image();
    auto _subrect = CGRectMake(subarea.bottom_left().x(), subarea.bottom_left().y(), subarea.size().x(), subarea.size().y());
    //I need to mirror this subrect...
    CGAffineTransform _transform = CGAffineTransformIdentity;
    _transform = CGAffineTransformScale(_transform, 1, -1);
    _transform = CGAffineTransformTranslate(_transform, 0, -rect.size.height);
    _subrect = CGRectApplyAffineTransform(_subrect, _transform);
//    _subrect.origin.y *= -1;
//    _subrect.origin.y += rect.size.height;
    CGImageRef _result = CGImageCreateWithImageInRect(_cg_img, _subrect);
    CGImageRelease(_cg_img);
    return _result;
}

void swap_lines(uint8_t* line0, uint8_t* line1, uint32_t size){
    for (uint32_t i = 0; i < size; i++) {
        uint8_t _temp = line0[i];
        line0[i] = line1[i];
        line1[i] = _temp;
    }
}

void* texture::to_cg_image() const{
    if(_cg_texture){
        CGImageRetain((CGImageRef)_cg_texture);
        return _cg_texture;
    }
#if defined(MACOSX_TARGET)
    assert(_texture_id);
    CGRect rect;
    if(_border == texture_border::none)
        rect = {{0, 0}, {(CGFloat)_texture_size.x(), (CGFloat)_texture_size.y()}};
    else
        rect = {{0, 0}, {(CGFloat)(_texture_size.x() + 2 * TEXTURE_BORDER), (CGFloat)(_texture_size.y() + 2 * TEXTURE_BORDER)}};
    
    auto _img_bytes = new uint8_t[(uint32_t)rect.size.width * 4 * (uint32_t)rect.size.height];
    boost::shared_array<uint8_t> myData(_img_bytes);
    while(glGetError() != GL_NO_ERROR); //clear error flags
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, _texture_id);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, myData.get()); //get the texture...
    assert(glGetError() == GL_NO_ERROR);
    
    auto _line_size = (uint32_t)rect.size.width * 4;
    uint8_t* _pLine0 = _img_bytes;
    uint8_t* _pLine1 = _img_bytes + _line_size * ((uint32_t)rect.size.height - 1);
    
    while(_pLine0 < _pLine1){
        swap_lines(_pLine0, _pLine1, _line_size);
        _pLine0 += _line_size;
        _pLine1 -= _line_size;
    }
    
    //create the context
    cg_resource space(CGColorSpaceCreateDeviceRGB());
    cg_resource myBitmapContext(CGBitmapContextCreate (myData.get(),
                                                       rect.size.width, rect.size.height, 8,
                                                       rect.size.width * 4, (CGColorSpaceRef)space.resource(),
                                                       kCGBitmapByteOrder32Big |
                                                       kCGImageAlphaPremultipliedLast));
    
    CGContextSetBlendMode((CGContextRef)myBitmapContext.resource(), kCGBlendModeCopy);
    CGContextSetInterpolationQuality((CGContextRef)myBitmapContext.resource(), kCGInterpolationHigh);
//    CGContextScaleCTM((CGContextRef)myBitmapContext.resource(), 1, -1);
//    CGContextTranslateCTM((CGContextRef)myBitmapContext.resource(), 0, -rect.size.height);
    
    //create the image
//    cg_resource flippedImageContext(CGBitmapContextCreate (myData.get(),
//                                                       rect.size.width, rect.size.height, 8,
//                                                       rect.size.width * 4, (CGColorSpaceRef)space.resource(),
//                                                       kCGBitmapByteOrder32Big |
//                                                       kCGImageAlphaPremultipliedLast));
//    cg_resource flippedImage(CGBitmapContextCreateImage((CGContextRef)flippedImageContext.resource()));
//    CGContextDrawImage((CGContextRef)myBitmapContext.resource(), NSMakeRect(0, 0, rect.size.width, rect.size.height), (CGImageRef)flippedImage.resource());
    
    CGImageRef image = CGBitmapContextCreateImage((CGContextRef)myBitmapContext.resource());
    return image;
#else
    return nullptr;
#endif
}

void texture::save_to_file(const URL_TYPE url) const{
#if defined(MACOSX_TARGET)
    CGImageRef _final_img = (CGImageRef)to_cg_image();
    //we need to invert _final_img due to the way we need to store images for OpenGL...
    if(!_final_img){
        ERROR("no core graphics image...");
        return;
    }
    CGColorSpaceRef _color_space = CGColorSpaceCreateDeviceRGB();
    CGContextRef _ctx = CGBitmapContextCreate(NULL, CGImageGetWidth(_final_img), CGImageGetHeight(_final_img), 8,
                          CGImageGetWidth(_final_img) * 4, _color_space,
                          kCGBitmapByteOrder32Big |
                          kCGImageAlphaPremultipliedLast);
    if(!_ctx){
        CGImageRelease(_final_img);
        if(_color_space)
            CGColorSpaceRelease(_color_space);
        ERROR("error creating the context...");
        return;
    }
    
    CGContextSetBlendMode(_ctx, kCGBlendModeCopy);
    CGContextSetInterpolationQuality(_ctx, kCGInterpolationHigh);
    CGContextTranslateCTM(_ctx, 0, CGImageGetHeight(_final_img));
    CGContextScaleCTM(_ctx, 1, -1);
    
    CGRect rect = {{0, 0}, {static_cast<CGFloat>(CGImageGetWidth(_final_img)), static_cast<CGFloat>(CGImageGetHeight(_final_img))}};
    CGContextDrawImage(_ctx, rect, _final_img);
    CGImageRef _inv_image = CGBitmapContextCreateImage(_ctx);
    
    if(!_inv_image) {
        CGContextRelease(_ctx);
        CGColorSpaceRelease(_color_space);
        CGImageRelease(_final_img);
        ERROR("error saving image...");
        return;
    }
    
    CGImageDestinationRef _destination = CGImageDestinationCreateWithURL((__bridge CFURLRef)(url), kUTTypePNG, 1, NULL);
    if(!_destination)
    {
        CGContextRelease(_ctx);
        CGColorSpaceRelease(_color_space);
        CGImageRelease(_final_img);
        CGImageRelease(_inv_image);
        ERROR("can't save texture to file...");
        return;
    }
    CGImageDestinationAddImage(_destination, (CGImageRef)_inv_image, NULL);
    //Liberamos os recursos alocados...
    CGImageDestinationFinalize(_destination);
    CFRelease(_destination);
    CGImageRelease(_final_img);
    CGContextRelease(_ctx);
    CGColorSpaceRelease(_color_space);
    CGImageRelease(_inv_image);
#endif
}

texture::~texture(){
    if(_texture_id)
        glDeleteTextures(1, &_texture_id);
    if(_cg_texture)
        CGImageRelease((CGImageRef)_cg_texture);
}