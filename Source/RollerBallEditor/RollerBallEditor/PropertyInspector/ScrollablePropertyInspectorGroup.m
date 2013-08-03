//
//  PropertyInspectorPanel.m
//  CocoaTest
//
//  Created by Danilo Carvalho on 05/12/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "ScrollablePropertyInspectorGroup.h"
#import "PropertyInspectorGroup.h"

@interface ScrollablePropertyInspectorGroup ()
{
    __weak PropertyInspectorGroup* fGroup;
    __weak NSScrollView* fScrollView;
}
@end

@implementation ScrollablePropertyInspectorGroup

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        fGroup = nil;
        fScrollView = nil;
        // Initialization code here.
        NSScrollView* scrollView = [[NSScrollView alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
        [scrollView setTranslatesAutoresizingMaskIntoConstraints:NO];
        [scrollView setDrawsBackground:NO];
        [scrollView setBackgroundColor:[NSColor controlColor]];
        [scrollView setBorderType:NSBezelBorder];
        [scrollView setFindBarPosition:NSScrollViewFindBarPositionAboveContent];
        [scrollView setHorizontalScrollElasticity:NSScrollElasticityAutomatic];
        [scrollView setVerticalScrollElasticity:NSScrollElasticityAutomatic];
        [scrollView setUsesPredominantAxisScrolling:NO];
        [[scrollView contentView] setCopiesOnScroll:NO];
        [scrollView setHasHorizontalScroller:NO];
        [scrollView setHasVerticalScroller:YES];
        
        fScrollView = scrollView;
        [self addSubview:scrollView];
        
        //NSRect frm = [scrollView.documentView frame];
        PropertyInspectorGroup* group = [[PropertyInspectorGroup alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
        [group setAutoresizingMask:NSViewWidthSizable | NSViewMaxYMargin];
        [group setType:pigtInline];
        [scrollView setDocumentView:group];
        fGroup = group;
    }
    
    return self;
}

-(PropertyInspectorGroup *)group{
    return fGroup;
}

- (void)drawRect:(NSRect)dirtyRect
{
    if([fGroup childCount] == 0){
        NSRect rect = [self bounds];
        NSString* title = @"No Selection";
        NSColor* titleColor = [NSColor textBackgroundColor];
        NSFont* titleFont = [NSFont boldSystemFontOfSize:13];
        NSSize size = [title sizeWithAttributes:[NSDictionary dictionaryWithObject:titleFont forKey:NSFontAttributeName]];
        NSSize bezierSize = NSMakeSize(size.width + 40.0, size.height + 20.0);
        NSRect drawRect;
        
        // Background
        drawRect = NSMakeRect(0.0, 0.0, bezierSize.width, bezierSize.height);
        drawRect.origin.x = round((rect.size.width * 0.5) - (bezierSize.width * 0.5));
        drawRect.origin.y = round((rect.size.height * 0.5) - (bezierSize.height * 0.5));
        
        [[NSColor headerColor] setFill];
        [[NSBezierPath bezierPathWithRoundedRect:drawRect xRadius:8.0 yRadius:8.0] fill];
        
        // String
        drawRect = NSMakeRect(0.0, 0.0, size.width, size.height);
        drawRect.origin.x = round((rect.size.width * 0.5) - (size.width * 0.5));
        drawRect.origin.y = round((rect.size.height * 0.5) - (size.height * 0.5));
        
        [title drawInRect:drawRect withAttributes:[NSDictionary dictionaryWithObjectsAndKeys:titleColor, NSForegroundColorAttributeName, titleFont, NSFontAttributeName, nil]];
    }
}

-(void)updateConstraints{
    [self removeConstraints:[self constraints]];
    NSScrollView* sv = fScrollView;
    PropertyInspectorGroup* grp = fGroup;
    NSDictionary* bindings = NSDictionaryOfVariableBindings(sv, grp);
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"V:|-(-1)-[sv]|" options:0 metrics:nil views:bindings]];
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"H:|[sv]|" options:0 metrics:nil views:bindings]];
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"H:|[grp]|" options:0 metrics:nil views:bindings]];
    
    [super updateConstraints];
}

@end
