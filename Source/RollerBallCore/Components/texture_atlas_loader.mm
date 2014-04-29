//
//  texture_atlas_loader.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 26/06/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#include "texture_atlas_loader.h"
#include "texture_atlas.h"
#include "director.h"
#include "scene.h"
#import "TextureAtlasLoader.h"

using namespace rb;

texture_atlas* texture_atlas_loader::_editor_atlas = nullptr;
std::unordered_map<texture_atlas*, rb_string> texture_atlas_loader::_inv_map;
std::unordered_map<rb_string, texture_atlas*> texture_atlas_loader::_map;
std::unordered_map<rb_string, texture_atlas*> texture_atlas_loader::_to_be_deleted;
std::unordered_map<rb_string, uint32_t> texture_atlas_loader::_ref_count;
uint32_t texture_atlas_loader::_max_unused_atlases = 4;

inline NSString* to_platform_string(const rb::rb_string& str){
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> _str_converter;
    auto _u8Str = _str_converter.to_bytes(str);
    NSString* _platform_str = [NSString stringWithUTF8String:_u8Str.c_str()];
    return _platform_str;
}

inline rb::rb_string from_platform_string(NSString* str){
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> _str_converter;
    return _str_converter.from_bytes([str UTF8String]);
}

void texture_atlas_loader::release_atlases(){
    if(_to_be_deleted.size() >= _max_unused_atlases){
        for (auto _p : _to_be_deleted){
            delete _p.second;
        }
        _to_be_deleted.clear();
    }
}

texture_atlas* texture_atlas_loader::load_atlas(const rb_string &url){
    release_atlases();
    
    NSString* urlStr = to_platform_string(url);
    NSString* expandedStr = [urlStr stringByExpandingTildeInPath];
    NSURL* originalUrl = [NSURL fileURLWithPath: expandedStr];
    urlStr = [originalUrl path];
    originalUrl = [NSURL fileURLWithPath:urlStr isDirectory:YES];
    NSURL* urlAtlas = [NSURL fileURLWithPath: urlStr];
    if([[[urlAtlas pathComponents] objectAtIndex:1] isEqualToString:[originalUrl host]])
    {
        NSMutableArray* array = [NSMutableArray arrayWithArray:[urlAtlas pathComponents]];
        [array removeObjectAtIndex:1];
        urlStr = [NSString pathWithComponents:array];
        urlAtlas = [NSURL fileURLWithPath: urlStr];
    }
    
    rb_string _atlas_path = from_platform_string([urlStr stringByAbbreviatingWithTildeInPath]);
    if(!director::in_editor()){
        if(_map.count(_atlas_path) != 0){
            _ref_count[_atlas_path]++;
            return _map[_atlas_path];
        }
        
        if(_to_be_deleted.count(_atlas_path) != 0){
            _ref_count.insert({_atlas_path, 1});
            _map.insert({_atlas_path, _to_be_deleted[_atlas_path]});
            _inv_map.insert({_to_be_deleted[_atlas_path], _atlas_path});
            _to_be_deleted.erase(_atlas_path);
            return _map[_atlas_path];
        }
    }
    
    texture_atlas* _loaded = (texture_atlas*)[TextureAtlasLoader loadTextureAtlas: urlAtlas];
    
    if(director::in_editor()){
        if (_editor_atlas){
            delete _editor_atlas;
            _editor_atlas = nullptr;
        }
        _editor_atlas = _loaded;
        return _editor_atlas;
    }
    else {
        _ref_count.insert({_atlas_path, 1});
        _map.insert({_atlas_path, _loaded});
        _inv_map.insert({_loaded, _atlas_path});
        return _loaded;
    }
}

void texture_atlas_loader::release_atlas(rb::texture_atlas **atlas){
    if(!atlas)
        return;
    
    if(!(*atlas))
        return;
    
    if(director::in_editor()){
        if(_editor_atlas == *atlas){
            delete _editor_atlas;
            _editor_atlas = nullptr;
            *atlas = nullptr;
        }
    }
    else {
        auto _atlas_path = _inv_map[*atlas];
        _ref_count[_atlas_path]--;
        auto _c = _ref_count[_atlas_path];
        if(_c == 0){
            _map.erase(_atlas_path);
            _inv_map.erase(*atlas);
            _ref_count.erase(_atlas_path);
            _to_be_deleted.insert({_atlas_path, *atlas});
        }
        *atlas = nullptr;
    }
}


































