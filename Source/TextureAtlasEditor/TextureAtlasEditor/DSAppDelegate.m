//
//  DSAppDelegate.m
//  TextureAtlasEditor
//
//  Created by Danilo Carvalho on 23/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "DSAppDelegate.h"
#import "SmallNumberValueTransformer.h"
#import "RadiansToDegreesValueTransformer.h"

@implementation DSAppDelegate
+(void)initialize{
    SmallNumberValueTransformer* smTransformer = [[SmallNumberValueTransformer alloc] init];
    RadiansToDegreesValueTransformer* rdTransformer = [[RadiansToDegreesValueTransformer alloc] init];
    [NSValueTransformer setValueTransformer:smTransformer forName:@"SmallNumberTransformer"];
    [NSValueTransformer setValueTransformer:rdTransformer forName:@"RadiansToDegreesTransformer"];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    
}

@end
