//
//  NavigablePropertyInspector.h
//  RollerBallEditor
//
//  Created by Danilo Santos de Carvalho on 03/06/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "ObservableObject.h"
#import "Observer.h"

@class PropertyInspector;
@interface NavigablePropertyInspector : NSView <Observer>
-(void)pushPropertyInspector:(PropertyInspector*)inspector withTitle:(NSString*)title;
-(void)pushObject:(id)obj withTitle: (NSString*)title;
@property (strong, nonatomic) id<ObservableObject> object;
@end
