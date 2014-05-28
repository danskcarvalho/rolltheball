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
    public:
        static bool is_intro();
        static bool set_intro(bool value);
    };
}

#endif /* defined(__RollerBallCore__ui_controller__) */
