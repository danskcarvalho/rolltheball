//
//  KeyValuePair.h
//  TextureAtlasEditor
//
//  Created by Danilo Carvalho on 31/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface KeyValuePair : NSObject <NSCoding>
@property (nonatomic, strong)   id  key;
@property (nonatomic, strong)   id  value;

- (id)initWithKey:(id)aKey andValue:(id)aValue;
+ (id)keyValuePairWithKey:(id)aKey value:(id)aValue;
-(void)encodeWithCoder:(NSCoder *)aCoder;
-(id)initWithCoder:(NSCoder *)aDecoder;
@end
