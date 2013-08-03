//
//  NavigationBar.m
//  Test
//
//  Created by Danilo Carvalho on 02/06/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import "NavigationBar.h"
#import "NSColor+ColorExtensions.h"
#import "KBButton.h"

@interface NavigationBar ()
{
    KBButton* fBack;
    NSTextField* fLabel;
}
@end

@implementation NavigationBar
-(NSString *)title{
    return fLabel.stringValue;
}

-(void)setTitle:(NSString *)title{
    fLabel.stringValue = title;
}

-(SEL)action{
    return fBack.action;
}

-(void)setAction:(SEL)action{
    fBack.action = action;
}

-(id)target{
    return fBack.target;
}

-(void)setTarget:(id)target{
    fBack.target = target;
}

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        fBack = [[KBButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
        fLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
        fBack.title = @"Back";
        fLabel.stringValue = @"Transformation";
        [fLabel setEditable:NO];
        [fLabel setSelectable:NO];
        [fLabel setBezeled:NO];
        [fLabel setBordered:NO];
        [fLabel setDrawsBackground:NO];
        [fLabel setContentCompressionResistancePriority:NSLayoutPriorityDefaultLow forOrientation:NSLayoutConstraintOrientationHorizontal];
        [fLabel setAlignment:NSCenterTextAlignment];
        [[fLabel cell] setLineBreakMode:NSLineBreakByTruncatingTail];
        [fLabel setTextColor:[NSColor whiteColor]];
        [fLabel setFont:[NSFont boldSystemFontOfSize:14]];
        
        [fBack setKBButtonType:BButtonTypePrimary];
        [fBack setButtonType:NSMomentaryPushInButton];
        [fBack setBezelStyle:NSRegularSquareBezelStyle];
        [fBack setFont:[NSFont boldSystemFontOfSize:13]];
        [self addSubview:fBack];
        [self addSubview:fLabel];
        [self setTranslatesAutoresizingMaskIntoConstraints:NO];
        [fBack setTranslatesAutoresizingMaskIntoConstraints:NO];
        [fLabel setTranslatesAutoresizingMaskIntoConstraints:NO];
    }
    
    return self;
}

- (NSColor*)getColorForBackground {
    return [NSColor colorWithCalibratedRed:0.13f green:0.13f blue:0.13f alpha:1.00f];
}

- (void)drawBezelWithFrame:(NSRect)frame
{
    NSGraphicsContext* ctx = [NSGraphicsContext currentContext];
    
    // corner radius
    CGFloat roundedRadius = 0.0f;
    
    NSColor *color = [self getColorForBackground];
    
    // create background color
    [ctx saveGraphicsState];
//    [[NSBezierPath bezierPathWithRoundedRect:frame
//                                     xRadius:roundedRadius
//                                     yRadius:roundedRadius] setClip];
    [[color darkenColorByValue:0.12f] setFill];
    NSRectFillUsingOperation(frame, NSCompositeSourceOver);
    [ctx restoreGraphicsState];
    
    
    //draw inner button area
    [ctx saveGraphicsState];

    NSBezierPath* bgPath = [NSBezierPath bezierPathWithRoundedRect:NSInsetRect(frame, 1.0f, 1.0f) xRadius:roundedRadius yRadius:roundedRadius];
//    [bgPath setClip];
    
    NSColor* topColor = [color lightenColorByValue:0.12f];
    
    // gradient for inner portion of button
    NSGradient* bgGradient = [[NSGradient alloc] initWithColorsAndLocations:
                              topColor, 0.0f,
                              color, 1.0f,
                              nil];
    [bgGradient drawInRect:[bgPath bounds] angle:90.0f];
    
    [ctx restoreGraphicsState];
}

-(void)setNeedsDisplay:(BOOL)flag{
    [fLabel setNeedsDisplay:flag];
    [super setNeedsDisplay:flag];
}

- (void)drawBackground
{
    NSRect bezelFrame = self.bounds;
    bezelFrame.origin.x -= 1;
    bezelFrame.size.width += 2;
    bezelFrame.origin.y += self.bounds.size.height - 40;
    bezelFrame.size.height = 40;
    [self drawBezelWithFrame:bezelFrame];
}

- (void)drawRect:(NSRect)dirtyRect
{
    [self drawBackground];
}

-(void)updateConstraints{
    [self removeConstraints:self.constraints];
    [super updateConstraints];
    KBButton* back = fBack;
    NSTextField* label = fLabel;
    NSDictionary* bindings = NSDictionaryOfVariableBindings(back, label);
    [self addConstraints: [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-8-[back(50)]-[label]-8-|" options:0 metrics:nil views:bindings]];
    [self addConstraints: [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-8-[back]" options:0 metrics:nil views:bindings]];
    [self addConstraint:[NSLayoutConstraint constraintWithItem:fLabel attribute:NSLayoutAttributeBaseline relatedBy:NSLayoutRelationEqual toItem:fBack attribute:NSLayoutAttributeBaseline multiplier:1 constant:0]];
    
}

@end
