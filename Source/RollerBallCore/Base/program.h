//
//  program.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 22/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__program__
#define __RollerBallCore__program__

#include "base.h"

namespace rb {
    class program {
    private:
        GLuint _gl_program;
    public:
        program(char* vertex_shader, char* fragment_shader);
        ~program();
        program(const program& other) = delete;
        program(program&& other) = delete;
        program& operator=(const program& other) = delete;
        program& operator=(program&& other) = delete;
        
        inline uint32_t program_id() const {
            return _gl_program;
        }
    };
}

#endif /* defined(__RollerBallCore__program__) */
