//
//  apple_program_manager.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 24/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__apple_program_manager__
#define __RollerBallCore__apple_program_manager__

#include "base.h"
#include "program_manager.h"

namespace rb {
    class program;
    class apple_program_manager : public program_manager {
    private:
        program* _basic_program;
        program* _basic_program_with_wrapping;
        void load_basic_program();
        void load_basic_program_with_wrapping();
    public:
        apple_program_manager();
        virtual ~apple_program_manager();
        virtual const program* get_basic_program() const;
        virtual const program* get_basic_program_with_wrapping() const;
    };
}

#endif /* defined(__RollerBallCore__apple_program_manager__) */
