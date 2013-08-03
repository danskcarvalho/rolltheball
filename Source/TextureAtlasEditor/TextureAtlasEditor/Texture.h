//
//  Texture.h
//  TextureAtlasEditor
//
//  Created by Danilo Carvalho on 24/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>

@class TextureStage;
@class NSGroup;
@class PrimitiveImage;
@class TextureAtlas;

@interface Texture : NSObject < NSCoding >
@property(nonatomic, weak, readwrite)TextureAtlas* atlas;
@property(nonatomic, assign, readwrite)BOOL group00;
@property(nonatomic, assign, readwrite)BOOL group01;
@property(nonatomic, assign, readwrite)BOOL group02;
@property(nonatomic, assign, readwrite)BOOL group03;
@property(nonatomic, assign, readwrite)BOOL group04;
@property(nonatomic, assign, readwrite)BOOL group05;
@property(nonatomic, assign, readwrite)BOOL group06;
@property(nonatomic, assign, readwrite)BOOL group07;
@property(nonatomic, assign, readwrite)BOOL group08;
@property(nonatomic, assign, readwrite)BOOL group09;
@property(nonatomic, assign, readwrite)BOOL maskAggressively;
@property(nonatomic, strong, readwrite)NSString* sizeAsString;
@property(nonatomic, strong, readwrite)NSString* name;
@property(strong, nonatomic, readwrite)id border;
@property(nonatomic, strong, readwrite)NSColor* radialGrandientColor;
@property(nonatomic, strong, readwrite)NSString* type;
@property(nonatomic, strong, readwrite)NSMutableArray* stages;
@property(nonatomic, assign, readwrite)double scale;
@property(nonatomic, assign, readwrite)double sizeInBlocks;
@property(nonatomic, assign, readwrite)double width;
@property(nonatomic, assign, readwrite)double height;
@property(nonatomic, strong, readwrite)NSImage* finalTextureImage;
-(NSUInteger)countOfStages;
-(id)objectInStagesAtIndex:(NSUInteger)index;
-(void)insertObject:(TextureStage *)object inStagesAtIndex:(NSUInteger)index;
-(void)removeObjectFromStagesAtIndex:(NSUInteger)index;
-(void)removeStagesAtIndexes:(NSIndexSet *)indexes;
-(void)removeAllStages;
-(id)initWithImage:(PrimitiveImage*)image atlas:(TextureAtlas*)atlas;
-(void)updateFinalTextureImage;
-(id)initWithCoder:(NSCoder *)aDecoder;
-(void)encodeWithCoder:(NSCoder *)aCoder;
-(void)setUpWithTextureAtlas: (TextureAtlas*)atlas;
-(BOOL)anyGroup;
-(void)addTextureToAtlas:(void*)atlas resolution:(CGSize)resolution;
@end
