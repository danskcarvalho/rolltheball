//
//  CppBridge.h
//  RollerBallEditor
//
//  Created by Danilo Carvalho on 28/02/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "ObservableObject.h"
#define _OBJC_BRIDGE_
#include "components_external.h"

@interface CppBridge : NSObject < CppBridgeProtocol, ObservableObject >
-(id)initWithCppObject:(rb::typed_object*)object;
+(void)textureAtlasChanged:(void*)newAtlas withURL:(NSURL*)url;
-(void)deleteInternalObject;
@end
