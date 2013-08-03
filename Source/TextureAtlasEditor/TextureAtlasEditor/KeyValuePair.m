//
//  KeyValuePair.m
//  TextureAtlasEditor
//
//  Created by Danilo Carvalho on 31/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "KeyValuePair.h"

@implementation KeyValuePair
@synthesize key, value;

-(void)encodeWithCoder:(NSCoder *)aCoder{
    [aCoder encodeObject:key forKey:@"key"];
    [aCoder encodeObject:value forKey:@"value"];
}

-(id)initWithCoder:(NSCoder *)aDecoder{
    if(self = [super init]){
        self.key = [aDecoder decodeObjectForKey:@"key"];
        self.value = [aDecoder decodeObjectForKey:@"value"];
    }
    return self;
}

+(id)keyValuePairWithKey:(id)aKey value:(id)aValue{
    return [[KeyValuePair alloc] initWithKey:aKey andValue:aValue];
}

- (id)initWithKey:(id)aKey andValue:(id)aValue {
    if (self = [super init]) {
        key     = aKey;
        value   = aValue;
    }
    return self;
}

- (id)copyWithZone:(NSZone *)zone {
    KeyValuePair *copy = [[KeyValuePair allocWithZone:zone] init];
    
    [copy setKey:self.key];
    [copy setValue:self.value];
    
    return copy;
}

- (BOOL)isEqual:(id)anObject {
    if (self == anObject) {
        return YES;
    }
    
    if (![anObject isKindOfClass:[KeyValuePair class]]) {
        return NO;
    }
    
    return [key isEqual:((KeyValuePair *)anObject).key]
    && [value isEqual:((KeyValuePair *)anObject).value];
}

- (NSUInteger)hash {
    NSUInteger hash = 0;
    hash = (hash * 31) + (key != nil ? [key hash] : 0);
    hash = (hash * 31) + (value != nil ? [value hash] : 0);
    return hash;
}
@end
