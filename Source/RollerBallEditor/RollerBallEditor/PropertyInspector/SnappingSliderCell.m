//
//  SnappingSliderCell.m
//  CocoaTest
//
//  Created by Danilo Carvalho on 01/12/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "SnappingSliderCell.h"
#include "Constants.h"

@interface SnappingSliderCell ()
{
    BOOL fTracking;
}
@end

@implementation SnappingSliderCell
-(id)init{
    if (self = [super init]) {
        
    }
    return self;
}
-(id)initWithCoder:(NSCoder *)aDecoder{
    if (self = [super initWithCoder:aDecoder]){
        
    }
    return self;
}
-(id)initTextCell:(NSString *)aString{
    if (self = [super initTextCell:aString])
    {
        
    }
    return self;
}
-(id)initImageCell:(NSImage *)image{
    if (self = [super initImageCell:image]) {
        
    }
    return self;
}

- (BOOL)startTrackingAt:(NSPoint)startPoint inView:(NSView *)controlView {
    if ([self numberOfTickMarks] > 0) fTracking = YES;
    return [super startTrackingAt:startPoint inView:controlView];
}

- (BOOL)continueTracking:(NSPoint)lastPoint at:(NSPoint)currentPoint
                  inView:(NSView *)controlView {
    if (fTracking && ([NSEvent modifierFlags] & NSShiftKeyMask))
        [self setAllowsTickMarkValuesOnly:YES];
    else
        [self setAllowsTickMarkValuesOnly:NO];
    return [super continueTracking:lastPoint at:currentPoint inView:controlView];
}

- (void)stopTracking:(NSPoint)lastPoint at:(NSPoint)stopPoint
              inView:(NSView *)controlView mouseIsUp:(BOOL)flag {
    [super stopTracking:lastPoint at:stopPoint inView:controlView mouseIsUp:flag];
}
@end













