//
//  PropertyInspectorCell.h
//  CocoaTest
//
//  Created by Danilo Carvalho on 29/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import <Cocoa/Cocoa.h>

enum PropertyInspectorCellType {
    pictText = 0,
    pictLabel = 10,
    pictInt = 1,
    pictFloat = 2,
    pictFloatPair = 3,
    pictBool = 4,
    pictRanged = 5,
    pictAngle = 6,
    pictColor = 7,
    pictImage = 8,
    pictEnum = 9,
    pictButton = 11,
    pictCombobox = 12,
    pictFlags = 13
};

enum PropertyInspectorCellBorder {
    picbNone = 0,
    picbTop = 1 << 1,
    picbBottom = 1 << 2
};

@interface PropertyInspectorCell : NSView
@property enum PropertyInspectorCellType type;
@property (strong)NSColor* backgroundColor;
@property (strong)NSColor* textColor;
@property BOOL continuous;
@property BOOL enabled;
@property (strong)NSFont* font;
@property (strong)NSFont* titleFont;
@property (strong)NSString* title;
@property (strong)NSString* stringValue;
@property (strong)NSNumber* integerValue;
@property (strong)NSNumber* floatValue;
@property (strong)NSNumber* secondFloatValue;
@property (strong)NSImage* imageValue;
@property (strong)NSColor* colorValue;
@property NSUInteger minimumNumericPrecision;
@property NSUInteger maximumNumericPrecision;
@property BOOL showDisclosure;
@property BOOL collapsed;
@property enum PropertyInspectorCellBorder border;
@property CGFloat borderWidth;
@property NSUInteger level;
@property (strong) NSColor* borderColor;
@property NSUInteger precision;
@property NSArray* items;
@property BOOL showNullButton;
@property (readonly) BOOL nullButtonState;
@property BOOL disclosureState;
//events
@property SEL disclosureStateChangedSelector;
@property(weak) id disclosureStateChangedTarget;
@property SEL actionSelector;
@property(weak) id target;
@property SEL clickSelector;
@property (weak) id clickTarget;
@property SEL comboboxItemsSelector;
@property (weak) id comboboxItemsTarget;
-(NSView*)firstLoopableView;
-(void)setLoop:(NSView*)nextView;
-(void)commitValues;
@end
