//
//  polygon_path.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 12/01/14.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__polygon_path__
#define __RollerBallCore__polygon_path__

#include "base.h"
#include "polygon.h"

namespace rb {
    class polygon_path {
    private:
        polygon _polygon;
        std::vector<float> _length;
        float _total_len;
        vec2 point_at(float l, size_t low_index, size_t hi_index) const;
    public:
        polygon_path();
        polygon_path(const polygon& p);
        const polygon& polygon() const;
        float length() const;
        vec2 point_at(float l, bool clamp) const;
        float length(const vec2& pt) const;
    };
}

#endif /* defined(__RollerBallCore__polygon_path__) */
