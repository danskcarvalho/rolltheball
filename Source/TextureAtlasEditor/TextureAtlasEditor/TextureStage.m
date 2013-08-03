//
//  TextureStage.m
//  TextureAtlasEditor
//
//  Created by Danilo Carvalho on 29/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "TextureStage.h"
#import "Texture.h"
#import "TextureAtlas.h"
#import "PrimitiveImage.h"
#import "KeyValuePair.h"

@interface TextureStage ()
{
    BOOL fUpdatingScale;
    PrimitiveImage* fPrimitiveImage;
    double fOpacity;
    NSColor* fColor;
    NSString* fType;
    id fBlendMode;
    BOOL fRepeat;
    double fTranslationX;
    double fTranslationY;
    double fScaleX;
    double fScaleY;
    double fRotation;
    BOOL fUniformResizing;
    NSString* fPrimitiveImageStr;
}
@end

@implementation TextureStage
@synthesize texture;
@synthesize base;
@synthesize enableTextureControls;
-(BOOL)uniformResizing{
    return fUniformResizing;
}
-(void)setUniformResizing:(BOOL)uniformResizing{
    fUniformResizing = uniformResizing;
    if(fUniformResizing)
        [self setScaleY:fScaleX];
}
-(PrimitiveImage *)primitiveImage{
    return fPrimitiveImage;
}
-(void)setPrimitiveImage:(PrimitiveImage *)primitiveImage{
    fPrimitiveImage = primitiveImage;
    [self.texture updateFinalTextureImage];
}
-(double)opacity{
    return fOpacity;
}
-(void)setOpacity:(double)opacity{
    fOpacity = opacity;
    [self.texture updateFinalTextureImage];
}
-(NSColor *)color{
    return fColor;
}
-(void)setColor:(NSColor *)color{
    fColor = color;
    [self.texture updateFinalTextureImage];
}
-(NSString *)type{
    return fType;
}
-(void)setType:(NSString *)type{
    fType = type;
    if([fType isEqualToString:@"Texture"]){
        [self setEnableTextureControls:YES];
    }
    else {
        [self setEnableTextureControls:NO];
    }
    [self.texture updateFinalTextureImage];
}
-(id)blendMode{
    return fBlendMode;
}
-(void)setBlendMode:(id)blendMode{
    fBlendMode = blendMode;
    [self.texture updateFinalTextureImage];
}
-(BOOL)repeat{
    return fRepeat;
}
-(void)setRepeat:(BOOL)repeat{
    fRepeat = repeat;
    [self.texture updateFinalTextureImage];
}
-(double)translationX{
    return fTranslationX;
}
-(void)setTranslationX:(double)translationX{
    fTranslationX = translationX;
    [self.texture updateFinalTextureImage];
}
-(double)translationY{
    return fTranslationY;
}
-(void)setTranslationY:(double)translationY{
    fTranslationY = translationY;
    [self.texture updateFinalTextureImage];
}
-(double)scaleX{
    return fScaleX;
}
-(void)setScaleX:(double)scaleX{
    fScaleX = scaleX;
    if(fUniformResizing && !fUpdatingScale){
        fUpdatingScale = YES;
        [self setScaleY:scaleX];
        fUpdatingScale = NO;
    }
    else
        [self.texture updateFinalTextureImage];
}
-(double)scaleY{
    return fScaleY;
}
-(void)setScaleY:(double)scaleY{
    fScaleY = scaleY;
    if(fUniformResizing && !fUpdatingScale){
        fUpdatingScale = YES;
        [self setScaleX:scaleY];
        fUpdatingScale = NO;
    }
    else
        [self.texture updateFinalTextureImage];
}
-(double)rotation{
    return fRotation;
}
-(void)setRotation:(double)rotation{
    fRotation = rotation;
    [self.texture updateFinalTextureImage];
}
-(id)initWithTexture:(Texture*)tex {
    if(self = [super init]){
        self.texture = tex;
        self.base = NO;
        fPrimitiveImage = nil;
        fOpacity = 1;
        fColor = [NSColor colorWithSRGBRed:1 green:1 blue:1 alpha:1];
        fType = @"Texture";
        fRepeat = NO; // working
        fTranslationX = fTranslationY = 0; //working
        fRotation = 0; //working
        fScaleX = fScaleY = 1; //working
        fBlendMode = [[self.texture.atlas blendModeValues] objectAtIndex:0];
        self.enableTextureControls = YES;
        if([tex.stages count] == 1)
            self.first = YES;
        else
            self.first = NO;
        if([tex.stages count] >= 1)
            self.last = YES;
        else
            self.last = NO;
        fUpdatingScale = NO;
    }
    
    return self;
}
-(void)deleteMe{
    if(self.base)
        return;
    NSUInteger myIndex = [self.texture.stages indexOfObject:self];
    [self.texture removeObjectFromStagesAtIndex:[self.texture.stages indexOfObject:self]];
    if(myIndex < [self.texture.stages count]){
        //we then adjust the last one
        TextureStage* currentObject = [self.texture.stages objectAtIndex:myIndex];
        
        if (self.first) {
            currentObject.first = YES;
        }
    }
    if (self.last) {
        TextureStage* previous = [self.texture.stages objectAtIndex:myIndex - 1];
        if(!previous.base){
            previous.last = YES;
        }
    }
}
-(void)alertDidEnd:(NSAlert *)alert returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo{
    
}
-(void)showErrorMessage:(NSString *)message informativeMessage:(NSString *)informativeMessage{
    NSAlert* alert = [[NSAlert alloc] init];
    [alert addButtonWithTitle:@"OK"];
    [alert setMessageText:message];
    [alert setInformativeText:informativeMessage];
    [alert setAlertStyle:NSCriticalAlertStyle];
    [alert beginSheetModalForWindow:[[NSApplication sharedApplication] keyWindow] modalDelegate:self didEndSelector:@selector(alertDidEnd:returnCode:contextInfo:) contextInfo:NULL];
}
-(void)resetTexture:(NSIndexSet*)indexes{
    if([indexes count] == 0){
        [self showErrorMessage:@"No Image Selected" informativeMessage:@"You need to select an image before continuing with this operation."];
    }
    else {
        self.primitiveImage = [self.texture.atlas objectInPrimitiveImagesAtIndex:[indexes firstIndex]];
    }
}
-(void)up{
    if(self.first || self.base)
        return;
    NSUInteger myIndex = [self.texture.stages indexOfObject:self];
    if(myIndex <= 1)
        return;
    TextureStage* previous = [self.texture objectInStagesAtIndex:(myIndex - 1)];
    [self.texture removeObjectFromStagesAtIndex:(myIndex - 1)];
    [self.texture insertObject:previous inStagesAtIndex:(myIndex)];
    if(previous.first){
        previous.first = NO;
        self.first = YES;
    }
    if(self.last) {
        self.last = NO;
        previous.last = YES;
    }
    
}
-(void)down{
    if(self.last || self.base)
        return;
    NSUInteger myIndex = [self.texture.stages indexOfObject:self];
    TextureStage* next = [self.texture objectInStagesAtIndex:(myIndex + 1)];
    [self.texture removeObjectFromStagesAtIndex:(myIndex + 1)];
    [self.texture insertObject:next inStagesAtIndex:(myIndex)];
    if(self.first){
        next.first = YES;
        self.first = NO;
    }
    if(next.last){
        self.last = YES;
        next.last = NO;
    }
}
-(void)encodeWithCoder:(NSCoder *)aCoder{
    [aCoder encodeBool:self.first forKey:@"first"];
    [aCoder encodeBool:self.last forKey:@"last"];
    [aCoder encodeObject:self.texture forKey:@"texture"];
    [aCoder encodeBool:self.enableTextureControls forKey:@"enableTextureControls"];
    [aCoder encodeBool:self.base forKey:@"base"];
    [aCoder encodeObject:[self.primitiveImage.url lastPathComponent] forKey:@"primitiveImage"];
    [aCoder encodeDouble:self.opacity forKey:@"opacity"];
    [aCoder encodeObject:self.color forKey:@"color"];
    [aCoder encodeObject:self.type forKey:@"type"];
    [aCoder encodeObject:[self.blendMode key] forKey:@"blendMode"];
    [aCoder encodeBool:self.repeat forKey:@"repeat"];
    [aCoder encodeDouble:self.translationX forKey:@"translationX"];
    [aCoder encodeDouble:self.translationY forKey:@"translationY"];
    [aCoder encodeDouble:self.scaleX forKey:@"scaleX"];
    [aCoder encodeDouble:self.scaleY forKey:@"scaleY"];
    [aCoder encodeDouble:self.rotation forKey:@"rotation"];
    [aCoder encodeBool:self.uniformResizing forKey:@"uniformResizing"];
}
-(id)initWithCoder:(NSCoder *)aDecoder{
    if(self = [super init]){
        self.first = [aDecoder decodeBoolForKey:@"first"];
        self.last = [aDecoder decodeBoolForKey:@"last"];
        self.texture = [aDecoder decodeObjectForKey:@"texture"];
        self.enableTextureControls = [aDecoder decodeBoolForKey:@"enableTextureControls"];
        self.base = [aDecoder decodeBoolForKey:@"base"];
        fPrimitiveImageStr = [aDecoder decodeObjectForKey:@"primitiveImage"];
        fOpacity = [aDecoder decodeDoubleForKey:@"opacity"];
        fColor = [aDecoder decodeObjectForKey:@"color"];
        fType = [aDecoder decodeObjectForKey:@"type"];
        fBlendMode = [aDecoder decodeObjectForKey:@"blendMode"];
        fRepeat = [aDecoder decodeBoolForKey:@"repeat"];
        fTranslationX = [aDecoder decodeDoubleForKey:@"translationX"];
        fTranslationY = [aDecoder decodeDoubleForKey:@"translationY"];
        fScaleX = [aDecoder decodeDoubleForKey:@"scaleX"];
        fScaleY = [aDecoder decodeDoubleForKey:@"scaleY"];
        fRotation = [aDecoder decodeDoubleForKey:@"rotation"];
        fUniformResizing = [aDecoder decodeBoolForKey:@"uniformResizing"];
        fUpdatingScale = NO;
    }
    return self;
}
-(void)setUpWithTextureAtlas:(TextureAtlas *)atlas texture:(Texture *)t{
    fPrimitiveImage = nil;
    for (int i = 0; i < [atlas.primitiveImages count]; i++) {
        PrimitiveImage* img = [atlas.primitiveImages objectAtIndex:i];
        if([[img.url lastPathComponent] isEqualToString:fPrimitiveImageStr]){
            fPrimitiveImage = img;
            break;
        }
    }
    if (!fPrimitiveImage) {
        fPrimitiveImage = [atlas.primitiveImages objectAtIndex:0];
    }
    for (int i = 0; i < [atlas.blendModeValues count]; i++) {
        KeyValuePair* pair = [atlas.blendModeValues objectAtIndex:i];
        if ([pair.key isEqualToNumber:self.blendMode]) {
            fBlendMode = pair;
            break;
        }
    }
}
@end


















