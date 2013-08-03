//
//  texture_atlas.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 02/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "texture_atlas.h"
#include "texture.h"
#include "transform_space.h"
#include "vertex.h"
#include "cg_resource.h"
#include "texture_packer.h"
#include "radial_gradient_texture.h"
#include "composited_texture.h"
#include <CoreGraphics/CoreGraphics.h>
#include <ImageIO/ImageIO.h>
#include <boost/shared_array.hpp>
#include "xnode.h"
#include "cg_texture_source.h"

using namespace rb;

texture_atlas::simple_texture_map::simple_texture_map(const transform_space& texture_transform, const rectangle& bounds){
    _texture_transform = texture_transform;
    _bounds = bounds;
}
void texture_atlas::simple_texture_map::set_texture_coords(vertex& v) const {
    auto _coords = _texture_transform.from_base_to_space().transformed_point(v.get_position());
    v.set_texture_coords(_coords);
    v.set_texture_bounds(_bounds);
}

const rectangle& texture_atlas::simple_texture_map::bounds() const{
    return _bounds;
}

const transform_space& texture_atlas::simple_texture_map::get_texture_space() const {
    return _texture_transform;
}

texture_atlas::simple_texture_map::~simple_texture_map(){
    
}

texture_atlas::texture_atlas(bool avoid_gl){
    _dirty = true;
    _max_atlas_size = 0;
    this->_avoid_gl = avoid_gl;
}

texture_atlas::~texture_atlas(){
    for (auto _a : _atlases){
        delete _a;
    }
}

bool texture_atlas::add_texture(const rb_string& name, const std::vector<rb_string>& groups, const class texture_source& source, const enum texture_border border, const float scale){
    if(_texture_names.count(name) != 0)
        return false;
    
    if(groups.size() == 0)
        return false;
    
    assert(scale > 0);
    
    _texture_names.insert(name);
    texture_info _tex_info(source.duplicate(), border, scale);
    auto _name_copy = name;
    std::pair<rb_string, texture_info> _pair(std::move(_name_copy), std::move(_tex_info));
    _texture_infos.insert(std::move(_pair));
    
    for(auto& _g : groups){
        if (_group_names.count(_g) == 0){
            _group_names.insert(_g);
        }
        
        _group_textures.insert({{_g, name}});
    }
    _dirty = true;
    return true;
}

struct radial_gradient_info {
    float size;
    class color color;
};

bool texture_atlas::add_radial_gradient_texture(const rb_string & name, const std::vector<rb_string>& groups, const float size, const class color& color){
    if(_texture_names.count(name) != 0)
        return false;
    
    if(groups.size() == 0)
        return false;
    
    float _size = size;
    assert(_size > 0);
    _size = roundf(_size);
    if(_size < 1)
        _size = 1;
    
    _texture_names.insert(name);
    radial_gradient_info* _r_info = (radial_gradient_info*)malloc(sizeof(radial_gradient_info));
    _r_info->size = _size;
    _r_info->color = color;
    texture_info _tex_info(vec2(_size, _size), [](void* ctx, void* user_data){
        radial_gradient_info* _r_info = (radial_gradient_info*)user_data;
        radial_gradient_texture::draw_radial_gradient(ctx, _r_info->size, _r_info->color);
    }, _r_info, sizeof(radial_gradient_info), texture_border::empty);
    free(_r_info);
    auto _name_copy = name;
    std::pair<rb_string, texture_info> _pair(std::move(_name_copy), std::move(_tex_info));
    _texture_infos.insert(std::move(_pair));
    
    for(auto& _g : groups){
        if (_group_names.count(_g) == 0){
            _group_names.insert(_g);
        }
        
        _group_textures.insert({{_g, name}});
    }
    _dirty = true;
    return true;
}

bool texture_atlas::add_dynamic_texture(const rb_string& name, const std::vector<rb_string>& groups, const vec2& size, const enum texture_border border, const std::function<void (void*, void*)>& f, void* user_data, uint32_t data_size){
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
    
    if(_texture_names.count(name) != 0)
        return false;
    
    if(groups.size() == 0)
        return false;
    
    _texture_names.insert(name);
    texture_info _tex_info(_size, f, user_data, data_size, border);
    auto _name_copy = name;
    std::pair<rb_string, texture_info> _pair(std::move(_name_copy), std::move(_tex_info));
    _texture_infos.insert(std::move(_pair));
    
    for(auto& _g : groups){
        if (_group_names.count(_g) == 0){
            _group_names.insert(_g);
        }
        
        _group_textures.insert({{_g, name}});
    }
    _dirty = true;
    return true;
}
bool texture_atlas::add_composited_texture(const rb_string& name, const std::vector<rb_string>& groups, const texture_composition* composition, const enum texture_border border){
    assert(composition);
    if(_texture_names.count(name) != 0)
        return false;
    
    if(groups.size() == 0)
        return false;
    
    _texture_names.insert(name);
    texture_info _tex_info(composition);
    _tex_info.border = border;
    _tex_info.scale = composition->scale();
    auto _name_copy = name;
    std::pair<rb_string, texture_info> _pair(std::move(_name_copy), std::move(_tex_info));
    _texture_infos.insert(std::move(_pair));
    
    for(auto& _g : groups){
        if (_group_names.count(_g) == 0){
            _group_names.insert(_g);
        }
        
        _group_textures.insert({{_g, name}});
    }
    _dirty = true;
    return true;
}
uint32_t texture_atlas::texture_count() const{
    return (uint32_t)_texture_names.size();
}
uint32_t texture_atlas::texture_group_count() const{
    return (uint32_t)_group_names.size();
}
const texture_atlas::name_iterator texture_atlas::texture_names_begin() const{
    return _texture_names.begin();
}
const texture_atlas::name_iterator texture_atlas::texture_names_end() const{
    return _texture_names.end();
}
const texture_atlas::name_iterator texture_atlas::texture_groups_begin() const{
    return _group_names.begin();
}
const texture_atlas::name_iterator texture_atlas::texture_groups_end() const{
    return _group_names.end();
}
void texture_atlas::get_texture_names(std::vector<rb_string>& names) const{
    names.clear();
    for (auto& _n : _texture_names)
        names.push_back(_n);
}
void texture_atlas::get_groups(std::vector<rb_string>& names) const{
    names.clear();
    for (auto& _n : _group_names)
        names.push_back(_n);
}
bool texture_atlas::contains_texture(const rb_string& name) const{
    return _texture_names.count(name) != 0;
}
bool texture_atlas::contains_group(const rb_string& name) const{
    for(auto& _grp : _group_names){
        if(_grp == name)
            return true;
    }
    return false;
}
void texture_atlas::get_groups(const rb_string& name, std::vector<rb_string>& group_names) const{
    std::unordered_set<rb_string> _grps;
    for (auto& _key : _group_textures){
        if(_key.second == name)
            _grps.insert(_key.first);
    }
    
    group_names.clear();
    for (auto& _g : _grps)
        group_names.push_back(_g);
}
bool texture_atlas::remove_texture(const rb_string& name){
    if(_texture_names.count(name) == 0)
        return false;
    
    _texture_names.erase(name);
    _texture_infos.erase(name);
    
    auto _it = _group_textures.begin();
    
    while (_it != _group_textures.end()){
        if (_it->second == name)
            _it = _group_textures.erase(_it);
        else
            _it++;
    }
    
    //remove unused group names
    auto _grp_it = _group_names.begin();
    while (_grp_it != _group_names.end()) {
        if (_group_textures.count(*_grp_it) == 0)
            _grp_it = _group_names.erase(_grp_it);
        else
            _grp_it++;
    }
    
    _dirty = true;
    return true;
}
bool texture_atlas::dirty() const{
    return _dirty;
}

void texture_atlas::free_textures(){
    for (auto _a : _atlases){
        delete _a;
    }
    _atlases.clear();
    _group_atlases.clear();
    _bounds.clear();
    _texture_atlases.clear();
}

////////COMPILATION
struct CGContextData {
    CGContextRef Context;
    uint8_t* Data;
    
    CGContextData(){
        Context = nullptr;
        Data = nullptr;
    }
    
    ~CGContextData(){
        if(Context)
            CGContextRelease(Context);
        if(Data)
            free(Data);
    }
};

bool create_context(const vec2& size, CGContextData& data){
    data.Data = (uint8_t*)malloc(4 * (size_t)size.x() * (size_t)size.y());
    bzero(data.Data, 4 * (size_t)size.x() * (size_t)size.y());
    
    if(!data.Data)
        return false;
    
    cg_resource space(CGColorSpaceCreateDeviceRGB());
    data.Context =  CGBitmapContextCreate (data.Data,
                       size.x(), size.y(), 8,
                       size.x() * 4, (CGColorSpaceRef)space.resource(),
                       kCGBitmapByteOrder32Big |
                       kCGImageAlphaPremultipliedLast);
    
    if(!data.Context)
    {
        ERROR("failed to create bitmap context...");
        free(data.Data);
        data.Data = nullptr;
        return false;
    }
    
    CGContextSetBlendMode(data.Context, kCGBlendModeCopy);
    CGContextScaleCTM(data.Context, 1, -1);
    CGContextTranslateCTM(data.Context, 0, -size.y());
    CGContextSetInterpolationQuality(data.Context, kCGInterpolationHigh);
    
    return true;
}

bool texture_atlas::draw_dynamic_image(const void* data, const rb_string& texture_name, const rectangle& bounds){
    CGContextData* _data = (CGContextData*)data;
    auto border = _texture_infos.at(texture_name).border;
    vec2 _total_size = bounds.size();
    
    uint8_t* myData = new uint8_t[(uint32_t)_total_size.x() * 4 * (uint32_t)_total_size.y()];
    
    if(!myData){
        ERROR("failed to alloc memory...");
        return false;
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
        return false;
    }
    
    CGContextSetBlendMode((CGContextRef)myBitmapContext.resource(), kCGBlendModeCopy);
    
    if(border == texture_border::empty){
        CGContextSaveGState((CGContextRef)myBitmapContext.resource());
        CGContextScaleCTM((CGContextRef)myBitmapContext.resource(), 1, -1);
        CGContextTranslateCTM((CGContextRef)myBitmapContext.resource(), TEXTURE_BORDER, -_total_size.y() + TEXTURE_BORDER); //we shift by one to account for the border..
    }
    else {
        CGContextScaleCTM((CGContextRef)myBitmapContext.resource(), 1, -1);
        CGContextTranslateCTM((CGContextRef)myBitmapContext.resource(), 0, -_total_size.y());
    }
    
    CGContextSaveGState((CGContextRef)myBitmapContext.resource());
    _texture_infos.at(texture_name).content_creator(
            (CGContextRef)myBitmapContext.resource(),
            _texture_infos.at(texture_name).user_data);
    CGContextRestoreGState((CGContextRef)myBitmapContext.resource());
    
    if(border == texture_border::empty){
        CGContextRestoreGState((CGContextRef)myBitmapContext.resource());
        CGContextScaleCTM((CGContextRef)myBitmapContext.resource(), 1, -1);
        CGContextTranslateCTM((CGContextRef)myBitmapContext.resource(), 0, -_total_size.y());
    }
    
    //corners
    CGRect low_left_corner_r = {{bounds.bottom_left().x(), bounds.bottom_left().y()}, {TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect low_right_corner_r = {{bounds.bottom_right().x() - TEXTURE_BORDER, bounds.bottom_right().y()}, {TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect up_left_corner_r = {{bounds.top_left().x(), bounds.top_left().y() - TEXTURE_BORDER}, {TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect up_right_corner_r = {{bounds.top_right().x() - TEXTURE_BORDER, bounds.top_right().y() - TEXTURE_BORDER}, {TEXTURE_BORDER, TEXTURE_BORDER}};
    //edges
    CGRect low_edge_r = {{low_left_corner_r.origin.x + TEXTURE_BORDER, low_left_corner_r.origin.y }, {bounds.size().x() - 2 * TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect up_edge_r = {{up_left_corner_r.origin.x + TEXTURE_BORDER, up_left_corner_r.origin.y }, {bounds.size().x() - 2 * TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect left_edge_r = {{low_left_corner_r.origin.x, low_left_corner_r.origin.y + TEXTURE_BORDER}, {TEXTURE_BORDER, bounds.size().y() - 2 * TEXTURE_BORDER}};
    CGRect right_edge_r = {{low_right_corner_r.origin.x, low_right_corner_r.origin.y + TEXTURE_BORDER}, {TEXTURE_BORDER, bounds.size().y() - 2 * TEXTURE_BORDER}};
    
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
    
    cg_resource myDynamicImage(CGBitmapContextCreateImage((CGContextRef)myBitmapContext.resource()));
    
    CGRect newRect = {{bounds.bottom_left().x(), bounds.bottom_left().y()}, {bounds.size().x(), bounds.size().y()}};
    CGContextDrawImage(_data->Context, newRect, (CGImageRef)myDynamicImage.resource());
    
    delete [] myData;
    return true;
}

bool texture_atlas::draw_image(const void* data, const rb_string& texture_name, const rectangle& bounds) {
    CGContextData* _data = (CGContextData*)data;
    auto source = _texture_infos.at(texture_name).source;
    auto border = _texture_infos.at(texture_name).border;
    
    bool _from_source = _texture_infos.at(texture_name).type == from_source;
    cg_resource myImageSourceRef(_from_source ? CGImageSourceCreateWithURL((__bridge CFURLRef)source->image_url(), NULL) : nullptr);
    if (_from_source && !myImageSourceRef.resource()) {
        ERROR("failed loading texture reference: ", source);
        return false;
    }
    
    cg_resource myImageRef(_from_source ? CGImageSourceCreateImageAtIndex((CGImageSourceRef)myImageSourceRef.resource(), 0, NULL) : composited_texture::create_image(_texture_infos[texture_name].composition));
    if(!myImageRef.resource()){
        ERROR("failed loading image:", source);
        return false;
    }
    
    float width = CGImageGetWidth((CGImageRef)myImageRef.resource());
    float height = CGImageGetHeight((CGImageRef)myImageRef.resource());
    
    //border rects
    //corners
    CGRect low_left_corner_r = {{bounds.bottom_left().x(), bounds.bottom_left().y()}, {TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect low_right_corner_r = {{bounds.bottom_right().x() - TEXTURE_BORDER, bounds.bottom_right().y()}, {TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect up_left_corner_r = {{bounds.top_left().x(), bounds.top_left().y() - TEXTURE_BORDER}, {TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect up_right_corner_r = {{bounds.top_right().x() - TEXTURE_BORDER, bounds.top_right().y() - TEXTURE_BORDER}, {TEXTURE_BORDER, TEXTURE_BORDER}};
    //edges
    CGRect low_edge_r = {{low_left_corner_r.origin.x + TEXTURE_BORDER, low_left_corner_r.origin.y }, {bounds.size().x() - 2 * TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect up_edge_r = {{up_left_corner_r.origin.x + TEXTURE_BORDER, up_left_corner_r.origin.y }, {bounds.size().x() - 2 * TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect left_edge_r = {{low_left_corner_r.origin.x, low_left_corner_r.origin.y + TEXTURE_BORDER}, {TEXTURE_BORDER, bounds.size().y() - 2 * TEXTURE_BORDER}};
    CGRect right_edge_r = {{low_right_corner_r.origin.x, low_right_corner_r.origin.y + TEXTURE_BORDER}, {TEXTURE_BORDER, bounds.size().y() - 2 * TEXTURE_BORDER}};
    
    //border rects of image...
    //corners
    CGRect i_low_left_corner_r = {{0, 0}, {TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect i_low_right_corner_r = {{width - TEXTURE_BORDER, 0}, {TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect i_up_left_corner_r = {{0, height - TEXTURE_BORDER}, {TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect i_up_right_corner_r = {{width - TEXTURE_BORDER, height - TEXTURE_BORDER}, {TEXTURE_BORDER, TEXTURE_BORDER}};
    //edges
    CGRect i_low_edge_r = {{TEXTURE_BORDER, 0}, {width - 2 * TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect i_up_edge_r = {{TEXTURE_BORDER, height - TEXTURE_BORDER}, {width - 2 * TEXTURE_BORDER, TEXTURE_BORDER}};
    CGRect i_left_edge_r = {{0, TEXTURE_BORDER}, {TEXTURE_BORDER, height - 2 * TEXTURE_BORDER}};
    CGRect i_right_edge_r = {{width - TEXTURE_BORDER, TEXTURE_BORDER}, {TEXTURE_BORDER, height - 2 * TEXTURE_BORDER}};
    
    if(border == texture_border::empty){
        CGContextSetRGBFillColor(_data->Context, 0, 0, 0, 0);
        CGContextFillRect(_data->Context, low_left_corner_r);
        CGContextFillRect(_data->Context, low_right_corner_r);
        CGContextFillRect(_data->Context, up_left_corner_r);
        CGContextFillRect(_data->Context, up_right_corner_r);
        
        CGContextFillRect(_data->Context, low_edge_r);
        CGContextFillRect(_data->Context, up_edge_r);
        CGContextFillRect(_data->Context, left_edge_r);
        CGContextFillRect(_data->Context, right_edge_r);
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
        
        CGContextDrawImage(_data->Context, low_left_corner_r, (CGImageRef)ii_low_left_corner.resource());
        CGContextDrawImage(_data->Context, low_right_corner_r, (CGImageRef)ii_low_left_corner.resource());
        CGContextDrawImage(_data->Context, up_left_corner_r, (CGImageRef)ii_up_left_corner.resource());
        CGContextDrawImage(_data->Context, up_right_corner_r, (CGImageRef)ii_up_right_corner.resource());
        
        CGContextDrawImage(_data->Context, low_edge_r, (CGImageRef)ii_low_edge.resource());
        CGContextDrawImage(_data->Context, up_edge_r, (CGImageRef)ii_up_edge.resource());
        CGContextDrawImage(_data->Context, left_edge_r, (CGImageRef)ii_left_edge.resource());
        CGContextDrawImage(_data->Context, right_edge_r, (CGImageRef)ii_right_edge.resource());
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
        
        CGContextDrawImage(_data->Context, low_left_corner_r, (CGImageRef)ii_up_right_corner.resource());
        CGContextDrawImage(_data->Context, low_right_corner_r, (CGImageRef)ii_up_left_corner.resource());
        CGContextDrawImage(_data->Context, up_left_corner_r, (CGImageRef)ii_low_right_corner.resource());
        CGContextDrawImage(_data->Context, up_right_corner_r, (CGImageRef)ii_low_left_corner.resource());
        
        CGContextDrawImage(_data->Context, low_edge_r, (CGImageRef)ii_up_edge.resource());
        CGContextDrawImage(_data->Context, up_edge_r, (CGImageRef)ii_low_edge.resource());
        CGContextDrawImage(_data->Context, left_edge_r, (CGImageRef)ii_right_edge.resource());
        CGContextDrawImage(_data->Context, right_edge_r, (CGImageRef)ii_left_edge.resource());
    }
    
    if (border == texture_border::none){
        CGRect newRect = {{bounds.bottom_left().x(), bounds.bottom_left().y()}, {bounds.size().x(), bounds.size().y()}};
        CGContextDrawImage(_data->Context, newRect, (CGImageRef)myImageRef.resource());
    }
    else
    {
        CGRect newRect = {{bounds.bottom_left().x() + TEXTURE_BORDER, bounds.bottom_left().y() + TEXTURE_BORDER}, {bounds.size().x() - 2 * TEXTURE_BORDER, bounds.size().y() - 2 * TEXTURE_BORDER}};
        CGContextDrawImage(_data->Context, newRect, (CGImageRef)myImageRef.resource());
    }
    
    return true;
}

texture* texture_atlas::create_texture(const void* data, const vec2& size){
    CGContextData* _data = (CGContextData*)data;
    if(_avoid_gl){
        auto _img = CGBitmapContextCreateImage(_data->Context);
        auto _texture = texture::from_cg_image(_img);
        CGImageRelease(_img);
        return _texture;
    }
    GLuint myTextureName;
    while(glGetError() != GL_NO_ERROR); //clear error flags
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &myTextureName);
    if(!myTextureName){
        ERROR("failed to create texture...");
        return nullptr;
    }
    glBindTexture(GL_TEXTURE_2D, myTextureName);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    if(glGetError() != GL_NO_ERROR){
        ERROR("error setting texture min filter to linear...");
        glDeleteTextures(1, &myTextureName);
        return nullptr;
    }
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if(glGetError() != GL_NO_ERROR){
        ERROR("error setting texture mag filter to linear...");
        glDeleteTextures(1, &myTextureName);
        return nullptr;
    }
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    if(glGetError() != GL_NO_ERROR){
        ERROR("error setting texture wrap s filter to clamp to edge...");
        glDeleteTextures(1, &myTextureName);
        return nullptr;
    }
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    if(glGetError() != GL_NO_ERROR){
        ERROR("error setting texture wrap t filter to clamp to edge...");
        glDeleteTextures(1, &myTextureName);
        return nullptr;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)size.x(), (GLsizei)size.y(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, _data->Data);
    if(glGetError() != GL_NO_ERROR){
        ERROR("error uploading texture data...");
        return nullptr;
    }
    texture* _tx = new texture();
    _tx->_texture_id = myTextureName;
    _tx->_texture_size = size;
    _tx->_original_size = size;
    _tx->_border = texture_border::none;
    _tx->_bounds = rectangle(0.5, 0.5, 1, 1);
    return _tx;
}

rectangle remove_border(const rectangle& bound, const texture_border border){
    if(border != texture_border::none)
        return rectangle(bound.center().x(), bound.center().y(), bound.size().x() - 2 * TEXTURE_BORDER, bound.size().y() - 2 * TEXTURE_BORDER);
    else
        return bound;
}

bool texture_atlas::try_compile_group(uint32_t max_size, const rb_string& group_name, const std::vector<rb_string>& names, const std::vector<vec2>& sizes){
    vec2 space_available(max_size, max_size);
    std::vector<std::vector<std::pair<uint32_t, rectangle>>> _new_atlases;
    std::list<std::pair<uint32_t, vec2>> _l_sizes;
    std::vector<vec2> _v_sizes;
    std::vector<std::pair<uint32_t, rectangle>> _packed;
    std::vector<vec2> _atlas_sizes;
    uint32_t _packed_count = 0;
    
    uint32_t _offset = 0;
    for (auto& _s : sizes){
        _l_sizes.push_back({_offset, _s});
        _offset++;
    }
    
    while(true){
        bool _packed_one = false;
        _v_sizes.clear();
        _packed.clear();
        for(auto& _s : _l_sizes)
            _v_sizes.push_back(_s.second);
        
        auto _pack_results = texture_packer::pack_textures(_v_sizes, space_available, false);
        
        auto _it = _l_sizes.begin();
        for (uint32_t i = 0; i < _pack_results.texture_count(); i++) { //_pack_results.texture_count() == _v_sizes.size()
            if(_pack_results.has_been_packed(i)){
                _packed.push_back({_it->first, _pack_results.texture_rectangle(i).value()});
                _it = _l_sizes.erase(_it);
                _packed_one = true;
                _packed_count++;
            }
            else
                _it++;
        }
        _new_atlases.push_back(_packed);
        _atlas_sizes.push_back(_pack_results.filled_area());
        
        if(_l_sizes.size() == 0 || !_packed_one)
            break;
    }
    
    if(_packed_count != sizes.size()) //we failed to pack everyone
        return false;
    
    //we then go create our images
    CGContextData data;
    std::vector<texture*> _textures;
    bool _failed = false;
    for (uint32_t i = 0; i < _new_atlases.size(); i++) {
        if(!create_context(_atlas_sizes[i], data)){
            _failed = true;
            break;
        }
        for (uint32_t j = 0; j < _new_atlases[i].size(); j++) {
            auto _dest = _new_atlases[i][j];
            if(_texture_infos.at(names[_dest.first]).type == from_source ||
               _texture_infos.at(names[_dest.first]).type == composition) {
                if(!draw_image(&data, names[_dest.first], _dest.second)){
                    _failed = true;
                    break;
                }
            }
            else {
                if(!draw_dynamic_image(&data, names[_dest.first], _dest.second)){
                    _failed = true;
                    break;
                }
            }
        }
        if(_failed)
            break;
        texture* _tx = create_texture(&data, _atlas_sizes[i]);
        if(_tx == nullptr)
        {
            _failed = true;
            break;
        }
        _textures.push_back(_tx);
        for (uint32_t j = 0; j < _new_atlases[i].size(); j++) {
            auto& _t_name = names[_new_atlases[i][j].first];
            this->_texture_atlases[{group_name, _t_name}] = _tx;
        }
    }
    
    if(_failed && _textures.size() != 0){
        for (auto _t : _textures)
            delete _t;
    }
    
    if(_failed)
        return false;
    
    for(auto _t : _textures){
        this->_atlases.push_back(_t);
    }
    
    this->_group_atlases[group_name] = _textures;
    this->_bounds[group_name].clear();
    
    for (uint32_t i = 0; i < _new_atlases.size(); i++) {
        for (uint32_t j = 0; j < _new_atlases[i].size(); j++) {
            auto& _t_name = names[_new_atlases[i][j].first];
            this->_bounds[group_name].push_back({_t_name, remove_border(_new_atlases[i][j].second, _texture_infos.at(_t_name).border)});
        }
    }
    
    return true;
}

bool texture_atlas::try_compile(uint32_t max_size, const std::vector<rb_string>& names, const std::vector<vec2>& sizes){
    std::vector<rb_string> _names_by_group;
    std::vector<vec2> _sizes_by_group;
    for(auto& _grp : _group_names){
        _names_by_group.clear();
        _sizes_by_group.clear();
        
        for (uint32_t i = 0; i < names.size(); i++) {
            bool _in_group = false;
            auto _range = _group_textures.equal_range(_grp);
            for (auto _it = _range.first; _it != _range.second; _it++) {
                if(_it->second == names[i])
                {
                    _in_group = true;
                    break;
                }
            }
            if(_in_group){
                _names_by_group.push_back(names[i]);
                _sizes_by_group.push_back(sizes[i]);
            }
        }
        
        if(!try_compile_group(max_size, _grp, _names_by_group, _sizes_by_group))
            return false;
    }
    
    return true;
}

vec2 get_texture_size(const texture_source& source, const texture_border border, const float scale){
    assert(scale > 0);
    cg_resource myImageSourceRef(CGImageSourceCreateWithURL((__bridge CFURLRef)source.image_url(), NULL));
    if (!myImageSourceRef.resource()) {
        ERROR("failed loading texture reference: ", source);
        return vec2::zero;
    }
    
    CGFloat width = 0.0f, height = 0.0f;
    CFDictionaryRef imageProperties = CGImageSourceCopyPropertiesAtIndex((CGImageSourceRef)myImageSourceRef.resource(), 0, NULL);
    if (imageProperties != NULL) {
        CFNumberRef widthNum  = (CFNumberRef)CFDictionaryGetValue(imageProperties, kCGImagePropertyPixelWidth);
        if (widthNum != NULL) {
            CFNumberGetValue(widthNum, kCFNumberCGFloatType, &width);
        }
        
        CFNumberRef heightNum = (CFNumberRef)CFDictionaryGetValue(imageProperties, kCGImagePropertyPixelHeight);
        if (heightNum != NULL) {
            CFNumberGetValue(heightNum, kCFNumberCGFloatType, &height);
        }
        
        CFRelease(imageProperties);
    }
    
    if(width == 0 || height == 0)
        return vec2::zero;
    
    width *= scale;
    height *= scale;
    width = roundf(width);
    height = roundf(height);
    
    if(width < 3)
        width = 3;
    if(height < 3)
        height = 3;
    
    if(border != texture_border::none)
    {
        width += 2 * TEXTURE_BORDER;
        height += 2 * TEXTURE_BORDER;
    }
    
    return vec2(width, height);
}

bool texture_atlas::get_texture_sizes(const std::vector<rb_string>& names, std::vector<vec2>& sizes) {
    sizes.clear();
    for (uint32_t i = 0; i < names.size(); i++) {
        auto& _info = _texture_infos.at(names[i]);
        if(_info.type == from_source){
            auto _size = get_texture_size(*_info.source, _info.border, _info.scale);
            if(_size == vec2::zero)
                return false;
            sizes.push_back(_size);
        }
        else if(_info.type == composition){
            auto _size = get_texture_size(*_info.composition->base_texture(), _info.border, _info.composition->scale());
            if(_size == vec2::zero)
                return false;
            sizes.push_back(_size);
        }
        else {
            sizes.push_back(_info.dynamic_image_size);
        }
    }
    
    return true;
}

//compile the atlas...
void texture_atlas::compile(){
    if(!_dirty)
        return;
    
    if(_texture_names.size() == 0) //no textures
        return; //we return...
    
    GLint _max_size;
    GLint _min_size = 512;
    if(!_avoid_gl)
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &_max_size);
    else
        _max_size = 1024 * 8;
    
    if(_max_atlas_size != 0 && _max_size > _max_atlas_size)
        _max_size = _max_atlas_size;
    
    if(_max_size < _min_size)
        _max_size = _min_size;
    
    free_textures();
    
    std::vector<vec2> _sizes;
    std::vector<rb_string> _names;
    
    for(auto& _n : _texture_names)
        _names.push_back(_n);
    
    if(!get_texture_sizes(_names, _sizes))
        return;
    
    while(!try_compile(_max_size, _names, _sizes) && _max_size >= _min_size){
        _max_size /= 2;
        free_textures();
    }
    
    //we may not be able to compile
    if(_atlases.size() != 0){
        //we clear the dirty flag...
        _dirty = false;
    }
}

uint32_t texture_atlas::max_atlas_size() const{
    return _max_atlas_size;
}
uint32_t texture_atlas::max_atlas_size(const uint32_t value){
    assert(value >= 64 || value == 0);
    _max_atlas_size = value;
    return value;
}

uint32_t texture_atlas::atlas_count() const{
    if(_dirty)
        return 0;
    return (uint32_t)_atlases.size();
}
const texture_atlas::atlas_iterator texture_atlas::atlases_begin() const{
    assert(!_dirty);
    return _atlases.begin();
}
const texture_atlas::atlas_iterator texture_atlas::atlases_end() const{
    assert(!_dirty);
    return _atlases.end();
}
void texture_atlas::get_atlases(std::vector<texture*>& atlases) const{
    atlases.clear();
    
    if(_dirty)
        return;
    
    for(auto _a : _atlases)
        atlases.push_back(_a);
}
//return null if no texture with this name...
void texture_atlas::get_atlas(const rb_string& group_name, std::vector<texture*>& atlases) const{
    atlases.clear();
    
    if(_dirty)
        return;
    
    if(!contains_group(group_name))
        return;
    
    auto& _textures = _group_atlases.at(group_name);
    
    for(auto _it = _textures.begin(); _it != _textures.end(); _it++){
        atlases.push_back(*_it);
    }
}

const texture* texture_atlas::get_atlas(const rb_string& group_name, const rb_string& texture_name) const{
    if(_dirty)
        return nullptr;
    
    if(!contains_texture(texture_name) || !contains_group(group_name))
        return nullptr;
    
    bool _in_group = false;
    for (auto& _key : _group_textures){
        if(_key.first == group_name && _key.second == texture_name){
            _in_group = true;
            break;
        }
    }
    
    if(!_in_group)
        return nullptr;
    
    return _texture_atlases.at({group_name, texture_name});
}

const texture_map* texture_atlas::create_mapping(const rb_string& group_name, const rb_string& texture_name, const transform_space& transform, texture_map* refurbished){
    auto _r = dynamic_cast<simple_texture_map*>(refurbished);
    if(refurbished)
        assert(_r);
    
    auto _tx = get_atlas(group_name, texture_name);
    
    if(_tx == nullptr)
        return nullptr;
    
    rectangle _b;
    
    for (auto& _tb : _bounds.at(group_name)){
        if(_tb.texture_name == texture_name)
        {
            _b = _tb.bounds;
        }
    }
    
    //maybe we need to shift by half a pixel?
    _b.center(_b.center() + vec2(0, 0));
    
    //adjust bounds to normalized coordinates
    _b = rectangle(_b.center().x() / _tx->texture_size().x(), _b.center().y() / _tx->texture_size().y(), _b.size().x() / _tx->texture_size().x(), _b.size().y() / _tx->texture_size().y());
    
    if(_r) {
        _r->~simple_texture_map();
        return new (_r) simple_texture_map(transform, _b);
    }
    else
        return new simple_texture_map(transform, _b);
}

rectangle texture_atlas::get_bounds_in_pixels(const rb_string &group_name, const rb_string &texture_name) const {
    auto _tx = get_atlas(group_name, texture_name);
    
    assert(_tx != nullptr);
    
    rectangle _b;
    
    for (auto& _tb : _bounds.at(group_name)){
        if(_tb.texture_name == texture_name)
        {
            _b = _tb.bounds;
        }
    }
    
    //maybe we need to shift by half a pixel?
    _b.center(_b.center() + vec2(0, 0));
    
    return _b;
}

void texture_atlas::get_textures(const texture* atlas, std::vector<rb_string>& texture_names) const{
    texture_names.clear();
    if(_dirty)
        return;
    
    for (auto& _p : _texture_atlases){
        if(_p.second == atlas)
            texture_names.push_back(_p.first.second);
    }
}

void texture_atlas::serialize_group_names(xnode& n){
    xnode grps;
    grps.name(u"all-groups");
    std::unordered_map<texture*, int> _indexes;
    for (uint32_t i = 0; i < _atlases.size(); i++) {
        _indexes.insert({_atlases[i], i});
    }
    for (auto& _grp_name : _group_names){
        xnode g;
        g.name(u"group");
        g.set_attribute_value(u"name", _grp_name);
        
        
        xnode textures;
        textures.name(u"textures");
        
        //we then serialize textures
        auto _range = _group_textures.equal_range(_grp_name);
        for (auto _it = _range.first; _it != _range.second; ++_it) {
            xnode tex;
            tex.name(u"texture");
            tex.set_attribute_value(u"name", _it->second);
            auto _atlas = _texture_atlases.at(std::make_pair(_grp_name, _it->second));
            tex.set_attribute_value(u"atlas-index", rb::to_string((uint32_t)_indexes[_atlas]));
            auto& _vector_bounds = _bounds.at(_grp_name);
            rectangle _tex_bounds;
            for (auto& _tb : _vector_bounds){
                if(_tb.texture_name == _it->second){
                    _tex_bounds = _tb.bounds;
                    break;
                }
            }
            tex.set_attribute_value(u"center-x", rb::to_string((float)_tex_bounds.center().x()));
            tex.set_attribute_value(u"center-y", rb::to_string((float)_tex_bounds.center().y()));
            tex.set_attribute_value(u"size-x", rb::to_string((float)_tex_bounds.size().x()));
            tex.set_attribute_value(u"size-y", rb::to_string((float)_tex_bounds.size().y()));
            textures.children().push_back(tex);
        }
        g.children().push_back(textures);
        
        xnode _g_atlases;
        _g_atlases.name(u"atlases");
        
        for (auto& _it : _group_atlases.at(_grp_name)){
            xnode _a;
            _a.name(u"atlas");
            _a.set_attribute_value(u"index", rb::to_string((uint32_t)_indexes[_it]));
            _g_atlases.children().push_back(_a);
        }
        
        g.children().push_back(_g_atlases);
        grps.children().push_back(g);
    }
    n.children().push_back(grps);
}

void texture_atlas::serialize_texture_names(rb::xnode &n){
    xnode txts;
    txts.name(u"all-textures");
    for (auto& _t_name : _texture_names){
        xnode _t;
        _t.name(u"texture");
        _t.set_attribute_value(u"name", _t_name);
        _t.set_attribute_value(u"scale", rb::to_string((float)_texture_infos.at(_t_name).scale));
        _t.set_attribute_value(u"border", rb::to_string(_texture_infos.at(_t_name).border));
        txts.children().push_back(_t);
    }
    n.children().push_back(txts);
}

xnode texture_atlas::serialize_to_xnode(){
    xnode n;
    n.name(u"texture-atlas");
    n.set_attribute_value(u"max-atlas-size", rb::to_string(_max_atlas_size));
    n.set_attribute_value(u"atlas-count", rb::to_string((uint32_t)_atlases.size()));
    serialize_group_names(n);
    serialize_texture_names(n);
    return n;
}

void serialize_atlas(NSURL* output_directory, uint32_t index, const texture* t){
    NSString* _fileName = [[NSNumber numberWithUnsignedInt:index] stringValue];
    _fileName = [_fileName stringByAppendingString:@".png"];
    NSURL* _fullFileUrl = [output_directory URLByAppendingPathComponent:_fileName];
    t->save_to_file(_fullFileUrl);
}

void texture_atlas::save_to_directory(URL_TYPE directory_path){
    //we create the texture directory
    compile();
    NSURL* _dir_url = directory_path;
    NSURL* _tex_dir = [_dir_url URLByAppendingPathComponent:@"Textures" isDirectory:YES];
    [[NSFileManager defaultManager] createDirectoryAtURL:_tex_dir withIntermediateDirectories:YES attributes:nil error:nil];
    for (uint32_t i = 0; i < _atlases.size(); i++) {
        serialize_atlas(_tex_dir, i, _atlases[i]);
    }
    NSURL* _xml_url = [_dir_url URLByAppendingPathComponent:@"Atlas-Info.xml"];
    xnode _xml = serialize_to_xnode();
    _xml.write_to_url(_xml_url);
}

inline NSString* to_platform_string(const rb::rb_string& str){
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> _str_converter;
    auto _u8Str = _str_converter.to_bytes(str);
    NSString* _platform_str = [NSString stringWithUTF8String:_u8Str.c_str()];
    return _platform_str;
}

texture_atlas* texture_atlas::load_from_directory(URL_TYPE directory_path, const vec2& resolution){
    NSString* widthString = [[NSNumber numberWithInteger:resolution.x()] stringValue];
    NSString* heightString = [[NSNumber numberWithInteger:resolution.y()] stringValue];
    NSString* resolutionName = [NSString stringWithFormat:@"%@x%@", widthString, heightString];
    texture_atlas* _result = new texture_atlas(false);
    NSURL* _dir_url = directory_path;
    //_dir_url points to  the url of the texture atlas...
    _dir_url = [_dir_url URLByAppendingPathComponent:@"Atlases"];
    _dir_url = [_dir_url URLByAppendingPathComponent: resolutionName];
    NSURL* _tex_dir = [_dir_url URLByAppendingPathComponent:@"Textures" isDirectory:YES];
    NSURL* _xml_url = [_dir_url URLByAppendingPathComponent:@"Atlas-Info.xml"];
    xnode _xml = _xml.read_from_url(_xml_url);
    NSString* _some_str = to_platform_string(_xml.get_attribute_value(u"atlas-count"));
    uint32_t _atlas_count = (uint32_t)[_some_str integerValue];
    _some_str = to_platform_string(_xml.get_attribute_value(u"max-atlas-size"));
    _result->_max_atlas_size = (uint32_t)[_some_str integerValue];
    
    //load atlases
    for (uint32_t i = 0; i < _atlas_count; i++) {
        NSString* _fileName = [[NSNumber numberWithUnsignedInt:i] stringValue];
        _fileName = [_fileName stringByAppendingString:@".png"];
        NSURL* _fullFileUrl = [_tex_dir URLByAppendingPathComponent:_fileName];
        cg_texture_source _source = cg_texture_source(_fullFileUrl);
        texture* _tex = new texture(_source);
        _result->_atlases.push_back(_tex);
    }
    
    assert(_xml.children().size() == 2);
    xnode& _all_groups = _xml.get_children_by_tag_name(u"all-groups");
    xnode& _all_textures = _xml.get_children_by_tag_name(u"all-textures");
    for(auto& _grp : _all_groups.children()){
        _result->_group_names.insert(_grp.get_attribute_value(u"name"));
        xnode& _textures = _grp.get_children_by_tag_name(u"textures");
        for (auto& _t : _textures.children()){
            
            std::pair<rb_string, rb_string> _grp_t = {_grp.get_attribute_value(u"name"), _t.get_attribute_value(u"name")};
            _result->_texture_names.insert(_t.get_attribute_value(u"name"));
            _result->_group_textures.insert(_grp_t);
            
            _some_str = to_platform_string(_t.get_attribute_value(u"atlas-index"));
            _result->_texture_atlases.insert({_grp_t, _result->_atlases[[_some_str integerValue]]});
            
            float _center_x, _center_y, _size_x, _size_y;
            _some_str = to_platform_string(_t.get_attribute_value(u"center-x"));
            _center_x = [_some_str floatValue];
            _some_str = to_platform_string(_t.get_attribute_value(u"center-y"));
            _center_y = [_some_str floatValue];
            _some_str = to_platform_string(_t.get_attribute_value(u"size-x"));
            _size_x = [_some_str floatValue];
            _some_str = to_platform_string(_t.get_attribute_value(u"size-y"));
            _size_y = [_some_str floatValue];
            texture_bound tb = {_grp_t.second, rectangle(_center_x, _center_y, _size_x, _size_y)};
            _result->_bounds[_grp_t.first].push_back(tb);
        }
        
        xnode& _atlases = _grp.get_children_by_tag_name(u"atlases");
        for (auto& _a : _atlases.children()){
            _some_str = to_platform_string(_a.get_attribute_value(u"index"));
            _result->_group_atlases[_grp.get_attribute_value(u"name")].push_back(_result->_atlases[[_some_str integerValue]]);
        }
    }
    
    for(auto& _t : _all_textures.children()){
        auto _t_name = _t.get_attribute_value(u"name");
        assert(_result->_texture_names.count(_t_name) == 1);
        texture_info ti;
        _some_str = to_platform_string(_t.get_attribute_value(u"scale"));
        ti.scale = [_some_str floatValue];
        ti.type = unknown;
        auto _border = _t.get_attribute_value(u"border");
        if(_border == u"rb::texture_border::empty")
            ti.border = texture_border::empty;
        else if(_border == u"rb::texture_border::clamp")
            ti.border = texture_border::clamp;
        else if(_border == u"rb::texture_border::repeat")
            ti.border = texture_border::repeat;
        else
            ti.border = texture_border::none;
        auto _name_copy = _t_name;
        std::pair<rb_string, texture_info> _pair(std::move(_name_copy), std::move(ti));
        _result->_texture_infos.insert(std::move(_pair));
    }
    
    //undirty...
    _result->_dirty = false;
    return _result;
}






























