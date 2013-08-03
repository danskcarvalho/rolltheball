//
//  platform.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 01/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__platform__
#define __RollerBallCore__platform__

#ifndef _FINAL_APP_
#include "precompiled.h"
#endif
#include "definitions.h"

namespace rb {
    class platform {
    public:
        static void error(const rb_string& str);
        static void msg(const rb_string& str);
        static void warn(const rb_string& str);
    };
}

#endif /* defined(__RollerBallCore__platform__) */
