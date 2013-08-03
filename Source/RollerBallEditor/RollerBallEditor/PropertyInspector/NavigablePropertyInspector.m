//
//  NavigablePropertyInspector.m
//  RollerBallEditor
//
//  Created by Danilo Santos de Carvalho on 03/06/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import "NavigablePropertyInspector.h"
#import "NavigationView.h"
#import "PropertyInspector.h"
#import "ByReferenceKey.h"
#import "CppBridge.h"
#import "PropertyInspectorGroup.h"

@interface NavigablePropertyInspector ()
{
    NavigationView* nView;
    PropertyInspector* baseInspector;
    NSMutableDictionary* objPropertyInspector;
}
@end

@implementation NavigablePropertyInspector

-(void)propertyChanged:(id<ObservableObject>)object property:(NSString *)property{
    
}

-(void)unionTagChanged:(id<ObservableObject>)object{
    
}

-(void)objectDeleted:(id<ObservableObject>)object{
    [self popObject:object];
}

-(void)backClicked:(id)toBeRemoved{
    NSArray* keys = [objPropertyInspector allKeysForObject:toBeRemoved];
    for (ByReferenceKey* k in keys){
        [k.object deleteInternalObject];
        [objPropertyInspector removeObjectForKey:k];
    }
}

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        nView = [[NavigationView alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
        baseInspector = [[PropertyInspector alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
        baseInspector.navigableInspector = self;
        [self addSubview:nView];
        [nView pushView:baseInspector withTitle:@"Doesnt matter!"];
        objPropertyInspector = [NSMutableDictionary dictionary];
        nView.target = self;
    }
    
    return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
    // Drawing code here.
}

-(void)updateConstraints{
    NSDictionary* bindings = NSDictionaryOfVariableBindings(nView);
    [self removeConstraints:self.constraints];
    [super updateConstraints];
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"H:|[nView]|" options:0 metrics:nil views:bindings]];
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"V:|[nView]|" options:0 metrics:nil views:bindings]];
}
-(void)pushPropertyInspector:(PropertyInspector *)inspector withTitle:(NSString*)title{
    [nView pushView:inspector withTitle:title];
}
-(void)popPropertyInspector:(PropertyInspector *)inspector{
    if ([nView peekView] == inspector){
        [nView popView];
    }
    else {
        [nView removeView:inspector];
    }
}

-(void)pushObject:(id)obj withTitle: (NSString*)title{
    if(obj != nil) //the user clicked the button
    {
        if([nView peekView]){
            PropertyInspector* pI = (PropertyInspector*)[nView peekView];
            [[pI group] commitValues];
        }
        PropertyInspector* fInspector = [[PropertyInspector alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
        fInspector.navigableInspector = self;
        [self pushPropertyInspector:fInspector withTitle:title];
        fInspector.object = obj;
        [obj addObserver:self];
        [objPropertyInspector setObject:fInspector forKey:[ByReferenceKey refKeyWithObject:obj]];
    }
}
-(void)popObject:(id)obj{
    PropertyInspector* fInspector = [objPropertyInspector objectForKey:[ByReferenceKey refKeyWithObject:obj]];
    if(fInspector) {
        [self popPropertyInspector:fInspector];
        [objPropertyInspector removeObjectForKey:[ByReferenceKey refKeyWithObject:obj]];
    }
    else {
        if (baseInspector.object == obj)
            baseInspector.object = nil;
    }
}

-(id)object{
    return baseInspector.object;
}

-(void)setObject:(id)object{
    if(baseInspector.object){ //TODO: Corrigir este erro no nil...
        [[baseInspector group] commitValues];
    }
    baseInspector.propertyName = nil;
    while (true){
        if([nView peekView] != baseInspector){
            PropertyInspector* pI = (PropertyInspector*)[nView peekView];
            [[pI group] commitValues];
            [nView removeView:[nView peekView]];
        }
        else
            break;
    }
    [baseInspector setHidden:NO];
    baseInspector.object = object;
    [object addObserver:self];
    NSArray* keys = [objPropertyInspector allKeys];
    for (NSUInteger i = 0; i < keys.count; i++) {
        ByReferenceKey* refKey = [keys objectAtIndex:i];
        CppBridge* obj = [refKey object];
        [obj deleteInternalObject];
    }
    [objPropertyInspector removeAllObjects];
}

@end
