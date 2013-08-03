//
//  CustomLabel.m
//  CocoaTest
//
//  Created by Danilo Carvalho on 15/02/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import "CustomLabel.h"

@implementation CustomLabel

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

-(void)mouseDown:(NSEvent *)theEvent{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-performSelector-leaks"
    if([self action] && [self target] && [[self target] respondsToSelector:[self action]])
       [[self target] performSelector:[self action] withObject:self];
#pragma clang diagnostic pop
}

@end
