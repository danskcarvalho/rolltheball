//
//  NavigationBar.h
//  Test
//
//  Created by Danilo Carvalho on 02/06/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface NavigationBar : NSView
@property (strong, nonatomic) NSString* title;
@property (nonatomic) SEL action;
@property (nonatomic, weak) id target;
@end
