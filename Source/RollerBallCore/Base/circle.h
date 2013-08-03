//
//  circle.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 04/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__circle__
#define __RollerBallCore__circle__

#include "base.h"
#include "vec2.h"

namespace rb {
    class polygon;
    class circle {
    private:
        vec2 _origin;
        float _radius;
    public:
        //constructors
        circle();
        circle(const vec2& origin, const float radius);
        
        //constants
        static const circle unit;
        
        //properties
        inline const vec2& origin() const {
            return _origin;
        }
        
        inline const vec2& origin(const vec2& value){
            _origin = value;
            return _origin;
        }
        
        inline float radius() const {
            return _radius;
        }
        
        inline float radius(const float value) {
            assert(value > 0);
            _radius = value;
            return _radius;
        }
        
        //tests
        bool contains_point(const vec2& pt) const;
        
        //methods
        float distance(const vec2& pt) const;
        static float distance(const circle& c1, const circle& c2);
        
        //operations
        circle& move_by(const vec2& v);
        circle& move_by(const float x, const float y);
        
        //to_string
        rb_string to_string() const;
        polygon& to_polygon(polygon& storage, int32_t quality = 16) const;
        
        //friend operators
        friend bool operator ==(const circle&, const circle&);
        friend bool operator !=(const circle&, const circle&);
    };
    
    bool operator ==(const circle& c1, const circle& c2);
    bool operator !=(const circle& c1, const circle& c2);
}

CLASS_NAME(rb::circle);

#endif /* defined(__RollerBallCore__circle__) */
