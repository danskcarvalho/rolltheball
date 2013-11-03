//
//  rectangle.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 04/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__rectangle__
#define __RollerBallCore__rectangle__

#include "base.h"
#include "vec2.h"
#include "math.h"

namespace rb {
    class polygon;
    class mesh;
    class rectangle {
    private:
        vec2 _center;
        //extents of the rectangle. this is the full width and full height of the rectangle...
        vec2 _size;
    public:
        rectangle();
        rectangle(const vec2& center, const vec2& size);
        rectangle(const float center_x, const float center_y, const float width, const float height);
        
        //constants
        static const rectangle unit;
            
        //properties
        inline const vec2& center() const {
            return _center;
        }
        inline const vec2& center(const vec2& value) {
            _center = value;
            return _center;
        }
        inline const vec2& size() const {
            return _size;
        }
        inline const vec2& size(const vec2& value) {
            assert(almost_greather_or_equal(value.x(), 0));
            assert(almost_greather_or_equal(value.y(), 0));
            _size = value;
            return _size;
        }
        inline vec2 top_left() const {
            return _center + vec2(-_size.x() / 2, _size.y() / 2);
        }
        inline vec2 top_right() const {
            return _center + vec2(_size.x() / 2, _size.y() / 2);
        }
        inline vec2 bottom_left() const {
            return _center + vec2(-_size.x() / 2, -_size.y() / 2);
        }
        inline vec2 bottom_right() const {
            return _center + vec2(_size.x() / 2, -_size.y() / 2);
        }

        //methods
        bool is_null() const;
        float area() const;
        rectangle& move_by(const vec2& v);
        polygon& to_polygon(polygon& storage) const;
        mesh& to_mesh(mesh& storage, const rectangle& texture_bounds, const float max_s = 1, const float max_t = 1, const uint32_t subdivisions = 0);
        std::vector<mesh*> to_meshes(const rectangle& texture_bounds, const uint32_t h_subdivs, const uint32_t v_subdivs);
        bool intersects(const vec2& pt) const;
        bool intersects(const rectangle& another) const;
        bool contains(const rectangle& another) const;
        rectangle compute_intersection(const rectangle& another) const;
        rectangle compute_union(const rectangle& another) const;
        template<class Iterator>
        static rectangle bounding(Iterator begin, Iterator end) {
            float   _min_x = std::numeric_limits<float>::max(),
                    _max_x = std::numeric_limits<float>::min(),
                    _min_y = std::numeric_limits<float>::max(),
                    _max_y = std::numeric_limits<float>::min();
            for(auto _it = begin; _it != end; _it++){
                _min_x = std::min(_min_x, _it->x());
                _max_x = std::max(_max_x, _it->x());
                _min_y = std::min(_min_y, _it->y());
                _max_y = std::max(_max_y, _it->y());
            }
            
            return rectangle((_min_x + _max_x) / 2.0, (_max_y + _min_y) / 2.0, (_max_x - _min_x), (_max_y - _min_y));
        }

        //convert to string
        rb_string to_string() const;

        friend bool operator ==(const rectangle&, const rectangle&);
        friend bool operator!=(const rectangle&, const rectangle&);
    };

    bool operator ==(const rectangle& first, const rectangle& second);
    bool operator!=(const rectangle& first, const rectangle& second);
}

CLASS_NAME(rb::rectangle);

#endif /* defined(__RollerBallCore__rectangle__) */
