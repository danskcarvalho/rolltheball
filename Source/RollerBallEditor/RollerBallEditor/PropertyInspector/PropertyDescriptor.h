//
//  PropertyDescriptor.h
//  CocoaTest
//
//  Created by Danilo Carvalho on 29/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>
@class TypeDescriptor;

@interface PropertyDescriptor : NSObject < NSCopying >
@property(strong, readonly) NSString* name;
@property(strong, readonly) TypeDescriptor* propertyType;
@property(weak, readonly) TypeDescriptor* declaringType;
@property(strong, readonly) NSString* displayName;
@property(assign, readonly) BOOL isEditable;
@property(assign, readonly) BOOL isContinuous;
@property(assign, readonly) NSUInteger precision;
@property(assign, readonly) BOOL isInlined;
+(PropertyDescriptor*)propertyDescriptorWithName:(NSString*)name displayName:(NSString*)displayName type: (TypeDescriptor*)type editable:(BOOL)editable continous:(BOOL)continuous inlined:(BOOL)inlined precision:(NSUInteger)precision;
-(id)init;
-(BOOL)isEqual:(id)other;
-(BOOL)isEqualToPropertyDescriptor:(PropertyDescriptor *)aTypeDescriptor;
-(NSUInteger)hash;
-(id)copyWithZone:(NSZone *)zone;
@end
