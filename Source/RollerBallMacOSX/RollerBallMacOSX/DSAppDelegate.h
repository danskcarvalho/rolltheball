//
//  DSAppDelegate.h
//  RollerBallMacOSX
//
//  Created by Danilo Carvalho on 30/08/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "DSCustomOpenGLView.h"

@interface DSAppDelegate : NSObject <NSApplicationDelegate>

@property (weak) IBOutlet DSCustomOpenGLView *openGLView;
@property (assign) IBOutlet NSWindow *window;
- (IBAction)setFullScreen:(id)sender;

@end
