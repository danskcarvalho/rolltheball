//
//  ConstructibleType.m
//  CocoaTest
//
//  Created by Danilo Carvalho on 08/02/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import "ConstructibleType.h"

@implementation ConstructibleType

-(id)newObject{
    return self.creatorBlock();
}
-(id)initFromTypeDescriptor:(TypeDescriptor *)typeDescriptor{
    if(self = [self init]){
        self.GTD = typeDescriptor.GTD;
        self.unionSelectorDisplayName = typeDescriptor.unionSelectorDisplayName;
        self.unionMap = typeDescriptor.unionMap;
        self.enumValues = typeDescriptor.enumValues;
        self.name = typeDescriptor.name;
        self.displayName = typeDescriptor.displayName;
        self.properties = typeDescriptor.properties;
        self.elementType = typeDescriptor.elementType;
        self.isNullable = typeDescriptor.isNullable;
    }
    return self;
}
+(ConstructibleType *)constructibleTypeFromTypeDescriptor:(TypeDescriptor *)typeDescriptor{
    return [[ConstructibleType alloc] initFromTypeDescriptor:typeDescriptor];
}
+(ConstructibleType *)nullableTypeFromTypeDescriptor:(NSString*)name type:(ConstructibleType *)type{
    ConstructibleType* td = [[ConstructibleType alloc] init];
    td.name = name;
    td.GTD = type.GTD;
    td.unionSelectorDisplayName = type.unionSelectorDisplayName;
    td.unionMap = type.unionMap;
    td.enumValues = type.enumValues;
    td.properties = type.properties;
    td.elementType = type.elementType;
    td.isNullable = YES;
    td.displayName = type.displayName;
    td.creatorBlock = type.creatorBlock;
    return td;
}
@end
