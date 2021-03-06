//
//  ui_controller.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 2014-05-26.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__ui_controller__
#define __RollerBallCore__ui_controller__

#include "components_base.h"

namespace rb {
    class ui_controller {
    public:
        static bool _is_intro;
        static bool _is_tutorial;
        static bool _is_playing;
        static bool _is_force_load_level;
        static size_t _set;
        static rb_string _set1_levels[10];
        static rb_string _set2_levels[10];
        static rb_string _tut_levels[6];
        static size_t _current_level;
        static float _coins;
        static float _time;
        static float _hearts;
        static float _deaths;
        static float _saved_hearts;
    public:
        static bool is_intro();
        static bool set_intro(bool value);
        static bool is_tutorial();
        static bool set_tutorial(bool value);
        static bool is_playing();
        static bool set_playing(bool value);
        static size_t get_set();
        static size_t set_set(size_t value);
        static size_t get_level_number();
        static nullable<rb_string> get_level();
        static bool next_level();
        static void goto_first_level();
        static bool is_force_load_level();
        static bool set_force_load_level(bool value);
        static bool is_last_level();
        static void save_hearts();
        static void restore_hearts();
        //statistics
        static float coins();
        static float coins(float value);
        static float time();
        static float time(float value);
        static float hearts();
        static float hearts(float value);
        static float deaths();
        static float deaths(float value);
    };
}

#endif /* defined(__RollerBallCore__ui_controller__) */
