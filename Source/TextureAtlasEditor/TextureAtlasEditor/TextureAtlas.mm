//
//  TextureAtlas.m
//  TextureAtlasEditor
//
//  Created by Danilo Carvalho on 24/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "TextureAtlas.h"
#import "PrimitiveImage.h"
#import "KeyValuePair.h"
#include "Constants.h"
#include <ImageIO/ImageIO.h>
#define _FINAL_APP_
#include "texture_atlas.h"
#import "Texture.h"
#import "Resolutions.h"

CGSize GetImageSize(NSURL* url){
    CFURLRef urlRef = (__bridge CFURLRef)url;
    CGImageSourceRef myImageSourceRef = CGImageSourceCreateWithURL(urlRef, NULL);
    if (!myImageSourceRef) {
        NSLog(@"failed loading texture reference: %@", [url path]);
        return CGSizeMake(0, 0);
    }
    
    CGFloat width = 0.0f, height = 0.0f;
    CFDictionaryRef imageProperties = CGImageSourceCopyPropertiesAtIndex((CGImageSourceRef)myImageSourceRef, 0, NULL);
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
    
    if(width == 0 || height == 0){
        CFRelease(myImageSourceRef);
        return CGSizeMake(0, 0);
    }
    
    return CGSizeMake(width, height);
}

@interface TextureAtlas ()
-(void)loadPrimitiveImages;
-(void)alertDidEnd:(NSAlert *)alert returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo;
-(void)showErrorMessage:(NSString *)message informativeMessage:(NSString *)informativeMessage;
@end

@implementation TextureAtlas
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

-(void)loadPrimitiveImages{
    NSURL* imagesURL = [NSURL fileURLWithPath:IMAGE_DIR isDirectory:YES];
    NSURL* iPad3ImagesDir = [imagesURL URLByAppendingPathComponent:@"2048x1536" isDirectory:YES];
    NSURL* thumbnailURL = [NSURL fileURLWithPath:THUMBNAIL_DIR isDirectory:YES];
    NSURL* iPad3ThumbnailDir = [thumbnailURL URLByAppendingPathComponent:@"2048x1536" isDirectory:YES];
    NSFileManager* fileManager = [NSFileManager defaultManager];
    NSArray* filesInImagesDir = [fileManager contentsOfDirectoryAtURL:iPad3ImagesDir includingPropertiesForKeys:nil options:0 error:nil];
    //they should have the same number of files
    for (int i = 0; i < [filesInImagesDir count]; i++) {
        NSURL* fileURL = [filesInImagesDir objectAtIndex:i];
        NSURL* thumbnailURL = [iPad3ThumbnailDir URLByAppendingPathComponent:[fileURL lastPathComponent]];
        CGSize imageSize = GetImageSize(fileURL);
        [self.primitiveImages addObject:[[PrimitiveImage alloc] initWithThumbnailURL:thumbnailURL imageDirectoryURL:imagesURL width:imageSize.width height:imageSize.height]];
    }
}
@synthesize textures;
@synthesize primitiveImages;
@synthesize borderValues;
@synthesize blendModeValues;
-(id)init{
    if(self = [super init]){
        self.textures = [NSMutableArray array];
        self.primitiveImages = [NSMutableArray array];
        self.borderValues = [NSMutableArray
                             arrayWithObjects:
                             [KeyValuePair keyValuePairWithKey:@0 value:@"No Border"],
                             [KeyValuePair keyValuePairWithKey:@1 value:@"Empty Border"],
                             [KeyValuePair keyValuePairWithKey:@2 value:@"Clamp Edges"],
                             [KeyValuePair keyValuePairWithKey:@3 value:@"Repeat Texture"], nil];
        self.blendModeValues = [NSMutableArray arrayWithObjects:
                                [KeyValuePair keyValuePairWithKey:@1 value:@"Normal"],
                                [KeyValuePair keyValuePairWithKey:@2 value:@"Multiply"],
                                [KeyValuePair keyValuePairWithKey:@3 value:@"Screen"],
                                [KeyValuePair keyValuePairWithKey:@4 value:@"Overlay"],
                                [KeyValuePair keyValuePairWithKey:@5 value:@"Darken"],
                                [KeyValuePair keyValuePairWithKey:@6 value:@"Lighten"],
                                [KeyValuePair keyValuePairWithKey:@7 value:@"Hue"],
                                [KeyValuePair keyValuePairWithKey:@8 value:@"Saturation"],
                                [KeyValuePair keyValuePairWithKey:@9 value:@"Color"],
                                [KeyValuePair keyValuePairWithKey:@10 value:@"Luminosity"],
                                nil];
        [self loadPrimitiveImages];
    }
    return self;
}
-(NSUInteger)countOfTextures{
    return [self.textures count];
}
-(id)objectInTexturesAtIndex:(NSUInteger)index{
    return [self.textures objectAtIndex:index];
}
-(void)insertObject:(Texture *)object inTexturesAtIndex:(NSUInteger)index{
    return [self.textures insertObject:object atIndex:index];
}
-(void)removeObjectFromTexturesAtIndex:(NSUInteger)index{
    return [self.textures removeObjectAtIndex:index];
}
-(NSUInteger)countOfPrimitiveImages{
    return [self.primitiveImages count];
}
-(id)objectInPrimitiveImagesAtIndex:(NSUInteger)index{
    return [self.primitiveImages objectAtIndex:index];
}
-(void)insertObject:(PrimitiveImage *)object inPrimitiveImagesAtIndex:(NSUInteger)index{
    return [self.primitiveImages insertObject:object atIndex:index];
}
-(void)removeObjectFromPrimitiveImagesAtIndex:(NSUInteger)index{
    return [self.primitiveImages removeObjectAtIndex:index];
}
-(void)removeTexturesAtIndexes:(NSIndexSet *)indexes{
    [indexes enumerateIndexesUsingBlock:^(NSUInteger idx, BOOL *stop) {
        [[self.textures objectAtIndex:idx] removeAllStages];
        *stop = NO;
    }];
    [self.textures removeObjectsAtIndexes:indexes];
}
-(void)removeAllTextures{
    NSIndexSet* indexes = [NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, [self.textures count])];
    [self removeTexturesAtIndexes:indexes];
}
-(void)loadFromDirectoryURL:(NSURL*)url WithName:(NSString*)name{
    NSFileManager* fm = [[NSFileManager alloc] init];
    BOOL isDirectory;
    if(![fm fileExistsAtPath:[url path] isDirectory:&isDirectory]){
        [self showErrorMessage:@"Invalid directory." informativeMessage:@"The directory from where to load the texture atlas does not exists."];
        return;
    }
    if(!isDirectory){
        [self showErrorMessage:@"Invalid directory." informativeMessage:@"The directory from where to load the texture isn't a directory."];
        return;
    }
    
    if (![[name pathExtension] isEqualToString:@"bundle"]) {
        name = [name stringByAppendingPathExtension:@"bundle"];
    }
    NSURL* atlasDirectory = [url URLByAppendingPathComponent:name];
    if(![fm fileExistsAtPath:[atlasDirectory path] isDirectory:&isDirectory]){
        [self showErrorMessage:@"Invalid texture atlas." informativeMessage:@"The texture atlas does not exists."];
        return;
    }
    if(!isDirectory){
        [self showErrorMessage:@"Invalid texture atlas." informativeMessage:@"Invalid texture atlas format."];
        return;
    }
    
    NSURL* atlasInfoURL = [atlasDirectory URLByAppendingPathComponent:@"Editor-Info.plist"];
    NSData* contents = [NSData dataWithContentsOfURL:atlasInfoURL];
    NSKeyedUnarchiver* unarchiver = [[NSKeyedUnarchiver alloc] initForReadingWithData:contents];
    NSMutableArray* unarchivedTextures = [unarchiver decodeObjectForKey:@"textures"];
    [self removeAllTextures];
    for (int i = 0; i < [unarchivedTextures count]; i++) {
        Texture* txt = [unarchivedTextures objectAtIndex:i];
        [txt setUpWithTextureAtlas:self];
        [self insertObject:txt inTexturesAtIndex:[self countOfTextures]];
    }
    
}
-(void)saveToDirectoryURL:(NSURL *)url WithName:(NSString *)name {    
    NSFileManager* fm = [[NSFileManager alloc] init];
    BOOL isDirectory;
    if(![fm fileExistsAtPath:[url path] isDirectory:&isDirectory]){
        [self showErrorMessage:@"Invalid directory." informativeMessage:@"The directory where to save the texture atlas does not exists."];
        return;
    }
    if(!isDirectory){
        [self showErrorMessage:@"Invalid directory." informativeMessage:@"The directory where to save the texture isn't a directory."];
        return;
    }
    
    if (![[name pathExtension] isEqualToString:@"bundle"]) {
        name = [name stringByAppendingPathExtension:@"bundle"];
    }
    NSURL* atlasDirectory = [url URLByAppendingPathComponent:name];
    
    [fm createDirectoryAtURL:atlasDirectory withIntermediateDirectories:YES attributes:@{NSFileExtensionHidden: @YES} error:nil];
    //clear the directory
    NSArray* contentsOfDirectory = [fm contentsOfDirectoryAtURL:atlasDirectory includingPropertiesForKeys:nil options:0 error:nil];
    for (int i = 0; i < [contentsOfDirectory count]; i++) {
        [fm removeItemAtURL:[contentsOfDirectory objectAtIndex:i] error:nil];
    }
    //the we start archiving this atlas...
    NSMutableData* dataStore = [NSMutableData data];
    NSKeyedArchiver* archiver = [[NSKeyedArchiver alloc] initForWritingWithMutableData:dataStore];
    [archiver setOutputFormat:NSPropertyListXMLFormat_v1_0];
    [archiver encodeObject:[self textures] forKey:@"textures"];
    [archiver finishEncoding];
    NSURL* editorInfoURL = [atlasDirectory URLByAppendingPathComponent:@"Editor-Info.plist"];
    [dataStore writeToURL: editorInfoURL atomically:YES];
    
    NSURL* realAtlasDirectory = [atlasDirectory URLByAppendingPathComponent:@"Atlases"];
    
    for (int i = 0; i < [Resolutions resolutions].count; i++) {
        CGSize resolution = [[[Resolutions resolutions] objectAtIndex:i] sizeValue];
        NSString* resolutionName = [Resolutions resolutionName:resolution];
        NSURL* atlasResolutionDirectory = [realAtlasDirectory URLByAppendingPathComponent:resolutionName isDirectory:YES];
        [fm createDirectoryAtURL:atlasResolutionDirectory withIntermediateDirectories:YES attributes:nil error:nil];
        rb::texture_atlas* realAtlas = new rb::texture_atlas(true);
        realAtlas->max_atlas_size([Resolutions maxTextureAtlasSizeForResolution:resolution]);
        for (int j = 0; j < [self.textures count]; j++) {
            Texture* currentTexture = [self.textures objectAtIndex:j];
            [currentTexture addTextureToAtlas:realAtlas resolution:resolution];
        }
        realAtlas->save_to_directory(atlasResolutionDirectory);
        delete realAtlas;
    }
}
@end




















