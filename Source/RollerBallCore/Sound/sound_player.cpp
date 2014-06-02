//
//  sound_player.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 2014-06-01.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#include "sound_player.h"
#include "director.h"
#import "OALSimpleAudio.h"

using namespace rb;

void rb::sound_player::play_background(){
    if(director::in_editor())
        return;
    OALSimpleAudio *audio = [OALSimpleAudio sharedInstance];
    [audio playBg:@"Soundtrack.mp3" loop:YES];
}

void sound_player::preload_effects(){
    if(director::in_editor())
        return;
    OALSimpleAudio *audio = [OALSimpleAudio sharedInstance];
    [audio preloadBg:@"Soundtrack.mp3"];
    [audio preloadEffect:@"Click.wav"];
    [audio preloadEffect:@"Coin.wav"];
    [audio preloadEffect:@"Explosion.wav"];
    [audio preloadEffect:@"Jump.wav"];
    [audio preloadEffect:@"BreakBlock.wav"];
    [audio preloadEffect:@"Disappear.wav"];
}

void sound_player::play_click(){
    if(director::in_editor())
        return;
    OALSimpleAudio *audio = [OALSimpleAudio sharedInstance];
    [audio playEffect:@"Click.wav"];
}

void sound_player::play_coin(){
    if(director::in_editor())
        return;
    OALSimpleAudio *audio = [OALSimpleAudio sharedInstance];
    [audio playEffect:@"Coin.wav"];
}

void sound_player::play_explosion(){
    if(director::in_editor())
        return;
    OALSimpleAudio *audio = [OALSimpleAudio sharedInstance];
    [audio playEffect:@"Explosion.wav"];
}

void sound_player::play_disappear(){
    if(director::in_editor())
        return;
    OALSimpleAudio *audio = [OALSimpleAudio sharedInstance];
    [audio playEffect:@"Disappear.wav"];
}

void sound_player::play_breakblock(){
    if(director::in_editor())
        return;
    OALSimpleAudio *audio = [OALSimpleAudio sharedInstance];
    [audio playEffect:@"BreakBlock.wav"];
}

void sound_player::play_jump(){
    if(director::in_editor())
        return;
    OALSimpleAudio *audio = [OALSimpleAudio sharedInstance];
    [audio playEffect:@"Jump.wav"];
}










