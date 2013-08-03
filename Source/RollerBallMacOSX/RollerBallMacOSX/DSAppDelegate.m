//
//  DSAppDelegate.m
//  RollerBallMacOSX
//
//  Created by Danilo Carvalho on 30/08/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "DSAppDelegate.h"

@implementation DSAppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application
}

-(void)setFullScreen:(id)sender{
    [self.openGLView setFullScreen];
}

@end
