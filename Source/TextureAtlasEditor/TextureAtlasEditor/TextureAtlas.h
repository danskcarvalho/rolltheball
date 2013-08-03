//
//  TextureAtlas.h
//  TextureAtlasEditor
//
//  Created by Danilo Carvalho on 24/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>

@class Texture;
@class PrimitiveImage;

@interface TextureAtlas : NSObject
@property(strong, nonatomic, readwrite)NSMutableArray* textures;
@property(strong, nonatomic, readwrite)NSMutableArray* primitiveImages;
//borders
@property(nonatomic, readwrite, strong)NSMutableArray* borderValues;
//blend modes
@property(nonatomic, readwrite, strong)NSMutableArray* blendModeValues;
-(id)init;
-(NSUInteger)countOfTextures;
-(id)objectInTexturesAtIndex:(NSUInteger)index;
-(void)insertObject:(Texture *)object inTexturesAtIndex:(NSUInteger)index;
-(void)removeObjectFromTexturesAtIndex:(NSUInteger)index;
-(void)removeTexturesAtIndexes:(NSIndexSet *)indexes;
-(void)removeAllTextures;
-(NSUInteger)countOfPrimitiveImages;
-(id)objectInPrimitiveImagesAtIndex:(NSUInteger)index;
-(void)insertObject:(PrimitiveImage *)object inPrimitiveImagesAtIndex:(NSUInteger)index;
-(void)removeObjectFromPrimitiveImagesAtIndex:(NSUInteger)index;
-(void)saveToDirectoryURL:(NSURL*)url WithName:(NSString*)name;
-(void)loadFromDirectoryURL:(NSURL*)url WithName:(NSString*)name;
@end
