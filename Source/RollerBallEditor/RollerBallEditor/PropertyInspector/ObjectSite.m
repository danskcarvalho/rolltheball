//
//  ObjectSite.m
//  CocoaTest
//
//  Created by Danilo Carvalho on 14/02/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import "ObjectSite.h"

@implementation ObjectSite
-(BOOL)isEqual:(id)object{
    if (object == self)
        return YES;
    if (!object || ![object isKindOfClass:[self class]])
        return NO;
    ObjectSite* other = object;
    return self.object == other.object && [self.property isEqualToString: other.property];
}
-(BOOL)isEqualTo:(id)object{
    return [self isEqual:object];
}
-(NSUInteger)hash{
    unsigned long long _hash = 1;
    unsigned long long _prime = 31;
    if(self.object)
        _hash = _prime * _hash + (unsigned long)(__bridge void*)self.object;
    if(self.property)
        _hash = _prime * _hash + [self.property hash];
    return _hash;
}
-(id)copyWithZone:(NSZone *)zone{
    return self;
}
-(id)initWithObject:(id<ObservableObject>)obj property:(NSString *)p{
    if(self = [self init]){
        self.object = obj;
        self.property = p;
    }
    return self;
}
+(ObjectSite *)objectSiteWithObject:(id<ObservableObject>)obj property:(NSString *)p{
    return [[ObjectSite alloc] initWithObject:obj property:p];
}
@end
