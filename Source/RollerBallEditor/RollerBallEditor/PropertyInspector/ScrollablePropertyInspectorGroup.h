//
//  PropertyInspectorPanel.h
//  CocoaTest
//
//  Created by Danilo Carvalho on 05/12/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class PropertyInspectorGroup;
@interface ScrollablePropertyInspectorGroup : NSView
@property (readonly) PropertyInspectorGroup* group;
@end
