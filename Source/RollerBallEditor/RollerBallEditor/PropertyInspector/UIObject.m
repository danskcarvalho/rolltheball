//
//  UIObject.m
//  CocoaTest
//
//  Created by Danilo Carvalho on 08/02/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import "UIObject.h"

@interface UIObject ()
{
    NSString* uTag;
    TypeDescriptor* td;
    NSMutableDictionary* properties;
    NSMutableDictionary* secondPart;
    NSMutableArray* observers;
}
@end

@implementation UIObject
-(void)callAction:(NSString *)property actionName:(NSString *)actionName{
    
}
-(NSArray *)getAutoCompleteSuggestions:(NSString *)property{
    return nil;
}
-(id)initFromTypeDescriptor:(TypeDescriptor *)typeDescriptor {
    if (self = [self init]) {
        td = typeDescriptor;
        uTag = nil;
        properties = [NSMutableDictionary dictionary];
        secondPart = [NSMutableDictionary dictionary];
        observers = [NSMutableArray array];
    }
    return self;
}
-(TypeDescriptor *)typeDescriptor{
    return td;
}

-(void)setString:(NSString *)value property:(NSString *)property{
    if(!value)
        value = (NSString*)[NSNull null];
    [properties setObject:value forKey:property];
}
-(void)setInt:(NSInteger)value property:(NSString *)property{
    [properties setObject:@(value) forKey:property];
}
-(void)setFloat:(float)value property:(NSString *)property{
    [properties setObject:@(value) forKey:property];
}
-(void)setFloatPair:(float)value1 second:(float)value2 property:(NSString *)property{
    [properties setObject:@(value1) forKey:property];
    [secondPart setObject:@(value2) forKey:property];
}
-(void)setBool:(BOOL)value property:(NSString *)property{
    [properties setObject:@(value) forKey:property];
}
-(void)setColor:(NSColor *)value property:(NSString *)property{
    if(!value)
        value = (NSColor*)[NSNull null];
    [properties setObject:value forKey:property];
}
-(void)setImage:(NSImage *)value property:(NSString *)property{
    if(!value)
        value = (NSImage*)[NSNull null];
    [properties setObject:value forKey:property];
}
-(void)setObject:(id)value property:(NSString *)property{
    if(!value)
        value = [NSNull null];
    [properties setObject:value forKey:property];
}
-(void)setVector:(NSArray *)value property:(NSString *)property{
    if(!value)
        value = (NSArray*)[NSNull null];
    [properties setObject:value forKey:property];
}
-(void)setNil:(NSString *)property{
    [properties setObject:[NSNull null] forKey:property];
    [secondPart setObject:[NSNull null] forKey:property];
}
-(void)setUnionTag:(NSString *)tag{
    uTag = tag;
}

-(void)notifyObservers:(NSString*)property{
    if(property){
        for (id<Observer> ob in observers) {
            [ob propertyChanged:self property:property];
        }
    }
    else{
        for (id<Observer> ob in observers) {
            [ob unionTagChanged:self];
        }
    }
}
-(void)setStringInternally:(NSString *)value property:(NSString *)property{
    [self setString:value property:property];
    [self notifyObservers:property];
}
-(void)setIntInternally:(NSInteger)value property:(NSString *)property{
    [self setInt:value property:property];
    [self notifyObservers:property];
}
-(void)setFloatInternally:(float)value property:(NSString *)property{
    [self setFloat:value property:property];
    [self notifyObservers:property];
}
-(void)setFloatPairInternally:(float)value1 second:(float)value2 property:(NSString *)property{
    [self setFloatPair:value1 second:value2 property:property];
    [self notifyObservers:property];
}
-(void)setBoolInternally:(BOOL)value property:(NSString *)property{
    [self setBool:value property:property];
    [self notifyObservers:property];
}
-(void)setColorInternally:(NSColor *)value property:(NSString *)property{
    [self setColor:value property:property];
    [self notifyObservers:property];
}
-(void)setImageInternally:(NSImage *)value property:(NSString *)property{
    [self setImage:value property:property];
    [self notifyObservers:property];
}
-(void)setObjectInternally:(id)value property:(NSString *)property{
    [self setObject:value property:property];
    [self notifyObservers:property];
}
-(void)setVectorInternally:(NSArray *)value property:(NSString *)property{
    [self setVector:value property:property];
    [self notifyObservers:property];
}
-(void)setNilInternally:(NSString *)property{
    [self setNil:property];
    [self notifyObservers:property];
}
-(void)setUnionTagInternally:(NSString *)tag{
    [self setUnionTag:tag];
    [self notifyObservers:nil];
}

-(void)getString:(NSString *__autoreleasing *)value property:(NSString *)property{
    *value = [properties objectForKey:property];
    if([(*value) isKindOfClass:[NSNull class]])
        *value = nil;
}
-(void)getInt:(NSNumber *__autoreleasing *)value property:(NSString *)property{
    *value = [properties objectForKey:property];
    if([(*value) isKindOfClass:[NSNull class]])
        *value = nil;
}
-(void)getFloat:(NSNumber *__autoreleasing *)value property:(NSString *)property{
    *value = [properties objectForKey:property];
    if([(*value) isKindOfClass:[NSNull class]])
        *value = nil;
}
-(void)getFloatPair:(NSNumber *__autoreleasing *)value1 second:(NSNumber *__autoreleasing *)value2 property:(NSString *)property{
    *value1 = [properties objectForKey:property];
    *value2 = [secondPart objectForKey:property];
    if([(*value1) isKindOfClass:[NSNull class]])
        *value1 = nil;
    if([(*value2) isKindOfClass:[NSNull class]])
        *value2 = nil;
}
-(void)getBool:(NSNumber *__autoreleasing *)value property:(NSString *)property{
    *value = [properties objectForKey:property];
    if([(*value) isKindOfClass:[NSNull class]])
        *value = nil;
}
-(void)getColor:(NSColor *__autoreleasing *)value property:(NSString *)property{
    *value = [properties objectForKey:property];
    if([(*value) isKindOfClass:[NSNull class]])
        *value = nil;
}
-(void)getImage:(NSImage *__autoreleasing *)value property:(NSString *)property {
    *value = [properties objectForKey:property];
    if([(*value) isKindOfClass:[NSNull class]])
        *value = nil;
}
-(void)getObject:(__autoreleasing id *)value property:(NSString *)property {
    *value = [properties objectForKey:property];
    if([(*value) isKindOfClass:[NSNull class]])
        *value = nil;
}
-(void)getVector:(NSArray *__autoreleasing *)value property:(NSString *)property{
    *value = [properties objectForKey:property];
    if([(*value) isKindOfClass:[NSNull class]])
        *value = nil;
}
-(void)getUnionTag:(NSString *__autoreleasing *)tag{
    *tag = uTag;
}

-(void)addObserver:(id<Observer>)observer{
    [self removeObserver:observer];
    [observers addObject:observer];
}
-(void)removeObserver:(id)observer{
    NSUInteger index = [observers indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop) {
        return obj == observer;
    }];
    if(index == NSNotFound)
        return;
    [observers removeObjectAtIndex:index];
}
@end
