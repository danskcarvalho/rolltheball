//
//  basic_process.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 22/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__basic_process__
#define __RollerBallCore__basic_process__

#include "base.h"
#include "transform_space.h"
#include "process.h"
#include "color.h"

namespace rb {
    class program_manager;
    class texture;
    class basic_process : public process {
    protected:
        virtual bool equals(const process& other) const;
    private:
        enum class uniforms {
            position_transform = 0,
            texture_transform = 1,
            texture_sampler = 2,
            ambient_color = 3
        };
        GLuint _gl_program;
        GLint _gl_uniforms[4];
        transform_space _position_transform;
        transform_space _texture_transform;
        color _ambient_color;
        _matrix3x3 _m_position_transform;
        _matrix3x3 _m_texture_transform;
        const texture* _texture_sampler;
    public:
        basic_process(const program_manager* manager, const bool can_transform_texture = false);
        virtual ~basic_process();
        inline const transform_space& position_transform() const {
            return _position_transform;
        }
        inline const transform_space& position_transform(const transform_space& value) {
            _position_transform = value;
            return _position_transform;
        }
        inline const transform_space& texture_transform() const {
            return _texture_transform;
        }
        inline const transform_space& texture_transform(const transform_space& value) {
            _texture_transform = value;
            return _texture_transform;
        }
        inline const texture* texture_sampler() const{
            return _texture_sampler;
        }
        inline const texture* texture_sampler(const texture* value){
            _texture_sampler = value;
            return _texture_sampler;
        }
        inline const color& ambient_color() const{
            return _ambient_color;
        }
        inline const color& ambient_color(const color& value){
            _ambient_color = value;
            return _ambient_color;
        }
        virtual void begin_draw();
        virtual void end_draw();
    };
}

#endif /* defined(__RollerBallCore__basic_process__) */
