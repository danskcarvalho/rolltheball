//
//  CustomComboBox.m
//  RollerBallEditor
//
//  Created by Danilo Santos de Carvalho on 04/06/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import "CustomComboBox.h"

@implementation CustomComboBox

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

-(BOOL)becomeFirstResponder{
    BOOL response = [super becomeFirstResponder];
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-performSelector-leaks"
    if (self.action && self.target)
        [self.target performSelector:self.action withObject:self];
#pragma clang diagnostic pop
    return response;
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

@end
