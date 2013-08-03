//
//  DSCustomOpenGLView.h
//  RollerBallEditor
//
//  Created by Danilo Carvalho on 21/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "DSCustomOpenGLViewDelegate.h"

@class DSEditorDelegate;

@interface DSCustomOpenGLView : NSOpenGLView < NSWindowDelegate >
{
@private
    NSTimer* _timer;
}
@property(assign, nonatomic)BOOL renderingEnabled;
-(void)drawFrame;
-(void)drawRect:(NSRect)dirtyRect;
-(void)reshape;
-(void)awakeFromNib;
-(void)keyDown:(NSEvent *)theEvent;
-(BOOL)acceptsFirstResponder;
-(BOOL)windowShouldClose:(id)sender;
-(void)updateGameLogic;
-(void)dealloc;
-(void)prepareOpenGL;
@property (weak) IBOutlet DSEditorDelegate* editorDelegate;
@property (weak) id<DSCustomOpenGLViewDelegate> delegate;
@end
