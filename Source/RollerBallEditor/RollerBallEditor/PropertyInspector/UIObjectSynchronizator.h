//
//  UIObjectSynchronizator.h
//  CocoaTest
//
//  Created by Danilo Carvalho on 28/01/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "TypeDescriptor.h"
#import "PropertyDescriptor.h"
#import "ObservableObject.h"
#import "Observer.h"
#import "ScrollablePropertyInspectorGroup.h"
#import "PropertyInspectorGroup.h"
#import "PropertyInspectorCell.h"

#define TagProperty @"##-TAG-PROPERTY"

@class NavigablePropertyInspector;
@interface UIObjectSynchronizator : NSObject < Observer >
+(BOOL)avoidLayout;
+(void)setAvoidLayout:(BOOL)value;
-(void)startWithObject:(id<ObservableObject>)object group:(PropertyInspectorGroup*)group forProperty:(NSString*)propertyName;
-(void)clear;
@property (nonatomic, weak) NavigablePropertyInspector* inspector;
@end
