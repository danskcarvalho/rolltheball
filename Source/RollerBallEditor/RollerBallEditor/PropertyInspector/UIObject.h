//
//  UIObject.h
//  CocoaTest
//
//  Created by Danilo Carvalho on 08/02/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "ObservableObject.h"
#import "TypeDescriptor.h"
@interface UIObject : NSObject <ObservableObject>
-(id)initFromTypeDescriptor:(TypeDescriptor*)typeDescriptor;
-(void)setStringInternally:(NSString *)value property:(NSString *)property;
-(void)setIntInternally:(NSInteger)value property:(NSString *)property;
-(void)setFloatInternally:(float)value property:(NSString *)property;
-(void)setFloatPairInternally:(float)value1 second:(float)value2 property:(NSString *)property;
-(void)setBoolInternally:(BOOL)value property:(NSString *)property;
-(void)setColorInternally:(NSColor *)value property:(NSString *)property;
-(void)setImageInternally:(NSImage *)value property:(NSString *)property;
-(void)setObjectInternally:(id)value property:(NSString *)property;
-(void)setVectorInternally:(NSArray *)value property:(NSString *)property;
-(void)setNilInternally:(NSString *)property;
-(void)setUnionTagInternally:(NSString *)tag;
@end
