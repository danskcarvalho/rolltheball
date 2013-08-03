//
//  ConstructibleType.h
//  CocoaTest
//
//  Created by Danilo Carvalho on 08/02/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "TypeDescriptor.h"
typedef id (^CreatorBlock)(void);

@interface ConstructibleType : TypeDescriptor
@property(strong) CreatorBlock creatorBlock;
-(id)newObject;
-(id)initFromTypeDescriptor:(TypeDescriptor*)typeDescriptor;
+(ConstructibleType*)constructibleTypeFromTypeDescriptor:(TypeDescriptor*)typeDescriptor;
+(ConstructibleType *)nullableTypeFromTypeDescriptor:(NSString*)name type:(ConstructibleType *)type;
@end
