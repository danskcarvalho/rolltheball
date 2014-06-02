//
//  sound_player.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 2014-06-01.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__sound_player__
#define __RollerBallCore__sound_player__

namespace rb {
    class sound_player {
    public:
        static void play_background();
        static void preload_effects();
        static void play_explosion();
        static void play_click();
        static void play_coin();
        static void play_breakblock();
        static void play_disappear();
        static void play_jump();
    };
}

#endif /* defined(__RollerBallCore__sound_player__) */
