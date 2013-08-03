//
//  UISite.h
//  CocoaTest
//
//  Created by Danilo Carvalho on 14/02/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>

#define uiSiteHeaderIndex ((NSUInteger)-1)

@class PropertyInspectorGroup;

@interface UISite : NSObject < NSCopying >
@property (weak) PropertyInspectorGroup* group;
@property NSUInteger cellIndex;
@property (strong, nonatomic) id tag;
-(BOOL)isEqual:(id)object;
-(BOOL)isEqualTo:(id)object;
-(NSUInteger)hash;
-(id)copyWithZone:(NSZone *)zone;
-(id)initWithGroup:(PropertyInspectorGroup*)grp index:(NSUInteger)index;
+(UISite*)uiSiteWithGroup:(PropertyInspectorGroup*)grp index:(NSUInteger)index;
+(UISite*)uiSiteWithGroup:(PropertyInspectorGroup*)grp index:(NSUInteger)index tag:(id)tag;
@end
