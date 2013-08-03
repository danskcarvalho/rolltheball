//
//  TextureStage.h
//  TextureAtlasEditor
//
//  Created by Danilo Carvalho on 29/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>

@class PrimitiveImage;
@class Texture;
@class TextureAtlas;

@interface TextureStage : NSObject <NSCoding>
@property(nonatomic, assign, readwrite)BOOL uniformResizing;
@property(nonatomic, assign, readwrite)BOOL first;
@property(nonatomic, assign, readwrite)BOOL last;
@property(nonatomic, weak, readwrite)Texture* texture;
@property(nonatomic, assign, readwrite)BOOL enableTextureControls;
@property(nonatomic, assign, readwrite, getter = isBase) BOOL base;
@property(nonatomic, strong, readwrite) PrimitiveImage* primitiveImage;
@property(nonatomic, assign, readwrite)double opacity;
@property(strong, nonatomic, readwrite)NSColor* color;
@property(strong, nonatomic, readwrite)NSString* type;
@property(strong, nonatomic, readwrite)id blendMode;
@property(assign, nonatomic, readwrite)BOOL repeat;
@property(assign, nonatomic, readwrite)double translationX;
@property(assign, nonatomic, readwrite)double translationY;
@property(assign, nonatomic, readwrite)double rotation;
@property(assign, nonatomic, readwrite)double scaleX;
@property(assign, nonatomic, readwrite)double scaleY;
-(id)initWithTexture:(Texture*)texture;
-(void)deleteMe;
-(void)resetTexture: (NSIndexSet*)indexes;
-(void)up;
-(void)down;
-(void)setUpWithTextureAtlas: (TextureAtlas*)atlas texture:(Texture*)t;
-(void)encodeWithCoder:(NSCoder *)aCoder;
-(id)initWithCoder:(NSCoder *)aDecoder;
@end
