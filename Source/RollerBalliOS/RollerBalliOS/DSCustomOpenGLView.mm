//
//  DSCustomOpenGLView.m
//  RollerBalliOS
//
//  Created by Danilo Carvalho on 20/08/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import "DSCustomOpenGLView.h"
#include <vector>
#define IOS_TARGET
#include "components_external.h"

using namespace rb;

@implementation DSCustomOpenGLView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    return self;
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/

-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event{
    NSArray* allTouches = [touches allObjects];
    std::vector<touch> _touches;
    for (int i = 0; i < [allTouches count]; i++) {
        UITouch* t = [allTouches objectAtIndex:i];
        CGPoint p = [t locationInView:self];
        auto _bounds = self.bounds;
        p.x /= _bounds.size.width;
        p.y /= _bounds.size.height;
        p.x *= 2;
        p.y *= 2;
        p.x -= 1;
        p.y = 1 - p.y;
        _touches.push_back(touch(t, vec2(p.x, p.y)));
    }
    auto _responder = director::active_responder();
    if(_responder)
        _responder->touches_began(_touches);
}

-(void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event{
    NSArray* allTouches = [touches allObjects];
    std::vector<touch> _touches;
    for (int i = 0; i < [allTouches count]; i++) {
        UITouch* t = [allTouches objectAtIndex:i];
        CGPoint p = [t locationInView:self];
        auto _bounds = self.bounds;
        p.x /= _bounds.size.width;
        p.y /= _bounds.size.height;
        p.x *= 2;
        p.y *= 2;
        p.x -= 1;
        p.y = 1 - p.y;
        _touches.push_back(touch(t, vec2(p.x, p.y)));
    }
    auto _responder = director::active_responder();
    if(_responder)
        _responder->touches_cancelled(_touches);
}

-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event{
    NSArray* allTouches = [touches allObjects];
    std::vector<touch> _touches;
    for (int i = 0; i < [allTouches count]; i++) {
        UITouch* t = [allTouches objectAtIndex:i];
        CGPoint p = [t locationInView:self];
        auto _bounds = self.bounds;
        p.x /= _bounds.size.width;
        p.y /= _bounds.size.height;
        p.x *= 2;
        p.y *= 2;
        p.x -= 1;
        p.y = 1 - p.y;
        _touches.push_back(touch(t, vec2(p.x, p.y)));
    }
    auto _responder = director::active_responder();
    if(_responder)
        _responder->touches_ended(_touches);
}

-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event{
    NSArray* allTouches = [touches allObjects];
    std::vector<touch> _touches;
    for (int i = 0; i < [allTouches count]; i++) {
        UITouch* t = [allTouches objectAtIndex:i];
        CGPoint p = [t locationInView:self];
        auto _bounds = self.bounds;
        p.x /= _bounds.size.width;
        p.y /= _bounds.size.height;
        p.x *= 2;
        p.y *= 2;
        p.x -= 1;
        p.y = 1 - p.y;
        _touches.push_back(touch(t, vec2(p.x, p.y)));
    }
    auto _responder = director::active_responder();
    if(_responder)
        _responder->touches_moved(_touches);
}

@end
