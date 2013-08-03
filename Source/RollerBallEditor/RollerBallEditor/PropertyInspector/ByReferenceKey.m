//
//  ByReferenceKey.m
//  CocoaTest
//
//  Created by Danilo Carvalho on 14/02/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import "ByReferenceKey.h"

@implementation ByReferenceKey
-(BOOL)isEqual:(id)object{
    if (object == self)
        return YES;
    if (!object || ![object isKindOfClass:[self class]])
        return NO;
    ByReferenceKey* other = object;
    return other.object == self.object;
}
-(BOOL)isEqualTo:(id)object{
    return [self isEqual:object];
}
-(NSUInteger)hash{
    NSUInteger _hash = (NSUInteger)(__bridge void*)self.object;
    return _hash;
}
-(id)initWithObject:(id)object{
    if(self = [self init]){
        self.object = object;
    }
    return self;
}
+(ByReferenceKey *)refKeyWithObject:(id)object{
    return [[ByReferenceKey alloc] initWithObject:object];
}
-(id)copyWithZone:(NSZone *)zone{
    return self;
}
@end
