//
//  PropertyInspectorGroup.h
//  CocoaTest
//
//  Created by Danilo Carvalho on 04/12/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "StackPanel.h"
#import "PropertyInspectorCell.h"

enum PropertyInspectorGroupType {
    pigtInline,
    pigtGroup,
    pigtCollapsible
};

enum PropertyInspectorGroupField {
    pigfText = 0,
    pigfInt = 1,
    pigfFloat = 2,
    pigfFloatPair = 3,
    pigfBool = 4,
    pigfRanged = 5,
    pigfAngle = 6,
    pigfColor = 7,
    pigfImage = 8,
    pigfEnum = 9
};

@class PropertyInspectorGroup;
typedef void (^SetGroupContents)(PropertyInspectorGroup*);

@interface ComboxBoxEventArgs : NSObject
@property (strong)NSNumber* index;
@property (strong)NSMutableArray* items;
@end

@interface PropertyInspectorGroup : StackPanel
@property enum PropertyInspectorGroupType type;
//cell messages
//set cell properties
-(NSUInteger)insertCellWithType:(enum PropertyInspectorCellType)pict title:(NSString*)title continuous:(BOOL)continuous;
-(void)removeChild:(NSUInteger)index;
-(void)clearChildren;
-(PropertyInspectorCell*)cell: (NSUInteger)index;
-(void)setCell:(NSUInteger)index title:(NSString*)title;
-(void)setCell:(NSUInteger)index continuous:(BOOL)continuous;
-(void)setCell:(NSUInteger)index enabled:(BOOL)value;
-(void)setCell:(NSUInteger)index minimumNumericPrecision:(NSUInteger)precision;
-(void)setCell:(NSUInteger)index maximumNumericPrecision:(NSUInteger)precision;
-(void)setCell:(NSUInteger)index precision:(NSUInteger)precision;
-(void)setCell:(NSUInteger)index nullable:(BOOL)nullable;
-(void)setCell:(NSUInteger)index stringValue:(NSString*)value;
-(void)setCell:(NSUInteger)index integerValue:(NSNumber*)value;
-(void)setCell:(NSUInteger)index floatValue:(NSNumber*)value;
-(void)setCell:(NSUInteger)index secondFloatValue:(NSNumber*)value;
-(void)setCell:(NSUInteger)index imageValue:(NSImage*)value;
-(void)setCell:(NSUInteger)index colorValue:(NSColor*)value;
-(void)setCell:(NSUInteger)index items:(NSArray*)items;

-(void)setCell:(NSUInteger)index actionSelector:(SEL)action;
-(void)setCell:(NSUInteger)index target:(id)target;
-(void)setCell:(NSUInteger)index clickSelector:(SEL)action;
-(void)setCell:(NSUInteger)index clickTarget:(id)target;
-(void)setCell:(NSUInteger)index comboBoxSelector:(SEL)action;
-(void)setCell:(NSUInteger)index comboBoxTarget:(id)target;
-(void)setHeaderActionSelector:(SEL)selector;
-(void)setHeaderTarget:(id)target;

//subgroups
-(BOOL)isCell:(NSUInteger)index;
-(NSUInteger)insertSubgroup:(PropertyInspectorGroup*)group;
-(PropertyInspectorGroup*)group:(NSUInteger)index;
-(id)child:(NSUInteger)index;
//get cell properties
-(NSUInteger)childCount;
-(enum PropertyInspectorCellType)cellType:(NSUInteger)index;
-(NSString*)cellTitle:(NSUInteger)index;
-(BOOL)cellContinuous:(NSUInteger)index;
-(BOOL)cellEnabled:(NSUInteger)index;
-(NSUInteger)cellMinimumNumericPrecision:(NSUInteger)index;
-(NSUInteger)cellMaximumNumericPrecision:(NSUInteger)index;
-(NSUInteger)cellPrecision:(NSUInteger)index;
-(BOOL)cellNullable:(NSUInteger)index;
-(NSString*)cellStringValue:(NSUInteger)index;

-(NSString*)headerStringValue;

-(NSNumber*)cellIntegerValue:(NSUInteger)index;
-(NSNumber*)cellFloatValue:(NSUInteger)index;
-(NSNumber*)cellSecondFloatValue:(NSUInteger)index;
-(NSImage*)cellImageValue:(NSUInteger)index;
-(NSColor*)cellColorValue:(NSUInteger)index;
-(NSArray*)cellItems:(NSUInteger)index;
//overall properties
@property (strong)NSColor* backgroundColor;
@property (strong)NSColor* alternatingBackgroundColor;
@property (strong)NSColor* textColor;
@property (strong)NSFont* font;
@property (strong)NSFont* titleFont;
@property CGFloat borderWidth;
@property (strong) NSColor* borderColor;
@property BOOL collapsed;
@property (strong) NSString* headerText;
@property (strong) NSArray* headerItems;
-(void)commitValues;
-(void)commitCollapsedState;
-(void)setLazyContents:(SetGroupContents)f;
//redraw
-(BOOL)hasLazyContent;
-(void)lazyLoad;
-(void)redraw;
-(void)fullLayout;
-(void)forcedFullLayout:(BOOL)forced;
-(void)recomputeKeyViewLoopOrder;
@end
