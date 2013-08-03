//
//  DSWindowDelegate.h
//  Image Batch Resizer
//
//  Created by Danilo Carvalho on 26/08/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface DSWindowDelegate : NSObject < NSWindowDelegate >
{
    @private
    BOOL _settedPaths;
    BOOL _outputtingFiles;
}
-(BOOL)windowShouldClose:(id)sender;
-(void)windowDidBecomeKey:(NSNotification *)notification;
@property (weak) IBOutlet NSApplication *application;
@property (weak) IBOutlet NSPathControl *xmlPath;
@property (weak) IBOutlet NSPathControl *inputPath;
@property (weak) IBOutlet NSPathControl *outputPath;
- (IBAction)resize:(id)sender;
@property (unsafe_unretained) IBOutlet NSWindow *window;
@property (weak) IBOutlet NSButton *resizeButton;

@end
