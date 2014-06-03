//
//  game_server.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 2014-06-02.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#include "game_saver.h"
#include "components_base.h"
#import <Foundation/Foundation.h>

using namespace rb;

void game_saver::load_saved(rb::saved_data_v1 *d){
    NSString* homeDir = NSHomeDirectory();
    homeDir = [homeDir stringByAppendingPathComponent: @"Library"];
    homeDir = [homeDir stringByAppendingPathComponent: @"saved_data"];
    NSData* data = [NSData dataWithContentsOfFile: homeDir];
    if(data == nil){
        d->version = 1;
        d->hearts = 0;
        d->set2Availability = 0; //not available
        return;
    }
    memcpy(d, [data bytes], sizeof(saved_data_v1));
}

void game_saver::save(rb::saved_data_v1 *d){
    NSString* homeDir = NSHomeDirectory();
    homeDir = [homeDir stringByAppendingPathComponent: @"Library"];
    homeDir = [homeDir stringByAppendingPathComponent: @"saved_data"];
    NSData* data = [NSData dataWithBytes:d length:sizeof(saved_data_v1)];
    BOOL written = [data writeToFile:homeDir atomically:NO];
    assert(written);
}





















