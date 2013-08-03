//
//  StackPanel.m
//  CocoaTest
//
//  Created by Danilo Carvalho on 30/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "StackPanel.h"

@interface StackPanel ()
{
    NSLayoutConstraint* fHeightConstraint;
}
+(NSLayoutAttribute)layoutAttributeFromString:(NSString*)value;
+(NSLayoutRelation)relationFromString:(NSString*)value;
-(void)addFastConstraintWithView1:(NSView*)v1 attr1:(NSString*)attr1 relation:(NSString*)rel view2:(NSView*)v2 attr2:(NSString*)attr2 multiplier: (CGFloat)multiplier constant: (CGFloat)constant priority: (NSLayoutPriority)priority;
-(NSLayoutConstraint*)fastConstraintWithView1:(NSView *)v1 attr1:(NSString *)attr1 relation:(NSString *)rel view2:(NSView *)v2 attr2:(NSString *)attr2 multiplier:(CGFloat)multiplier constant:(CGFloat)constant priority:(NSLayoutPriority)priority;
@end

@implementation StackPanel

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        fHeightConstraint = nil;
    }
    
    return self;
}

+(NSLayoutAttribute)layoutAttributeFromString:(NSString *)value{
    value = [value lowercaseString];
    if([value isEqualToString: @"left"])
        return NSLayoutAttributeLeft;
    else if([value isEqualToString: @"right"])
        return NSLayoutAttributeRight;
    else if([value isEqualToString: @"top"])
        return NSLayoutAttributeTop;
    else if([value isEqualToString: @"bottom"])
        return NSLayoutAttributeBottom;
    else if([value isEqualToString: @"leading"])
        return NSLayoutAttributeLeading;
    else if([value isEqualToString: @"trailing"])
        return NSLayoutAttributeTrailing;
    else if([value isEqualToString: @"width"])
        return NSLayoutAttributeWidth;
    else if([value isEqualToString: @"height"])
        return NSLayoutAttributeHeight;
    else if([value isEqualToString: @"centerx"])
        return NSLayoutAttributeCenterX;
    else if([value isEqualToString: @"centery"])
        return NSLayoutAttributeCenterY;
    else if([value isEqualToString: @"baseline"])
        return NSLayoutAttributeBaseline;
    else
        return NSLayoutAttributeNotAnAttribute;
}
+(NSLayoutRelation)relationFromString:(NSString *)value{
    if([value isEqualToString: @"=="])
        return NSLayoutRelationEqual;
    else if([value isEqualToString: @">="])
        return NSLayoutRelationGreaterThanOrEqual;
    else
        return NSLayoutRelationLessThanOrEqual;
}
-(void)addFastConstraintWithView1:(NSView *)v1 attr1:(NSString *)attr1 relation:(NSString *)rel view2:(NSView *)v2 attr2:(NSString *)attr2 multiplier:(CGFloat)multiplier constant:(CGFloat)constant priority:(NSLayoutPriority)priority{
    NSLayoutConstraint* constraint = [NSLayoutConstraint constraintWithItem:v1 attribute:[StackPanel layoutAttributeFromString:attr1] relatedBy:[StackPanel relationFromString:rel] toItem:v2 attribute:[StackPanel layoutAttributeFromString:attr2] multiplier:multiplier constant:constant];
    constraint.priority = priority;
    [self addConstraint: constraint];
}

-(NSLayoutConstraint*)fastConstraintWithView1:(NSView *)v1 attr1:(NSString *)attr1 relation:(NSString *)rel view2:(NSView *)v2 attr2:(NSString *)attr2 multiplier:(CGFloat)multiplier constant:(CGFloat)constant priority:(NSLayoutPriority)priority{
    NSLayoutConstraint* constraint = [NSLayoutConstraint constraintWithItem:v1 attribute:[StackPanel layoutAttributeFromString:attr1] relatedBy:[StackPanel relationFromString:rel] toItem:v2 attribute:[StackPanel layoutAttributeFromString:attr2] multiplier:multiplier constant:constant];
    constraint.priority = priority;
    return constraint;
}

-(void)setSubviews:(NSArray *)newSubviews{
    [super setSubviews:newSubviews];
    for (int i = 0; i < [newSubviews count]; i++) {
        [[newSubviews objectAtIndex:i] setTranslatesAutoresizingMaskIntoConstraints:NO];
    }
    [self setNeedsUpdateConstraints:YES];
}

-(void)addSubview:(NSView *)aView{
    [super addSubview:aView];
    [aView setTranslatesAutoresizingMaskIntoConstraints:NO];
    [self setNeedsUpdateConstraints:YES];
}

-(void)addSubview:(NSView *)aView positioned:(NSWindowOrderingMode)place relativeTo:(NSView *)otherView{
    [super addSubview:aView positioned:place relativeTo:otherView];
    [aView setTranslatesAutoresizingMaskIntoConstraints:NO];
    [self setNeedsUpdateConstraints:YES];
}
-(void)replaceSubview:(NSView *)oldView with:(NSView *)newView{
    [super replaceSubview:oldView with:newView];
    [newView setTranslatesAutoresizingMaskIntoConstraints:NO];
    [self setNeedsUpdateConstraints:YES];
}
-(void)sortSubviewsUsingFunction:(NSComparisonResult (*)(__strong id, __strong id, void *))compare context:(void *)context{
    [super sortSubviewsUsingFunction:compare context:context];
    [self setNeedsUpdateConstraints:YES];
}

-(void)layout{
    for (int i = 0; i < [self subviews].count; i++) {
        [[[self subviews] objectAtIndex:i] layout];
    }
    [super layout];
    if(fHeightConstraint){
        [self removeConstraint:fHeightConstraint];
        fHeightConstraint = nil;
    }
    CGFloat height = 0;
    for (int i = 0; i < [self subviews].count; i++) {
        NSView* v = [[self subviews] objectAtIndex:i];
        height += [v frame].size.height;
    }
    int line = 1;
    if([[self superview] isKindOfClass:[StackPanel class]])
        line = 0;
    if([[self superview] isKindOfClass:[StackPanel class]]){
        fHeightConstraint = [self fastConstraintWithView1:self attr1:@"height" relation:@"==" view2:self attr2:@"height" multiplier:0 constant:height + line priority:1000];
    
        [self addConstraint:fHeightConstraint];
        [super layout];
    }
    else {
        NSRect currentFrame = self.frame;
        self.frame = NSMakeRect(currentFrame.origin.x, 0, currentFrame.size.width, height + line);
    }
}

-(void)updateSize{
    CGFloat height = 0;
    for (int i = 0; i < [self subviews].count; i++) {
        NSView* v = [[self subviews] objectAtIndex:i];
        height += [v frame].size.height;
    }
    int line = 1;
    if([[self superview] isKindOfClass:[StackPanel class]])
        line = 0;
    if(![[self superview] isKindOfClass:[StackPanel class]]){
        NSRect currentFrame = self.frame;
        self.frame = NSMakeRect(currentFrame.origin.x, 0, currentFrame.size.width, height + line);
    }
}

-(BOOL)isFlipped{
    if(![[self superview] isKindOfClass:[StackPanel class]])
        return YES;
    else
        return NO;
}

-(void)updateConstraints{
    [self removeConstraints:[self constraints]];
    fHeightConstraint = nil;
    NSView* previousView = nil;
    for (int i = 0; i < [self.subviews count]; i++) {
        NSView* view = [self.subviews objectAtIndex:i];
        NSDictionary* bindings = NSDictionaryOfVariableBindings(view);
        [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"H:|[view]|" options:0 metrics:nil views:bindings]];
        if(i == 0){
            [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"V:|[view]" options:0 metrics:nil views:bindings]];
        }
        else {
            [self addFastConstraintWithView1:[self.subviews objectAtIndex:i] attr1:@"top" relation:@"==" view2:previousView attr2:@"bottom" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
        }
        previousView = [self.subviews objectAtIndex:i];
    }
    [super updateConstraints];
}
-(void)drawRect:(NSRect)dirtyRect{
}
@end
