//
//  DSTextureAtlasController.m
//  TextureAtlasEditor
//
//  Created by Danilo Carvalho on 30/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "DSTextureAtlasController.h"
#import "Texture.h"
#import "TextureStage.h"

@interface DSTextureAtlasController ()
{
    NSSavePanel* fSavePanel;
    NSOpenPanel* fOpenPanel;
}
-(void)showErrorMessage:(NSString*)message informativeMessage:(NSString*)informativeMessage;
@end

@implementation DSTextureAtlasController
-(void)showErrorMessage:(NSString *)message informativeMessage:(NSString *)informativeMessage{
    NSAlert* alert = [[NSAlert alloc] init];
    [alert addButtonWithTitle:@"OK"];
    [alert setMessageText:message];
    [alert setInformativeText:informativeMessage];
    [alert setAlertStyle:NSCriticalAlertStyle];
    [alert beginSheetModalForWindow:[[NSApplication sharedApplication] keyWindow] modalDelegate:self didEndSelector:@selector(alertDidEnd:returnCode:contextInfo:) contextInfo:NULL];
}
@synthesize textureAtlas;
- (IBAction)newTexture:(id)sender {
    NSUInteger selectedIndex = [self.primitiveImageArrayController selectionIndex];
    if(selectedIndex == NSNotFound){
        [self showErrorMessage:@"No Image Selected" informativeMessage:@"You need to select an image before continuing with this operation."];
    }
    else {
        Texture* texture = [[Texture alloc] initWithImage:[self.textureAtlas.primitiveImages objectAtIndex: selectedIndex] atlas:self.textureAtlas];
        [self.textureAtlas insertObject:texture inTexturesAtIndex:[self.textureAtlas countOfTextures]];
    }
}

-(void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context{
    if([keyPath isEqualToString:@"selectionIndex"]){
        NSUInteger newIndex = [self.textureArrayController selectionIndex];
        if (newIndex == NSNotFound) {
            [self.btnNewStage setEnabled:NO];
        }
        else {
            [self.btnNewStage setEnabled:YES];
        }
    }
    else
        [super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
}

- (IBAction)newStage:(id)sender {
    NSUInteger selectedIndex = [self.primitiveImageArrayController selectionIndex];
    if(selectedIndex == NSNotFound){
        [self showErrorMessage:@"No Image Selected" informativeMessage:@"You need to select an image before continuing with this operation."];
    }
    else {
        Texture* texture = [self.textureAtlas.textures objectAtIndex: [self.textureArrayController selectionIndex]];
        TextureStage* stage = [[TextureStage alloc] initWithTexture:texture];
        [stage setPrimitiveImage:[self.textureAtlas.primitiveImages objectAtIndex:selectedIndex]];
        [texture insertObject:stage inStagesAtIndex:[texture countOfStages]];
        
        if([texture.stages count] > 2){
            TextureStage* toBeAdjusted = [texture.stages objectAtIndex:([texture.stages count] - 2)];
            toBeAdjusted.last = NO;
        }
    }
}

- (IBAction)saveAtlas:(id)sender {
    if([self.textureAtlas.textures count] == 0){
        [self showErrorMessage:@"No textures." informativeMessage:@"You can't create an atlas with no textures."];
        return;
    }
    NSMutableSet* setOfTextureNames = [NSMutableSet set];
    for (int i = 0; i < [self.textureAtlas.textures count]; i++) {
        Texture* tex = [self.textureAtlas objectInTexturesAtIndex:i];
        if (tex.name == nil) {
            [self showErrorMessage:@"Texture without a name." informativeMessage:@"You need to name all textures before saving this atlas."];
            self.textureArrayController.selectionIndex = i;
            return;
        }
        [tex setName:[tex.name stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]]];
        if([tex.name isEqualToString:@""]){
            [self showErrorMessage:@"Texture without a name." informativeMessage:@"You need to name all textures before saving this atlas."];
            self.textureArrayController.selectionIndex = i;
            return;
        }
        if([setOfTextureNames containsObject:tex.name]){
            [self showErrorMessage:@"Repeated texture name." informativeMessage:@"Each texture must have a unique texture name.s"];
            self.textureArrayController.selectionIndex = i;
            return;
        }
        [setOfTextureNames addObject:tex.name];
        if(![tex anyGroup]){
            [self showErrorMessage:@"Texture with no group." informativeMessage:@"A texture must belong to at least one group."];
            self.textureArrayController.selectionIndex = i;
            return;
        }
    }
    
    [fSavePanel beginSheetModalForWindow:[[NSApplication sharedApplication] keyWindow] completionHandler:^(NSInteger result) {
        if(result == NSFileHandlingPanelCancelButton)
            return;
        NSURL* fileNameURL = [fSavePanel URL];
        NSString* name = [fileNameURL lastPathComponent];
        if(![[name pathExtension] isEqualToString:@"bundle"]){
            name =  [name stringByAppendingPathExtension:@"bundle"];
        }
        NSURL* pathURL = [fileNameURL URLByDeletingLastPathComponent];
        [self.textureAtlas saveToDirectoryURL:pathURL WithName:name];
    }];
}

- (IBAction)newAtlas:(id)sender {
    [self.textureAtlas removeAllTextures];
}

- (IBAction)openAtlas:(id)sender {
    [fOpenPanel beginSheetModalForWindow:[[NSApplication sharedApplication] keyWindow] completionHandler:^(NSInteger result) {
        if(result == NSFileHandlingPanelCancelButton)
            return;
        NSURL* fileNameURL = [[fOpenPanel URLs] lastObject];
        NSString* name = [fileNameURL lastPathComponent];
        if(![[name pathExtension] isEqualToString:@"bundle"]){
            name =  [name stringByAppendingPathExtension:@"bundle"];
        }
        NSURL* pathURL = [fileNameURL URLByDeletingLastPathComponent];
        [self.textureAtlas loadFromDirectoryURL:pathURL WithName:name];
    }];
}

-(void)awakeFromNib{
    fSavePanel = [NSSavePanel savePanel];
    [fSavePanel setExtensionHidden:YES];
    [fSavePanel setAllowedFileTypes:@[@"bundle"]];
    [fSavePanel setAllowsOtherFileTypes:NO];
    fOpenPanel = [NSOpenPanel openPanel];
    [fOpenPanel setExtensionHidden:YES];
    [fOpenPanel setAllowedFileTypes:@[@"bundle"]];
    [fOpenPanel setAllowsOtherFileTypes:NO];
    [fOpenPanel setAllowsMultipleSelection:NO];
    self.textureAtlas = [[TextureAtlas alloc] init];
    [self.textureArrayController addObserver:self forKeyPath:@"selectionIndex" options:(NSKeyValueObservingOptionNew | NSKeyValueObservingOptionInitial) context:NULL];
    [self.textureCollectionView setTextureAtlas:self.textureAtlas];
}

-(void)alertDidEnd:(NSAlert *)alert returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo{
    
}
@end
