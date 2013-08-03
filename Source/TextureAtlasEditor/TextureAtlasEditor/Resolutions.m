//
//  Resolutions.m
//  TextureAtlasEditor
//
//  Created by Danilo Carvalho on 22/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "Resolutions.h"
#ifdef IOS_TARGET
#import <UIKit/UIKit.h>
#endif

@implementation Resolutions
static NSArray* internalResolutions;
+(void)initialize{
    if([self class] == [Resolutions class]){
#if !defined(IOS_TARGET)
        internalResolutions = @[[NSValue valueWithSize:CGSizeMake(480, 320)],
        [NSValue valueWithSize:CGSizeMake(960, 640)],
        [NSValue valueWithSize:CGSizeMake(1024, 768)],
        [NSValue valueWithSize:CGSizeMake(1280, 720)],
        [NSValue valueWithSize:CGSizeMake(1920, 1080)],
        [NSValue valueWithSize:CGSizeMake(2048, 1536)],
        [NSValue valueWithSize:CGSizeMake(2560, 1600)],
        [NSValue valueWithSize:CGSizeMake(2880, 1800)]];
#else
        internalResolutions = @[[NSValue valueWithCGSize:CGSizeMake(480, 320)],
                                [NSValue valueWithCGSize:CGSizeMake(960, 640)],
                                [NSValue valueWithCGSize:CGSizeMake(1024, 768)],
                                [NSValue valueWithCGSize:CGSizeMake(1280, 720)],
                                [NSValue valueWithCGSize:CGSizeMake(1920, 1080)],
                                [NSValue valueWithCGSize:CGSizeMake(2048, 1536)],
                                [NSValue valueWithCGSize:CGSizeMake(2560, 1600)],
                                [NSValue valueWithCGSize:CGSizeMake(2880, 1800)]];
#endif
    }
}
+(NSArray *)resolutions{
    return internalResolutions;
}
+(float)maxTextureAtlasSizeForResolution:(CGSize)resolution{
    if(CGSizeEqualToSize(resolution, CGSizeMake(480, 320)))
        return 1024;
    else if(CGSizeEqualToSize(resolution, CGSizeMake(960, 640)) || CGSizeEqualToSize(resolution, CGSizeMake(1024, 768)))
        return 1024 * 2;
    else
        return 1024 * 4;
}
+(NSString *)resolutionName:(CGSize)resolution{
    NSString* widthString = [[NSNumber numberWithInteger:resolution.width] stringValue];
    NSString* heightString = [[NSNumber numberWithInteger:resolution.height] stringValue];
    NSString* name = [NSString stringWithFormat:@"%@x%@", widthString, heightString];
    return name;
}
+(CGSize)resolutionFromName:(NSString *)name{
    NSRange xIndex = [name rangeOfString:@"x" options:NSCaseInsensitiveSearch];
    if(xIndex.location == NSNotFound)
        [NSException raise:NSInvalidArgumentException format:@"Invalid resolution: %@", name];
    NSString* widthStr = [name substringWithRange:NSMakeRange(0, xIndex.location)];
    NSString* heightStr = [name substringWithRange:NSMakeRange(xIndex.location + 1, [name length] - xIndex.location - 1)];
    return CGSizeMake([widthStr integerValue], [heightStr integerValue]);
}
+(float)blockSizeForResolution:(CGSize)resolution{
    float defaultResolutionWidth = 2048;
    float adjust = MAX(resolution.width, resolution.height) / defaultResolutionWidth;
    return MAX(1, floorf(128.0f * adjust));
}
@end
