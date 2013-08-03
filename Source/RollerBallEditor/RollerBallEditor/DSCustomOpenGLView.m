//
//  DSCustomOpenGLView.m
//  RollerBallEditor
//
//  Created by Danilo Carvalho on 21/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//
#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#import "DSCustomOpenGLView.h"
#define MACOSX_TARGET
#include "components_external.h"
#import "Resolutions.h"
#import "CppBridge.h"
#import "TextureAtlasLoader.h"
#import "DSEditorDelegate.h"
using namespace rb;

inline NSString* to_platform_string(const rb::rb_string& str){
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> _str_converter;
    auto _u8Str = _str_converter.to_bytes(str);
    NSString* _platform_str = [NSString stringWithUTF8String:_u8Str.c_str()];
    return _platform_str;
}

inline rb_string from_platform_string(NSString* str){
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> _str_converter;
    return _str_converter.from_bytes([str UTF8String]);
}

@interface DSCustomOpenGLView () {
}
- (void)setupGL;
- (void)tearDownGL;
@end


@implementation DSCustomOpenGLView
@synthesize renderingEnabled;
- (void)tearDownGL
{
    [[self openGLContext] makeCurrentContext];
    director::active_scene(nullptr, true);
}

-(void)setupGL
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glDisable(GL_DITHER);
    NSSize bounds = [self convertSizeToBacking: [self bounds].size];
    glViewport(0, 0, bounds.width, bounds.height);
    
    director::in_editor(true);
    [TextureAtlasLoader setBridgeClass:[CppBridge class]];
    //set the current scene
    director::active_scene(new scene(), YES);
}

-(BOOL)windowShouldClose:(id)sender{
    [self.editorDelegate quitApplication:self];
    return NO;
}

-(void)drawFrame{
    if(!self.renderingEnabled)
        return;
    auto _responder = director::active_responder();
    if(_responder)
        _responder->render();
    [[self openGLContext] flushBuffer];
}

-(void)prepareOpenGL{
    GLint swap = 1;
    [[self openGLContext] setValues:&swap forParameter:NSOpenGLCPSwapInterval];
    _timer = [NSTimer scheduledTimerWithTimeInterval: DESIRED_FPS target:self selector:@selector(updateGameLogic) userInfo:nil repeats:YES];
    [[NSRunLoop currentRunLoop] addTimer:_timer forMode:NSEventTrackingRunLoopMode];
    [self setupGL];
}

-(void)drawRect:(NSRect)dirtyRect{
    [super drawRect:dirtyRect];
    [self drawFrame];
}

-(void)updateGameLogic{
    if(!self.renderingEnabled)
        return;
    [[self openGLContext] makeCurrentContext];
    auto _responder = director::active_responder();
    if(_responder)
        _responder->update();
    if(self.delegate)
        [self.delegate onUpdate];
    [self drawFrame];
}

-(void)reshape{
    [super reshape];
    NSSize bounds = [self convertSizeToBacking: [self bounds].size];
    glViewport(0, 0, bounds.width, bounds.height);
    auto _responder = director::active_responder();
    if(_responder)
        _responder->viewport_resized();
}

-(void)awakeFromNib{
    [super awakeFromNib];
    [[self window] makeFirstResponder:self];
    [[self window] setDelegate:self];
    [self setAcceptsTouchEvents:YES];
    [self setRenderingEnabled:YES];
}

-(void)cancelOperation:(id)sender{
    auto _responder = director::active_responder();
    if(_responder)
        _responder->keydown(KEY_ESCAPE, responder::modifiers());
}

-(void)keyDown:(NSEvent *)theEvent{
    keyboard_modifier _mod = (keyboard_modifier)([theEvent modifierFlags] & NSDeviceIndependentModifierFlagsMask);
    auto _responder = director::active_responder();
    if(_responder)
        _responder->keydown([theEvent keyCode], _mod);
}

-(void)keyUp:(NSEvent *)theEvent{
    keyboard_modifier _mod = (keyboard_modifier)([theEvent modifierFlags] & NSDeviceIndependentModifierFlagsMask);
    auto _responder = director::active_responder();
    if(_responder)
        _responder->keyup([theEvent keyCode], _mod);
}

-(void)beginGestureWithEvent:(NSEvent *)event{
    auto _responder = director::active_responder();
    if(_responder)
        _responder->begin_gesture();
}

-(void)endGestureWithEvent:(NSEvent *)event{
    auto _responder = director::active_responder();
    if(_responder)
        _responder->end_gesture();
}

-(void)magnifyWithEvent:(NSEvent *)event{
    auto _responder = director::active_responder();
    if(_responder)
        _responder->magnify([event magnification]);
}

-(void)rotateWithEvent:(NSEvent *)event{
    auto _responder = director::active_responder();
    if(_responder)
        _responder->rotate(TO_RADIANS([event rotation]));
}

-(void)swipeWithEvent:(NSEvent *)event{
    auto _responder = director::active_responder();
    if(_responder)
        _responder->swipe(vec2([event deltaX], [event deltaY]));
}

-(void)scrollWheel:(NSEvent *)theEvent{
    auto _responder = director::active_responder();
    if(_responder)
        _responder->scroll(vec2([theEvent scrollingDeltaX], [theEvent scrollingDeltaY]));
}

-(void)touchesBeganWithEvent:(NSEvent *)event{
    NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseBegan inView:self];
    NSArray* allTouches = [touches allObjects];
    std::vector<touch> _touches;
    for (int i = 0; i < [allTouches count]; i++) {
        NSTouch* t = [allTouches objectAtIndex:i];
        _touches.push_back(touch(t));
    }
    auto _responder = director::active_responder();
    if(_responder)
        _responder->touches_began(_touches);
}

-(void)touchesEndedWithEvent:(NSEvent *)event{
    NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseEnded inView:self];
    NSArray* allTouches = [touches allObjects];
    std::vector<touch> _touches;
    for (int i = 0; i < [allTouches count]; i++) {
        NSTouch* t = [allTouches objectAtIndex:i];
        _touches.push_back(touch(t));
    }
    auto _responder = director::active_responder();
    if(_responder)
        _responder->touches_ended(_touches);
}

-(void)touchesCancelledWithEvent:(NSEvent *)event{
    NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseCancelled inView:self];
    NSArray* allTouches = [touches allObjects];
    std::vector<touch> _touches;
    for (int i = 0; i < [allTouches count]; i++) {
        NSTouch* t = [allTouches objectAtIndex:i];
        _touches.push_back(touch(t));
    }
    auto _responder = director::active_responder();
    if(_responder)
        _responder->touches_cancelled(_touches);
}

-(void)touchesMovedWithEvent:(NSEvent *)event{
    NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseMoved inView:self];
    NSArray* allTouches = [touches allObjects];
    std::vector<touch> _touches;
    for (int i = 0; i < [allTouches count]; i++) {
        NSTouch* t = [allTouches objectAtIndex:i];
        _touches.push_back(touch(t));
    }
    auto _responder = director::active_responder();
    if(_responder)
        _responder->touches_moved(_touches);
}

-(vec2)toNormalizedPosition:(NSPoint)pos{
    vec2 normalizedPos = vec2(pos.x, pos.y);
    NSSize bounds = [self bounds].size;
    normalizedPos /= vec2(bounds.width, bounds.height);
    normalizedPos *= 2;
    normalizedPos -= vec2(1, 1);
    return normalizedPos;
}

-(void)mouseDown:(NSEvent *)theEvent{
    NSPoint eventLocation = [theEvent locationInWindow];
    eventLocation = [self convertPoint:eventLocation fromView:nil];
    vec2 normalizedPos = [self toNormalizedPosition:eventLocation];
    auto _responder = director::active_responder();
    if(_responder)
        _responder->mouse_down(normalizedPos);
}

-(void)mouseUp:(NSEvent *)theEvent{
    NSPoint eventLocation = [theEvent locationInWindow];
    eventLocation = [self convertPoint:eventLocation fromView:nil];
    vec2 normalizedPos = [self toNormalizedPosition:eventLocation];
    auto _responder = director::active_responder();
    if(_responder)
        _responder->mouse_up(normalizedPos);
}

-(void)mouseDragged:(NSEvent *)theEvent{
    NSPoint eventLocation = [theEvent locationInWindow];
    eventLocation = [self convertPoint:eventLocation fromView:nil];
    vec2 normalizedPos = [self toNormalizedPosition:eventLocation];
    auto _responder = director::active_responder();
    if(_responder)
        _responder->mouse_dragged(normalizedPos);
}

-(void)copy:(id)sender{
    if(director::active_scene()){
        if(director::active_scene()->current()){
            std::vector<node*> _selection;
            director::active_scene()->current()->fill_with_selection(_selection, node_filter::all, true);
            if(_selection.size() != 0){
                NSString* data = to_platform_string(director::active_scene()->copy_selected_nodes());
                NSPasteboard* pb = [NSPasteboard generalPasteboard];
                NSArray* pb_types = [NSArray arrayWithObjects:@"com.rb-editor.nodes", nil];
                [pb declareTypes:pb_types owner:nil];
                [pb setData:[data dataUsingEncoding:NSUTF8StringEncoding] forType:@"com.rb-editor.nodes"];
            }
        }
    }
}

-(void)paste:(id)sender{
    if(director::active_scene()){
        if(director::active_scene()->current()){
            NSPasteboard* pb = [NSPasteboard generalPasteboard];
            NSData* archived_data = [pb dataForType:@"com.rb-editor.nodes"];
            if(archived_data){
                NSString* str = [[NSString alloc] initWithData:archived_data encoding:NSUTF8StringEncoding];
                director::active_scene()->paste_nodes(from_platform_string(str));
            }
        }
    }
}

-(BOOL)acceptsFirstResponder{
    return YES;
}

-(void)dealloc{
    [_timer invalidate];
    [self tearDownGL];
}
@end