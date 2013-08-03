//
//  PropertyInspectorGroup.m
//  CocoaTest
//
//  Created by Danilo Carvalho on 04/12/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "PropertyInspectorGroup.h"
#import "PropertyInspectorCell.h"
#import "ByReferenceKey.h"
#import "UIObjectSynchronizator.h"

//TODO: Remove performSelector because it may cause memory leaks...
//TODO: Rework the event infastructure. Don't use NSMutableDictionary. It may cause leaks...
//TODO: The use of ByReferenceKey may cause leaks...
@implementation ComboxBoxEventArgs

@end

@interface PropertyInspectorGroup ()
{
    BOOL fCollapsed;
    enum PropertyInspectorGroupType pigt;
    NSMutableDictionary* fEventDictionary;
    NSMutableDictionary* fClickedEventDictionary;
    NSMutableDictionary* fComboBoxEventDictionary;
    SetGroupContents fSetGroupContents;
}
-(PropertyInspectorGroup*)root;
-(NSArray*)flattenedHierarchy;
-(NSArray*)flattenedHierarchy:(NSMutableArray*)array;
-(void)computeBackgroundColors:(BOOL)current;
-(NSColor*)colorFrom:(BOOL)value;
+(void)computeBorders:(NSArray*)cells;
-(void)disclosureClicked: (id)sender;
+(void)computeKeyViewLoopOrder:(NSArray*)array;
-(void)privateCellAction:(id)sender;
@end

@implementation PropertyInspectorGroup
-(void)privateCellComboBoxItems:(id)sender array: (NSMutableArray*)array{
    NSUInteger index = [[self subviews] indexOfObject:sender];
    assert(index >= 0);
    
    NSArray* ev = [fComboBoxEventDictionary objectForKey:[ByReferenceKey refKeyWithObject:sender]];
    if(!ev)
        return;
    if(ev.count != 2)
        return;
    id target = [ev objectAtIndex:0];
    if(!target || target == [NSNull null])
        return;
    if([ev objectAtIndex:1] == [NSNull null])
        return;
    SEL selector = NSSelectorFromString([ev objectAtIndex:1]);
    if(!selector)
        return;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-performSelector-leaks"
    if([target respondsToSelector:selector]){
        NSNumber* oIndex = @(index - 1);
        ComboxBoxEventArgs* args = [[ComboxBoxEventArgs alloc] init];
        [args setIndex:oIndex];
        [args setItems:array];
        [target performSelector:selector withObject:self withObject:args];
    }
#pragma clang diagnostic pop
}
-(void)privateCellClicked:(id)sender{
    NSUInteger index = [[self subviews] indexOfObject:sender];
    assert(index >= 0);
    
    NSArray* ev = [fClickedEventDictionary objectForKey:[ByReferenceKey refKeyWithObject:sender]];
    if(!ev)
        return;
    if(ev.count != 2)
        return;
    id target = [ev objectAtIndex:0];
    if(!target || target == [NSNull null])
        return;
    if([ev objectAtIndex:1] == [NSNull null])
        return;
    SEL selector = NSSelectorFromString([ev objectAtIndex:1]);
    if(!selector)
        return;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-performSelector-leaks"
    if([target respondsToSelector:selector]){
        NSNumber* oIndex = @(index - 1);
        [target performSelector:selector withObject:self withObject:oIndex];
    }
#pragma clang diagnostic pop
}
-(void)privateCellAction:(id)sender{
    NSUInteger index = [[self subviews] indexOfObject:sender];
    assert(index >= 0);

    NSArray* ev = [fEventDictionary objectForKey:[ByReferenceKey refKeyWithObject:sender]];
    if(!ev)
        return;
    if(ev.count != 2)
        return;
    id target = [ev objectAtIndex:0];
    if(!target || target == [NSNull null])
        return;
    if([ev objectAtIndex:1] == [NSNull null])
        return;
    SEL selector = NSSelectorFromString([ev objectAtIndex:1]);
    if(!selector)
        return;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-performSelector-leaks"
    if(index == 0) //it's header
    {
        if([target respondsToSelector:selector])
            [target performSelector:selector withObject:self];
    }
    else {
        if([target respondsToSelector:selector]){
            NSNumber* oIndex = @(index - 1);
            [target performSelector:selector withObject:self withObject:oIndex];
        }
    }
#pragma clang diagnostic pop
}
-(void)commitValues{
    for (NSUInteger i = 0; i < [self subviews].count; i++){
        if([[[self subviews] objectAtIndex:i] isKindOfClass:[PropertyInspectorCell class]])
        {
            PropertyInspectorCell* _cell = [[self subviews] objectAtIndex:i];
            [_cell commitValues];
        }
        else if([[[self subviews] objectAtIndex:i] isKindOfClass:[PropertyInspectorGroup class]])
        {
            PropertyInspectorGroup* _group = [[self subviews] objectAtIndex:i];
            [_group commitValues];
        }
    }
}
-(void)setCell:(NSUInteger)index actionSelector:(SEL)action{
    NSMutableArray* ev = [fEventDictionary objectForKey:[ByReferenceKey refKeyWithObject:[self cell:index]]];
    id fAction = nil;
    if(action)
        fAction = NSStringFromSelector(action);
    if(!fAction)
        fAction = [NSNull null];
    [ev replaceObjectAtIndex:1 withObject:fAction];
}
-(void)setCell:(NSUInteger)index target:(id)target{
    NSMutableArray* ev = [fEventDictionary objectForKey:[ByReferenceKey refKeyWithObject:[self cell:index]]];
    id fTarget = target;
    if(!fTarget)
        fTarget = [NSNull null];
    [ev replaceObjectAtIndex:0 withObject:fTarget];
}
-(void)setCell:(NSUInteger)index comboBoxSelector:(SEL)action{
    NSMutableArray* ev = [fComboBoxEventDictionary objectForKey:[ByReferenceKey refKeyWithObject:[self cell:index]]];
    id fAction = nil;
    if(action)
        fAction = NSStringFromSelector(action);
    if(!fAction)
        fAction = [NSNull null];
    [ev replaceObjectAtIndex:1 withObject:fAction];
}
-(void)setCell:(NSUInteger)index comboBoxTarget:(id)target{
    NSMutableArray* ev = [fComboBoxEventDictionary objectForKey:[ByReferenceKey refKeyWithObject:[self cell:index]]];
    id fTarget = target;
    if(!fTarget)
        fTarget = [NSNull null];
    [ev replaceObjectAtIndex:0 withObject:fTarget];
}
-(void)setCell:(NSUInteger)index clickSelector:(SEL)action{
    NSMutableArray* ev = [fClickedEventDictionary objectForKey:[ByReferenceKey refKeyWithObject:[self cell:index]]];
    id fAction = nil;
    if(action)
        fAction = NSStringFromSelector(action);
    if(!fAction)
        fAction = [NSNull null];
    [ev replaceObjectAtIndex:1 withObject:fAction];
}
-(void)setCell:(NSUInteger)index clickTarget:(id)target{
    NSMutableArray* ev = [fClickedEventDictionary objectForKey:[ByReferenceKey refKeyWithObject:[self cell:index]]];
    id fTarget = target;
    if(!fTarget)
        fTarget = [NSNull null];
    [ev replaceObjectAtIndex:0 withObject:fTarget];
}
-(void)setHeaderActionSelector:(SEL)action{
    NSMutableArray* ev = [fEventDictionary objectForKey:[ByReferenceKey refKeyWithObject:[[self subviews] objectAtIndex:0]]];
    id fAction = nil;
    if(action)
        fAction = NSStringFromSelector(action);
    if(!fAction)
        fAction = [NSNull null];
    [ev replaceObjectAtIndex:1 withObject:fAction];
}
-(void)setHeaderTarget:(id)target{
    NSMutableArray* ev = [fEventDictionary objectForKey:[ByReferenceKey refKeyWithObject:[[self subviews] objectAtIndex:0]]];
    id fTarget = target;
    if(!fTarget)
        fTarget = [NSNull null];
    [ev replaceObjectAtIndex:0 withObject:fTarget];
}
-(NSString *)headerStringValue{
    return [[[self subviews] objectAtIndex:0] stringValue];
}
-(NSString *)headerText{
    PropertyInspectorCell* firstCell = (PropertyInspectorCell*)[[self subviews] objectAtIndex:0];
    return [firstCell title];
}
-(void)setHeaderText:(NSString *)headerText{
    PropertyInspectorCell* firstCell = (PropertyInspectorCell*)[[self subviews] objectAtIndex:0];
    [firstCell setTitle:headerText];
}
-(enum PropertyInspectorGroupType)type{
    return pigt;
}
-(void)setType:(enum PropertyInspectorGroupType)type{
    pigt = type;
    PropertyInspectorCell* firstCell = (PropertyInspectorCell*)[[self subviews] objectAtIndex:0];
    if(type == pigtCollapsible){
        [firstCell setShowDisclosure:YES];
        [firstCell setCollapsed:NO];
    }
    else {
        [firstCell setShowDisclosure:NO];
        [firstCell setCollapsed:YES];
    }
}
-(void)fullLayout: (NSView*)view {
    [view setNeedsUpdateConstraints:YES];
    [view setNeedsLayout:YES];
    for (int i = 0; i < [view subviews].count; i++) {
        NSView* sb = [[view subviews] objectAtIndex:i];
        [self fullLayout:sb];
    }
}
-(void)fullLayout {
    [self forcedFullLayout:YES];
}
-(void)forcedFullLayout:(BOOL)forced {
    if(![UIObjectSynchronizator avoidLayout]){
        PropertyInspectorGroup* r = [self root];
        [self fullLayout:r];
        if(forced){
            [[self window] layoutIfNeeded];
            [r layoutSubtreeIfNeeded];
        }
    }
}
-(void)disclosureClicked:(id)sender{
    PropertyInspectorCell* firstCell = ((PropertyInspectorCell*)[[self subviews] objectAtIndex:0]);
    if([firstCell disclosureState])
        self.collapsed = NO;
    else
        self.collapsed = YES;
    [self fullLayout];
    [self recomputeKeyViewLoopOrder];
}

-(PropertyInspectorGroup*)root{
    if ([[self superview] class] == [PropertyInspectorGroup class]) {
        return [(PropertyInspectorGroup*)[self superview] root];
    }
    else
        return self;
}
-(NSColor *)colorFrom:(BOOL)value{
    if(value)
        return [self backgroundColor];
    else
        return [self alternatingBackgroundColor];
}

-(void)computeBackgroundColors:(BOOL)current {
    if (pigt == pigtInline && [self childCount] == 0)
        return;
    
    BOOL _previousGroup = NO;
    
    for (int i = 0; i < [self subviews].count; i++) {
        id subview = [[self subviews] objectAtIndex:i];
        if([subview class] == [PropertyInspectorCell class]) {
            if(_previousGroup)
                current = !current;
            [subview setBackgroundColor:[self colorFrom:current]];
            [subview setLevel:0];
            _previousGroup = NO;
        }
        else {
            current = !current;
            _previousGroup = YES;
            [subview computeBackgroundColors:current];
        }
    }
}

-(NSArray *)flattenedHierarchy{
    NSMutableArray* array = [NSMutableArray array];
    return [self flattenedHierarchy: array];
}

-(NSArray *)flattenedHierarchy:(NSMutableArray *)array{
    int start = 0;
    if(pigt != pigtCollapsible)
        start = 1;
    for (int i = start; i < [self subviews].count; i++) {
        id subview = [[self subviews] objectAtIndex:i];
        if([subview class] == [PropertyInspectorCell class]) {
            [array addObject:subview];
        }
        else {
            [((PropertyInspectorGroup*)subview) flattenedHierarchy:array];
        }
    }
    
    return array;
}

+(void)computeBorders:(NSArray *)cells{
    for (int i = 0; i < cells.count; i++) {
        PropertyInspectorGroup* superview = (PropertyInspectorGroup*)[[cells objectAtIndex: i] superview];
        [[cells objectAtIndex:i] setTextColor:[superview textColor]];
        [[cells objectAtIndex:i] setFont:[superview font]];
        [[cells objectAtIndex:i] setTitleFont:[superview titleFont]];
        if(i == 0){
            PropertyInspectorCell* currentCell = [cells objectAtIndex:i];
            [currentCell setBorderColor:[superview borderColor]];
            [currentCell setBorderWidth:[superview borderWidth]];
            [currentCell setBorder:picbTop];
        }
        else {
            PropertyInspectorCell* currentCell = [cells objectAtIndex:i];
            PropertyInspectorCell* previousCell = [cells objectAtIndex:(i - 1)];
            if ([currentCell.backgroundColor isEqual:previousCell.backgroundColor])
                [currentCell setBorder:picbNone];
            else {
                [currentCell setBorderColor:[superview borderColor]];
                [currentCell setBorderWidth:[superview borderWidth]];
                [currentCell setBorder:picbTop];
            }
        }
    }
}

-(void)drawRect:(NSRect)dirtyRect{
    if([[self superview] class] != [PropertyInspectorGroup class]){
        NSSize size = [self frame].size;
        [self.borderColor set];
        NSRectFill(NSMakeRect(0, size.height - 1, size.width, self.borderWidth));
    }
}

+(void)computeKeyViewLoopOrder:(NSArray *)array{
    for (int i = 0; i < array.count; i++) {
        if(i == (array.count - 1))
            [[array objectAtIndex:i] setLoop:[[array objectAtIndex:0] firstLoopableView]];
        else
            [[array objectAtIndex:i] setLoop:[[array objectAtIndex:i + 1] firstLoopableView]];
    }
}

-(void)setTitleTopBorders {
    PropertyInspectorCell* firstCell = ((PropertyInspectorCell*)[[self subviews] objectAtIndex:0]);
    [self setUpTitleBorder:firstCell];
    for (int i = 0; i < self.subviews.count; i++) {
        NSView* v = [self.subviews objectAtIndex:i];
        if([v isKindOfClass:[PropertyInspectorGroup class]]){
            [((PropertyInspectorGroup*)v) setTitleTopBorders];
        }
    }
}

-(void)redraw{
    if(![UIObjectSynchronizator avoidLayout]){
        PropertyInspectorGroup* r = [self root];
        [r computeBackgroundColors:NO];
        NSArray* hierarchy = [r flattenedHierarchy];
        [PropertyInspectorGroup computeBorders:hierarchy];
        [r setTitleTopBorders];
        NSMutableArray* nonCollapsed = [NSMutableArray array];
        for (int i = 0; i < hierarchy.count; i++) {
            if (![[hierarchy objectAtIndex:i] collapsed])
                [nonCollapsed addObject:[hierarchy objectAtIndex:i]];
        }
        [PropertyInspectorGroup computeKeyViewLoopOrder: nonCollapsed];
        for (int i = 0; i < hierarchy.count; i++) {
            [[hierarchy objectAtIndex:i] setBackgroundColor:self.backgroundColor];
        }
    }
}

-(void)recomputeKeyViewLoopOrder{
    PropertyInspectorGroup* r = [self root];
    NSArray* hierarchy = [r flattenedHierarchy];
    NSMutableArray* nonCollapsed = [NSMutableArray array];
    for (int i = 0; i < hierarchy.count; i++) {
        if (![[hierarchy objectAtIndex:i] collapsed])
            [nonCollapsed addObject:[hierarchy objectAtIndex:i]];
    }
    [PropertyInspectorGroup computeKeyViewLoopOrder: nonCollapsed];
}

-(NSUInteger)childCount{
    return [self subviews].count  - 1;
}

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        pigt = pigtCollapsible;
        self.backgroundColor = [NSColor controlColor];
        self.alternatingBackgroundColor = [NSColor secondarySelectedControlColor];
        self.textColor = [NSColor controlTextColor];
        self.font = [NSFont systemFontOfSize:11];
        self.titleFont = [NSFont boldSystemFontOfSize:11];
        self.borderWidth = 1;
        self.borderColor = [NSColor controlShadowColor];
        fCollapsed = NO;
        fSetGroupContents = nil;
        PropertyInspectorCell* pic = [[PropertyInspectorCell alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
        [pic setType:pictLabel];
        [pic setBackgroundColor:self.backgroundColor];
        [pic setTextColor:self.textColor];
        [pic setEnabled:YES];
        [pic setFont:self.font];
        [pic setTitleFont:self.titleFont];
        [pic setStringValue:@""];
        [pic setShowDisclosure:YES];
        [pic setCollapsed:NO];
        [pic setBorder:picbTop];
        [pic setTitle:@""];
        [pic setLevel:0];
        [pic setBorderColor:self.borderColor];
        [pic setBorderWidth:self.borderWidth];
        [pic setDisclosureStateChangedSelector: @selector(disclosureClicked:)];
        [pic setDisclosureStateChangedTarget: self];
        [pic setDisclosureState:YES];
        [pic setTarget:self];
        [pic setActionSelector:@selector(privateCellAction:)];
        [self addSubview: pic];
        fEventDictionary = [NSMutableDictionary dictionary];
        [fEventDictionary setObject:[NSMutableArray arrayWithArray:@[[NSNull null], [NSNull null]]] forKey:[ByReferenceKey refKeyWithObject:pic]];
        fClickedEventDictionary = [NSMutableDictionary dictionary];
        fComboBoxEventDictionary = [NSMutableDictionary dictionary];
    }
    
    return self;
}

-(BOOL)collapsed{
    return fCollapsed;
}

-(void)collapse:(BOOL)value{
    BOOL originalValue = value;
    if(!value)
        value = fCollapsed;
    
    for (int i = 0; i < [self subviews].count; i++) {
        id subview = [[self subviews] objectAtIndex:i];
        if([subview class] == [PropertyInspectorCell class])
            [((PropertyInspectorCell*)subview) setCollapsed:value];
        else
            [((PropertyInspectorGroup*)subview) collapse:value];
    }
    
    //restore the first cell
    if(!originalValue && pigt == pigtCollapsible){
        PropertyInspectorCell* firstCell = ((PropertyInspectorCell*)[[self subviews] objectAtIndex:0]);
        [firstCell setCollapsed:NO];
    }
    else if(!originalValue && pigt != pigtCollapsible){
        PropertyInspectorCell* firstCell = ((PropertyInspectorCell*)[[self subviews] objectAtIndex:0]);
        [firstCell setCollapsed:YES];
    }
}

-(void)commitCollapsedState{
    PropertyInspectorGroup* r = [self root];
    [r commitCollapsedState:NO];
}

-(void)commitCollapsedState:(BOOL)collapsed {
    for (int i = 0; i < [self subviews].count; i++) {
        id subview = [[self subviews] objectAtIndex:i];
        if(i == 0) //header
        {
            [((PropertyInspectorCell*)subview) setDisclosureState:!fCollapsed];
            [((PropertyInspectorCell*)subview) setCollapsed:(collapsed || pigt != pigtCollapsible)];
        }
        else if([subview class] == [PropertyInspectorCell class])
            [((PropertyInspectorCell*)subview) setCollapsed:(collapsed || fCollapsed)];
        else
            [((PropertyInspectorGroup*)subview) commitCollapsedState:(collapsed || fCollapsed)];
    }
}

-(BOOL)hasLazyContent{
    return fCollapsed && fSetGroupContents;
}

-(void)setLazyContents:(SetGroupContents)f{
    fCollapsed = YES;
    PropertyInspectorCell* firstCell = ((PropertyInspectorCell*)[[self subviews] objectAtIndex:0]);
    [firstCell setDisclosureState:!fCollapsed];
    fSetGroupContents = f;
}

-(void)selfLazyLoad{
    if(fCollapsed && fSetGroupContents){
        SetGroupContents _temp = fSetGroupContents;
        fSetGroupContents = nil;
        _temp(self);
        return;
    }
}

-(void)lazyLoad{
    [self selfLazyLoad];
    //Teoricamente, não precisamos disso...
//    for (int i = 0; i < [self subviews].count; i++) {
//        id subview = [[self subviews] objectAtIndex:i];
//        if([subview class] == [PropertyInspectorGroup class]){
//            [subview lazyLoad];
//        }
//    }
}

- (void)setUpTitleBorder:(PropertyInspectorCell *)firstCell {
    //set the border
    PropertyInspectorCell* secondChild = nil;
    if([self subviews].count > 1)
        secondChild = ((PropertyInspectorCell*)[[self subviews] objectAtIndex:1]);
    if(self.collapsed)
        [firstCell setBorder:picbTop];
    else {
        if([secondChild isKindOfClass:[PropertyInspectorCell class]]){
            [firstCell setBorder:picbTop | picbBottom];
        }
        else if([secondChild isKindOfClass:[PropertyInspectorGroup class]]){
            [firstCell setBorder:picbTop];
        }
    }
}

-(void)setCollapsed:(BOOL)collapsed{
    fCollapsed = collapsed;
    if([UIObjectSynchronizator avoidLayout])
        return;
    if(!fCollapsed && fSetGroupContents){
        //set the disclosure state...
        PropertyInspectorCell* firstCell = ((PropertyInspectorCell*)[[self subviews] objectAtIndex:0]);
        [firstCell setDisclosureState:!fCollapsed];
        SetGroupContents _temp = fSetGroupContents;
        fSetGroupContents = nil;
        _temp(self);
        [self redraw];
        [self forcedFullLayout:NO];
        [self setUpTitleBorder:firstCell];
        return;
    }
    if(pigt != pigtCollapsible){
        [self collapse:fCollapsed];
        return;
    }
    for (int i = 1; i < [self subviews].count; i++) {
        id subview = [[self subviews] objectAtIndex:i];
        if([subview class] == [PropertyInspectorCell class])
            [((PropertyInspectorCell*)subview) setCollapsed:fCollapsed];
        else
            [((PropertyInspectorGroup*)subview) collapse:fCollapsed];
    }
    PropertyInspectorCell* firstCell = ((PropertyInspectorCell*)[[self subviews] objectAtIndex:0]);
    [firstCell setDisclosureState:!collapsed];
    //set the border
    [self setUpTitleBorder:firstCell];
}
-(BOOL)isNotCollapsed{
    if([self collapsed])
        return NO;
    
    if([[self superview] respondsToSelector:@selector(isNotCollapsed)])
        return [[self superview] performSelector:@selector(isNotCollapsed)];
    
    return YES;
}

-(NSUInteger)insertCellWithType:(enum PropertyInspectorCellType)pict title:(NSString *)title continuous:(BOOL)continuous{
    PropertyInspectorCell* cell = [[PropertyInspectorCell alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    cell.type = pict;
    cell.title = title;
    cell.continuous = continuous;
    cell.target = self;
    cell.clickTarget = self;
    cell.comboboxItemsTarget = self;
    cell.actionSelector = @selector(privateCellAction:);
    cell.clickSelector = @selector(privateCellClicked:);
    cell.comboboxItemsSelector = @selector(privateCellComboBoxItems:array:);
    [fEventDictionary setObject:[NSMutableArray arrayWithArray: @[[NSNull null], [NSNull null]]] forKey:[ByReferenceKey refKeyWithObject:cell]];
    [fClickedEventDictionary setObject:[NSMutableArray arrayWithArray: @[[NSNull null], [NSNull null]]] forKey:[ByReferenceKey refKeyWithObject:cell]];
    [fComboBoxEventDictionary setObject:[NSMutableArray arrayWithArray: @[[NSNull null], [NSNull null]]] forKey:[ByReferenceKey refKeyWithObject:cell]];
    [self addSubview:cell];
    [self setCollapsed:![self isNotCollapsed]];
    return [self.subviews count] - 2;
}
-(NSUInteger)insertSubgroup:(PropertyInspectorGroup *)group{
    [self addSubview:group];
    [self setCollapsed:![self isNotCollapsed]];
    return [self.subviews count] - 2;
}
-(void)removeChild:(NSUInteger)index{
    assert(index >= 0);
    assert(index < [self childCount]);
    NSMutableArray* newSubviewsArray = [NSMutableArray arrayWithArray:[self subviews]];
    id cell = [newSubviewsArray objectAtIndex:index + 1];
    if([cell isKindOfClass:[PropertyInspectorCell class]]){
        [cell setTarget:nil];
        [cell setActionSelector:nil];
    }
    [newSubviewsArray removeObjectAtIndex:index + 1];
    [self setSubviews:newSubviewsArray];
}
-(void)clearChildren{
    NSMutableArray* newSubviewsArray = [NSMutableArray arrayWithArray:[self subviews]];
    id headerCell = [newSubviewsArray objectAtIndex:0];
    for (int i = 1; i < newSubviewsArray.count; i++) {
        id cell = [newSubviewsArray objectAtIndex:i];
    
        if([cell isKindOfClass:[PropertyInspectorCell class]]){
            [cell setTarget:nil];
            [cell setActionSelector:nil];
        }
    }
    
    [newSubviewsArray removeAllObjects];
    [newSubviewsArray addObject:headerCell];
    [self setSubviews:newSubviewsArray];
}
-(BOOL)isCell:(NSUInteger)index{
    assert(index >= 0);
    assert(index < [self childCount]);
    return [[[self subviews] objectAtIndex:index + 1] class] == [PropertyInspectorCell class];
}

-(PropertyInspectorCell *)cell:(NSUInteger)index{
    assert([self isCell:index]);
    return [[self subviews] objectAtIndex:index + 1];
}

-(void)setCell:(NSUInteger)index colorValue:(NSColor *)value{
    [[self cell:index] setColorValue:value];
}

-(void)setCell:(NSUInteger)index continuous:(BOOL)continuous{
    [[self cell:index] setContinuous:continuous];
}

-(void)setCell:(NSUInteger)index enabled:(BOOL)value{
    [[self cell:index] setEnabled: value];
}

-(void)setCell:(NSUInteger)index floatValue:(NSNumber *)value{
    [[self cell:index] setFloatValue:value];
}

-(void)setCell:(NSUInteger)index imageValue:(NSImage *)value{
    [[self cell:index] setImageValue:value];
}

-(void)setCell:(NSUInteger)index integerValue:(NSNumber *)value{
    [[self cell:index] setIntegerValue:value];
}

-(void)setCell:(NSUInteger)index items:(NSArray *)items{
    [[self cell:index] setItems:items];
}

-(void)setCell:(NSUInteger)index maximumNumericPrecision:(NSUInteger)precision{
    [[self cell:index] setMaximumNumericPrecision:precision];
}

-(void)setCell:(NSUInteger)index minimumNumericPrecision:(NSUInteger)precision{
    [[self cell:index] setMinimumNumericPrecision:precision];
}

-(void)setCell:(NSUInteger)index nullable:(BOOL)nullable{
    [[self cell:index] setShowNullButton:nullable];
}

-(void)setCell:(NSUInteger)index precision:(NSUInteger)precision{
    [[self cell:index] setPrecision:precision];
}

-(void)setCell:(NSUInteger)index secondFloatValue:(NSNumber *)value{
    [[self cell:index] setSecondFloatValue:value];
}

-(void)setCell:(NSUInteger)index stringValue:(NSString *)value{
    [[self cell:index] setStringValue:value];
}

-(void)setCell:(NSUInteger)index title:(NSString *)title{
    [[self cell:index] setTitle:title];
}

-(PropertyInspectorGroup *)group:(NSUInteger)index{
    assert(![self isCell:index]);
    return [[self subviews] objectAtIndex:index + 1];
}
-(id)child:(NSUInteger)index{
    assert(index >= 0);
    assert(index < [self childCount]);
    return [[self subviews] objectAtIndex:index + 1];
}

-(NSString *)cellTitle:(NSUInteger)index{
    return [[self cell:index] title];
}
-(enum PropertyInspectorCellType)cellType:(NSUInteger)index{
    return [[self cell:index] type];
}
-(NSColor *)cellColorValue:(NSUInteger)index{
    return [[self cell:index] colorValue];
}
-(BOOL)cellContinuous:(NSUInteger)index{
    return [[self cell:index] continuous];
}
-(BOOL)cellEnabled:(NSUInteger)index{
    return [[self cell: index] enabled];
}
-(NSUInteger)cellMinimumNumericPrecision:(NSUInteger)index{
    return [[self cell:index] minimumNumericPrecision];
}
-(NSUInteger)cellMaximumNumericPrecision:(NSUInteger)index{
    return [[self cell:index] maximumNumericPrecision];
}
-(NSUInteger)cellPrecision:(NSUInteger)index{
    return [[self cell:index] precision];
}
-(BOOL)cellNullable:(NSUInteger)index{
    return [[self cell:index] showNullButton];
}
-(NSString *)cellStringValue:(NSUInteger)index{
    return [[self cell:index] stringValue];
}
-(NSNumber *)cellIntegerValue:(NSUInteger)index{
    return [[self cell:index] integerValue];
}
-(NSNumber *)cellFloatValue:(NSUInteger)index{
    return [[self cell:index] floatValue];
}
-(NSNumber *)cellSecondFloatValue:(NSUInteger)index{
    return [[self cell:index] secondFloatValue];
}
-(NSImage *)cellImageValue:(NSUInteger)index{
    return [[self cell:index] imageValue];
}
-(NSArray *)cellItems:(NSUInteger)index{
    return [[self cell:index] items];
}
-(NSArray *)headerItems{
    PropertyInspectorCell* firstCell = ((PropertyInspectorCell*)[[self subviews] objectAtIndex:0]);
    return [firstCell items];
}
-(void)setHeaderItems:(NSArray *)headerItems{
    PropertyInspectorCell* firstCell = ((PropertyInspectorCell*)[[self subviews] objectAtIndex:0]);
    firstCell.items = headerItems;
}
@end
























