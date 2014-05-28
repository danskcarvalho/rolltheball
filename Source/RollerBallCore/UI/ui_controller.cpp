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

bool ui_controller::is_intro(){
    return _is_intro;
}

bool ui_controller::set_intro(bool value){
    return _is_intro = value;
}















