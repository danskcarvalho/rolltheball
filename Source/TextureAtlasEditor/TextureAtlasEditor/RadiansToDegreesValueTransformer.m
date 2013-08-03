//
//  RadiansToDegreesValueTransformer.m
//  TextureAtlasEditor
//
//  Created by Danilo Carvalho on 02/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "RadiansToDegreesValueTransformer.h"
#include <math.h>

@implementation RadiansToDegreesValueTransformer
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
    
    dValue = (dValue / M_PI) * 180;
    
    return [NSString localizedStringWithFormat:@"%.0f", dValue];
}
-(id)reverseTransformedValue:(id)value{
    if(value == nil)
        return @0;
    
    double dValue;
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
    
    dValue = (dValue / 180) * M_PI;
    
    return [NSNumber numberWithDouble:dValue];
}
@end
