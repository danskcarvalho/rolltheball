//
//  ExtendedNSCollectionView.h
//  TextureAtlasEditor
//
//  Created by Danilo Carvalho on 31/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "TextureAtlas.h"
@interface ExtendedNSCollectionView : NSCollectionView
@property (weak) IBOutlet NSArrayController *arrayController;
@property (weak) IBOutlet TextureAtlas* textureAtlas;
@end
