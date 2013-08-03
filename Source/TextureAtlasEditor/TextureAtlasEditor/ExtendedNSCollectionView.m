//
//  ExtendedNSCollectionView.m
//  TextureAtlasEditor
//
//  Created by Danilo Carvalho on 31/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "ExtendedNSCollectionView.h"
#import "Texture.h"
#define KEY_DELETE 51

@implementation ExtendedNSCollectionView

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
    [super drawRect:dirtyRect];
}

-(void)keyDown:(NSEvent *)theEvent{
    unsigned int keycode = [theEvent keyCode];
    if(keycode == KEY_DELETE){
        NSUInteger selectedIndex = [self.arrayController selectionIndex];
        if(selectedIndex == NSNotFound)
            [super keyDown:theEvent];
        else {
            Texture* texture = [self.textureAtlas objectInTexturesAtIndex:selectedIndex];
            [texture removeAllStages];
            [self.textureAtlas removeObjectFromTexturesAtIndex:selectedIndex];
        }
    }
    else
        [super keyDown:theEvent];
}

@end
