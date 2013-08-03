//
//  ObservableObject.h
//  CocoaTest
//
//  Created by Danilo Carvalho on 28/01/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Observer.h"

@class TypeDescriptor;

@protocol ObservableObject <NSObject>

-(TypeDescriptor*)typeDescriptor;

//set methods
-(void)setString:(NSString*)value property:(NSString*)property;
-(void)setInt:(NSInteger)value property:(NSString*)property;
-(void)setFloat:(float)value property:(NSString*)property;
-(void)setFloatPair:(float)value1 second:(float)value2 property:(NSString*)property;
-(void)setBool:(BOOL)value property:(NSString*)property;
-(void)setColor:(NSColor*)value property:(NSString*)property;
-(void)setImage:(NSImage*)value property:(NSString*)property;
-(void)setObject:(id)value property:(NSString*)property;
-(void)setVector:(NSArray*)value property:(NSString*)property;
-(void)setNil:(NSString*)property;
-(void)setUnionTag:(NSString*)tag;
-(void)callAction:(NSString*)property actionName:(NSString*)actionName;

//get methods
-(void)getString:(NSString**)value property:(NSString*)property;
-(void)getInt:(NSNumber**)value property:(NSString*)property;
-(void)getFloat:(NSNumber**)value property:(NSString*)property;
-(void)getFloatPair:(NSNumber**)value1 second:(NSNumber**)value2 property:(NSString*)property;
-(void)getBool:(NSNumber**)value property:(NSString*)property;
-(void)getColor:(NSColor**)value property:(NSString*)property;
-(void)getImage:(NSImage**)value property:(NSString*)property;
-(void)getObject:(id*)value property:(NSString*)property;
-(void)getVector:(NSArray**)value property:(NSString*)property;
-(void)getUnionTag:(NSString**)tag;
-(NSArray*)getAutoCompleteSuggestions:(NSString*)property;

//add observer
-(void)addObserver:(id<Observer>)observer;
-(void)removeObserver:(id)observer;
@end