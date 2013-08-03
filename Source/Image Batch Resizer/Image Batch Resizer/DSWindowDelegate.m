//
//  DSWindowDelegate.m
//  Image Batch Resizer
//
//  Created by Danilo Carvalho on 26/08/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "DSWindowDelegate.h"
#import "DSResizeDefFile.h"

@implementation DSWindowDelegate
@synthesize window;
@synthesize resizeButton;
@synthesize application;
@synthesize xmlPath;
@synthesize inputPath;
@synthesize outputPath;

-(id)init{
    self = [super init];
    if(self){
        _settedPaths = NO;
        _outputtingFiles = NO;
    }
    return self;
}

-(BOOL)windowShouldClose:(id)sender{
    if(!_outputtingFiles){
        [application terminate:sender];
        return YES;
    }
    else
        return NO;
}

- (IBAction)resize:(id)sender {
    DSResizeDefFile* resizer = [[DSResizeDefFile alloc] initWithXmlPath:[xmlPath URL] basePath:[inputPath URL] outputPath:[outputPath URL]];
    if(resizer){
        [resizeButton setEnabled:NO];
        
        if ([resizer writeOutputPathWithDelegate:nil]){
            NSAlert* alert =[NSAlert alertWithMessageText:@"Redimensionamento executado com sucesso!" defaultButton:@"OK" alternateButton:nil otherButton:nil informativeTextWithFormat:@"Todas as imagens foram redimensionadas.", nil];
            alert.alertStyle = NSInformationalAlertStyle;
            [alert runModal];
            
        }
        else {
            NSAlert* alert =[NSAlert alertWithMessageText:@"Erro no redimensionamento!" defaultButton:@"OK" alternateButton:nil otherButton:nil informativeTextWithFormat:@"Uma ou mais imagens talvez não tenham sido redimensionadas corretamente.", nil];
            alert.alertStyle = NSCriticalAlertStyle;
            [alert runModal];
        }
        
        [resizeButton setEnabled:YES];
    }
    else {
        NSAlert* alert =[NSAlert alertWithMessageText:@"Erro no redimensionamento!" defaultButton:@"OK" alternateButton:nil otherButton:nil informativeTextWithFormat:@"Uma ou mais imagens talvez não tenham sido redimensionadas corretamente.", nil];
        alert.alertStyle = NSCriticalAlertStyle;
        [alert runModal];
    }
}

-(void)windowDidBecomeKey:(NSNotification *)notification{
    if(!_settedPaths){
        _settedPaths = YES;
        [xmlPath setURL: [NSURL URLWithString:[@"~" stringByExpandingTildeInPath]]];
        [inputPath setURL: [NSURL URLWithString:[@"~" stringByExpandingTildeInPath]]];
        [outputPath setURL: [NSURL URLWithString:[@"~" stringByExpandingTildeInPath]]];
    }
}
@end
