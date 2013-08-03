//
//  ComponentCollectionView.m
//  RollerBallEditor
//
//  Created by Danilo Santos de Carvalho on 23/05/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import "ComponentCollectionView.h"
#define KEY_DELETE 51

@implementation ComponentCollectionView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
    // Drawing code here.
}

-(void)keyDown:(NSEvent *)theEvent{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-performSelector-leaks"
    unsigned int keycode = [theEvent keyCode];
    if(keycode == KEY_DELETE){
        if(self.deleteTarget)
            [self.deleteTarget performSelector:self.deleteAction withObject:self withObject:theEvent];
    }
    else
        [super keyDown:theEvent];
#pragma clang diagnostic pop
}

@end
