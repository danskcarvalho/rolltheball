//
//  ComponentInfo.m
//  RollerBallEditor
//
//  Created by Danilo Santos de Carvalho on 09/05/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import "ComponentInfo.h"

@implementation ComponentInfo
-(id)init{
    if(self = [super init]){
        self.name = nil;
        self.subname = nil;
        self.tag = NULL;
    }
    return self;
}
-(id)initWithName:(NSString *)name subname:(NSString *)subname tag:(void *)tag{
    if(self = [super init]){
        self.name = name;
        self.subname = subname;
        self.tag = tag;
    }
    return self;
}
@end
