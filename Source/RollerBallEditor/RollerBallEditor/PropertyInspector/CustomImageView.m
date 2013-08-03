//
//  CustomImageView.m
//  CocoaTest
//
//  Created by Danilo Carvalho on 02/12/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "CustomImageView.h"

@implementation CustomImageView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

-(BOOL)acceptsFirstResponder{
    return YES;
}

-(void)mouseDown:(NSEvent *)theEvent{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-performSelector-leaks"
    if([self target] && [self action])
        [[self target] performSelector:[self action] withObject:self];
#pragma clang diagnostic pop
}

@end
