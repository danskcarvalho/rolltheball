//
//  TextureAtlasLoader.m
//  RollerBallCore
//
//  Created by Danilo Carvalho on 24/06/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import "TextureAtlasLoader.h"
#include "vec2.h"
#include "texture_atlas.h"
#include "director.h"
#include "scene.h"
#include "node_container.h"
#include "node.h"
#import "Resolutions.h"
#ifdef IOS_TARGET
#import <UIKit/UIKit.h>
#endif
#import "CppBridgeProtocol.h"

using namespace rb;

@implementation TextureAtlasLoader
static __weak Class _bridgeClass;

+(Class)bridgeClass{
    return _bridgeClass;
}

+(void)setBridgeClass:(Class)bridgeClass{
    _bridgeClass = bridgeClass;
}

+(void*)loadTextureAtlas: (NSURL*)atlasUrl{
    CGSize bestResolution = [self bestResolution];
    vec2 _bestResolution = vec2(bestResolution.width, bestResolution.height);
    NSURL* url = nil;
    if([atlasUrl pathComponents].count == 1)
        url = [[NSBundle mainBundle] URLForResource:[[atlasUrl URLByDeletingPathExtension] lastPathComponent] withExtension:@"bundle"];
    else {
        if(director::in_editor()){
            url = atlasUrl;
#if defined(MACOSX_TARGET)
            if (![[NSFileManager defaultManager] fileExistsAtPath:url.path])
                url = [[NSBundle mainBundle] URLForResource:[[atlasUrl URLByDeletingPathExtension] lastPathComponent] withExtension:@"bundle"];
#endif
        }
        else {
            url = [[NSBundle mainBundle] URLForResource:[[atlasUrl URLByDeletingPathExtension] lastPathComponent] withExtension:@"bundle"];
        }
    }
    
    texture_atlas* atlas = texture_atlas::load_from_directory(url, _bestResolution);
    
    if(director::in_editor() && director::active_scene())
        director::active_scene()->atlas(atlas);
    
    if(director::in_editor()){
        [_bridgeClass textureAtlasChanged: const_cast<rb::texture_atlas*>(atlas) withURL:atlasUrl];
        
        if(director::active_scene()){
            auto _current = director::active_scene()->current();
            if (_current){
                std::vector<rb::node *> _selection;
                _current->fill_with_selection(_selection);
                
                if(_selection.size() != 0){
                    for (auto _n : _selection){
                        _n->notify_atlas_changed();
                    }
                }
            }
        }
    }
    
    return atlas;
}

+(CGSize)bestResolution{
#if defined(MACOSX_TARGET) || defined(TESTS_TARGET)
    CGSize currentResolution = [[NSScreen mainScreen] convertRectToBacking:[[NSScreen mainScreen] frame]].size;
#else
    CGSize currentResolution = [[UIScreen mainScreen] bounds].size;
    currentResolution.width *= [[UIScreen mainScreen] scale];
    currentResolution.height *= [[UIScreen mainScreen] scale];
#endif
    float currentArea = currentResolution.width * currentResolution.height;
    float maxDiff = std::numeric_limits<float>::max();
    CGSize finalResolution = currentResolution;
    for (int i = 0; i < [[Resolutions resolutions] count]; i++) {
#if defined(IOS_TARGET)
        CGSize resolution = [[[Resolutions resolutions] objectAtIndex:i] CGSizeValue];
#else
        CGSize resolution = [[[Resolutions resolutions] objectAtIndex:i] sizeValue];
#endif
        float area = resolution.width * resolution.height;
        float diff = fabsf(currentArea - area);
        if(diff < maxDiff)
        {
            maxDiff = diff;
            finalResolution = resolution;
        }
    }
    return finalResolution;
}
@end
