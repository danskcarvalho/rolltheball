//
//  Observer.h
//  CocoaTest
//
//  Created by Danilo Carvalho on 28/01/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>
@protocol ObservableObject;
@protocol Observer <NSObject>
-(void)propertyChanged:(id<ObservableObject>)object property:(NSString*)property;
-(void)unionTagChanged:(id<ObservableObject>)object;
-(void)objectDeleted:(id<ObservableObject>)object;
@end
