//
//  NavigationView.m
//  Test
//
//  Created by Danilo Carvalho on 02/06/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import "NavigationView.h"
#import "NavigationBar.h"

#define NAVIGATION_ANIMATION_DURATION 0.3

@interface AnimationEndedForPop : NSObject
@property (weak, nonatomic) NSView* subview;
-(void)animationDidStop:(CAAnimation *)anim finished:(BOOL)flag;
@end

@implementation AnimationEndedForPop
-(void)animationDidStop:(CAAnimation *)anim finished:(BOOL)flag{
    [self.subview removeFromSuperview];
}
@end

@interface NavigationView ()
{
    NavigationBar* fNavigationBar;
    NSMutableArray* fViewStack;
    NavigationView* fRoot;
    BOOL fInBack;
}
-(NSView *)internalPeekView;
@end

@implementation NavigationView

-(void)backClicked:(id)sender{
    fRoot->fInBack = YES;
    [fRoot popView];
    fRoot->fInBack = NO;
}

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        fRoot = self;
        fViewStack = [NSMutableArray array];
        fNavigationBar = [[NavigationBar alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
        [self addSubview:fNavigationBar];
        [self setTranslatesAutoresizingMaskIntoConstraints:NO];
        [fNavigationBar setTranslatesAutoresizingMaskIntoConstraints:NO];
        [fNavigationBar setTarget:self];
        [fNavigationBar setAction:@selector(backClicked:)];
        [fNavigationBar setHidden:YES];
        [self setWantsLayer:YES];
        self.target = nil;
        fInBack = NO;
    }
    
    return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
    
}

-(void)updateConstraints{
    [self removeConstraints:self.constraints];
    [super updateConstraints];
    
    NSDictionary* bindings = nil;
    
    if(fRoot != self){
        NSView* navBar = fNavigationBar;
        bindings = NSDictionaryOfVariableBindings(navBar);
        
        [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"V:|[navBar(40)]" options:0 metrics:nil views:bindings]];
        [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"H:|[navBar]|" options:0 metrics:nil views:bindings]];
        
        for (int i = 0; i < self.subviews.count; i++){
            NSView* view = [self.subviews objectAtIndex:i];
            if([view isKindOfClass:[NavigationBar class]])
                continue;
            bindings = NSDictionaryOfVariableBindings(navBar, view);
            [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"V:|[navBar(40)][view]|" options:0 metrics:nil views:bindings]];
            [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"H:|[view]|" options:0 metrics:nil views:bindings]];
        }
    }
    else {
        for (int i = 0; i < self.subviews.count; i++){
            NSView* view = [self.subviews objectAtIndex:i];
            bindings = NSDictionaryOfVariableBindings(view);
            [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"V:|[view]|" options:0 metrics:nil views:bindings]];
            [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"H:|[view]|" options:0 metrics:nil views:bindings]];
        }
    }
}

- (CABasicAnimation *)positionAnimationFromPoint:(CGPoint)fromPoint toPoint:(CGPoint)toPoint {
    CABasicAnimation *a = [CABasicAnimation animationWithKeyPath:@"position"];
    a.fromValue = [NSValue valueWithPoint:fromPoint];
    a.toValue = [NSValue valueWithPoint:toPoint];
    return a;
}

-(void)pushView:(NSView *)view withTitle: (NSString*)title {
    [view setTranslatesAutoresizingMaskIntoConstraints:NO];
    if(fViewStack.count == 0){
        [fViewStack addObject:view];
        [self addSubview:view];
        [self setNeedsUpdateConstraints:YES];
        [view setWantsLayer:YES];
    }
    else {
        NavigationView* nv = [[NavigationView alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
        nv->fRoot = self;
        [nv->fNavigationBar setHidden:NO];
        [nv->fNavigationBar setTitle:title];
        [nv pushView:view withTitle:@"Don't care"];
        
        NSView* previous = [self internalPeekView];
        [previous setHidden:NO];
        [nv setHidden:YES];
        [fViewStack addObject:nv];
        [self addSubview:nv];
        [self setNeedsUpdateConstraints:YES];
        
        CATransition* transition = [CATransition animation];
        transition.startProgress = 0;
        transition.endProgress = 1.0;
        transition.type = kCATransitionPush;
        transition.subtype = kCATransitionFromRight;
        transition.timingFunction = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseOut];
        transition.duration = NAVIGATION_ANIMATION_DURATION;
        
        // Add the transition animation to both layers
        [previous.layer addAnimation:transition forKey:@"transition"];
        [nv.layer addAnimation:transition forKey:@"transition"];
        
        // Finally, change the visibility of the layers.
        previous.hidden = YES;
        nv.hidden = NO;
    }
}

-(void)popView {
    if(fViewStack.count == 0)
        return;
    
    if(fViewStack.count == 1){
        [[self internalPeekView] removeFromSuperview];
        [self setNeedsUpdateConstraints:YES];
        [fViewStack removeLastObject];
    }
    else {
        NSView* previous = [self internalPeekView];
        if(fInBack){
            if(self.target){
                if([previous isKindOfClass:[NavigationView class]])
                    [self.target performSelector:@selector(backClicked:) withObject:[((id)previous) peekView]];
                else
                    [self.target performSelector:@selector(backClicked:) withObject:previous];
            }
        }
        NSView* view = [fViewStack objectAtIndex:fViewStack.count - 2];
        [previous setHidden:NO];
        [view setHidden:YES];
        [fViewStack removeLastObject];
        [self addSubview:view];
        [self setNeedsUpdateConstraints:YES];
        [self setNeedsDisplay:YES];
        [self setNeedsLayout:YES];
        [view setWantsLayer:YES];
        
        AnimationEndedForPop* aefp = [[AnimationEndedForPop alloc] init];
        aefp.subview = previous;
        CATransition* transition = [CATransition animation];
        transition.delegate = aefp;
        transition.startProgress = 0;
        transition.endProgress = 1.0;
        transition.type = kCATransitionPush;
        transition.subtype = kCATransitionFromLeft;
        transition.duration = NAVIGATION_ANIMATION_DURATION;
        
        // Add the transition animation to both layers
        [previous.layer addAnimation:transition forKey:@"transition"];
        [view.layer addAnimation:transition forKey:@"transition"];
        
        // Finally, change the visibility of the layers.
        previous.hidden = YES;
        view.hidden = NO;
    }
}

-(void)removeView:(NSView *)view{
    id superview = [view superview];
    if(superview != fRoot && [superview isKindOfClass:[NavigationView class]])
        view = superview;
    NSMutableArray* mArray = [NSMutableArray array];
    for (NSUInteger i = 0; i < self.subviews.count; i++) {
        NSView* v = (NSView*)[self.subviews objectAtIndex:i];
        if(v != view)
            [mArray addObject:v];
    }
    [self setSubviews:mArray];
    mArray = [NSMutableArray array];
    for (NSUInteger i = 0; i < fViewStack.count; i++) {
        NSView* v = (NSView*)[fViewStack objectAtIndex:i];
        if(v != view)
            [mArray addObject:v];
    }
    fViewStack = mArray;
    [self setNeedsUpdateConstraints:YES];
}

-(NSView *)internalPeekView{
    return [fViewStack objectAtIndex:fViewStack.count - 1];
}
-(NSView *)peekView{
    id v = [self internalPeekView];
    if([v isKindOfClass:[NavigationView class]])
        return [v peekView];
    else
        return v;
}
@end
