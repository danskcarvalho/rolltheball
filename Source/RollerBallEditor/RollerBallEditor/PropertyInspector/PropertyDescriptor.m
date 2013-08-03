//
//  PropertyDescriptor.m
//  CocoaTest
//
//  Created by Danilo Carvalho on 29/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "PropertyDescriptor.h"
#import "TypeDescriptor.h"

@interface PropertyDescriptor ()
@property(strong, readwrite) NSString* name;
@property(strong, readwrite) TypeDescriptor* propertyType;
@property(weak, readwrite) TypeDescriptor* declaringType;
@property(strong, readwrite) NSString* displayName;
@property(assign, readwrite) BOOL isEditable;
@property(assign, readwrite) BOOL isContinuous;
@property(assign, readwrite) NSUInteger precision;
@property(assign, readwrite) BOOL isInlined;
@end

@implementation PropertyDescriptor
-(id)init{
    if(self = [super init]){
        self.name = nil;
        self.propertyType = nil;
        self.declaringType = nil;
        self.displayName = nil;
        self.isEditable = NO;
        self.isContinuous = NO;
        self.precision = 0;
        self.isInlined = YES;
    }
    return self;
}
+(PropertyDescriptor *)propertyDescriptorWithName:(NSString *)name displayName:(NSString *)displayName type:(TypeDescriptor *)type editable:(BOOL)editable continous:(BOOL)continuous inlined:(BOOL)inlined precision:(NSUInteger)precision{
    PropertyDescriptor* desc = [[PropertyDescriptor alloc] init];
    desc.name = name;
    desc.displayName = displayName ? displayName : name;
    desc.propertyType = type;
    desc.isEditable = editable;
    desc.isContinuous = continuous;
    desc.precision = precision;
    desc.isInlined = inlined;
    return desc;
}
-(BOOL)isEqual:(id)other{
    if (other == self)
        return YES;
    if (!other || ![other isKindOfClass:[self class]])
        return NO;
    return [self isEqualToPropertyDescriptor:other];
}
-(BOOL)isEqualToPropertyDescriptor:(PropertyDescriptor *)aTypeDescriptor{
    if (self == aTypeDescriptor)
        return YES;
    return [[self name] isEqualToString:[aTypeDescriptor name]] && [[self declaringType] isEqual:[aTypeDescriptor declaringType]];
}
-(NSUInteger)hash{
    unsigned long long _hash = 1;
    unsigned long long _prime = 31;
    if(self.name)
        _hash = _prime * _hash + [self.name hash];
    if(self.declaringType)
        _hash = _prime * _hash + [self.declaringType hash];
    return _hash;
}
-(id)copyWithZone:(NSZone *)zone{
    return self;
}
@end
