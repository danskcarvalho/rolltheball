//
//  texture_source.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 15/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__texture_source__
#define __RollerBallCore__texture_source__

#include "base.h"

namespace rb {
    class texture_source {
    public:
        texture_source();
        virtual const URL_TYPE image_url() const = 0;
        virtual rb_string to_string() const = 0;
        virtual texture_source* duplicate() const = 0;
        
        virtual ~texture_source();
    };
}

#endif /* defined(__RollerBallCore__texture_source__) */
