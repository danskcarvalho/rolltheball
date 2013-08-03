//
//  Texture.m
//  TextureAtlasEditor
//
//  Created by Danilo Carvalho on 24/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "Texture.h"
#import "PrimitiveImage.h"
#import "TextureStage.h"
#import "TextureAtlas.h"
#import "KeyValuePair.h"
#include "Constants.h"
#define _FINAL_APP_
#include "texture_atlas.h"
#include "composited_texture.h"
#include "texture_layer.h"
#include "texture_composition.h"
#include "radial_gradient_texture.h"
#include "color.h"
#include "cg_texture_source.h"
#include "transform_space.h"
#include "vec2.h"
#import "Resolutions.h"

@interface Texture ()
{
    NSString* fType;
    double fScale;
    double fSizeInBlocks;
    double fWidth;
    double fHeight;
    NSColor* fRadialGradientColor;
    NSMutableArray* fStages;
    BOOL fMaskAggressively;
}
-(void)updateTextureSize;
@end

@implementation Texture
@synthesize group00;
@synthesize group01;
@synthesize group02;
@synthesize group03;
@synthesize group04;
@synthesize group05;
@synthesize group06;
@synthesize group07;
@synthesize group08;
@synthesize group09;
@synthesize name;
@synthesize border;
@synthesize finalTextureImage;
@synthesize sizeAsString;

-(void)encodeWithCoder:(NSCoder *)aCoder{
    [aCoder encodeBool:self.group00 forKey:@"group00"];
    [aCoder encodeBool:self.group01 forKey:@"group01"];
    [aCoder encodeBool:self.group02 forKey:@"group02"];
    [aCoder encodeBool:self.group03 forKey:@"group03"];
    [aCoder encodeBool:self.group04 forKey:@"group04"];
    [aCoder encodeBool:self.group05 forKey:@"group05"];
    [aCoder encodeBool:self.group06 forKey:@"group06"];
    [aCoder encodeBool:self.group07 forKey:@"group07"];
    [aCoder encodeBool:self.group08 forKey:@"group08"];
    [aCoder encodeBool:self.group09 forKey:@"group09"];
    [aCoder encodeBool:self.maskAggressively forKey:@"maskAggressively"];
    [aCoder encodeObject:self.sizeAsString forKey:@"sizeAsString"];
    [aCoder encodeObject:self.name forKey:@"name"];
    [aCoder encodeObject:[self.border key] forKey:@"border"];
    [aCoder encodeObject:self.radialGrandientColor forKey:@"radialGradientColor"];
    [aCoder encodeObject:self.type forKey:@"type"];
    [aCoder encodeObject:self.stages forKey:@"stages"];
    [aCoder encodeDouble:self.scale forKey:@"scale"];
    [aCoder encodeDouble:self.sizeInBlocks forKey:@"sizeInBlocks"];
    [aCoder encodeDouble:self.width forKey:@"width"];
    [aCoder encodeDouble:self.height forKey:@"height"];
}
-(id)initWithCoder:(NSCoder *)aDecoder{
    if (self = [super init]) {
        self.group00 = [aDecoder decodeBoolForKey:@"group00"];
        self.group01 = [aDecoder decodeBoolForKey:@"group01"];
        self.group02 = [aDecoder decodeBoolForKey:@"group02"];
        self.group03 = [aDecoder decodeBoolForKey:@"group03"];
        self.group04 = [aDecoder decodeBoolForKey:@"group04"];
        self.group05 = [aDecoder decodeBoolForKey:@"group05"];
        self.group06 = [aDecoder decodeBoolForKey:@"group06"];
        self.group07 = [aDecoder decodeBoolForKey:@"group07"];
        self.group08 = [aDecoder decodeBoolForKey:@"group08"];
        self.group09 = [aDecoder decodeBoolForKey:@"group09"];
        fMaskAggressively = [aDecoder decodeBoolForKey:@"maskAggressively"];
        self.sizeAsString = [aDecoder decodeObjectForKey:@"sizeAsString"];
        self.name = [aDecoder decodeObjectForKey:@"name"];
        self.border = [aDecoder decodeObjectForKey:@"border"];
        fRadialGradientColor = [aDecoder decodeObjectForKey:@"radialGradientColor"];
        fType = [aDecoder decodeObjectForKey:@"type"];
        fStages = [aDecoder decodeObjectForKey:@"stages"];
        fScale = [aDecoder decodeDoubleForKey:@"scale"];
        fSizeInBlocks = [aDecoder decodeDoubleForKey:@"sizeInBlocks"];
        fWidth = [aDecoder decodeDoubleForKey:@"width"];
        fHeight = [aDecoder decodeDoubleForKey:@"height"];
    }
    return self;
}
-(void)setUpWithTextureAtlas:(TextureAtlas *)atlas{
    self.atlas = atlas;
    for (int i = 0; i < [self countOfStages]; i++) {
        [[self objectInStagesAtIndex:i] setUpWithTextureAtlas:atlas texture:self];
    }
    for (int i = 0; i < [atlas.borderValues count]; i++) {
        KeyValuePair* pair = [atlas.borderValues objectAtIndex:i];
        if ([pair.key isEqualToNumber:self.border]) {
            self.border = pair;
            break;
        }
    }
    [self updateTextureSize];
    [self updateFinalTextureImage];
}
-(void)setRadialGrandientColor:(NSColor *)radialGrandientColor{
    fRadialGradientColor = radialGrandientColor;
    [self updateFinalTextureImage];
}
-(NSColor *)radialGrandientColor{
    return fRadialGradientColor;
}
-(BOOL)anyGroup{
    return self.group00 || self.group01 || self.group02 || self.group03 || self.group04 || self.group05 || self.group06 || self.group07 || self.group08 || self.group09;
}
-(void)updateFinalTextureImage{
    [self updateTextureSize];
    if([self.type isEqualToString:@"Texture"]){
        if([self.stages count] == 0)
        {
            self.finalTextureImage = nil;
            return;
        }
        
        TextureStage* baseTexture = [self.stages objectAtIndex:0];
        rb::cg_texture_source* texture_source = (rb::cg_texture_source*)[baseTexture.primitiveImage thumbnailTextureSource];
        std::vector<rb::texture_layer*> layers;
        for (int i = 1; i < [self.stages count]; i++) {
            TextureStage* stageLayer = [self.stages objectAtIndex:i];
            if([stageLayer.type isEqualToString:@"Texture"]){
                rb::cg_texture_source* layer_texture_source = (rb::cg_texture_source*)[stageLayer.primitiveImage thumbnailTextureSource];
                rb::transform_space transform = rb::transform_space(rb::vec2([stageLayer translationX], [stageLayer translationY]), rb::vec2([stageLayer scaleX], [stageLayer scaleY]), -[stageLayer rotation]);
                layers.push_back(rb::texture_layer::from_texture([stageLayer opacity], (rb::texture_layer_blend_mode)[[(KeyValuePair*)[stageLayer blendMode] key] integerValue], [stageLayer repeat], transform, *layer_texture_source));
            }
            else {
                layers.push_back(rb::texture_layer::from_solid_color([stageLayer opacity], (rb::texture_layer_blend_mode)[[(KeyValuePair*)[stageLayer blendMode] key] integerValue], rb::color::from_rgba([stageLayer.color redComponent], [stageLayer.color greenComponent], [stageLayer.color blueComponent], [stageLayer.color alphaComponent])));
            }
        }
        rb::texture_composition* _composition = new rb::texture_composition(*texture_source, 1, self.maskAggressively, layers);
        rb::composited_texture* _composited_texture = new rb::composited_texture(_composition, (rb::texture_border)[[(KeyValuePair*)self.border key] integerValue], true);
        CGImageRef imageRef = (CGImageRef)_composited_texture->to_cg_image();
        self.finalTextureImage = [[NSImage alloc] initWithCGImage:imageRef size:NSZeroSize];
        CGImageRelease(imageRef);
        delete _composited_texture;
    }
    else {
        //we assume iPad 3 resolution
        rb::radial_gradient_texture* _texture = new rb::radial_gradient_texture(128, rb::color::from_rgba([self.radialGrandientColor redComponent], [self.radialGrandientColor greenComponent], [self.radialGrandientColor blueComponent], [self.radialGrandientColor alphaComponent]), true);
        CGImageRef imageRef = (CGImageRef)_texture->to_cg_image();
        self.finalTextureImage = [[NSImage alloc] initWithCGImage:imageRef size:NSZeroSize];
        CGImageRelease(imageRef);
        delete _texture;
    }
}
inline rb::rb_string from_platform_string(NSString* str){
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> _str_converter;
    return _str_converter.from_bytes([str UTF8String]);
}

-(void)addTextureToAtlas:(void*)atlas resolution:(CGSize)resolution{
    std::vector<rb::rb_string> groups;
    if(self.group00) groups.push_back(u"g00");
    if(self.group01) groups.push_back(u"g01");
    if(self.group02) groups.push_back(u"g02");
    if(self.group03) groups.push_back(u"g03");
    if(self.group04) groups.push_back(u"g04");
    if(self.group05) groups.push_back(u"g05");
    if(self.group06) groups.push_back(u"g06");
    if(self.group07) groups.push_back(u"g07");
    if(self.group08) groups.push_back(u"g08");
    if(self.group09) groups.push_back(u"g09");
    
    rb::texture_atlas* sAtlas = (rb::texture_atlas*)atlas;
    if([self.type isEqualToString:@"Texture"]){
        TextureStage* baseTexture = [self.stages objectAtIndex:0];
        rb::cg_texture_source* texture_source = (rb::cg_texture_source*)[baseTexture.primitiveImage textureSourceForResolution:resolution];
        std::vector<rb::texture_layer*> layers;
        for (int i = 1; i < [self.stages count]; i++) {
            TextureStage* stageLayer = [self.stages objectAtIndex:i];
            if([stageLayer.type isEqualToString:@"Texture"]){
                rb::cg_texture_source* layer_texture_source = (rb::cg_texture_source*)[stageLayer.primitiveImage textureSourceForResolution:resolution];
                rb::transform_space transform = rb::transform_space(rb::vec2([stageLayer translationX], [stageLayer translationY]), rb::vec2([stageLayer scaleX], [stageLayer scaleY]), -[stageLayer rotation]);
                layers.push_back(rb::texture_layer::from_texture([stageLayer opacity], (rb::texture_layer_blend_mode)[[(KeyValuePair*)[stageLayer blendMode] key] integerValue], [stageLayer repeat], transform, *layer_texture_source));
            }
            else {
                layers.push_back(rb::texture_layer::from_solid_color([stageLayer opacity], (rb::texture_layer_blend_mode)[[(KeyValuePair*)[stageLayer blendMode] key] integerValue], rb::color::from_rgba([stageLayer.color redComponent], [stageLayer.color greenComponent], [stageLayer.color blueComponent], [stageLayer.color alphaComponent])));
            }
        }
        rb::texture_composition* _composition = new rb::texture_composition(*texture_source, self.scale, self.maskAggressively, layers);
        rb::texture_border _border = (rb::texture_border)[[self.border key] integerValue];
        sAtlas->add_composited_texture(from_platform_string(self.name), groups, _composition, _border);
    }
    else {
        //we assume iPad 3 resolution
        sAtlas->add_radial_gradient_texture(from_platform_string(self.name), groups, [Resolutions blockSizeForResolution:resolution], rb::color::from_rgba([self.radialGrandientColor redComponent], [self.radialGrandientColor greenComponent], [self.radialGrandientColor blueComponent], [self.radialGrandientColor alphaComponent]));
    }
}
-(void)updateTextureSize{
    if([self.type isEqualToString:@"Texture"]){
        if([self.stages count] == 0){
            [self setWidth:0];
            [self setHeight:0];
        }
        else {
            TextureStage* baseTexture = [self objectInStagesAtIndex:0];
            double baseWidth = [baseTexture.primitiveImage width] * self.scale;
            double baseHeight = [baseTexture.primitiveImage height] * self.scale;
            if(baseWidth < 1)
                baseWidth = 1;
            if(baseHeight < 1)
                baseHeight = 1;
            [self setWidth:baseWidth];
            [self setHeight:baseHeight];
        }
    }
    else {
        double baseWidth = [self sizeInBlocks] * IPAD3_BLOCK_SIZE;
        double baseHeight = baseWidth;
        if(baseWidth < 1)
            baseWidth = 1;
        if(baseHeight < 1)
            baseHeight = 1;
        [self setWidth:baseWidth];
        [self setHeight:baseHeight];
    }
}
-(NSString *)type{
    return fType;
}
-(BOOL)maskAggressively{
    return fMaskAggressively;
}
-(void)setMaskAggressively:(BOOL)maskAggressively{
    fMaskAggressively = maskAggressively;
    [self updateFinalTextureImage];
}
-(void)setType:(NSString *)type{
    fType = type;
    [self updateTextureSize];
    [self updateFinalTextureImage];
}
-(void)setStages:(NSMutableArray *)stages{
    fStages = stages;
    [self updateTextureSize];
    [self updateFinalTextureImage];
}
-(NSMutableArray *)stages{
    return fStages;
}
-(void)setScale:(double)scale{
    fScale = scale;
    [self updateTextureSize];
}
-(double)scale{
    return fScale;
}
-(void)setSizeInBlocks:(double)sizeInBlocks{
    if(sizeInBlocks > 12)
        sizeInBlocks = 12;
    fSizeInBlocks = sizeInBlocks;
    [self updateTextureSize];
}
-(double)sizeInBlocks{
    return fSizeInBlocks;
}
-(double)width{
    return fWidth;
}
-(void)setWidth:(double)width{
    NSString* sizeStr = [NSString stringWithFormat:@"%.0f", width];
    sizeStr = [sizeStr stringByAppendingString:@" X "];
    sizeStr = [sizeStr stringByAppendingFormat:@"%.0f", fHeight];
    [self setSizeAsString:sizeStr];
    fWidth = width;
}
-(double)height{
    return fHeight;
}
-(void)setHeight:(double)height{
    NSString* sizeStr = [NSString stringWithFormat:@"%.0f", fWidth];
    sizeStr = [sizeStr stringByAppendingString:@" X "];
    sizeStr = [sizeStr stringByAppendingFormat:@"%.0f", height];
    [self setSizeAsString:sizeStr];
    fHeight = height;
}
-(id)initWithImage:(PrimitiveImage*)image atlas:(TextureAtlas *)atlas {
    if (self = [super init]) {
        fStages = [NSMutableArray array];
        fType = @"Texture";
        fSizeInBlocks = 1;
        fRadialGradientColor = [NSColor colorWithSRGBRed:1 green:1 blue:1 alpha:1];
        fScale = 1;
        self.border = [atlas.borderValues objectAtIndex:0];
        self.group00 = YES;
        self.group01 = self.group02 = self.group03 = self.group04 = self.group05 = self.group06 = self.group07 = self.group08 = self.group09 = NO;
        self.name = nil;
        self.atlas = atlas;
        self.maskAggressively = NO;
        TextureStage* baseStage = [[TextureStage alloc] initWithTexture:self];
        [baseStage setBase:YES];
        [baseStage setPrimitiveImage:image];
        [fStages addObject:baseStage];
        [self updateTextureSize];
        [self updateFinalTextureImage];
    }
    return self;
}
-(NSUInteger)countOfStages{
    return [self.stages count];
}
-(id)objectInStagesAtIndex:(NSUInteger)index{
    return [self.stages objectAtIndex:index];
}
-(void)insertObject:(TextureStage *)object inStagesAtIndex:(NSUInteger)index{
    [self.stages insertObject:object atIndex:index];
    [self updateTextureSize];
    [self updateFinalTextureImage];
}
-(void)removeObjectFromStagesAtIndex:(NSUInteger)index{
    [self.stages removeObjectAtIndex:index];
    [self updateTextureSize];
    [self updateFinalTextureImage];
}
-(void)removeStagesAtIndexes:(NSIndexSet *)indexes{
    [self.stages removeObjectsAtIndexes:indexes];
}
-(void)removeAllStages{
    NSIndexSet* indexes = [NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, [self.stages count])];
    [self removeStagesAtIndexes:indexes];
}

@end
