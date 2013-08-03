//
//  TextureAtlasLoader.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 24/06/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface TextureAtlasLoader : NSObject
+(Class) bridgeClass;
+(void) setBridgeClass:(Class)bridgeClass;
+(void*)loadTextureAtlas: (NSURL*)atlasUrl;
@end
