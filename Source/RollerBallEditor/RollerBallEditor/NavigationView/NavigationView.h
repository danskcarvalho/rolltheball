//
//  NavigationView.h
//  Test
//
//  Created by Danilo Carvalho on 02/06/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>

@interface NavigationView : NSView
-(void)pushView:(NSView*)view withTitle:(NSString*)title;
-(void)popView;
-(NSView*)peekView;
-(void)removeView:(NSView*)view;
@property(weak, nonatomic) id target;
@end
