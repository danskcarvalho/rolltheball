//
//  DSCustomOpenGLView.h
//  RollerBallMacOSX
//
//  Created by Danilo Carvalho on 31/08/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface DSCustomOpenGLView : NSOpenGLView < NSWindowDelegate >
{
    @private
    BOOL _fullScreen;
    NSRect _previousFrame;
    NSTimer* _timer;
}
-(void)drawFrame;
-(void)drawRect:(NSRect)dirtyRect;
-(void)reshape;
-(void)awakeFromNib;
-(void)keyDown:(NSEvent *)theEvent;
-(void)cancelOperation:(id)sender;
-(BOOL)acceptsFirstResponder;
-(void)setFullScreen;
-(BOOL)windowShouldClose:(id)sender;
-(void)updateGameLogic;
-(void)dealloc;
-(void)prepareOpenGL;
@end
