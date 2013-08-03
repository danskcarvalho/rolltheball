//
//  DSTextureAtlasController.h
//  TextureAtlasEditor
//
//  Created by Danilo Carvalho on 30/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "TextureAtlas.h"
#import "ExtendedNSCollectionView.h"

@interface DSTextureAtlasController : NSObject
@property(nonatomic, readwrite, strong) TextureAtlas* textureAtlas;
@property (weak) IBOutlet NSArrayController *textureArrayController;
@property (weak) IBOutlet ExtendedNSCollectionView *textureCollectionView;

- (IBAction)newTexture:(id)sender;
@property (weak) IBOutlet NSButton *btnNewStage;
@property (weak) IBOutlet NSArrayController *primitiveImageArrayController;
@property (weak) IBOutlet NSButton *deleteButton;
- (IBAction)newAtlas:(id)sender;
- (IBAction)openAtlas:(id)sender;
- (IBAction)newStage:(id)sender;
- (IBAction)saveAtlas:(id)sender;
-(void)awakeFromNib;
@end
