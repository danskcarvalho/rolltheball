//
//  program_manager.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 22/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__program_manager__
#define __RollerBallCore__program_manager__

#include "base.h"

namespace rb {
    class program;
    class program_manager {
    public:
        program_manager();
        virtual ~program_manager();
        virtual const program* get_basic_program() const = 0;
        virtual const program* get_basic_program_with_wrapping() const = 0;
    };
}

#endif /* defined(__RollerBallCore__program_manager__) */
