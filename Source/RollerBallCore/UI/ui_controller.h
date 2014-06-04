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
    struct score {
        size_t set;
        float value;
    };
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
        static std::vector<score> _scores_to_be_reported;
        static size_t _show_leaderboard;
        static bool _buy_hearts;
        static bool _buy_set2;
        static bool _set2_unlocked;
        static bool _played_tutorial;
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
        static bool& played_tutorial();
        //statistics
        static float coins();
        static float coins(float value);
        static float time();
        static float time(float value);
        static float hearts();
        static float hearts(float value);
        static float deaths();
        static float deaths(float value);
        //scores to be reported
        static std::vector<score>& scores();
        static size_t& leaderboard_to_show();
        //buying
        static bool buy_hearts();
        static bool buy_set2();
        static bool buy_hearts(bool value);
        static bool buy_set2(bool value);
        static void unlock_set2();
        static bool set2_unlocked();
    };
}

#endif /* defined(__RollerBallCore__ui_controller__) */
