//
//  edge.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 04/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__edge__
#define __RollerBallCore__edge__

#include "base.h"
#include "vec2.h"

namespace rb {
    class edge {
    private:
        vec2 _pt0;
        vec2 _pt1;
        vec2 _normal;
    public:
        edge();
        edge(const vec2& pt0, const vec2& pt1, const vec2& normal);
        
        //properties
        inline const vec2& pt0() const {
            return _pt0;
        }
        
        inline const vec2& pt1() const {
            return _pt1;
        }
        inline const vec2& normal() const {
            return _normal;
        }
        
        //other methods
        float length() const;
        float distance(const vec2& pt) const;
        bool is_degenerate() const;
        edge translate(const vec2& m, bool flip_normal) const;
        static nullable<vec2> intersection(const edge& e1, const edge& e2);
        static bool test_intersection(const edge& e1, const edge& e2);
        
        //to_string
        rb_string to_string() const;
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
        //operators
        friend bool operator ==(const edge&, const edge&);
        friend bool operator !=(const edge&, const edge&);
    };
    
    bool operator ==(const edge& e1, const edge& e2);
    bool operator !=(const edge& e1, const edge& e2);
}

DEFINE_HASH(rb::edge);
CLASS_NAME(rb::edge);

namespace std {
    template<>
    struct equal_to<rb::edge> {
        bool operator()( const rb::edge& lhs, const rb::edge& rhs ) const{
            return lhs.pt0().x() == rhs.pt0().x() && lhs.pt0().y() == rhs.pt0().y() &&
                   lhs.pt1().x() == rhs.pt1().x() && lhs.pt1().y() == rhs.pt1().y() &&
                   lhs.normal().x() == rhs.normal().x() && lhs.normal().y() == rhs.normal().y();
        }
    };
}

#endif /* defined(__RollerBallCore__edge__) */
