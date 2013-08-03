//
//  ObjectSite.h
//  CocoaTest
//
//  Created by Danilo Carvalho on 14/02/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "ObservableObject.h"

@interface ObjectSite : NSObject <NSCopying>
@property (weak) id<ObservableObject> object;
@property (strong) NSString* property;
-(BOOL)isEqual:(id)object;
-(BOOL)isEqualTo:(id)object;
-(NSUInteger)hash;
-(id)copyWithZone:(NSZone *)zone;
-(id)initWithObject:(id<ObservableObject>)obj property:(NSString*)p;
+(ObjectSite*)objectSiteWithObject:(id<ObservableObject>)obj property:(NSString*)p;
@end
