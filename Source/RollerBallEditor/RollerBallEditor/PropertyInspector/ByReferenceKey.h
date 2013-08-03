//
//  ByReferenceKey.h
//  CocoaTest
//
//  Created by Danilo Carvalho on 14/02/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface ByReferenceKey : NSObject < NSCopying >
@property (weak) id object;
-(BOOL)isEqual:(id)object;
-(BOOL)isEqualTo:(id)object;
-(NSUInteger)hash;
-(id)initWithObject:(id)object;
+(ByReferenceKey*)refKeyWithObject:(id)object;
@end
