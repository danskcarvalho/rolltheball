//
//  ComponentInfo.h
//  RollerBallEditor
//
//  Created by Danilo Santos de Carvalho on 09/05/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface ComponentInfo : NSObject
@property(strong) NSString* name;
@property(strong) NSString* subname;
@property(assign) void* tag;
-(id)init;
-(id)initWithName: (NSString*)name subname:(NSString*)subname tag:(void*)tag;
@end
