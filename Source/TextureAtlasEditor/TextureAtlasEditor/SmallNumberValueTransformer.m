//
//  SmallNumberValueTransformer.m
//  TextureAtlasEditor
//
//  Created by Danilo Carvalho on 02/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "SmallNumberValueTransformer.h"

@implementation SmallNumberValueTransformer
+(BOOL)allowsReverseTransformation{
    return YES;
}
+(Class)transformedValueClass{
    return [NSString class];
}
-(id)transformedValue:(id)value{
    double dValue;
    if(value == nil)
        return @"0";
    
    if([value isKindOfClass:[NSString class]]){
        NSScanner* scanner = [NSScanner localizedScannerWithString:value];
        if(![scanner scanDouble:&dValue])
            dValue = 0;
    }
    else if([value isKindOfClass:[NSNumber class]]){
        dValue = [value doubleValue];
    }
    else {
        [NSException raise:NSInternalInconsistencyException format:@"Unknown type %@", [value class]];
    }
    
    return [NSString localizedStringWithFormat:@"%.2f", dValue];
}
-(id)reverseTransformedValue:(id)value{
    if(value == nil)
        return @0;
    
    NSNumber* dValue;
    if([value isKindOfClass:[NSString class]]){
        double ddValue;
        NSScanner* scanner = [NSScanner localizedScannerWithString:value];
        if(![scanner scanDouble:&ddValue])
            ddValue = 0;
        dValue = [NSNumber numberWithDouble:ddValue];
    }
    else if([value isKindOfClass:[NSNumber class]]){
        dValue = value;
    }
    else {
        [NSException raise:NSInternalInconsistencyException format:@"Unknown type %@", [value class]];
    }
    
    return dValue;
}
@end
