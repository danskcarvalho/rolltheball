//
//  PrimitiveImage.h
//  TextureAtlasEditor
//
//  Created by Danilo Carvalho on 24/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface PrimitiveImage : NSObject
-(id)initWithThumbnailURL: (NSURL*) thumbnailURL imageDirectoryURL:(NSURL*)imageDirectoryURL width:(NSInteger)width height:(NSInteger)height;
@property(strong, nonatomic) NSImage* image;
@property(nonatomic, strong, readwrite) NSMutableDictionary* textureSources;
@property(nonatomic, readonly) void* thumbnailTextureSource;
@property(strong, nonatomic) NSURL* url;
@property(assign, nonatomic) NSInteger width;
@property(assign, nonatomic) NSInteger height;
-(void)dealloc;
-(void*)textureSourceForResolution:(CGSize)resolution;
@end
