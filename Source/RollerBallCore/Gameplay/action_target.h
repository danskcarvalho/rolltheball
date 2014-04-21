//
//  action_target.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 2014-04-20.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__action_target__
#define __RollerBallCore__action_target__

#include "components_base.h"

namespace rb {
    class action_target {
    public:
        virtual void do_action(const rb_string& action_name, const rb_string& arg) = 0;
    };
}

#endif /* defined(__RollerBallCore__action_target__) */
