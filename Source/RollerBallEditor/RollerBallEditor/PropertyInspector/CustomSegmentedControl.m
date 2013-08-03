//
//  CustomSegmentedControl.m
//  RollerBallEditor
//
//  Created by Danilo Santos de Carvalho on 04/06/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import "CustomSegmentedControl.h"

@implementation CustomSegmentedControl

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

-(void)layout {
    [super layout];
    
    float frameWidth = self.frame.size.width;
    NSSegmentedControl* control = [[NSSegmentedControl alloc] initWithFrame:NSMakeRect(5, 5, frameWidth, 0)];
    [control setSegmentStyle:self.segmentStyle];
    
    [control setSegmentCount:self.segmentCount];
    
    for (int j = 0; j < self.segmentCount; j++) {
        [control setWidth:1 forSegment:j];
    }
    
    NSSize size = control.intrinsicContentSize;
    float a = (size.width - (float)(1 * self.segmentCount)) / (float)self.segmentCount;
    
    for (NSUInteger i = 0; i < self.segmentCount; i++) {
        float segmentSize = (self.frame.size.width / (float)self.segmentCount) - a;
        [self setWidth:segmentSize forSegment:i];
    }
    [super layout];
}

-(void)setFrame:(NSRect)frameRect{
    [super setFrame:frameRect];
    [super setNeedsLayout:YES];
}

@end
