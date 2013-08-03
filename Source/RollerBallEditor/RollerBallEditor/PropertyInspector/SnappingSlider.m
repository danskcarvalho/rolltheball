//
//  SnappingSlider.m
//  CocoaTest
//
//  Created by Danilo Carvalho on 01/12/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "SnappingSlider.h"
#import "SnappingSliderCell.h"

@implementation SnappingSlider

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        [self setCell:[[SnappingSliderCell alloc] init]];
    }
    
    return self;
}



@end
