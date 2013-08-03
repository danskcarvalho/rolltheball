//
//  CppBridgeProtocol.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 28/02/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol CppBridgeProtocol <NSObject>
-(void)notifyObservers:(NSString*)property;
-(void)notifyObserversOfDeletedObject;
+(void)textureAtlasChanged:(void*)newAtlas;
@end
