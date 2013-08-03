//
//  TypeDescriptor.h
//  CocoaTest
//
//  Created by Danilo Carvalho on 29/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import <Cocoa/Cocoa.h>

enum GeneralTypeDescriptor {
    gtdText,
    gtdInt,
    gtdFloat,
    gtdFloatPair,
    gtdBool,
    gtdRanged,
    gtdAngle,
    gtdColor,
    gtdImage,
    gtdEnum,
    gtdObj,
    gtdUnion,
    gtdVector,
    gtdStringWithAutoCompletion,
    gtdFlags,
    gtdAction
};

enum ActionFlags {
    afSingleDispatch = 0,
    afMultiDispatch = 1,
    afDispatchMask = 1,
    afCantUndo = 1 << 2
};

@interface TypeDescriptor : NSObject < NSCopying >
@property enum GeneralTypeDescriptor GTD;
@property(strong) NSString* unionSelectorDisplayName;
//Map between NSString and Types
@property(strong) NSDictionary* unionMap;
//The keys are the textual values to be displayed...
//The values are NSNumber instances that is the value associated with that text...
@property(strong) NSDictionary* enumValues;
@property(strong) NSArray* actionNames;
@property(strong) NSArray* flagNames;
@property(assign) enum ActionFlags actionFlags;
//Name of the type
@property(strong) NSString* name;
//Display name of the types
@property(strong) NSString* displayName;
//List Of Properties
@property(strong) NSArray* properties;
//Element Type
@property(strong) TypeDescriptor* elementType;
//Is Nullable?
@property(assign) BOOL isNullable;

-(BOOL)isEqual:(id)object;
-(BOOL)isEqualToTypeDescriptor:(TypeDescriptor *)aTypeDescriptor;
-(NSUInteger)hash;
-(id)copyWithZone:(NSZone *)zone;

//Primitive Types
+(TypeDescriptor*)textType;
+(TypeDescriptor*)intType;
+(TypeDescriptor*)floatType;
+(TypeDescriptor*)boolType;
+(TypeDescriptor*)rangedType;
+(TypeDescriptor*)angleType;
+(TypeDescriptor*)colorType;
+(TypeDescriptor*)imageType;
+(TypeDescriptor*)floatPairType;
+(TypeDescriptor*)stringWithAutoCompletionType;

+(TypeDescriptor*)nullableTextType;
+(TypeDescriptor*)nullableIntType;
+(TypeDescriptor*)nullableFloatType;
+(TypeDescriptor*)nullableBoolType;
+(TypeDescriptor*)nullableRangedType;
+(TypeDescriptor*)nullableAngleType;
+(TypeDescriptor*)nullableColorType;
+(TypeDescriptor*)nullableImageType;
+(TypeDescriptor*)nullableFloatPairType;
+(TypeDescriptor*)nullableStringWithAutoCompletionType;

//Compound Types
+(TypeDescriptor*)actionTypeDescriptorFromName: (NSString*)name displayName:(NSString*)displayName actionNames: (NSArray*)actionNames;
+(TypeDescriptor*)flagTypeDescriptorFromName: (NSString*)name displayName:(NSString*)displayName flagNames: (NSArray*)flagNames;
+(TypeDescriptor*)enumTypeDescriptorFromName:(NSString*)name displayName:(NSString*)displayName enumValues: (NSDictionary*)enumValues;
+(TypeDescriptor*)unionTypeDescriptorFromName: (NSString*)name displayName:(NSString*)displayName selectorDisplayName: (NSString*)selectorName unionMap: (NSDictionary*)unionMap;
+(TypeDescriptor*)objectTypeDescriptorFromName: (NSString*)name displayName:(NSString*)displayName properties: (NSArray*) properties;
+(TypeDescriptor *)nullableTypeFromTypeDescriptor:(NSString*)name type:(TypeDescriptor *)type;
//Compound Types
+(TypeDescriptor*)vectorTypeDescriptorFromName:(NSString*)name displayName:(NSString*)displayName elementType: (TypeDescriptor*)elementType;
@end
