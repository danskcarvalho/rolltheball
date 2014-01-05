//
//  UIObjectSynchronizator.m
//  CocoaTest
//
//  Created by Danilo Carvalho on 28/01/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import "UIObjectSynchronizator.h"
#import "ConstructibleType.h"
#import "ObjectSite.h"
#import "UISite.h"
#import "NavigablePropertyInspector.h"
#import "PropertyInspector.h"

#define OBJECT_LINK     1
#define OBJECT_GROUP    2
#define ARRAY_LINK      3
#define ARRAY_GROUP     4

@interface UIObjectSynchronizator ()
{
    //from UISite to ObjectSite
    NSMutableDictionary* fUIObjectMapping;
    id<ObservableObject> fRoot;
    PropertyInspectorGroup* fRootGroup;
}
-(void)createUIFromObject:(id<ObservableObject>)object type:(TypeDescriptor*)descriptor inside: (PropertyInspectorGroup*)group forProperty:(NSString*)propertyName;
-(void)clearContents:(PropertyInspectorGroup*)group;
-(void)relayout;
@end

@implementation UIObjectSynchronizator
static BOOL fAvoidLayout = NO;

-(void)objectDeleted:(id<ObservableObject>)object{
    
}
+(BOOL)avoidLayout{
    return fAvoidLayout;
}

+(void)setAvoidLayout:(BOOL)value{
    fAvoidLayout = value;
}

-(void)propertyChanged:(id<ObservableObject>)object property:(NSString *)property{
    //unions can't have properties themselves
    //all properties inside the unionized should be unique.
    TypeDescriptor* otd = [object typeDescriptor];
    
    NSUInteger propIndex = [otd.properties indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop) {
        return [[obj name] isEqualToString: property];
    }];
    PropertyDescriptor* prop = nil;
    if(propIndex != NSNotFound)
        prop = [otd.properties objectAtIndex: propIndex];
    TypeDescriptor* td = [prop propertyType];
    
    if(td.GTD == gtdAngle){
        NSNumber* fValue = nil;
        [object getFloat:&fValue property:property];
        NSArray* uiSites = [fUIObjectMapping allKeysForObject:[ObjectSite objectSiteWithObject:object property:property]];
        for (UISite* site in uiSites) {
            [site.group setCell:site.cellIndex floatValue:fValue];
        }
    }
    else if(td.GTD == gtdBool){
        NSNumber* value = nil;
        [object getBool:&value property:property];
        NSArray* uiSites = [fUIObjectMapping allKeysForObject:[ObjectSite objectSiteWithObject:object property:property]];
        for (UISite* site in uiSites) {
            [site.group setCell:site.cellIndex integerValue:value];
        }
    }
    else if(td.GTD == gtdColor){
        NSColor* value = nil;
        [object getColor:&value property:property];
        NSArray* uiSites = [fUIObjectMapping allKeysForObject:[ObjectSite objectSiteWithObject:object property:property]];
        for (UISite* site in uiSites) {
            [site.group setCell:site.cellIndex colorValue:value];
        }
    }
    else if(td.GTD == gtdEnum){
        NSNumber* value = nil;
        [object getInt:&value property:property];
        NSString* strValue = [[[td enumValues] allKeysForObject:value] objectAtIndex:0];
        NSArray* uiSites = [fUIObjectMapping allKeysForObject:[ObjectSite objectSiteWithObject:object property:property]];
        for (UISite* site in uiSites) {
            [site.group setCell:site.cellIndex stringValue:strValue];
        }
    }
    else if(td.GTD == gtdFloat){
        NSNumber* value = nil;
        [object getFloat:&value property:property];
        NSArray* uiSites = [fUIObjectMapping allKeysForObject:[ObjectSite objectSiteWithObject:object property:property]];
        for (UISite* site in uiSites) {
            [site.group setCell:site.cellIndex floatValue:value];
        }
    }
    else if(td.GTD == gtdFloatPair){
        NSNumber* value = nil;
        NSNumber* value2 = nil;
        [object getFloatPair:&value second:&value2 property:property];
        NSArray* uiSites = [fUIObjectMapping allKeysForObject:[ObjectSite objectSiteWithObject:object property:property]];
        for (UISite* site in uiSites) {
            [site.group setCell:site.cellIndex floatValue:value];
            [site.group setCell:site.cellIndex secondFloatValue:value2];
        }
    }
    else if(td.GTD == gtdImage){
        NSImage* value = nil;
        [object getImage:&value property:property];
        NSArray* uiSites = [fUIObjectMapping allKeysForObject:[ObjectSite objectSiteWithObject:object property:property]];
        for (UISite* site in uiSites) {
            [site.group setCell:site.cellIndex imageValue:value];
        }
    }
    else if(td.GTD == gtdInt){
        NSNumber* value = nil;
        [object getInt:&value property:property];
        NSArray* uiSites = [fUIObjectMapping allKeysForObject:[ObjectSite objectSiteWithObject:object property:property]];
        for (UISite* site in uiSites) {
            [site.group setCell:site.cellIndex integerValue:value];
        }
    }
    else if(td.GTD == gtdObj || td.GTD == gtdUnion){
        id value = nil;
        [object getObject:&value property:property];
        NSArray* uiSites = [fUIObjectMapping allKeysForObject:[ObjectSite objectSiteWithObject:object property:property]];
        for (UISite* site in uiSites) {
            if([site.group hasLazyContent])
                continue;
            
            if(site.tag && [site.tag isEqual:@(OBJECT_LINK)]){
                if(value)
                    [site.group setCell:site.cellIndex stringValue:[NSString stringWithFormat:@"Edit %@", td.displayName]];
                else
                    [site.group setCell:site.cellIndex stringValue:@"<NULL>"];
            }
            else {
                [self clearContents:site.group];
                [self createUIFromObject:value type:nil inside:site.group forProperty:nil];
            }
        }
        [self relayout];
    }
    else if(td.GTD == gtdRanged){
        NSNumber* value = nil;
        [object getFloat:&value property:property];
        NSArray* uiSites = [fUIObjectMapping allKeysForObject:[ObjectSite objectSiteWithObject:object property:property]];
        for (UISite* site in uiSites) {
            [site.group setCell:site.cellIndex floatValue:value];
        }
    }
    else if(td.GTD == gtdText){
        NSString* value = nil;
        [object getString:&value property:property];
        NSArray* uiSites = [fUIObjectMapping allKeysForObject:[ObjectSite objectSiteWithObject:object property:property]];
        for (UISite* site in uiSites) {
            [site.group setCell:site.cellIndex stringValue:value];
        }
    }
    else if(td.GTD == gtdStringWithAutoCompletion){
        NSString* value = nil;
        [object getString:&value property:property];
        NSArray* uiSites = [fUIObjectMapping allKeysForObject:[ObjectSite objectSiteWithObject:object property:property]];
        for (UISite* site in uiSites) {
            [site.group setCell:site.cellIndex stringValue:value];
        }
    }
    else if(td.GTD == gtdFlags){
        NSNumber* value = nil;
        [object getInt:&value property:property];
        NSArray* uiSites = [fUIObjectMapping allKeysForObject:[ObjectSite objectSiteWithObject:object property:property]];
        for (UISite* site in uiSites) {
            [site.group setCell:site.cellIndex integerValue:value];
        }
    }
    else if(td.GTD == gtdAction){
        //never will be here...
    }
    else { //gtdVector
        NSArray* value = nil;
        [object getVector:&value property:property];
        NSArray* uiSites = [fUIObjectMapping allKeysForObject:[ObjectSite objectSiteWithObject:object property:property]];
        for (UISite* site in uiSites) {
            if([site.group hasLazyContent])
                continue;
            
            if(site.tag && [site.tag isEqual:@(ARRAY_LINK)]){
                //do nothing...
            }
            else {
                PropertyDescriptor* prop = [otd.properties objectAtIndex: [otd.properties indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop) {
                    return [[obj name] isEqualToString: property];
                }]];
                [self clearContents:site.group];
                [self createUIArrayContents:site.group arrayValue:value propertyType:prop.propertyType collapsed:nil];
            }
        }
        [self relayout];
    }
}
-(void)unionTagChanged:(id<ObservableObject>)object{
    NSString* value = nil;
    [object getUnionTag:&value];
    NSArray* uiSites = [fUIObjectMapping allKeysForObject:[ObjectSite objectSiteWithObject:object property:TagProperty]];
    for (UISite* site in uiSites) {
        [site.group setCell:site.cellIndex stringValue:value];
        for (NSUInteger i = 0; i < [site.group childCount]; i++) {
            id child = [site.group child:i];
            if(![child isKindOfClass:[PropertyInspectorGroup class]])
                continue;
            if([[child headerText] isEqualToString:value])
                [child setCollapsed:NO];
            else
                [child setCollapsed:YES];
        }
    }
    [self relayout];
}

-(NSMutableArray*)collapsedArray:(PropertyInspectorGroup*)arrayGroup{
    NSMutableArray* a = [NSMutableArray array];
    for (NSUInteger i = 0; i < [arrayGroup childCount]; i++) {
        PropertyInspectorGroup* grp = [arrayGroup child:i];
        if([grp isKindOfClass:[PropertyInspectorGroup class]]){
            [a addObject:@([grp collapsed])];
        }
    }
    return a;
}

-(void)headerValueChanged:(id)group{
    [group lazyLoad];
    NSString* value = [group headerStringValue];
    ObjectSite* objSite = [fUIObjectMapping objectForKey:[UISite uiSiteWithGroup:group index:uiSiteHeaderIndex]];
    TypeDescriptor* otd = [objSite.object typeDescriptor];
//    if (otd.GTD == gtdUnion){
//        NSString* tag = nil;
//        [objSite.object getUnionTag:&tag];
//        otd = [otd.unionMap objectForKey:tag];
//    }
    [group commitValues];
    
    if(objSite.property){
        if([value isEqualToString:@"Reset"]){
            [objSite.object setNil:objSite.property];
            [self clearContents:group];
            id newObj;
            [objSite.object getObject:&newObj property:objSite.property];
            [self createUIFromObject:newObj type:nil inside:group forProperty:nil];
            [self relayout:group forced:NO];
        }
        else if([value isEqualToString: @"New Object"]){
            PropertyDescriptor* prop = [otd.properties objectAtIndex: [otd.properties indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop) {
                return [[obj name] isEqualToString: objSite.property];
            }]];
            ConstructibleType* ct = (ConstructibleType*)[prop propertyType];
            if([ct isKindOfClass:[ConstructibleType class]]){
                id newObj = [ct newObject];
                [objSite.object setObject:newObj property:objSite.property];
                [self clearContents:group];
                [objSite.object getObject:&newObj property:objSite.property];
                [self createUIFromObject:newObj type:nil inside:group forProperty:nil];
            }
            [self relayout:group forced:NO];
        }
        else if([value isEqualToString: @"Prepend Item"]){
            PropertyDescriptor* prop = [otd.properties objectAtIndex: [otd.properties indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop) {
                return [[obj name] isEqualToString: objSite.property];
            }]];
            ConstructibleType* ct = (ConstructibleType*)[[prop propertyType] elementType];
            if([ct isKindOfClass:[ConstructibleType class]]){
                NSMutableArray* collapsedV = [self collapsedArray:group];
                id newObj = [ct newObject];
                
                NSArray* oldVector = nil;
                [objSite.object getVector:&oldVector property:objSite.property];
                NSMutableArray* newVector = [NSMutableArray arrayWithArray:oldVector];
                [newVector insertObject:newObj atIndex:0]; //at start
                [collapsedV insertObject:@(YES) atIndex:0];
                [objSite.object setVector:newVector property:objSite.property];
                
                [self clearContents:group];
                [objSite.object getVector:&newVector property:objSite.property];
                [self createUIArrayContents:group arrayValue:newVector propertyType:prop.propertyType collapsed:collapsedV];
            }
            [self relayout:group forced:NO];
        }
        else if([value isEqualToString: @"Append Item"]){
            PropertyDescriptor* prop = [otd.properties objectAtIndex: [otd.properties indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop) {
                return [[obj name] isEqualToString: objSite.property];
            }]];
            ConstructibleType* ct = (ConstructibleType*)[[prop propertyType] elementType];
            if([ct isKindOfClass:[ConstructibleType class]]){
                NSMutableArray* collapsedV = [self collapsedArray:group];
                id newObj = [ct newObject];
                
                NSArray* oldVector = nil;
                [objSite.object getVector:&oldVector property:objSite.property];
                NSMutableArray* newVector = [NSMutableArray arrayWithArray:oldVector];
                [newVector insertObject:newObj atIndex:[oldVector count]]; //at end
                [collapsedV insertObject:@(YES) atIndex:[oldVector count]];
                [objSite.object setVector:newVector property:objSite.property];
                
                [self clearContents:group];
                [objSite.object getVector:&newVector property:objSite.property];
                [self createUIArrayContents:group arrayValue:newVector propertyType:prop.propertyType collapsed:collapsedV];
            }
            [self relayout:group forced:NO];
        }
        else { //Reset Vector
            PropertyDescriptor* prop = [otd.properties objectAtIndex: [otd.properties indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop) {
                return [[obj name] isEqualToString: objSite.property];
            }]];
            
            [objSite.object setVector:@[] property:objSite.property];
            [self clearContents:group];
            NSMutableArray* newVector;
            [objSite.object getVector:&newVector property:objSite.property];
            [self createUIArrayContents:group arrayValue:newVector propertyType:prop.propertyType collapsed:nil];
            [self relayout:group forced:NO];
        }
    }
    else {
        if([value isEqualToString:@"Prepend Item"]){
            PropertyInspectorGroup* grpItem = group;
            //ObjectSite* itemSite = [fUIObjectMapping objectForKey:[UISite uiSiteWithGroup:grpItem index:uiSiteHeaderIndex]];
            
            group = [group superview];
            objSite = [fUIObjectMapping objectForKey:[UISite uiSiteWithGroup:group index:uiSiteHeaderIndex]];
            otd = [objSite.object typeDescriptor];
            
            PropertyDescriptor* prop = [otd.properties objectAtIndex: [otd.properties indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop) {
                return [[obj name] isEqualToString: objSite.property];
            }]];
            ConstructibleType* ct = (ConstructibleType*)[[prop propertyType] elementType];
            if([ct isKindOfClass:[ConstructibleType class]]){
                NSMutableArray* collapsedV = [self collapsedArray:group];
                
                NSArray* oldVector = nil;
                [objSite.object getVector:&oldVector property:objSite.property];
                NSMutableArray* newVector = [NSMutableArray arrayWithArray:oldVector];
                NSUInteger index = [[group subviews] indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop) {
                    return obj == grpItem;
                }] - 1;
                if(index > newVector.count)
                    return;
                id newObj = [ct newObject];
                [newVector insertObject:newObj atIndex:index]; //at start
                [collapsedV insertObject:@(YES) atIndex:index];
                [objSite.object setVector:newVector property:objSite.property];
                
                [self clearContents:group];
                [objSite.object getVector:&newVector property:objSite.property];
                [self createUIArrayContents:group arrayValue:newVector propertyType:prop.propertyType collapsed:collapsedV];
            }
            [self relayout:group forced:NO];
        }
        else if([value isEqualToString: @"Append Item"]){
            PropertyInspectorGroup* grpItem = group;
            //ObjectSite* itemSite = [fUIObjectMapping objectForKey:[UISite uiSiteWithGroup:grpItem index:uiSiteHeaderIndex]];
            
            group = [group superview];
            objSite = [fUIObjectMapping objectForKey:[UISite uiSiteWithGroup:group index:uiSiteHeaderIndex]];
            otd = [objSite.object typeDescriptor];
            
            PropertyDescriptor* prop = [otd.properties objectAtIndex: [otd.properties indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop) {
                return [[obj name] isEqualToString: objSite.property];
            }]];
            ConstructibleType* ct = (ConstructibleType*)[[prop propertyType] elementType];
            if([ct isKindOfClass:[ConstructibleType class]]){
                NSMutableArray* collapsedV = [self collapsedArray:group];
                
                NSArray* oldVector = nil;
                [objSite.object getVector:&oldVector property:objSite.property];
                NSMutableArray* newVector = [NSMutableArray arrayWithArray:oldVector];
                NSUInteger index = [[group subviews] indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop) {
                    return obj == grpItem;
                }] - 1;
                if(index > newVector.count)
                    return;
                id newObj = [ct newObject];
                [newVector insertObject:newObj atIndex:(index + 1)]; //at end
                [collapsedV insertObject:@(YES) atIndex:(index + 1)];
                [objSite.object setVector:newVector property:objSite.property];
                
                [self clearContents:group];
                [objSite.object getVector:&newVector property:objSite.property];
                [self createUIArrayContents:group arrayValue:newVector propertyType:prop.propertyType collapsed:collapsedV];
            }
            [self relayout:group forced:NO];
        }
        else if([value isEqualToString: @"Move Up"]){
            PropertyInspectorGroup* grpItem = group;
            //ObjectSite* itemSite = [fUIObjectMapping objectForKey:[UISite uiSiteWithGroup:grpItem index:uiSiteHeaderIndex]];
            
            group = [group superview];
            objSite = [fUIObjectMapping objectForKey:[UISite uiSiteWithGroup:group index:uiSiteHeaderIndex]];
            otd = [objSite.object typeDescriptor];
            
            PropertyDescriptor* prop = [otd.properties objectAtIndex: [otd.properties indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop) {
                return [[obj name] isEqualToString: objSite.property];
            }]];
            
            NSMutableArray* collapsedV = [self collapsedArray:group];
            
            NSArray* oldVector = nil;
            [objSite.object getVector:&oldVector property:objSite.property];
            NSMutableArray* newVector = [NSMutableArray arrayWithArray:oldVector];
            NSUInteger index = [[group subviews] indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop) {
                return obj == grpItem;
            }] - 1;
            if(index != 0 && index < newVector.count && index < oldVector.count){
                id oldItem = [oldVector objectAtIndex:index];
                [newVector removeObjectAtIndex:index];
                [newVector insertObject:oldItem atIndex:index - 1];
                oldItem = [collapsedV objectAtIndex:index];
                [collapsedV removeObjectAtIndex:index];
                [collapsedV insertObject:oldItem atIndex:index - 1];
            }
            
            [objSite.object setVector:newVector property:objSite.property];
            [self clearContents:group];
            [objSite.object getVector:&newVector property:objSite.property];
            [self createUIArrayContents:group arrayValue:newVector propertyType:prop.propertyType collapsed:collapsedV];
    
            [self relayout:group forced:NO];
        }
        else if([value isEqualToString: @"Move Down"]){
            PropertyInspectorGroup* grpItem = group;
            //ObjectSite* itemSite = [fUIObjectMapping objectForKey:[UISite uiSiteWithGroup:grpItem index:uiSiteHeaderIndex]];
            
            group = [group superview];
            objSite = [fUIObjectMapping objectForKey:[UISite uiSiteWithGroup:group index:uiSiteHeaderIndex]];
            otd = [objSite.object typeDescriptor];
            
            PropertyDescriptor* prop = [otd.properties objectAtIndex: [otd.properties indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop) {
                return [[obj name] isEqualToString: objSite.property];
            }]];
            NSMutableArray* collapsedV = [self collapsedArray:group];
            
            NSArray* oldVector = nil;
            [objSite.object getVector:&oldVector property:objSite.property];
            NSMutableArray* newVector = [NSMutableArray arrayWithArray:oldVector];
            NSUInteger index = [[group subviews] indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop) {
                return obj == grpItem;
            }] - 1;
            if(index != ([oldVector count] - 1) && index < newVector.count && index < oldVector.count){
                id oldItem = [oldVector objectAtIndex:index];
                [newVector removeObjectAtIndex:index];
                [newVector insertObject:oldItem atIndex:index + 1];
                oldItem = [collapsedV objectAtIndex:index];
                [collapsedV removeObjectAtIndex:index];
                [collapsedV insertObject:oldItem atIndex:index + 1];
            }
            
            [objSite.object setVector:newVector property:objSite.property];
            [self clearContents:group];
            [objSite.object getVector:&newVector property:objSite.property];
            [self createUIArrayContents:group arrayValue:newVector propertyType:prop.propertyType collapsed:collapsedV];
            
            [self relayout:group forced:NO];
        }
        else { //Delete
            PropertyInspectorGroup* grpItem = group;
            //ObjectSite* itemSite = [fUIObjectMapping objectForKey:[UISite uiSiteWithGroup:grpItem index:uiSiteHeaderIndex]];
            
            group = [group superview];
            objSite = [fUIObjectMapping objectForKey:[UISite uiSiteWithGroup:group index:uiSiteHeaderIndex]];
            otd = [objSite.object typeDescriptor];
            
            PropertyDescriptor* prop = [otd.properties objectAtIndex: [otd.properties indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop) {
                return [[obj name] isEqualToString: objSite.property];
            }]];
            
            NSMutableArray* collapsedV = [self collapsedArray:group];
            
            NSArray* oldVector = nil;
            [objSite.object getVector:&oldVector property:objSite.property];
            NSMutableArray* newVector = [NSMutableArray arrayWithArray:oldVector];
            NSUInteger index = [[group subviews] indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop) {
                return obj == grpItem;
            }] - 1;
            if(index >= newVector.count)
                return;
            [newVector removeObjectAtIndex:index];
            [collapsedV removeObjectAtIndex:index];
            
            [objSite.object setVector:newVector property:objSite.property];
            [self clearContents:group];
            [objSite.object getVector:&newVector property:objSite.property];
            [self createUIArrayContents:group arrayValue:newVector propertyType:prop.propertyType collapsed:collapsedV];
            
            [self relayout:group forced:NO];
        }
    }
}
- (void)relayout:(PropertyInspectorGroup*)grp forced:(BOOL)forced {
    if(![UIObjectSynchronizator avoidLayout]){
        [grp redraw];
        [grp forcedFullLayout:forced];
        [[grp superview] setNeedsDisplay:YES];
    }
}
- (void)relayout {
    [self relayout: fRootGroup forced:YES];
}
-(void)cellAutoCompletion:(id)group args:(ComboxBoxEventArgs*)args{
    [args.items removeAllObjects];
    NSNumber* index = args.index;
    
    ObjectSite* objSite = [fUIObjectMapping objectForKey:[UISite uiSiteWithGroup:group index:[index unsignedIntegerValue]]];
    NSArray* suggestions = [objSite.object getAutoCompleteSuggestions:objSite.property];
    [args.items addObjectsFromArray:suggestions];
}

-(void)cellClicked:(id)group index:(NSNumber*)index{
    ObjectSite* objSite = [fUIObjectMapping objectForKey:[UISite uiSiteWithGroup:group index:[index unsignedIntegerValue]]];
    PropertyInspectorGroup* grp = group;
    enum PropertyInspectorCellType type = [grp cellType:[index unsignedIntegerValue]];
    TypeDescriptor* otd = [objSite.object typeDescriptor];
    
    if(type == pictButton){
        PropertyDescriptor* prop = [otd.properties objectAtIndex: [otd.properties indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop) {
            return [[obj name] isEqualToString: objSite.property];
        }]];
        id objValue;
        [objSite.object getObject:&objValue property:objSite.property];
        if(objValue != nil) //the user clicked the button
        {
            PropertyInspector* fInspector = [[PropertyInspector alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
            fInspector.propertyName = objSite.property;
            fInspector.navigableInspector = self.inspector;
            [self.inspector pushPropertyInspector:fInspector withTitle:prop.displayName];
            fInspector.object = objSite.object;
        }
    }
}
-(void)cellValueChanged:(id)group index:(NSNumber*)index{
    ObjectSite* objSite = [fUIObjectMapping objectForKey:[UISite uiSiteWithGroup:group index:[index unsignedIntegerValue]]];
    PropertyInspectorGroup* grp = group;
    enum PropertyInspectorCellType type = [grp cellType:[index unsignedIntegerValue]];
    TypeDescriptor* otd = [objSite.object typeDescriptor];
    
    if(type == pictButton){
        PropertyDescriptor* prop = [otd.properties objectAtIndex: [otd.properties indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop) {
            return [[obj name] isEqualToString: objSite.property];
        }]];
        
        if(prop.propertyType.GTD == gtdObj || prop.propertyType.GTD == gtdUnion){
            PropertyInspectorCell* cell = [grp cell:[index unsignedIntegerValue]];
            BOOL isNull = ![cell nullButtonState];
            ConstructibleType* ct = (ConstructibleType*)[prop propertyType];
            if(isNull){
                [objSite.object setNil:objSite.property];
            }
            else {
                [objSite.object setObject:[ct newObject] property:objSite.property];
            }
            
            id objValue = nil;
            [objSite.object getObject:&objValue property:objSite.property];
            if(objValue){
                [cell setStringValue:[NSString stringWithFormat:@"Edit %@", prop.propertyType.displayName]];
            }
            else {
                [cell setStringValue:@"<NULL>"];
            }
        }
    }
    else if(type == pictAngle){
        NSNumber* value = [grp cellFloatValue:[index unsignedIntegerValue]];
        if(value == nil)
            [objSite.object setNil:objSite.property];
        else
            [objSite.object setFloat:[value floatValue] property:objSite.property];
    }
    else if(type == pictBool){
        NSNumber* value = [grp cellIntegerValue:[index unsignedIntegerValue]];
        if(value == nil)
            [objSite.object setNil:objSite.property];
        else
            [objSite.object setBool:[value boolValue] property:objSite.property];
    }
    else if(type == pictColor){
        [objSite.object setColor:[grp cellColorValue:[index unsignedIntegerValue]] property:objSite.property];
    }
    else if(type == pictEnum){
        if([objSite.property isEqualToString:TagProperty]){ //Tag
            NSString* uTag = [grp cellStringValue:[index unsignedIntegerValue]];
            for (NSUInteger i = 0; i < [group childCount]; i++) {
                id child = [group child:i];
                if(![child isKindOfClass:[PropertyInspectorGroup class]])
                    continue;
                if([[child headerText] isEqualToString:uTag])
                    [child setCollapsed:NO];
                else
                    [child setCollapsed:YES];
            }
            [objSite.object setUnionTag: uTag];
            //it seems that we don't need that...
            //[self relayout];
        }
        else {
            PropertyDescriptor* prop = [otd.properties objectAtIndex: [otd.properties indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop) {
                return [[obj name] isEqualToString: objSite.property];
            }]];
            NSString* value = [grp cellStringValue:[index unsignedIntegerValue]];
            NSNumber* nId = nil;
            if(value)
                nId = [[prop propertyType].enumValues objectForKey: value];
            
            if(!nId)
                [objSite.object setNil:objSite.property];
            else
                [objSite.object setInt:[nId integerValue] property:objSite.property];
        }
    }
    else if(type == pictFloat){
        NSNumber* value = [grp cellFloatValue:[index unsignedIntegerValue]];
        if(value == nil)
            [objSite.object setNil:objSite.property];
        else
            [objSite.object setFloat:[value floatValue] property:objSite.property];
    }
    else if(type == pictFloatPair){
        NSNumber* value = [grp cellFloatValue:[index unsignedIntegerValue]];
        NSNumber* value2 = [grp cellSecondFloatValue:[index unsignedIntegerValue]];
        
        if(value == nil || value2 == nil)
            [objSite.object setNil:objSite.property];
        else
            [objSite.object setFloatPair:[value floatValue] second:[value2 floatValue] property:objSite.property];
    }
    else if(type == pictImage){
        [objSite.object setImage:[grp cellImageValue:[index unsignedIntegerValue]] property:objSite.property];
    }
    else if(type == pictInt){
        NSNumber* value = [grp cellIntegerValue:[index unsignedIntegerValue]];
        if(value == nil)
            [objSite.object setNil:objSite.property];
        else
            [objSite.object setInt:[value integerValue] property:objSite.property];
    }
    else if(type == pictLabel){
        //do nothing
    }
    else if(type == pictRanged){
        NSNumber* value = [grp cellFloatValue:[index unsignedIntegerValue]];
        if(value == nil)
            [objSite.object setNil:objSite.property];
        else
            [objSite.object setFloat:[value floatValue] property:objSite.property];
    }
    else if(type == pictCombobox){
        [objSite.object setString:[grp cellStringValue:[index unsignedIntegerValue]] property:objSite.property];
    }
    else if(type == pictFlags){
        PropertyDescriptor* prop = [otd.properties objectAtIndex: [otd.properties indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop) {
            return [[obj name] isEqualToString: objSite.property];
        }]];
        if(prop.propertyType.GTD == gtdFlags){
            NSNumber* value = [grp cellIntegerValue:[index unsignedIntegerValue]];
            if(value == nil)
                [objSite.object setNil:objSite.property];
            else
                [objSite.object setInt:[value integerValue] property:objSite.property];
        }
        else { //action
            NSUInteger value = [[grp cellIntegerValue:[index unsignedIntegerValue]] unsignedIntegerValue];
            NSUInteger _fl = 1;
            NSUInteger _i = -1;
            for (NSUInteger i = 0; i < [grp cell:[index unsignedIntegerValue]].items.count; i++){
                if(_fl & value){
                    _i = i;
                    break;
                }
                _fl <<= 1;
            }
            assert(_i != -1);
            NSString* _actionName = [[grp cell:[index unsignedIntegerValue]].items objectAtIndex:_i];
            [objSite.object callAction:objSite.property actionName:_actionName];
            [grp setCell:[index unsignedIntegerValue] integerValue:@0];
        }
    }
    else { //pictText
        [objSite.object setString:[grp cellStringValue:[index unsignedIntegerValue]] property:objSite.property];
    }
}

-(void)clearContents:(PropertyInspectorGroup *)group{
    for (NSUInteger i = 0; i < [group childCount]; i++) {
        id child = [group child:i];
        if([child isKindOfClass:[PropertyInspectorCell class]]) //cell
        {
            [group setCell:i target:nil];
            [group setCell:i actionSelector:nil];
            [fUIObjectMapping removeObjectForKey:[UISite uiSiteWithGroup:group index:i]];
        }
        else { //group
            PropertyInspectorGroup* grp = child;
            [grp setHeaderTarget:nil];
            [grp setHeaderActionSelector:nil];
            [fUIObjectMapping removeObjectForKey:[UISite uiSiteWithGroup:grp index:uiSiteHeaderIndex]];
            [self clearContents:grp];
        }
    }
    
    //removes the observer from the top root object
    ObjectSite* objSite = [fUIObjectMapping objectForKey:[UISite uiSiteWithGroup:group index:uiSiteHeaderIndex]];
    if(!objSite.property && [objSite.object respondsToSelector:@selector(removeObserver:)])
        [objSite.object removeObserver:self];
    else
    {
        id nObj = nil;
        [objSite.object getObject:&nObj property:objSite.property];
        if([nObj respondsToSelector:@selector(removeObserver:)])
            [nObj removeObserver:self];
    }
    
    [group clearChildren];
}

-(void)clear {
    [self clearContents:fRootGroup];
    [fRoot removeObserver:self];
    [fUIObjectMapping removeAllObjects];
    [self relayout];
}

-(void)clearLayout:(NSView*)v{
    [v setNeedsLayout:NO];
    [v setNeedsUpdateConstraints:NO];
    
    for (NSView* subV in [v subviews]){
        [self clearLayout:subV];
    }
}

-(void)startWithObject:(id<ObservableObject>)object group:(PropertyInspectorGroup *)group forProperty:(NSString *)propertyName {
    assert(!fUIObjectMapping);
    assert(object);
    
    fUIObjectMapping = [NSMutableDictionary dictionary];
    fRoot = object;
    fRootGroup = group;
    
    [UIObjectSynchronizator setAvoidLayout:YES];
    PropertyInspectorGroup* grp = [[PropertyInspectorGroup alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    [fUIObjectMapping setObject:[ObjectSite objectSiteWithObject:object property:nil] forKey:[UISite uiSiteWithGroup:grp index:uiSiteHeaderIndex]];
    [grp setHeaderText:@"Object"];
    [grp setType:pigtGroup];
    
    [grp setHeaderTarget:self];
    [grp setHeaderActionSelector:@selector(headerValueChanged:)];
    
    [group insertSubgroup:grp];
    [self createUIFromObject:fRoot type:nil inside:grp forProperty:propertyName];
    [UIObjectSynchronizator setAvoidLayout:NO];
    [fRootGroup commitCollapsedState];
    //Maybe it's not needed...
    //[self relayout];
    [self clearLayout:fRootGroup];
}

- (void)createUIArrayContents:(PropertyInspectorGroup *)grp arrayValue:(NSArray *)arrayValue propertyType:(TypeDescriptor*)pType collapsed: (NSArray*)collapsed {
    int i = 0;
    for (id<ObservableObject> item in arrayValue) {
        PropertyInspectorGroup* grpItem = [[PropertyInspectorGroup alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
        
        [grpItem setHeaderTarget:self];
        [grpItem setHeaderActionSelector:@selector(headerValueChanged:)];
        
        NSString* _displayName = [pType elementType].displayName;
        [grpItem setHeaderText:[NSString stringWithFormat:@"%@ %d", _displayName, i]];
        [grpItem setHeaderItems: @[@"Prepend Item", @"Append Item", @"Move Up", @"Move Down", @"Delete"]];
        [grpItem setType:pigtCollapsible];
        [grp insertSubgroup:grpItem];
        [self createUIFromObject:item type:nil inside:grpItem forProperty:nil];
        if(collapsed && i < collapsed.count)
            [grpItem setCollapsed:[[collapsed objectAtIndex:i] boolValue]];
        else
            [grpItem setCollapsed:YES];
        [fUIObjectMapping setObject:[ObjectSite objectSiteWithObject:item property:nil] forKey:[UISite uiSiteWithGroup:grpItem index:uiSiteHeaderIndex]];
        i++;
    }
}

-(void)createUIFromObject:(id<ObservableObject>)object type:(TypeDescriptor*)descriptor inside: (PropertyInspectorGroup*)group forProperty:(NSString *)propertyName {
    if(object == nil)
        return;
    
    if (descriptor == nil)
        descriptor = [object typeDescriptor];
    
    if (descriptor == nil)
        return;
    
    NSNumber* fValue;
    NSNumber* fSecondValue;
    NSNumber* bValue;
    NSNumber* iValue;
    NSColor* colorValue;
    NSImage* imageValue;
    NSString* sValue;
    id objValue;
    
    [object addObserver:self];
    
    if(descriptor.GTD == gtdUnion && propertyName == nil){
        //unions can't be directly inside unions...
        NSUInteger index = [group insertCellWithType:pictEnum title:[descriptor unionSelectorDisplayName] continuous:YES];
        [fUIObjectMapping setObject:[ObjectSite objectSiteWithObject:object property: TagProperty] forKey:[UISite uiSiteWithGroup:group index:index]];
        [group setCell:index target:self];
        [group setCell:index actionSelector:@selector(cellValueChanged:index:)];
        [group setCell:index clickTarget:self];
        [group setCell:index clickSelector:@selector(cellClicked:index:)];
        
        [group setCell:index enabled:YES];
        [group setCell:index nullable:NO];
        [group setCell:index items:[[[descriptor unionMap] allKeys] sortedArrayUsingSelector:@selector(compare:)]];
        [object getUnionTag:&sValue];
        [group setCell:index stringValue: sValue];
        
        for (NSString* tKey in [[descriptor unionMap] allKeys]) {
            TypeDescriptor* tType = [[descriptor unionMap] objectForKey:tKey];
            PropertyInspectorGroup* inGroup = [[PropertyInspectorGroup alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
            [inGroup setHeaderText:tKey];
            [inGroup setType:pigtInline];
            NSString* unionTag;
            [object getUnionTag:&unionTag];
            [group insertSubgroup:inGroup];
            [self createUIFromObject:object type:tType inside:inGroup forProperty:nil];
            //must be after creation
            BOOL toBeCollapsed = ![unionTag isEqualToString:tKey];
            [inGroup setCollapsed: toBeCollapsed];
        }
    }
    else {
        for (PropertyDescriptor* p in [descriptor properties]) {
            if(propertyName != nil && ![propertyName isEqualToString:p.name])
                continue;
            TypeDescriptor* pType = p.propertyType;
            if(pType.GTD == gtdAngle){
                NSUInteger index = [group insertCellWithType:pictAngle title:[p displayName] continuous:[p isContinuous]];
                [fUIObjectMapping setObject:[ObjectSite objectSiteWithObject:object property:[p name]] forKey:[UISite uiSiteWithGroup:group index:index]];
                
                [group setCell:index target:self];
                [group setCell:index actionSelector:@selector(cellValueChanged:index:)];
                [group setCell:index clickTarget:self];
                [group setCell:index clickSelector:@selector(cellClicked:index:)];
                
                [group setCell:index enabled:[p isEditable]];
                [group setCell:index nullable:[pType isNullable]];
                [object getFloat:&fValue property:p.name];
                [group setCell:index floatValue: fValue];
                [group setCell:index precision:p.precision];
            }
            else if(pType.GTD == gtdBool){
                NSUInteger index = [group insertCellWithType:pictBool title:[p displayName] continuous:[p isContinuous]];
                [fUIObjectMapping setObject:[ObjectSite objectSiteWithObject:object property:[p name]] forKey:[UISite uiSiteWithGroup:group index:index]];
                
                [group setCell:index target:self];
                [group setCell:index actionSelector:@selector(cellValueChanged:index:)];
                [group setCell:index clickTarget:self];
                [group setCell:index clickSelector:@selector(cellClicked:index:)];
                
                [group setCell:index enabled:[p isEditable]];
                [group setCell:index nullable:[pType isNullable]];
                [object getBool:&bValue property:p.name];
                [group setCell:index integerValue:bValue];
                [group setCell:index precision:p.precision];
            }
            else if(pType.GTD == gtdColor){
                NSUInteger index = [group insertCellWithType:pictColor title:[p displayName] continuous:[p isContinuous]];
                [fUIObjectMapping setObject:[ObjectSite objectSiteWithObject:object property:[p name]] forKey:[UISite uiSiteWithGroup:group index:index]];
                
                [group setCell:index target:self];
                [group setCell:index actionSelector:@selector(cellValueChanged:index:)];
                [group setCell:index clickTarget:self];
                [group setCell:index clickSelector:@selector(cellClicked:index:)];
                
                [group setCell:index enabled:[p isEditable]];
                [group setCell:index nullable:[pType isNullable]];
                [object getColor:&colorValue property:p.name];
                [group setCell:index colorValue:colorValue];
                [group setCell:index precision:p.precision];
            }
            else if(pType.GTD == gtdEnum){
                NSUInteger index = [group insertCellWithType:pictEnum title:[p displayName] continuous:[p isContinuous]];
                [fUIObjectMapping setObject:[ObjectSite objectSiteWithObject:object property:[p name]] forKey:[UISite uiSiteWithGroup:group index:index]];
                NSArray* keys = [[pType.enumValues allKeys] sortedArrayUsingSelector:@selector(compare:)];
                
                [group setCell:index target:self];
                [group setCell:index actionSelector:@selector(cellValueChanged:index:)];
                [group setCell:index clickTarget:self];
                [group setCell:index clickSelector:@selector(cellClicked:index:)];
                
                [group setCell:index items: keys];
                [group setCell:index enabled:[p isEditable]];
                [group setCell:index nullable:[pType isNullable]];
                [object getInt:&iValue property:p.name];
                if(iValue)
                    [group setCell:index stringValue:[[pType.enumValues allKeysForObject:iValue] objectAtIndex:0]];
                else
                    [group setCell:index stringValue:nil];
                [group setCell:index precision:p.precision];
            }
            else if(pType.GTD == gtdFlags){
                NSUInteger index = [group insertCellWithType:pictFlags title:[p displayName] continuous:[p isContinuous]];
                [fUIObjectMapping setObject:[ObjectSite objectSiteWithObject:object property:[p name]] forKey:[UISite uiSiteWithGroup:group index:index]];
                NSArray* keys = pType.flagNames;
                
                [group setCell:index target:self];
                [group setCell:index actionSelector:@selector(cellValueChanged:index:)];
                [group setCell:index clickTarget:self];
                [group setCell:index clickSelector:@selector(cellClicked:index:)];
                
                [group setCell:index items: keys];
                [group setCell:index enabled:[p isEditable]];
                [group setCell:index nullable:[pType isNullable]];
                [object getInt:&iValue property:p.name];
                [group setCell:index integerValue:iValue];
                [group setCell:index precision:p.precision];
            }
            else if(pType.GTD == gtdAction){
                NSUInteger index = [group insertCellWithType:pictFlags title:[p displayName] continuous:[p isContinuous]];
                [fUIObjectMapping setObject:[ObjectSite objectSiteWithObject:object property:[p name]] forKey:[UISite uiSiteWithGroup:group index:index]];
                NSArray* keys = pType.actionNames;
                
                [group setCell:index target:self];
                [group setCell:index actionSelector:@selector(cellValueChanged:index:)];
                [group setCell:index clickTarget:self];
                [group setCell:index clickSelector:@selector(cellClicked:index:)];
                
                [group setCell:index items: keys];
                [group setCell:index enabled:[p isEditable]];
                [group setCell:index nullable:NO];
                [group setCell:index integerValue:@0];
                [group setCell:index precision:p.precision];
            }
            else if(pType.GTD == gtdFloat){
                NSUInteger index = [group insertCellWithType:pictFloat title:[p displayName] continuous:[p isContinuous]];
                [fUIObjectMapping setObject:[ObjectSite objectSiteWithObject:object property:[p name]] forKey:[UISite uiSiteWithGroup:group index:index]];
                
                [group setCell:index target:self];
                [group setCell:index actionSelector:@selector(cellValueChanged:index:)];
                [group setCell:index clickTarget:self];
                [group setCell:index clickSelector:@selector(cellClicked:index:)];
                
                [group setCell:index enabled:[p isEditable]];
                [group setCell:index nullable:[pType isNullable]];
                [object getFloat:&fValue property:p.name];
                [group setCell:index floatValue: fValue];
                [group setCell:index maximumNumericPrecision:p.precision];
                [group setCell:index minimumNumericPrecision:0];
            }
            else if(pType.GTD == gtdFloatPair){
                NSUInteger index = [group insertCellWithType:pictFloatPair title:[p displayName] continuous:[p isContinuous]];
                [fUIObjectMapping setObject:[ObjectSite objectSiteWithObject:object property:[p name]] forKey:[UISite uiSiteWithGroup:group index:index]];
                
                [group setCell:index target:self];
                [group setCell:index actionSelector:@selector(cellValueChanged:index:)];
                [group setCell:index clickTarget:self];
                [group setCell:index clickSelector:@selector(cellClicked:index:)];
                
                [group setCell:index enabled:[p isEditable]];
                [group setCell:index nullable:[pType isNullable]];
                [object getFloatPair:&fValue second:&fSecondValue property:p.name];
                [group setCell:index floatValue:fValue];
                [group setCell:index secondFloatValue:fSecondValue];
                [group setCell:index maximumNumericPrecision:p.precision];
                [group setCell:index minimumNumericPrecision:0];
            }
            else if (pType.GTD == gtdImage){
                NSUInteger index = [group insertCellWithType:pictImage title:[p displayName] continuous:[p isContinuous]];
                [fUIObjectMapping setObject:[ObjectSite objectSiteWithObject:object property:[p name]] forKey:[UISite uiSiteWithGroup:group index:index]];
                
                [group setCell:index target:self];
                [group setCell:index actionSelector:@selector(cellValueChanged:index:)];
                [group setCell:index clickTarget:self];
                [group setCell:index clickSelector:@selector(cellClicked:index:)];
                
                [group setCell:index enabled:[p isEditable]];
                [group setCell:index nullable:[pType isNullable]];
                [object getImage:&imageValue property:p.name];
                [group setCell:index imageValue: imageValue];
                [group setCell:index maximumNumericPrecision:p.precision];
            }
            else if (pType.GTD == gtdInt){
                NSUInteger index = [group insertCellWithType:pictInt title:[p displayName] continuous:[p isContinuous]];
                [fUIObjectMapping setObject:[ObjectSite objectSiteWithObject:object property:[p name]] forKey:[UISite uiSiteWithGroup:group index:index]];
                
                [group setCell:index target:self];
                [group setCell:index actionSelector:@selector(cellValueChanged:index:)];
                [group setCell:index clickTarget:self];
                [group setCell:index clickSelector:@selector(cellClicked:index:)];
                
                [group setCell:index enabled:[p isEditable]];
                [group setCell:index nullable:[pType isNullable]];
                [object getInt:&iValue property:p.name];
                [group setCell:index integerValue: iValue];
                [group setCell:index maximumNumericPrecision:0];
                [group setCell:index minimumNumericPrecision:0];
            }
            else if (pType.GTD == gtdObj || pType.GTD == gtdUnion){
                if(pType.isNullable && !propertyName){
                    NSUInteger index = [group insertCellWithType:pictButton title:[p displayName] continuous:[p isContinuous]];
                    [fUIObjectMapping setObject:[ObjectSite objectSiteWithObject:object property:[p name]] forKey:[UISite uiSiteWithGroup:group index:index tag:@(OBJECT_LINK)]];
                    
                    [group setCell:index target:self];
                    [group setCell:index actionSelector:@selector(cellValueChanged:index:)];
                    [group setCell:index clickTarget:self];
                    [group setCell:index clickSelector:@selector(cellClicked:index:)];
                    
                    [group setCell:index enabled:[p isEditable]];
                    [group setCell:index nullable:[pType isNullable]];
                    
                    [object getObject:&objValue property:p.name];
                    if(objValue)
                        [group setCell:index stringValue: [NSString stringWithFormat:@"Edit %@", p.propertyType.displayName]];
                    else
                        [group setCell:index stringValue: @"<NULL>"];
                }
                else {
                    PropertyInspectorGroup* grp = [[PropertyInspectorGroup alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
                    
                    [grp setHeaderTarget:self];
                    [grp setHeaderActionSelector:@selector(headerValueChanged:)];
                    
                    [grp setHeaderText:p.displayName];
                    [grp setType:(propertyName ? pigtInline : pigtCollapsible)];
                    [group insertSubgroup:grp];
                    
                    [object getObject:&objValue property:p.name];
                    [fUIObjectMapping setObject:[ObjectSite objectSiteWithObject:object property:[p name]] forKey:[UISite uiSiteWithGroup:grp index:uiSiteHeaderIndex tag:@(OBJECT_GROUP)]];
                    [self createUIFromObject:objValue type:nil inside:grp forProperty:nil];
                }
            }
            else if (pType.GTD == gtdRanged){
                NSUInteger index = [group insertCellWithType:pictRanged title:[p displayName] continuous:[p isContinuous]];
                [fUIObjectMapping setObject:[ObjectSite objectSiteWithObject:object property:[p name]] forKey:[UISite uiSiteWithGroup:group index:index]];
                
                [group setCell:index target:self];
                [group setCell:index actionSelector:@selector(cellValueChanged:index:)];
                [group setCell:index clickTarget:self];
                [group setCell:index clickSelector:@selector(cellClicked:index:)];
                
                [group setCell:index enabled:[p isEditable]];
                [group setCell:index nullable:[pType isNullable]];
                [object getFloat:&fValue property:p.name];
                [group setCell:index floatValue: fValue];
                [group setCell:index precision:p.precision];
            }
            else if (pType.GTD == gtdText){
                NSUInteger index = [group insertCellWithType:pictText title:[p displayName] continuous:[p isContinuous]];
                [fUIObjectMapping setObject:[ObjectSite objectSiteWithObject:object property:[p name]] forKey:[UISite uiSiteWithGroup:group index:index]];
                
                [group setCell:index target:self];
                [group setCell:index actionSelector:@selector(cellValueChanged:index:)];
                [group setCell:index clickTarget:self];
                [group setCell:index clickSelector:@selector(cellClicked:index:)];
                
                [group setCell:index enabled:[p isEditable]];
                [group setCell:index nullable:[pType isNullable]];
                [object getString:&sValue property:p.name];
                [group setCell:index stringValue: sValue];
                [group setCell:index precision:p.precision];
            }
            else if(pType.GTD == gtdStringWithAutoCompletion){
                NSUInteger index = [group insertCellWithType:pictCombobox title:[p displayName] continuous:[p isContinuous]];
                [fUIObjectMapping setObject:[ObjectSite objectSiteWithObject:object property:[p name]] forKey:[UISite uiSiteWithGroup:group index:index]];
                
                [group setCell:index target:self];
                [group setCell:index actionSelector:@selector(cellValueChanged:index:)];
                [group setCell:index clickTarget:self];
                [group setCell:index clickSelector:@selector(cellClicked:index:)];
                [group setCell:index comboBoxTarget:self];
                [group setCell:index comboBoxSelector:@selector(cellAutoCompletion:args:)];
                
                [group setCell:index enabled:[p isEditable]];
                [group setCell:index nullable:[pType isNullable]];
                [object getString:&sValue property:p.name];
                [group setCell:index stringValue: sValue];
                [group setCell:index precision:p.precision];
            }
            else { //gtdVector
                //vectors can't have nullable elements
                //vectors can't be nullable
                if(propertyName == nil){
                    NSUInteger index = [group insertCellWithType:pictButton title:[p displayName] continuous:[p isContinuous]];
                    [fUIObjectMapping setObject:[ObjectSite objectSiteWithObject:object property:[p name]] forKey:[UISite uiSiteWithGroup:group index:index tag:@(ARRAY_LINK)]];
                    
                    [group setCell:index target:self];
                    [group setCell:index actionSelector:@selector(cellValueChanged:index:)];
                    [group setCell:index clickTarget:self];
                    [group setCell:index clickSelector:@selector(cellClicked:index:)];
                    
                    [group setCell:index enabled:[p isEditable]];
                    [group setCell:index nullable:[pType isNullable]];
                    [group setCell:index stringValue: @"Edit Vector"];
                }
                else {
                    PropertyInspectorGroup* grp = [[PropertyInspectorGroup alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
                    
                    [grp setHeaderTarget:self];
                    [grp setHeaderActionSelector:@selector(headerValueChanged:)];
                    
                    [grp setHeaderText:p.displayName];
                    [grp setHeaderItems:@[@"Prepend Item", @"Append Item", @"Reset Vector"]];
                    [grp setType:pigtCollapsible];
                    [group insertSubgroup:grp];
                    
                    NSArray* arrayValue;
                    [object getVector:&arrayValue property:p.name];
                    [fUIObjectMapping setObject:[ObjectSite objectSiteWithObject:object property:[p name]] forKey:[UISite uiSiteWithGroup:grp index:uiSiteHeaderIndex tag:@(ARRAY_GROUP)]];
                    [self createUIArrayContents:grp arrayValue:arrayValue propertyType:pType collapsed:nil];
                }
            }
        }
    }
}

@end
