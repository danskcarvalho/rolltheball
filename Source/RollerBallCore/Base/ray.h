//
//  ray.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 04/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__ray__
#define __RollerBallCore__ray__

#include "base.h"
#include "vec2.h"

namespace rb {
    class ray {
    private:
        vec2 _origin;
        vec2 _direction;
    public:
        ray();
        ray(const vec2& origin, const vec2& direction);
        
        //constants
        static const ray up;
        static const ray down;
        static const ray left;
        static const ray right;
        
        //properties
        inline const vec2& origin() const{
            return _origin;
        }
        inline const vec2& origin(const vec2& value) {
            _origin = value;
            return _origin;
        }
        inline const vec2& direction() const {
            return _direction;
        }
        inline const vec2& direction(const vec2& value) {
            assert(!value.is_zero());
            if(!value.is_unit())
                _direction = value.normalized();
            else
                _direction = value;
            return _direction;
        }
        
        //tests
        bool is_parallel_to(const ray& r) const;
        bool is_collinear_to(const ray& r) const;
        bool is_perpendicular_to(const ray& r) const;
        bool contains_point(const vec2& pt) const;
        bool is_in_line(const vec2& pt) const;
        
        //methods
        vec2 sample(const float t) const;
        //get the parameter such as sample(get_parameter(pt)) == pt
        float get_parameter(const vec2& pt) const;
        float distance(const vec2& pt) const;
        
        //operations
        ray rotated90(const rotation_direction rd = rotation_direction::ccw) const;
        ray& rotate90(const rotation_direction rd = rotation_direction::ccw);
        ray rotated(const float angle) const;
        ray& rotate(const float angle);
        static nullable<vec2> intersection(const ray& r1, const ray& r2);
        
        //to_string
        rb_string to_string() const;
        
        friend bool operator ==(const ray&, const ray&);
        friend bool operator !=(const ray&, const ray&);
    };
    
    bool operator ==(const ray& r1, const ray& r2);
    bool operator !=(const ray& r1, const ray& r2);
}

CLASS_NAME(rb::ray);

#endif /* defined(__RollerBallCore__ray__) */
