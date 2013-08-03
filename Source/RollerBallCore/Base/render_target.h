//
//  render_target.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 06/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__render_target__
#define __RollerBallCore__render_target__

#include "base.h"
#include "texture.h"

namespace rb {
    class color;
    class render_target : public texture {
    private:
        GLuint _fbo_id;
        GLint _old_fbo;
        GLfloat _old_viewport[4];
    public:
        render_target(const vec2& size, const nullable<color>& background_color);
        void bind();
        void unbind();
        virtual ~render_target();
    };
}

#endif /* defined(__RollerBallCore__render_target__) */
