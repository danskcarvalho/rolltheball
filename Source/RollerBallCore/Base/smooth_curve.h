//
//  smooth_curve.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 08/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__smooth_curve__
#define __RollerBallCore__smooth_curve__

#include "base.h"
#include "vec2.h"

namespace rb {
    class polygon;
    class matrix3x3;
    class smooth_curve {
    private:
        std::vector<vec2> _cvs;
        std::vector<vec2> _cvs_c;
        std::vector<float> _knots;
        bool _is_closed;
        void update_knots();
        void update_internal_state();
    public:
        smooth_curve();
        
        //builder methods
        static smooth_curve& build_open_curve(const std::vector<vec2>& cvs, smooth_curve& storage);
        static smooth_curve& build_closed_curve(const std::vector<vec2>& cvs, smooth_curve& storage);
    
        //sampling
        vec2 sample_along_path(const float u) const;
    
        //modification and inspection
        int32_t cv_count() const;
        const vec2& get_cv(const int32_t at) const;
        smooth_curve& add_cv_after(const vec2& cv, const int32_t at);
        smooth_curve& add_cv_before(const vec2& cv, const int32_t at);
        smooth_curve& remove_cv(const int32_t at);
        smooth_curve& set_cv(const vec2& cv, int32_t at);
        bool is_closed() const;
        smooth_curve& close_curve();
        smooth_curve& open_curve();
        smooth_curve& reset();
        //to_string
        rb_string to_string() const;
    
        //to_polygon
        polygon to_polygon(polygon& storage, float quality = 0.1f, int32_t division_level = 5) const;
        
        friend bool operator ==(const smooth_curve&, const smooth_curve&);
        friend bool operator !=(const smooth_curve&, const smooth_curve&);
        
        friend class ::rb::matrix3x3;
    };
    
    bool operator ==(const smooth_curve& c1, const smooth_curve& c2);
    bool operator !=(const smooth_curve& c1, const smooth_curve& c2);
}

CLASS_NAME(rb::smooth_curve);

#endif /* defined(__RollerBallCore__smooth_curve__) */
