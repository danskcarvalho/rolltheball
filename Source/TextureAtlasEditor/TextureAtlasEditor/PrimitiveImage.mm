//
//  PrimitiveImage.m
//  TextureAtlasEditor
//
//  Created by Danilo Carvalho on 24/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "PrimitiveImage.h"
#define _FINAL_APP_
#include "cg_texture_source.h"
#import "Resolutions.h"

@interface PrimitiveImage ()
@property(nonatomic, readwrite)void* thumbnailTextureSource;
@end

@implementation PrimitiveImage
@synthesize image;
@synthesize height;
@synthesize width;
@synthesize thumbnailTextureSource;
@synthesize textureSources;
-(id)initWithThumbnailURL: (NSURL*) thumbnailURL imageDirectoryURL:(NSURL *)imageDirectoryURL width:(NSInteger)w height:(NSInteger)h{
    if(self = [super init]){
        self.url = thumbnailURL;
        self.image = [[NSImage alloc] initWithContentsOfURL:thumbnailURL];
        self.width = w;
        self.height = h;
        self.thumbnailTextureSource = new rb::cg_texture_source(thumbnailURL);
        self.textureSources = [NSMutableDictionary dictionary];
        for (int i = 0; i < [[Resolutions resolutions] count]; i++) {
            CGSize resolution = [[[Resolutions resolutions] objectAtIndex:i] sizeValue];
            NSString* resolutionName = [Resolutions resolutionName:resolution];
            NSString* imageName = [thumbnailURL lastPathComponent];
            NSURL* imageURL = [[imageDirectoryURL URLByAppendingPathComponent:resolutionName isDirectory:YES] URLByAppendingPathComponent: imageName];
            void* source = new rb::cg_texture_source(imageURL);
            NSValue* resolutionObject = [[Resolutions resolutions] objectAtIndex:i];
            [self.textureSources setObject:[NSValue valueWithPointer:source] forKey:resolutionObject];
        }
    }
    return self;
}
-(void)dealloc{
    delete (rb::cg_texture_source*)self.thumbnailTextureSource;
    NSArray* resolutions = [self.textureSources allKeys];
    for (int i = 0; i < [resolutions count]; i++) {
        rb::cg_texture_source* source = (rb::cg_texture_source*)[[self.textureSources objectForKey:[resolutions objectAtIndex:i]] pointerValue];
        delete source;
    }
    
}
-(void *)textureSourceForResolution:(CGSize)resolution{
    NSValue* resolutionObject = [NSValue valueWithSize:resolution];
    return [[self.textureSources objectForKey:resolutionObject] pointerValue];
}
@end
