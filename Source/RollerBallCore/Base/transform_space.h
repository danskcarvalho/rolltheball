//
//  transform_space.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 03/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__transform_space__
#define __RollerBallCore__transform_space__

#include "base.h"
#include "vec2.h"
#include "matrix3x3.h"

namespace rb {
    class buffer;
    class transform_space {
    private:
        bool _dirty;
        vec2 _axisScale;
        vec2 _axisRotation;
        vec2 _origin;
        matrix3x3 _matrix;
        nullable<matrix3x3> _inv_matrix;
        
        void init();
        //updates the matrix only if dirty
        void update_matrix();
        void update_inv_matrix();
        void update_from_matrix(const matrix3x3& m);
        void load_from_buffer(const void* buffer, const void** next);
        void store_in_buffer(void* buffer, size_t* size, void** next) const;
    public:
        //constructors
        static transform_space from_matrix(const matrix3x3& m);
        transform_space();
        transform_space(const transform_space& other);
        transform_space(transform_space&& other);
        transform_space(const vec2& origin, const vec2& scale = vec2(1, 1), const vec2& rotation = vec2(0, (M_PI / 2.0f)));
        transform_space(const vec2& origin, const vec2& scale, const float rotation);
        transform_space(const vec2& origin, const float scale, const float rotation = 0);
        transform_space(buffer buffer);
        
        //get methods
        inline const vec2& scale() const {
            return _axisScale;
        }
        inline const vec2& scale(const vec2& value) {
            _axisScale = value;
            _dirty = true;
            return _axisScale;
        }
        inline const vec2& scale(const float value) {
            _axisScale = vec2(value, value);
            _dirty = true;
            return _axisScale;
        }
        inline const vec2& rotation() const {
            return _axisRotation;
        }
        inline const vec2& rotation(const vec2& value) {
            _axisRotation = value;
            _dirty = true;
            return _axisRotation;
        }
        inline const vec2& rotation(const float value) {
            _axisRotation = vec2(value, value + (M_PI / 2.0f));
            _dirty = true;
            return _axisRotation;
        }
        inline const vec2& origin() const {
            return _origin;
        }
        inline const vec2& origin(const vec2& value) {
            _origin = value;
            _dirty = true;
            return _origin;
        }
        
        //operations
        transform_space& move_by(const vec2& m);
        transform_space& move_by(const float x, const float y);
        transform_space& rotate_by(const vec2& angles);
        transform_space& rotate_by(const float angle_x, const float angle_y);
        transform_space& rotate_by(const float angle);
        transform_space& scale_by(const vec2& s);
        transform_space& scale_by(const float sx, const float sy);
        transform_space& scale_by(const float s);
        //const operations
        transform_space moved(const vec2& m) const;
        transform_space moved(const float x, const float y) const;
        transform_space rotated(const vec2& angles) const;
        transform_space rotated(const float angle_x, const float angle_y) const;
        transform_space scaled(const vec2& s) const;
        transform_space scaled(const float sx, const float sy) const;
        transform_space scaled(const float s) const;
        //other operations
        transform_space inverse() const;
        transform_space& invert();
        transform_space canonical() const;
        transform_space& to_canonical();
        
        //vectors of mesh
        static buffer to_buffer(const std::vector<transform_space>& ts);
        static std::vector<transform_space> from_buffer(buffer b);
        
        //matrix operations
        inline __attribute__ ((always_inline)) const matrix3x3& from_space_to_base() const{
            if(_dirty)
                const_cast<transform_space*>(this)->update_matrix();
            return _matrix;
        }
        const matrix3x3& from_base_to_space() const;
        
        //possible transformations
        bool test_direction(transform_direction d) const;
        bool both_directions() const;
        
        //to string
        rb_string to_string() const;
        buffer to_buffer() const;
        
        //operators
        const transform_space& operator=(const transform_space& other);
        const transform_space& operator=(transform_space&& other);
        const transform_space& operator*=(const transform_space& other);
        
        //friend operators
        friend transform_space operator *(const transform_space &, const transform_space &);
        friend bool operator ==(const transform_space&, const transform_space&);
        friend bool operator!=(const transform_space&, const transform_space&);
    };
    
    transform_space operator *(const transform_space & first, const transform_space & second);
    bool operator ==(const transform_space& first, const transform_space& second);
    bool operator!=(const transform_space& first, const transform_space& second);
}

CLASS_NAME(rb::transform_space);

#endif /* defined(__RollerBallCore__transform_space__) */
