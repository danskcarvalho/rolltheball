//
//  polygon.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 04/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__polygon__
#define __RollerBallCore__polygon__

#include "base.h"
#include "vec2.h"
#include "rectangle.h"
#include "vertex.h"
#include "edge.h"

#define MITER_LIMIT 165

namespace rb {
    class ray;
    class edge;
    class matrix3x3;
    class mesh;
    class texture_map;
    class buffer;
    class polygon {
    private:
        std::vector<vec2> _points;
        std::vector<edge> _edges;
        bool _is_closed;
        nullable<bool> _is_simple;
        nullable<bool> _is_convex;
        nullable<float> _area;
        nullable<float> _perimeter;
        nullable<rectangle> _bounds;
        //private methods
        void update_is_simple_flag();
        void update_is_convex_flag();
        void update_area();
        void update_perimeter();
        void update_bounds();
        void clear_cache();
        vec2 get_normal(const vec2& pt0, const vec2& pt1);
        const std::vector<edge>& get_edges();
        static void compute_halfs(const rectangle& box, const ray& sa, polygon& half_1, polygon& half_2);
        polygon& private_outline(polygon& connector, const float stroke_width, const corner_type ct);
        void _to_mesh(std::vector<vertex>& vertexes, std::vector<uint16_t>& indexes, const uint32_t subdivisions, const texture_map& map);
    public:
        polygon();
        
        //building polygons
        static polygon& build_open_polygon(const std::vector<vec2>& points, polygon& storage);
        static polygon& build_closed_polygon(const std::vector<vec2>& points, polygon& storage);
        
        //modifying polygon
        polygon& reset();
        polygon& open_polygon();
        polygon& close_polygon();
        polygon& add_point_after(const vec2& point, int at);
        polygon& add_point_before(const vec2& point, int at);
        polygon& remove_point(int at);
        polygon& set_point(const vec2& point, int at);
        polygon& remove_duplicated_points();
        polygon& revert();
        void optimize();
        
        //inspect polygon
        bool is_empty() const;
        nullable<bool> is_simple() const;
        nullable<bool> is_convex() const;
        bool is_closed() const;
        const vec2& get_point(uint32_t at) const;
        const uint32_t point_count() const;
        const uint32_t edge_count() const;
        const edge& get_edge(uint32_t at) const;
        const nullable<float> signed_area() const;
        const nullable<float> area() const;
        const nullable<float> perimeter() const;
        vec2 sample_along_path(float length) const;
        const nullable<rectangle> bounds() const;
        const polygon& convex_hull(polygon& storage);
        const point_ordering get_ordering() const;
        
        //other operations
        polygon& split(const ray& separating_axis, std::vector<polygon>& other_polygons);
        polygon& join(const polygon& other);
        polygon& intersection(const polygon& other, std::vector<polygon>& other_polygons);
        bool test_intersection(const polygon& other) const;
        bool test_intersection(const vec2& other) const;
        mesh& to_outline_mesh(mesh& storage, const texture_map& map, const float stroke_width, const corner_type ct = corner_type::miter, const bool textureless = false);
        polygon& offset(const float strength);
        const edge closest_edge(const vec2& pt) const;
        rb_string to_string() const;
        
        //conversion to mesh
        mesh& to_line_mesh(mesh& storage, const rectangle& texture_bounds, const float max_s);
        mesh& to_mesh(mesh& storage, const uint32_t subdivisions, const texture_map& map);
        mesh& to_untextured_mesh(mesh& storage, const uint32_t subdivisions);
        mesh& textured_outline(mesh& storage, const rectangle& texture_bounds, const float max_s, const float stroke_width);
        
        //serialization
        buffer to_buffer() const;
        polygon(const buffer b);
        
        //operators
        friend bool operator ==(const polygon&, const polygon&);
        friend bool operator !=(const polygon&, const polygon&);
        
        friend class ::rb::matrix3x3;
    };
    
    bool operator ==(const polygon& p1, const polygon& p2);
    bool operator !=(const polygon& p1, const polygon& p2);
}

CLASS_NAME(rb::polygon);

#endif /* defined(__RollerBallCore__polygon__) */
