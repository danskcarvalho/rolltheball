//
//  CustomTextField.m
//  CocoaTest
//
//  Created by Danilo Carvalho on 30/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "CustomTextField.h"

@implementation CustomTextField

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

-(void)textDidChange:(NSNotification *)notification{
    [super textDidChange:notification];
    if ([self.target respondsToSelector:@selector(textDidChange:)]) {
        [self.target performSelector:@selector(textDidChange:) withObject:self];
    }
}

-(void)textDidEndEditing:(NSNotification *)notification{
    [super textDidEndEditing:notification];
    if ([self.target respondsToSelector:@selector(textDidEndEditing:)]) {
        [self.target performSelector:@selector(textDidEndEditing:) withObject:self];
    }
}
-(void)setStringValue:(NSString *)aString{
    [super setStringValue:aString];
    if([aString isEqualToString:@""])
        [self setToolTip:nil];
    else
        [self setToolTip:aString];
}
@end
