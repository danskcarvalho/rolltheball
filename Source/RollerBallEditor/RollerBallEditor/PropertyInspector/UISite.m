//
//  UISite.m
//  CocoaTest
//
//  Created by Danilo Carvalho on 14/02/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import "UISite.h"

@implementation UISite
-(BOOL)isEqual:(id)object{
    if (object == self)
        return YES;
    if (!object || ![object isKindOfClass:[self class]])
        return NO;
    UISite* other = object;
    return self.group == other.group && self.cellIndex == other.cellIndex;
}
-(BOOL)isEqualTo:(id)object{
    return [self isEqual:object];
}
-(NSUInteger)hash{
    unsigned long long _hash = 1;
    unsigned long long _prime = 31;
    if(self.group)
        _hash = _prime * _hash + (unsigned long)(__bridge void*)self.group;
    
    _hash = _prime * _hash + (unsigned long)self.cellIndex;
    return _hash;
}
-(id)copyWithZone:(NSZone *)zone{
    return self;
}
-(id)initWithGroup:(PropertyInspectorGroup *)grp index:(NSUInteger)index{
    if(self = [self init]){
        self.group = grp;
        self.cellIndex = index;
        self.tag = nil;
    }
    return self;
}
+(UISite *)uiSiteWithGroup:(PropertyInspectorGroup *)grp index:(NSUInteger)index{
    return [[UISite alloc] initWithGroup:grp index:index];
}
+(UISite *)uiSiteWithGroup:(PropertyInspectorGroup *)grp index:(NSUInteger)index tag:(id)tag{
    UISite* s = [UISite uiSiteWithGroup:grp index:index];
    s.tag = tag;
    return s;
}
@end
