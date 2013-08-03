//
//  Resolutions.h
//  TextureAtlasEditor
//
//  Created by Danilo Carvalho on 22/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>

@interface Resolutions : NSObject
+(NSArray*)resolutions;
+(float)maxTextureAtlasSizeForResolution:(CGSize)resolution;
+(float)blockSizeForResolution:(CGSize)resolution;
+(NSString*)resolutionName:(CGSize)resolution;
+(CGSize)resolutionFromName:(NSString*)name;
@end
