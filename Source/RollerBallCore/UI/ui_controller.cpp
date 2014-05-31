//
//  ui_controller.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 2014-05-26.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#include "ui_controller.h"

using namespace rb;

bool ui_controller::_is_intro = false;
bool ui_controller::_is_tutorial = false;
bool ui_controller::_is_playing = false;
size_t ui_controller::_set = 1;
rb_string ui_controller::_set1_levels[10] = {u"Level0", u"Level2", u"Level4", u"Level6", u"Level8", u"Level10", u"Level12", u"Level14", u"Level16", u"Level18"};
rb_string ui_controller::_set2_levels[10] = {u"Level1", u"Level3", u"Level5", u"Level7", u"Level9", u"Level11", u"Level13", u"Level15", u"Level17", u"Level19"};
rb_string ui_controller::_tut_levels[6] = {u"Tutorial0", u"Tutorial1", u"Tutorial2", u"Tutorial3", u"Tutorial4", u"Tutorial5"};
size_t ui_controller::_current_level = 0;
bool ui_controller::_is_force_load_level = false;

bool ui_controller::is_intro(){
    return _is_intro;
}

bool ui_controller::set_intro(bool value){
    return _is_intro = value;
}

bool ui_controller::is_tutorial(){
    return _is_tutorial;
}

bool ui_controller::set_tutorial(bool value){
    return _is_tutorial = value;
}

bool ui_controller::is_playing(){
    return _is_playing;
}

bool ui_controller::set_playing(bool value){
    return _is_playing = value;
}

bool ui_controller::is_force_load_level(){
    return _is_force_load_level;
}

bool ui_controller::set_force_load_level(bool value){
    return _is_force_load_level = value;
}

size_t ui_controller::get_set(){
    return _set;
}

size_t ui_controller::set_set(size_t value){
    return _set = value;
}

void ui_controller::goto_first_level(){
    _current_level = 0;
}

bool ui_controller::next_level(){
    if(_is_tutorial){
        if(_current_level > 5)
            return false;
        _current_level++;
        return true;
    }
    else {
        if(_current_level > 9)
            return false;
        _current_level++;
        return true;
    }
}

nullable<rb_string> ui_controller::get_level(){
    if(_is_intro)
        return nullable<rb_string>(u"Intro");
    if(_is_tutorial){
        if(_current_level > 5)
            return nullptr;
        return _tut_levels[_current_level];
    }
    else {
        if(_current_level > 9)
            return nullptr;
        if(_set == 1)
            return _set1_levels[_current_level];
        else
            return _set2_levels[_current_level];
    }
}

















