//
//  TypeDescriptor.m
//  CocoaTest
//
//  Created by Danilo Carvalho on 29/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "TypeDescriptor.h"
#import "PropertyDescriptor.h"

@implementation TypeDescriptor

static TypeDescriptor* textType;
static TypeDescriptor* intType;
static TypeDescriptor* floatType;
static TypeDescriptor* boolType;
static TypeDescriptor* rangedType;
static TypeDescriptor* angleType;
static TypeDescriptor* colorType;
static TypeDescriptor* imageType;
static TypeDescriptor* floatPairType;
static TypeDescriptor* stringWithAutoCompletionType;

static TypeDescriptor* nullableTextType;
static TypeDescriptor* nullableIntType;
static TypeDescriptor* nullableFloatType;
static TypeDescriptor* nullableBoolType;
static TypeDescriptor* nullableRangedType;
static TypeDescriptor* nullableAngleType;
static TypeDescriptor* nullableColorType;
static TypeDescriptor* nullableImageType;
static TypeDescriptor* nullableFloatPairType;
static TypeDescriptor* nullableStringWithAutoCompletionType;

- (id)init
{
    self = [super init];
    if (self) {
        // Initialization code here.
        self.GTD = gtdText;
        self.unionSelectorDisplayName = nil;
        self.unionMap = nil;
        self.enumValues = nil;
        self.name = nil;
        self.displayName = nil;
        self.properties = nil;
        self.elementType = nil;
        self.isNullable = NO;
        self.actionNames = nil;
        self.flagNames = nil;
        self.actionFlags = afMultiDispatch;
    }
    
    return self;
}

+(TypeDescriptor*)createPrimitiveWithGTD:(enum GeneralTypeDescriptor)gtd {
    TypeDescriptor* descriptor = [[TypeDescriptor alloc] init];
    [descriptor setGTD:gtd];
    if(gtd == gtdAngle)
        [descriptor setName:@"basicTypeAngle"];
    else if(gtd == gtdBool)
        [descriptor setName:@"basicTypeBool"];
    else if(gtd == gtdColor)
        [descriptor setName:@"basicTypeColor"];
    else if(gtd == gtdFloat)
        [descriptor setName:@"basicTypeFloat"];
    else if(gtd == gtdImage)
        [descriptor setName:@"basicTypeImage"];
    else if(gtd == gtdInt)
        [descriptor setName:@"basicTypeInt"];
    else if(gtd == gtdRanged)
        [descriptor setName:@"basicTypeRanged"];
    else if(gtd == gtdText)
        [descriptor setName:@"basicTypeText"];
    else if(gtd == gtdFloatPair)
        [descriptor setName:@"basicTypeFloatPair"];
    else if(gtd == gtdStringWithAutoCompletion)
        [descriptor setName:@"basicStringWithAutoCompletion"];
    else
        [NSException raise:NSInvalidArgumentException format:@"No primitive type...", nil];
    
    [descriptor setProperties:[NSArray array]];
    return descriptor;
}

+(void)initialize{
    if([self class] == [TypeDescriptor class]){
        textType = [self createPrimitiveWithGTD:gtdText];
        intType = [self createPrimitiveWithGTD:gtdInt];
        floatType = [self createPrimitiveWithGTD:gtdFloat];
        boolType = [self createPrimitiveWithGTD:gtdBool];
        rangedType = [self createPrimitiveWithGTD:gtdRanged];
        angleType = [self createPrimitiveWithGTD:gtdAngle];
        colorType = [self createPrimitiveWithGTD:gtdColor];
        imageType = [self createPrimitiveWithGTD:gtdImage];
        floatPairType = [self createPrimitiveWithGTD:gtdFloatPair];
        stringWithAutoCompletionType = [self createPrimitiveWithGTD:gtdStringWithAutoCompletion];
        
        nullableTextType = [self nullableTypeFromTypeDescriptor:[NSString stringWithFormat:@"%@%@", @"%@nullable_", [textType name]] type:textType];
        nullableIntType = [self nullableTypeFromTypeDescriptor:[NSString stringWithFormat:@"%@%@", @"%@nullable_", [intType name]] type:intType];
        nullableFloatType = [self nullableTypeFromTypeDescriptor:[NSString stringWithFormat:@"%@%@", @"%@nullable_", [floatType name]] type:floatType];
        nullableBoolType = [self nullableTypeFromTypeDescriptor:[NSString stringWithFormat:@"%@%@", @"%@nullable_", [boolType name]] type:boolType];
        nullableRangedType = [self nullableTypeFromTypeDescriptor:[NSString stringWithFormat:@"%@%@", @"%@nullable_", [rangedType name]] type:rangedType];
        nullableAngleType = [self nullableTypeFromTypeDescriptor:[NSString stringWithFormat:@"%@%@", @"%@nullable_", [angleType name]] type:angleType];
        nullableColorType = [self nullableTypeFromTypeDescriptor:[NSString stringWithFormat:@"%@%@", @"%@nullable_", [colorType name]] type:colorType];
        nullableImageType = [self nullableTypeFromTypeDescriptor:[NSString stringWithFormat:@"%@%@", @"%@nullable_", [imageType name]] type:imageType];
        nullableFloatPairType = [self nullableTypeFromTypeDescriptor:[NSString stringWithFormat:@"%@%@", @"%@nullable_", [floatPairType name]] type:floatPairType];
        nullableStringWithAutoCompletionType = [self nullableTypeFromTypeDescriptor:[NSString stringWithFormat:@"%@%@", @"%@nullable_", [stringWithAutoCompletionType name]] type:stringWithAutoCompletionType];
    }
}

+(TypeDescriptor *)textType{
    return textType;
}
+(TypeDescriptor *)intType{
    return intType;
}
+(TypeDescriptor *)floatType{
    return floatType;
}
+(TypeDescriptor *)boolType{
    return boolType;
}
+(TypeDescriptor *)rangedType{
    return rangedType;
}
+(TypeDescriptor *)angleType{
    return angleType;
}
+(TypeDescriptor *)colorType{
    return colorType;
}
+(TypeDescriptor *)imageType{
    return imageType;
}
+(TypeDescriptor *)floatPairType{
    return floatPairType;
}
+(TypeDescriptor *)stringWithAutoCompletionType{
    return stringWithAutoCompletionType;
}

+(TypeDescriptor *)nullableTextType{
    return nullableTextType;
}
+(TypeDescriptor *)nullableIntType{
    return nullableIntType;
}
+(TypeDescriptor *)nullableFloatType{
    return nullableFloatType;
}
+(TypeDescriptor *)nullableBoolType{
    return nullableBoolType;
}
+(TypeDescriptor *)nullableRangedType{
    return nullableRangedType;
}
+(TypeDescriptor *)nullableAngleType{
    return nullableAngleType;
}
+(TypeDescriptor *)nullableColorType{
    return nullableColorType;
}
+(TypeDescriptor *)nullableImageType{
    return nullableImageType;
}
+(TypeDescriptor *)nullableFloatPairType{
    return nullableFloatPairType;
}
+(TypeDescriptor *)nullableStringWithAutoCompletionType{
    return nullableStringWithAutoCompletionType;
}

+(TypeDescriptor *)actionTypeDescriptorFromName:(NSString *)name displayName:(NSString *)displayName actionNames:(NSArray *)actionNames{
    TypeDescriptor* td = [[TypeDescriptor alloc] init];
    td.name = name;
    td.actionNames = [NSArray arrayWithArray:actionNames];
    td.displayName = displayName;
    td.GTD = gtdAction;
    return td;
}

+(TypeDescriptor *)flagTypeDescriptorFromName:(NSString *)name displayName:(NSString *)displayName flagNames:(NSArray *)flagNames{
    TypeDescriptor* td = [[TypeDescriptor alloc] init];
    td.name = name;
    td.flagNames = [NSArray arrayWithArray:flagNames];
    td.displayName = displayName;
    td.GTD = gtdFlags;
    return td;
}

+(TypeDescriptor*)enumTypeDescriptorFromName:(NSString*)name displayName:(NSString*)displayName enumValues: (NSDictionary*)enumValues{
    //we don't validate parameters
    //the parameters will be validated in C++ code
    TypeDescriptor* td = [[TypeDescriptor alloc] init];
    td.name = name;
    td.enumValues = [NSDictionary dictionaryWithDictionary:enumValues];
    td.displayName = displayName;
    td.GTD = gtdEnum;
    return td;
}
+(TypeDescriptor*)unionTypeDescriptorFromName: (NSString*)name displayName:(NSString*)displayName selectorDisplayName: (NSString*)selectorName unionMap: (NSDictionary*)unionMap{
    //we don't validate parameters
    //the parameters will be validated in C++ code
    TypeDescriptor* td = [[TypeDescriptor alloc] init];
    td.name = name;
    td.unionSelectorDisplayName = selectorName;
    td.unionMap = [NSDictionary dictionaryWithDictionary:unionMap];
    td.displayName = displayName;
    td.GTD = gtdUnion;
    NSMutableArray* unionizedProperties = [NSMutableArray array];
    for (TypeDescriptor* ctd in [unionMap allValues]) {
        [unionizedProperties addObjectsFromArray:ctd.properties];
    }
    td.properties = unionizedProperties;
    return td;
}
+(TypeDescriptor*)objectTypeDescriptorFromName: (NSString*)name displayName:(NSString*)displayName properties: (NSArray*) properties{
    //we don't validate parameters
    //the parameters will be validated in C++ code
    TypeDescriptor* td = [[TypeDescriptor alloc] init];
    td.name = name;
    td.properties = [NSArray arrayWithArray:properties];
    td.displayName = displayName;
    td.GTD = gtdObj;
    for (PropertyDescriptor* descriptor in td.properties){
        if([descriptor respondsToSelector:@selector(setDeclaringType:)]){
            [descriptor performSelector:@selector(setDeclaringType:) withObject:self];
        }
    }
    return td;
}
+(TypeDescriptor*)vectorTypeDescriptorFromName:(NSString*)name displayName:(NSString*)displayName elementType: (TypeDescriptor*)elementType{
    TypeDescriptor* td = [[TypeDescriptor alloc] init];
    td.name = name;
    td.GTD = gtdVector;
    td.elementType = elementType;
    td.properties = [NSArray array];
    td.displayName = displayName;
    return td;
}
+(TypeDescriptor *)nullableTypeFromTypeDescriptor:(NSString*)name type:(TypeDescriptor *)type{
    TypeDescriptor* td = [[TypeDescriptor alloc] init];
    td.name = name;
    td.GTD = type.GTD;
    td.unionSelectorDisplayName = type.unionSelectorDisplayName;
    td.unionMap = type.unionMap;
    td.enumValues = type.enumValues;
    td.properties = type.properties;
    td.elementType = type.elementType;
    td.isNullable = YES;
    td.displayName = type.displayName;
    td.actionNames = type.actionNames;
    td.flagNames = type.flagNames;
    td.actionFlags = type.actionFlags;
    return td;
}

//equality
- (BOOL)isEqual:(id)other {
    if (other == self)
        return YES;
    if (!other || ![other isKindOfClass:[self class]])
        return NO;
    return [self isEqualToTypeDescriptor:other];
}

- (BOOL)isEqualToTypeDescriptor:(TypeDescriptor *)aTypeDescriptor {
    if (self == aTypeDescriptor)
        return YES;
    if (self.name == aTypeDescriptor.name)
        return YES;
    else
        return NO;
}
-(NSUInteger)hash{
    if(!self.name)
        return 0;
    else
        return [self.name hash];
}
-(id)copyWithZone:(NSZone *)zone{
    return self;
}
@end








