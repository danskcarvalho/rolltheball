//
//  SmallNumberValueTransformer.h
//  TextureAtlasEditor
//
//  Created by Danilo Carvalho on 02/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface SmallNumberValueTransformer : NSValueTransformer
+(BOOL)allowsReverseTransformation;
+(Class)transformedValueClass;
-(id)transformedValue:(id)value;
-(id)reverseTransformedValue:(id)value;
@end
