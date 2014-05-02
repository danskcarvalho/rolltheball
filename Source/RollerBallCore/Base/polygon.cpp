//
//  polygon.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 04/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "polygon.h"
#include "edge.h"
#include "ray.h"
#include "mesh.h"
#include "texture_map.h"
#include "vertex.h"
#include "transform_space.h"
#include "matrix3x3.h"
#include "null_texture_map.h"
#include "buffer.h"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <Triangulate.h>

using namespace std;
using namespace rb;

polygon::polygon(){
    _is_closed = false;
    _is_simple = nullptr;
    _is_convex = nullptr;
    _area = nullptr;
    _perimeter = nullptr;
    _bounds = nullptr;
}

void polygon::clear_cache() {
    _is_simple = nullptr;
    _is_convex = nullptr;
    _area = nullptr;
    _perimeter = nullptr;
    _bounds = nullptr;
    _edges.clear();
}

void polygon::optimize(){
    remove_duplicated_points();
    if(_points.size() <= 3)
        return;
    for (int32_t i = 0; i < _points.size(); ++i) {
        if(_points.size() <= 3)
            return;
        
        if((i == 0 || i == (_points.size() - 1)) && !_is_closed)
            continue;
        
        auto _previous = i == 0 ? _points[_points.size() - 1] : _points[i - 1];
        auto _next = i == (_points.size() - 1) ? _points[0] : _points[i + 1];
        if(_next == _previous)
            continue;
        
        auto _r = ray(_previous, _next - _previous);
        if(almost_equal(_r.distance(_points[i]), 0)){
            _points.erase(_points.begin() + i);
            i--; //we stay at the same position...
            continue;
        }
    }
}

void polygon::update_is_simple_flag(){
    if(_is_simple.has_value())
        return;
    
    if(_points.size() == 0){
        _is_simple = nullptr; //a empty polygon is not simple nor complex...
        return;
    }
    if(_points.size() == 1){
        _is_simple = 1; //a pointer is always simple...
        return;
    }
    if(_points.size() == 2){
        //a line is simple if the two points are different
        _is_simple = _points[0] != _points[1];
        return;
    }
    
    if(_is_closed){
        typedef boost::geometry::model::d2::point_xy<float> b_point;
        typedef boost::geometry::model::polygon<b_point, false> b_polygon;
        b_polygon _poly;
        vector<b_point> _b_points;
        vector<vec2> _c_points = _points;
        //we add the points
        if(get_ordering() == point_ordering::cw)
            std::reverse(_c_points.begin(), _c_points.end());
        for (auto& _p : _c_points)
            _b_points.push_back(b_point(_p.x(), _p.y()));
        
        //we initialize the polygon
        boost::geometry::append(_poly, _b_points);
        boost::geometry::correct(_poly);
        
        //we then compute the convex hull
        _is_simple = !boost::geometry::intersects(_poly);
    }
    else {
        typedef boost::geometry::model::d2::point_xy<float> b_point;
        typedef boost::geometry::model::polygon<b_point, false, false> b_polygon;
        b_polygon _poly;
        vector<b_point> _b_points;
        vector<vec2> _c_points = _points;
        //we add the points
        if(get_ordering() == point_ordering::cw)
            std::reverse(_c_points.begin(), _c_points.end());
        for (auto& _p : _c_points)
            _b_points.push_back(b_point(_p.x(), _p.y()));
        
        //we initialize the polygon
        boost::geometry::append(_poly, _b_points);
        boost::geometry::correct(_poly);
        
        //we then compute the convex hull
        _is_simple = !boost::geometry::intersects(_poly);
    }
}

bool polygon::test_intersection(const rb::polygon &other) const {
    auto _this = *this;
    _this.optimize();
    auto _other = other;
    _other.optimize();
    
    typedef boost::geometry::model::d2::point_xy<float> b_point;
    typedef boost::geometry::model::polygon<b_point, false> b_polygon;
    b_polygon _poly;
    b_polygon _poly2;
    vector<b_point> _b_points;
    vector<b_point> _b_points2;
    vector<vec2> _c_points = _this._points;
    vector<vec2> _c_points2 = _other._points;
    //we add the points
    if(_this.get_ordering() == point_ordering::cw)
        std::reverse(_c_points.begin(), _c_points.end());
    for (auto& _p : _c_points)
        _b_points.push_back(b_point(_p.x(), _p.y()));
    //we add the points 2
    if(_other.get_ordering() == point_ordering::cw)
        std::reverse(_c_points2.begin(), _c_points2.end());
    for (auto& _p : _c_points2)
        _b_points2.push_back(b_point(_p.x(), _p.y()));
    
    //we initialize the polygon
    boost::geometry::append(_poly, _b_points);
    boost::geometry::correct(_poly);
    //we initialize the polygon 2
    boost::geometry::append(_poly2, _b_points2);
    boost::geometry::correct(_poly2);
    
    //we then compute the convex hull
    return boost::geometry::intersects(_poly, _poly2);
}

bool polygon::test_intersection(const rb::vec2 &other) const {
    auto _this = *this;
    _this.optimize();
    
    typedef boost::geometry::model::d2::point_xy<float> b_point;
    typedef boost::geometry::model::polygon<b_point, false> b_polygon;
    b_polygon _poly;
    vector<b_point> _b_points;
    vector<vec2> _c_points = _this._points;
    //we add the points
    if(_this.get_ordering() == point_ordering::cw)
        std::reverse(_c_points.begin(), _c_points.end());
    for (auto& _p : _c_points)
        _b_points.push_back(b_point(_p.x(), _p.y()));
    
    //we initialize the polygon
    boost::geometry::append(_poly, _b_points);
    boost::geometry::correct(_poly);
    
    //we then compute the convex hull
    return boost::geometry::within(b_point(other.x(), other.y()), _poly);
}


rb_string polygon::to_string() const{
    return rb::to_string(_points);
}
vec2 polygon::get_normal(const vec2& pt0, const vec2& pt1){
    auto _ordering = get_ordering();
    
    if(_ordering == point_ordering::unknown)
        return vec2::zero; //at first, we don't throw a exception
    else if(_ordering == point_ordering::ccw){
        return (pt1 - pt0).rotated90(rotation_direction::cw).normalize();
    }
    else { //cw
        return (pt1 - pt0).rotated90(rotation_direction::ccw).normalize();
    }
}
const std::vector<edge>& polygon::get_edges(){
    if(_edges.size() != 0)
        return _edges;
    
    if(_points.size() <= 1)
        return _edges; //no edges...
    
    for (int i = 0; i < _points.size(); ++i) {
        if (i == (_points.size() - 1)){
            if(_is_closed)
                _edges.push_back(edge(_points[i], _points[0], get_normal(_points[i], _points[0])));
        }
        else
            _edges.push_back(edge(_points[i], _points[i + 1], get_normal(_points[i], _points[i + 1])));
    }
    
    return _edges;
}
void polygon::update_is_convex_flag(){
    int32_t _p = 0, _n = 0;
    
    if(_is_convex.has_value())
        return;
    
    if(_points.size() <= 2){
        _is_convex = nullptr; //don't know
        return;
    }
    
    if(_points.size() == 3){
        _is_convex = true; //a triangle is always convex but it may be degenerate...
        return;
    }
    
    auto& _edges = get_edges();
    
    for (int i = 0; i < _edges.size(); i++) {
        auto _e1 = _edges[i];
        auto _e2 = i == 0 ? _edges[_edges.size() - 1] : _edges[i - 1];
        
        auto _v1 = _e1.pt1() - _e1.pt0();
        auto _v2 = _e2.pt1() - _e2.pt0();
        
        auto _sign = vec2::cross(_v1, _v2);
        
        if(_sign > 0)
            _p++;
        else if(_sign < 0)
            _n++;
    }
    
    if(_p * _n == 0)
        _is_convex = true;
    else
        _is_convex = false;
}
void polygon::update_area(){
    if(_area.has_value())
        return;
    
    if(_points.size() == 0){
        _area = nullptr; //can't know;
        return;
    }
    
    if(_points.size() <= 2){
        _area = 0.0f;
        return;
    }
    
    float _sum = 0.0;
    
    for (int i = 0; i < _points.size(); ++i) {
        if(i == (_points.size() - 1))
            _sum += _points[i].x() * _points[0].y() - _points[0].x() * _points[i].y();
        else
            _sum += _points[i].x() * _points[i + 1].y() - _points[i + 1].x() * _points[i].y();
    }
    
    _area = 0.5f * _sum;
}
void polygon::update_perimeter(){
    if(_perimeter.has_value())
        return;
    
    if(_points.size() == 0)
    {
        _perimeter = nullptr;
        return;
    }
    
    auto& _edges = get_edges();
    
    float _sum = 0;
    
    for (auto& _e : _edges){
        _sum += _e.length();
    }
    
    _perimeter = _sum;
}
void polygon::update_bounds(){
    if(_bounds.has_value())
        return;
    
    if(_points.size() == 0)
        return; //no bounds
    
    vec2 _min(numeric_limits<float32>::max(), numeric_limits<float32>::max()),
        _max(numeric_limits<float32>::min(), numeric_limits<float32>::min());
    
    for (auto& _p : _points){
        _min.x(std::min(_p.x(), _min.x()));
        _min.y(std::min(_p.y(), _min.y()));
        
        _max.x(std::max(_p.x(), _max.x()));
        _max.y(std::max(_p.y(), _max.y()));
    }
    
    _bounds = rectangle(vec2((_min.x() + _max.x()) / 2.0f, (_min.y() + _max.y()) / 2.0f),
                        vec2((_max.x() - _min.x()),(_max.y() - _min.y())));
}

polygon& polygon::build_open_polygon(const vector<vec2>& points, polygon& storage){
    storage.reset();
    for (const vec2& _pt : points){
        storage._points.push_back(_pt);
    }
    storage.open_polygon();
    return storage;
}

polygon& polygon::build_closed_polygon(const vector<vec2>& points, polygon& storage){
    storage.reset();
    for (const vec2& _pt : points){
        storage._points.push_back(_pt);
    }
    storage.close_polygon();
    return storage;
}

polygon& polygon::reset(){
    _points.clear();
    clear_cache();
    return *this;
}
polygon& polygon::open_polygon(){
    clear_cache();
    _is_closed = false;
    return *this;
}
polygon& polygon::close_polygon(){
    clear_cache();
    _is_closed = true;
    return *this;
}
polygon& polygon::add_point_after(const vec2& point, int32_t at){
    clear_cache();
    assert(at >= 0);
    if(_points.size() != 0){
        assert(at < _points.size());
        _points.insert(_points.begin() + at + 1, point);
    }
    else {
        assert(at == 0);
        _points.push_back(point);
    }
    return *this;
}
polygon& polygon::add_point_before(const vec2& point, int32_t at){
    clear_cache();
    assert(at >= 0);
    if(_points.size() != 0){
        assert(at < _points.size());
        _points.insert(_points.begin() + at, point);
    }
    else {
        assert(at == 0);
        _points.push_back(point);
    }
    return *this;
}
polygon& polygon::remove_point(int at){
    clear_cache();
    assert(at >= 0);
    assert(at < _points.size());
    _points.erase(_points.begin() + at);
    return *this;
}
polygon& polygon::set_point(const vec2& point, int at){
    clear_cache();
    assert(at >= 0);
    assert(at < _points.size());
    _points[at] = point;
    return *this;
}
polygon& polygon::remove_duplicated_points(){
    if(_points.size() <= 1)
        return *this;
    for (int i = 0; i < _points.size(); ++i) {
        if(i == 0 && _is_closed){ //we can't do this if the polygon is open...
            if (_points[i] == _points[_points.size() - 1]){
                _points.erase(_points.begin()); //remove this element...
                i--; //do that so we stay in the same element in next iteration...
                continue;
            }
        }
        else if(i != 0){
            if(_points[i] == _points[i - 1]){
                _points.erase(_points.begin() + i);
                i--;
                continue;
            }
        }
    }
    clear_cache();
    return *this;
}
polygon& polygon::revert(){
    std::reverse(_points.begin(), _points.end());
    clear_cache();
    return *this;
}

bool polygon::is_empty() const{
    return _points.size() == 0;
}
nullable<bool> polygon::is_simple() const{
    const_cast<polygon*>(this)->update_is_simple_flag();
    return _is_simple;
}
nullable<bool> polygon::is_convex() const{
    const_cast<polygon*>(this)->update_is_convex_flag();
    return _is_convex;
}
bool polygon::is_closed() const{
    return _is_closed;
}
const vec2& polygon::get_point(uint32_t at) const{
    assert(at >= 0);
    assert(at < _points.size());
    return _points[at];
}
const uint32_t polygon::point_count() const{
    return (uint32_t)_points.size();
}
const uint32_t polygon::edge_count() const{
    const_cast<polygon*>(this)->get_edges(); //update edges
    return (uint32_t)_edges.size();
}
const edge& polygon::get_edge(uint32_t at) const{
    const_cast<polygon*>(this)->get_edges(); //update edges
    assert(at >= 0);
    assert(at < _edges.size());
    return _edges[at];
}
const nullable<float> polygon::signed_area() const{
    const_cast<polygon*>(this)->update_area();
    return _area;
}
const nullable<float> polygon::area() const{
    auto _s_a = signed_area();
    return _s_a.has_value() ? fabsf(_s_a.value()) : _s_a;
}
const nullable<float> polygon::perimeter() const{
    const_cast<polygon*>(this)->update_perimeter();
    return _perimeter;
}
vec2 polygon::sample_along_path(float length) const{
    //first of all, let's avoid cycling through the edges multiple times...
    assert(_points.size() != 0);
    if(_points.size() == 1)
        return _points[0];
    
    auto& _edges = const_cast<polygon*>(this)->get_edges();
    assert(perimeter().has_value());
    auto _perimeter = perimeter().value();
    
    assert(!almost_equal(_perimeter, 0));
    
    if(length > _perimeter)
        length -= floorf(length / _perimeter) * _perimeter;
    
    float _sum_lens = 0;
    for (auto& _e : _edges){
        auto _e_len = _e.length();
        if(length >= _sum_lens && length <= (_sum_lens + _e_len)){
            auto _p = (length - _sum_lens) / _e_len;
            return (1-_p)*_e.pt0() +  _p * _e.pt1();
        }
        _sum_lens += _e_len;
    }
    
    return _edges[_edges.size() - 1].pt1();
}
const nullable<rectangle> polygon::bounds() const{
    const_cast<polygon*>(this)->update_bounds();
    return _bounds;
}
const polygon& polygon::convex_hull(polygon& storage){
    assert(!is_empty());
    assert(is_simple().has_value() && is_simple().value() == true);
    assert(area().has_value());
    assert(is_closed());
    assert(get_ordering() != point_ordering::unknown);
    
    typedef boost::geometry::model::d2::point_xy<float> b_point;
    typedef boost::geometry::model::polygon<b_point, false> b_polygon;
    b_polygon _poly;
    b_polygon _convex_hull;
    vector<b_point> _b_points;
    vector<vec2> _c_points = _points;
    //we add the points
    if(get_ordering() == point_ordering::cw)
        std::reverse(_c_points.begin(), _c_points.end());
    for (auto& _p : _c_points)
        _b_points.push_back(b_point(_p.x(), _p.y()));
    
    //we initialize the polygon
    boost::geometry::append(_poly, _b_points);
    boost::geometry::correct(_poly);
    
    //we then compute the convex hull
    boost::geometry::convex_hull(_poly, _convex_hull);
    
    //we then set back the polygon
    storage.reset();
    //we then add the points
    vector<b_point> _out_points = _convex_hull.outer();
    for (auto& _p : _out_points)
        storage._points.push_back(vec2(_p.x(), _p.y()));
    //and close our polygon
    storage.close_polygon();
    storage.remove_duplicated_points();
    return storage;
}

const point_ordering polygon::get_ordering() const{
    if(_points.size() == 2 && !_is_closed)
        return point_ordering::ccw; //we arbitrarily choose ccw.
    
    auto _s_a = signed_area();
    if(!_s_a.has_value() || _s_a.value() == 0.0f)
        return point_ordering::unknown;
    return _s_a.value() > 0.0f ? point_ordering::ccw : point_ordering::cw;
}

nullable<vec2> intersect_line_with_line_segment(const ray& line, const ray& line_segment,
                                                const float line_segment_length){
    auto _i_up_01 = ray::intersection(line, line_segment);
    if(_i_up_01.has_value()){
        float _t = line_segment.get_parameter(_i_up_01.value());
        if(!(almost_less(_t, 0) || almost_greather(_t, line_segment_length)))
            return _i_up_01;
    }
    auto _i_up_02 = ray::intersection(ray(line.origin(), -line.direction()), line_segment);
    if(_i_up_02.has_value()){
        float _t = line_segment.get_parameter(_i_up_02.value());
        if(!(almost_less(_t, 0) || almost_greather(_t, line_segment_length)))
            return _i_up_02;
    }
    
    return nullptr;
}

bool is_in_front(const ray& r, const vec2& pt){
    vec2 _normal = r.direction().rotated90();
    vec2 _v = pt - r.origin();
    _v.project(_normal);
    auto _dot = vec2::dot(_v, _normal);
    if(_dot == 0)
        return false;
    return _dot > 0;
}

bool is_in_back(const ray& r, const vec2& pt){
    vec2 _normal = r.direction().rotated90();
    vec2 _v = pt - r.origin();
    _v.project(_normal);
    auto _dot = vec2::dot(_v, _normal);
    if(_dot == 0)
        return false;
    return _dot < 0;
}

nullable<vec2> intersection_in_edge(unordered_set<vec2> intersections, const vec2& v1, const vec2& v2){
    for(auto& _v : intersections){
        if(v1 == _v || v2 == _v)
            return _v;
        
        edge _e = edge(v1, v2, vec2::zero);
        if(almost_equal(_e.distance(_v), 0))
            return _v;
    }
    
    return nullptr;
}

void polygon::compute_halfs(const rectangle& box, const ray& sa, polygon& half_1, polygon& half_2){
    //we intersect ray sa with our horizontal and vertical walls
    ray _up = ray(box.top_left(), vec2::right);
    ray _bottom = ray(box.bottom_left(), vec2::right);
    ray _left = ray(box.top_left(), vec2::down);
    ray _right = ray(box.top_right(), vec2::down);
    
    //we compute intersections between the walls of the box
    auto _up_i = intersect_line_with_line_segment(sa, _up, box.size().x());
    auto _bottom_i = intersect_line_with_line_segment(sa, _bottom, box.size().x());
    auto _left_i = intersect_line_with_line_segment(sa, _left, box.size().y());
    auto _right_i = intersect_line_with_line_segment(sa, _right, box.size().y());
    
    //we now compute the points of the half
    unordered_set<vec2> _intersections;
    if(_up_i.has_value())
        _intersections.insert(_up_i.value());
    if(_bottom_i.has_value())
        _intersections.insert(_bottom_i.value());
    if(_left_i.has_value())
        _intersections.insert(_left_i.value());
    if(_right_i.has_value())
        _intersections.insert(_right_i.value());
    
    //all points on the intersection belongs to both halfs...
    //also we must have two points in the ordered set...
    if(_intersections.size() != 2)
        return;
    //half 01
    //we test the four points of box in ccw order...
    unordered_set<vec2> _i_c = _intersections;
    vector<vec2> _half01;
    if(is_in_front(sa, box.top_left()) && _intersections.count(box.top_left()) == 0 && std::count(_half01.begin(), _half01.end(), box.top_left()) == 0)
        _half01.push_back(box.top_left());
    {
        auto _some_i = intersection_in_edge(_i_c, box.top_left(), box.bottom_left());
        if(_some_i.has_value()){
            _i_c.erase(_some_i.value());
            _half01.push_back(_some_i.value());
        }
    }
    if(is_in_front(sa, box.bottom_left()) && _intersections.count(box.bottom_left()) == 0 && std::count(_half01.begin(), _half01.end(), box.bottom_left()) == 0)
        _half01.push_back(box.bottom_left());
    {
        auto _some_i = intersection_in_edge(_i_c, box.bottom_left(), box.bottom_right());
        if(_some_i.has_value()){
            _i_c.erase(_some_i.value());
            _half01.push_back(_some_i.value());
        }
    }
    if(is_in_front(sa, box.bottom_right()) && _intersections.count(box.bottom_right()) == 0 && std::count(_half01.begin(), _half01.end(), box.bottom_right()) == 0)
        _half01.push_back(box.bottom_right());
    {
        auto _some_i = intersection_in_edge(_i_c, box.bottom_right(), box.top_right());
        if(_some_i.has_value()){
            _i_c.erase(_some_i.value());
            _half01.push_back(_some_i.value());
        }
    }
    if(is_in_front(sa, box.top_right()) && _intersections.count(box.top_right()) == 0 && std::count(_half01.begin(), _half01.end(), box.top_right()) == 0)
        _half01.push_back(box.top_right());
    { //we always do those tests...
        auto _some_i = intersection_in_edge(_i_c, box.top_right(), box.top_left());
        if(_some_i.has_value()){
            _i_c.erase(_some_i.value());
            _half01.push_back(_some_i.value());
        }
    }
    
    half_1.reset();
    if(_half01.size() >= 3) //then we have this half
    {
        for (auto& _p : _half01)
            half_1._points.push_back(_p);
        half_1.close_polygon();
    }
    
    //half2
    _i_c = _intersections;
    vector<vec2> _half02;
    if(is_in_back(sa, box.top_left()) && _intersections.count(box.top_left()) == 0 && std::count(_half02.begin(), _half02.end(), box.top_left()) == 0)
        _half02.push_back(box.top_left());
    {
        auto _some_i = intersection_in_edge(_i_c, box.top_left(), box.bottom_left());
        if(_some_i.has_value()){
            _i_c.erase(_some_i.value());
            _half02.push_back(_some_i.value());
        }
    }
    if(is_in_back(sa, box.bottom_left()) && _intersections.count(box.bottom_left()) == 0 && std::count(_half02.begin(), _half02.end(), box.bottom_left()) == 0)
        _half02.push_back(box.bottom_left());
    {
        auto _some_i = intersection_in_edge(_i_c, box.bottom_left(), box.bottom_right());
        if(_some_i.has_value()){
            _i_c.erase(_some_i.value());
            _half02.push_back(_some_i.value());
        }
    }
    if(is_in_back(sa, box.bottom_right()) && _intersections.count(box.bottom_right()) == 0 && std::count(_half02.begin(), _half02.end(), box.bottom_right()) == 0)
        _half02.push_back(box.bottom_right());
    {
        auto _some_i = intersection_in_edge(_i_c, box.bottom_right(), box.top_right());
        if(_some_i.has_value()){
            _i_c.erase(_some_i.value());
            _half02.push_back(_some_i.value());
        }
    }
    if(is_in_back(sa, box.top_right()) && _intersections.count(box.top_right()) == 0 && std::count(_half02.begin(), _half02.end(), box.top_right()) == 0)
        _half02.push_back(box.top_right());
    { //we always do those tests...
        auto _some_i = intersection_in_edge(_i_c, box.top_right(), box.top_left());
        if(_some_i.has_value()){
            _i_c.erase(_some_i.value());
            _half02.push_back(_some_i.value());
        }
    }
    
    half_2.reset();
    if(_half02.size() >= 3) //then we have this half
    {
        for (auto& _p : _half02)
            half_2._points.push_back(_p);
        half_2.close_polygon();
    }
}

polygon& polygon::split(const ray& separating_axis, vector<polygon>& other_polygons){
    assert(!is_empty());
    assert(is_simple().has_value() && is_simple().value() == true);
    assert(area().has_value());
    assert(is_closed());
    assert(get_ordering() != point_ordering::unknown);
    update_bounds();
    assert(bounds().has_value());
    other_polygons.clear();
    //we first construct the two halfs of or box.
    polygon half_1, half_2;
    compute_halfs(rectangle(bounds().value().center(), vec2(bounds().value().size().x() + 2,
                                                            bounds().value().size().y() + 2)), separating_axis, half_1, half_2);
    if(half_1.is_empty() || half_2.is_empty()) //there isn't anything to separate...
        return *this;
    //we then intersect our polygon with each half...
    //we build boost polygon for our polygon and each half
    typedef boost::geometry::model::d2::point_xy<float> b_point;
    typedef boost::geometry::model::polygon<b_point, false> b_polygon;
    b_polygon _b_polygon;
    b_polygon _b_half_1;
    b_polygon _b_half_2;
    vector<b_point> _b_points_polygon;
    vector<b_point> _b_points_half_1;
    vector<b_point> _b_points_half_2;
    for (auto& _p : this->_points)
        _b_points_polygon.push_back(b_point(_p.x(), _p.y()));
    for (auto& _p : half_1._points)
        _b_points_half_1.push_back(b_point(_p.x(), _p.y()));
    for (auto& _p : half_2._points)
        _b_points_half_2.push_back(b_point(_p.x(), _p.y()));
    if(get_ordering() == point_ordering::cw)
        std::reverse(_b_points_polygon.begin(), _b_points_polygon.end());
    if(half_1.get_ordering() == point_ordering::cw)
        std::reverse(_b_points_half_1.begin(), _b_points_half_1.end());
    if(half_2.get_ordering() == point_ordering::cw)
        std::reverse(_b_points_half_2.begin(), _b_points_half_2.end());
    //we finally construct the polygons
    boost::geometry::append(_b_polygon, _b_points_polygon);
    boost::geometry::append(_b_half_1, _b_points_half_1);
    boost::geometry::append(_b_half_2, _b_points_half_2);
    boost::geometry::correct(_b_polygon);
    boost::geometry::correct(_b_half_1);
    boost::geometry::correct(_b_half_2);
    
    //the we test for intersection
    vector<b_polygon> _p_list01, _p_list02;
    boost::geometry::intersection(_b_polygon, _b_half_1, _p_list01);
    boost::geometry::intersection(_b_polygon, _b_half_2, _p_list02);
    //combines the two lists
    for(auto& _g : _p_list02)
        _p_list01.push_back(_g);
    
    if(_p_list01.size() == 0)
        return *this;
    
    //we set the first polygon to ourselves...
    reset();
    //we then add the points
    vector<b_point> _out_points = _p_list01[0].outer();
    for (auto& _p : _out_points)
        _points.push_back(vec2(_p.x(), _p.y()));
    //and close our polygon
    close_polygon();
    optimize();
    
    //we then fill the list passed with the others polygons
    for (auto it = _p_list01.begin() + 1; it != _p_list01.end(); ++it) {
        auto _g = *it;
        polygon _current_p;
        _current_p.reset();
        _out_points = _g.outer();
        for (auto& _p : _out_points)
            _current_p._points.push_back(vec2(_p.x(), _p.y()));
        //and close our polygon
        _current_p.close_polygon();
        _current_p.optimize();
        //we add to list
        other_polygons.push_back(_current_p);
    }
    
    return *this;
}

polygon& polygon::intersection(const polygon& other, std::vector<polygon>& other_polygons){
    assert(!is_empty());
    assert(is_simple().has_value() && is_simple().value() == true);
    assert(area().has_value());
    //assert(is_closed());
    assert(get_ordering() != point_ordering::unknown);
    
    other_polygons.clear();
    
    typedef boost::geometry::model::d2::point_xy<float> b_point;
    typedef boost::geometry::model::polygon<b_point, false> b_polygon;
    b_polygon _poly01;
    b_polygon _poly02;
    b_polygon _result;
    vector<b_point> _b_points01;
    vector<b_point> _b_points02;
    vector<vec2> _c_points01 = _points;
    vector<vec2> _c_points02 = other._points;
    //we add the points to _poly01
    if(get_ordering() == point_ordering::cw)
        std::reverse(_c_points01.begin(), _c_points01.end());
    for (auto& _p : _c_points01)
        _b_points01.push_back(b_point(_p.x(), _p.y()));
    //we initialize the polygon
    boost::geometry::append(_poly01, _b_points01);
    
    //we add the points to _poly02
    if(other.get_ordering() == point_ordering::cw)
        std::reverse(_c_points02.begin(), _c_points02.end());
    for (auto& _p : _c_points02)
        _b_points02.push_back(b_point(_p.x(), _p.y()));
    //we initialize the polygon
    boost::geometry::append(_poly02, _b_points02);
    
    boost::geometry::correct(_poly01);
    boost::geometry::correct(_poly02);
    
    //we then compute the union
    std::vector<b_polygon> _result_list;
    boost::geometry::intersection(_poly01, _poly02, _result_list);
    
    if(_result_list.size() == 0){
        reset();
        return *this;
    }
    
    _result = _result_list[0];
    
    //we then set back our polygon
    reset();
    //we then add the points
    vector<b_point> _out_points = _result.outer();
    for (auto& _p : _out_points)
        _points.push_back(vec2(_p.x(), _p.y()));
    //and close our polygon
    close_polygon();
    optimize();
    
    //we create other polygons for the result
    for (uint32_t i = 1; i < _result_list.size(); i++) {
        polygon _new_polygon;
        auto _result = _result_list[i];
        
        _new_polygon.reset();
        _out_points = _result_list[i].outer();
        for (auto& _p : _out_points)
            _new_polygon._points.push_back(vec2(_p.x(), _p.y()));
        //and close our polygon
        _new_polygon.close_polygon();
        _new_polygon.optimize();
        //add to the vector
        other_polygons.push_back(_new_polygon);
    }
    
    return *this;
}

polygon& polygon::difference(const rb::polygon &other){
    assert(!is_empty());
    assert(is_simple().has_value() && is_simple().value() == true);
    assert(area().has_value());
    assert(is_closed());
    assert(get_ordering() != point_ordering::unknown);
    
    typedef boost::geometry::model::d2::point_xy<float> b_point;
    typedef boost::geometry::model::polygon<b_point, false> b_polygon;
    b_polygon _poly01;
    b_polygon _poly02;
    b_polygon _result;
    vector<b_point> _b_points01;
    vector<b_point> _b_points02;
    vector<vec2> _c_points01 = _points;
    vector<vec2> _c_points02 = other._points;
    //we add the points to _poly01
    if(get_ordering() == point_ordering::cw)
        std::reverse(_c_points01.begin(), _c_points01.end());
    for (auto& _p : _c_points01)
        _b_points01.push_back(b_point(_p.x(), _p.y()));
    //we initialize the polygon
    boost::geometry::append(_poly01, _b_points01);
    
    //we add the points to _poly02
    if(other.get_ordering() == point_ordering::cw)
        std::reverse(_c_points02.begin(), _c_points02.end());
    for (auto& _p : _c_points02)
        _b_points02.push_back(b_point(_p.x(), _p.y()));
    //we initialize the polygon
    boost::geometry::append(_poly02, _b_points02);
    
    boost::geometry::correct(_poly01);
    boost::geometry::correct(_poly02);
    
    //we then compute the union
    std::vector<b_polygon> _result_list;
    boost::geometry::difference(_poly01, _poly02, _result_list);
    
    if(_result_list.size() != 1)
        return *this; //we return without modifying this polygon
    
    _result = _result_list[0];
    
    //we then set back our polygon
    reset();
    //we then add the points
    vector<b_point> _out_points = _result.outer();
    for (auto& _p : _out_points)
        _points.push_back(vec2(_p.x(), _p.y()));
    //and close our polygon
    close_polygon();
    optimize();
    return *this;
}

polygon& polygon::join(const polygon& other){
    assert(!is_empty());
    assert(is_simple().has_value() && is_simple().value() == true);
    assert(area().has_value());
    assert(is_closed());
    assert(get_ordering() != point_ordering::unknown);
    
    typedef boost::geometry::model::d2::point_xy<float> b_point;
    typedef boost::geometry::model::polygon<b_point, false> b_polygon;
    b_polygon _poly01;
    b_polygon _poly02;
    b_polygon _result;
    vector<b_point> _b_points01;
    vector<b_point> _b_points02;
    vector<vec2> _c_points01 = _points;
    vector<vec2> _c_points02 = other._points;
    //we add the points to _poly01
    if(get_ordering() == point_ordering::cw)
        std::reverse(_c_points01.begin(), _c_points01.end());
    for (auto& _p : _c_points01)
        _b_points01.push_back(b_point(_p.x(), _p.y()));
    //we initialize the polygon
    boost::geometry::append(_poly01, _b_points01);
    
    //we add the points to _poly02
    if(other.get_ordering() == point_ordering::cw)
        std::reverse(_c_points02.begin(), _c_points02.end());
    for (auto& _p : _c_points02)
        _b_points02.push_back(b_point(_p.x(), _p.y()));
    //we initialize the polygon
    boost::geometry::append(_poly02, _b_points02);
    
    boost::geometry::correct(_poly01);
    boost::geometry::correct(_poly02);
    
    //we then compute the union
    std::vector<b_polygon> _result_list;
    boost::geometry::union_(_poly01, _poly02, _result_list);
    
    if(_result_list.size() != 1)
        return *this; //we return without modifying this polygon
    
    _result = _result_list[0];
    
    //we then set back our polygon
    reset();
    //we then add the points
    vector<b_point> _out_points = _result.outer();
    for (auto& _p : _out_points)
        _points.push_back(vec2(_p.x(), _p.y()));
    //and close our polygon
    close_polygon();
    optimize();
    return *this;
}

vec2 compute_normal(const vec2& v1, const vec2& v2, point_ordering ordering){
    auto _normal = v2 - v1;
    _normal.normalize();
    if(ordering == point_ordering::cw)
        _normal.rotate90(rotation_direction::ccw);
    else
        _normal.rotate90(rotation_direction::cw);
    return _normal;
}

void join_edges(const float stroke_width, point_ordering ordering, const edge& e1, const edge& e2, const corner_type ct, vector<edge>& edges){
    edges.clear();
    auto _pt_int = edge::intersection(e1, e2);
    
    if(_pt_int.has_value() && _pt_int.value().is_valid()){
        edges.push_back(edge(e1.pt0(), _pt_int.value(), e1.normal()));
        edges.push_back(edge(_pt_int.value(), e2.pt1(), e2.normal()));
    }
    else {
        if(edge::test_intersection(e1, e2) || e1.pt1() == e2.pt0()) { //line segment
            auto _mid = (e1.pt0() + e2.pt1()) / 2.0f;
            edges.push_back(edge(e1.pt0(), _mid, e1.normal()));
            edges.push_back(edge(_mid, e2.pt1(), e1.normal())); //just so we have 2 edges or more always
        }
        else { //no intersection, we must join then anyway
            auto r1 = ray(e1.pt1(), e1.pt1() - e1.pt0());
            auto r2 = ray(e2.pt0(), e2.pt0() - e2.pt1());
            
            if(ct == corner_type::miter){
                auto _pt_int_2 = ray::intersection(r1, r2);
                if(!_pt_int_2.has_value()){
                    //force bevel... the polygon won't be simple anymore...
                    auto _normal =  compute_normal(e1.pt1(), e2.pt0(), ordering);
                    edges.push_back(e1);
                    edges.push_back(edge(e1.pt1(), e2.pt0(), _normal));
                    edges.push_back(e2);
                    return;
                }
                //the miter limit is constant...
                if(vec2::distance(_pt_int_2.value(), r1.origin()) <= (1.5f * stroke_width) &&
                   _pt_int_2.value().is_valid()) {
                    edges.push_back(edge(e1.pt0(), _pt_int_2.value(), e1.normal()));
                    edges.push_back(edge(_pt_int_2.value(), e2.pt1(), e2.normal()));
                }
                else {
                    auto _e1_pt1_ex = r1.sample(1.5f * stroke_width);
                    auto _e2_pt0_ex = r2.sample(1.5f * stroke_width);
                    auto _e1_e2_pt_ex = (_e1_pt1_ex + _e2_pt0_ex) / 2.0f;
                    edges.push_back(edge(e1.pt0(), _e1_e2_pt_ex, e1.normal()));
                    //auto _normal = compute_normal(_e1_pt1_ex, _e2_pt0_ex, ordering);
                    //edges.push_back(edge(_e1_pt1_ex, _e2_pt0_ex, _normal));
                    edges.push_back(edge(_e1_e2_pt_ex, e2.pt1(), e2.normal()));
                }
            }
            else {
                auto _normal =  compute_normal(e1.pt1(), e2.pt0(), ordering);
                edges.push_back(e1);
                edges.push_back(edge(e1.pt1(), e2.pt0(), _normal));
                edges.push_back(e2);
            }
        }
    }
}

template <class iterator>
iterator increment(iterator it, int n) {
    if(n == 0)
        return it;
    
    if(n > 0){
        for (int i = 0; i < n; i++) {
            it++;
        }
    }
    else {
        for (int i = 0; i < abs(n); i++) {
            it--;
        }
    }
    
    return it;
}

polygon extrude_edge(const edge& e, const float stroke_width, border_placement bd_place, const polygon& shell){
    edge _en;
    edge _enn;
    if(bd_place == border_placement::middle){
        _en = e.translate(e.normal() * (stroke_width / 2.0), false);
        _enn = e.translate(-e.normal() * (stroke_width / 2.0), true);
    }
    else if(bd_place == border_placement::outside){
        _en = e.translate(e.normal() * stroke_width, false);
        _enn = e.translate(e.normal() * 0, true);
    }
    else {
        _en = e.translate(e.normal() * 0, false);
        _enn = e.translate(-e.normal() * (stroke_width / 1.0), true);
    }
    std::vector<vec2> _pts;
    _pts.push_back(_en.pt0());
    _pts.push_back(_en.pt1());
    _pts.push_back(_enn.pt1());
    _pts.push_back(_enn.pt0());
    polygon _result;
    polygon::build_closed_polygon(_pts, _result);
    if(shell.area().has_value() && !almost_equal(shell.area().value(), 0)){
        if(bd_place == border_placement::inside){
            std::vector<polygon> _others;
            _result.intersection(shell, _others);
        }
        else if(bd_place == border_placement::outside){
            _result.difference(shell);
        }
    }
    return _result;
}

mesh& polygon::to_outline_mesh(rb::mesh &storage, const texture_map& map, const float stroke_width, const corner_type ct, const bool textureless, border_placement bd_place){
    assert(stroke_width > 0);
    assert(!is_empty());
    assert(perimeter().has_value() && perimeter().value() != 0);
    optimize();
    auto& _edges = get_edges();
    auto _ordering = get_ordering();
    std::vector<polygon> _polygons;
    std::vector<vec2> _temp_points;
    
    for (int i = 0; i < _edges.size(); i++) {
        auto _current = _edges[i];
        _polygons.push_back(extrude_edge(_current, stroke_width, bd_place, *this));
        if(i == 0 && !is_closed())
            continue;
        auto _previous = i == 0 ? _edges[_edges.size() - 1] : _edges[i - 1];
        auto _v1 = _current.pt1() - _current.pt0();
        auto _v2 = _previous.pt0() - _previous.pt1();
        float _int_angle = 0;
        float _ext_angle = 0;
        if(_ordering == point_ordering::ccw){
            _int_angle = _v1.angle_between(_v2, rotation_direction::ccw);
            _ext_angle = _v1.angle_between(_v2, rotation_direction::cw);
        }
        else {
            _int_angle = _v1.angle_between(_v2, rotation_direction::cw);
            _ext_angle = _v1.angle_between(_v2, rotation_direction::ccw);
        }
        //we need to find what corner we need to miter or bevel...
        if(almost_equal(_int_angle, M_PI) || almost_equal(_int_angle, 0)) //no need to miter or bevel...
            continue;
        vec2 _n1;
        vec2 _n2;
        bool _internal_mitter = false;
        if(_int_angle > _ext_angle) //we need to adjust the corner in the direction of -normal
        {
            _n1 = -_current.normal();
            _n2 = -_previous.normal();
            _internal_mitter = true;
        }
        else { //we adjust in the direction of normal...
            _n1 = _current.normal();
            _n2 = _previous.normal();
        }
        float _angle = _n1.angle_between(_n2, rotation_direction::shortest);
        bool _exceeded_miter = _angle >= TO_RADIANS(MITER_LIMIT);
        nullable<vec2> _miter_pt = nullptr;
        
        auto _stroke_divisor = bd_place == border_placement::middle ? 2.0 : 1.0;
        if(ct == corner_type::miter && !_exceeded_miter){
            ray _r1 = ray(_current.pt0() + _n1 * (stroke_width / _stroke_divisor), -_v1);
            ray _r2 = ray(_previous.pt1()  + _n2 * (stroke_width / _stroke_divisor), -_v2);
            _miter_pt = ray::intersection(_r1, _r2);
            if(_miter_pt.has_value() && !_miter_pt.value().is_valid())
                _miter_pt = nullptr;
        }
        
        //corner polygon
        polygon _corner;
        _temp_points.clear();
        auto _nm = (_n1 + _n2) / 2.0;
        _nm = -_nm;
        if(bd_place == border_placement::middle)
            _temp_points.push_back(_current.pt0() + _nm * (stroke_width / 3.0)); //the _nm * (stroke_width / 3.0) is to avoid precision issues...
        else
            _temp_points.push_back(_current.pt0());
        
        _temp_points.push_back(_current.pt0() + _n1 * (stroke_width / _stroke_divisor));
        if(_miter_pt.has_value())
            _temp_points.push_back(_miter_pt.value());
        
        _temp_points.push_back(_previous.pt1()  + _n2 * (stroke_width / _stroke_divisor));
        _corner = polygon::build_closed_polygon(_temp_points, _corner);
        if(bd_place == border_placement::middle)
            _polygons.push_back(_corner);
        else {
            if(bd_place == border_placement::inside){
                if(_internal_mitter)
                    _polygons.push_back(_corner);
            }
            else {
                if(!_internal_mitter)
                    _polygons.push_back(_corner);
            }
        }
    }
    
    std::vector<mesh*> _meshes;
    std::vector<vertex> _vertexes;
    std::vector<uint16_t> _indexes;
    
    for(auto& _p : _polygons){
        if(!textureless){
            mesh* _m = new mesh();
            _p.to_mesh(*_m, 0, map);
            if(!_m->is_empty())
                _meshes.push_back(_m);
        }
        else
            _p._to_mesh(_vertexes, _indexes, 0, map);
    }
    
    if(!textureless){
        mesh* _merged = mesh::merge_meshes(_meshes);
        for (auto _m : _meshes)
            delete _m;
        if(_merged){
            storage = std::move(*_merged);
            delete _merged;
        }
        return storage;
    }
    else {
        vertex* _vb = (vertex*)malloc(sizeof(vertex) * _vertexes.size());
        uint16_t* _ib = (uint16_t*)malloc(sizeof(uint16_t) * _indexes.size());
        
        for (auto i = 0; i < _vertexes.size(); i++) {
            _vb[i] = _vertexes[i];
        }
        
        for (auto i = 0; i < _indexes.size(); i++) {
            _ib[i] = _indexes[i];
        }
        
        storage.set_buffers(_vb, (uint32_t)_vertexes.size(), _ib, (uint32_t)_indexes.size(), true);
        return storage;
    }
}

polygon& polygon::offset(const float strength) {
    assert(strength >= 0);
    assert(!is_empty());
    assert(is_simple().has_value() && is_simple().value() == true);
    assert(area().has_value());
    assert(is_convex().has_value() && is_convex().value() == true);
    assert(is_closed());
    assert(get_ordering() != point_ordering::unknown);
    list<edge> _top_edges;
    auto& _edges = const_cast<polygon*>(this)->get_edges();
    vector<edge> _joined;
    auto _ordering = get_ordering();
    
    if(strength == 0)
        return *this;
    
    for (auto& _e : _edges){
        _top_edges.push_back(_e.translate(_e.normal() * strength, false));
        
        if(_top_edges.size() >= 2) { //we need to join with the previous edge
            join_edges(strength, _ordering,
                       *increment(_top_edges.end(), -2), _top_edges.back(),
                       corner_type::miter, _joined);
            _top_edges.pop_back();
            _top_edges.pop_back(); //remove the last two
            for (auto& _j : _joined){ //add all joined
                _top_edges.push_back(_j);
            }
        }
    }
    
    //do the same with first and last segments if the polygon is closed...
    //if(is_closed()){
    //top edge...
    join_edges(strength, _ordering, _top_edges.back(), _top_edges.front(), corner_type::miter, _joined);
    _top_edges.pop_front();
    _top_edges.pop_back(); //remove the first and the last
    
    for (auto& _j : _joined){ //add all joined
        _top_edges.push_back(_j);
    }
    //}
    
    //now we need to add 2 more edges to close the polygon and join the top and bottom edges in just one...
    list<edge> _final_edges;
    //insert the top edges
    for (auto& _e : _top_edges)
        _final_edges.push_back(_e);
    
    //we now have a closed polygon representing our outline...
    //we reset the storage
    reset();
    //we then add the points
    for (auto& _e : _final_edges)
        _points.push_back(_e.pt0());
    //and close our polygon
    close_polygon();
    return *this;
}


const edge polygon::closest_edge(const vec2& pt, uint32_t& index) const{
    assert(!is_empty());
//    assert(is_simple());
    auto& _edges = const_cast<polygon*>(this)->get_edges();
    nullable<edge> _best_edge = nullptr;
    float _last_distance = numeric_limits<float>::max();
    index = 0;
    uint32_t _i = 0;
    for (auto& _e : _edges){
        auto _distance_vector = _e.distance_vector(pt);
        auto _distance_length = _distance_vector.length();
        if(_distance_length < _last_distance){
            _last_distance = _distance_length;
            _best_edge = _e;
            index = _i;
        }
        else if(almost_equal(_distance_length, _last_distance) && _best_edge.has_value()){
            //we get the edge that aligns best with the normal of the edge and the distance vector
            auto _last_d_v = _best_edge.value().distance_vector(pt).normalized();
            auto _d_v = _distance_vector.normalized();
            if ( vec2::dot(_d_v, _e.normal()) > vec2::dot(_last_d_v, _best_edge.value().normal())){
                _last_distance = _distance_length;
                _best_edge = _e;
                index = _i;
            }
        }
        _i++;
    }
    assert(_best_edge.has_value());
    return _best_edge.value();
}

bool rb::operator ==(const polygon& p1, const polygon& p2){
    //two polygons are equal if they contain the same set of pointers in the same order and are both closed or open...
    if(p1._is_closed != p2._is_closed)
        return false;
    
    if(p1._points.size() != p2._points.size())
        return false;
    
    for (int i = 0; i < p1._points.size(); i++) {
        if(p1._points[i] != p2._points[i])
            return false;
    }
    
    return true;
}
bool rb::operator !=(const polygon& p1, const polygon& p2){
    if(p1._is_closed != p2._is_closed)
        return true;
    
    if(p1._points.size() != p2._points.size())
        return true;
    
    for (int i = 0; i < p1._points.size(); i++) {
        if(p1._points[i] != p2._points[i])
            return true;
    }
    
    return false;
}

void subdivide_triangle(uint32_t subdv, vector<vec2>& points, const vec2& v1, const vec2& v2, const vec2& v3){
    if(subdv <= 0){
        points.push_back(v1);
        points.push_back(v2);
        points.push_back(v3);
        return;
    }
    
    //we subdivide...
    vec2 v4 = vec2::lerp(0.5f, v1, v2);
    vec2 v5 = vec2::lerp(0.5f, v4, v3);
    
    //we have the following triangles...
    //[v1 v5 v4] [v1 v3 v5] [v2 v4 v5] [v2 v3 v5]
    subdivide_triangle(subdv - 1, points, v4, v5, v1);
    subdivide_triangle(subdv - 1, points, v5, v3, v1);
    subdivide_triangle(subdv - 1, points, v2, v5, v4);
    subdivide_triangle(subdv - 1, points, v2, v3, v5);
}

mesh& polygon::to_untextured_mesh(rb::mesh &storage, const uint32_t subdivisions){
    optimize();
    assert(!is_empty());
    assert(is_simple().has_value() && is_simple().value() == true);
    assert(area().has_value());
    assert(get_ordering() != point_ordering::unknown);
    assert(subdivisions >= 0);
    vector<vertex> _vertexes;
    vector<uint16_t> _indexes;
    this->_to_mesh(_vertexes, _indexes, subdivisions, null_texture_map());
    return storage;
}

mesh& polygon::to_mesh(mesh& storage, const uint32_t subdivisions, const texture_map& map){
    optimize();
    if(_points.size() <= 2)
        return storage;
    assert(!is_empty());
    assert(is_simple().has_value() && is_simple().value() == true);
    assert(area().has_value());
    assert(get_ordering() != point_ordering::unknown);
    assert(subdivisions >= 0);
    
    //we construct a subdivided quad-mesh...
    //first we need to find the bounds in the texture space...
    float   _min_x = numeric_limits<float>::max(),
            _min_y = numeric_limits<float>::max(),
            _max_x = numeric_limits<float>::min(),
            _max_y = numeric_limits<float>::min();
    
    for(auto& _p : _points){
        auto _p_in_tex_space = map.get_texture_space().from_base_to_space().transformed_point(_p);
        if(_p_in_tex_space.x() < _min_x)
            _min_x = _p_in_tex_space.x();
        
        if(_p_in_tex_space.y() < _min_y)
            _min_y = _p_in_tex_space.y();
        
        if(_p_in_tex_space.x() > _max_x)
            _max_x = _p_in_tex_space.x();
        
        if(_p_in_tex_space.y() > _max_y)
            _max_y = _p_in_tex_space.y();
    }
    
    //now, we're gonna expand the maxes and mins to a integer boundary...
    _max_x = ceilf(_max_x);
    _max_y = ceilf(_max_y);
    _min_x = floorf(_min_x);
    _min_y = floorf(_min_y);
    
    //now we subdivide in 1x1 sized blocks and intersect with our polygon...
    auto _n_hor = _max_x - _min_x;
    auto _n_ver = _max_y - _min_y;
    
    polygon _rect;
    polygon _me;
    vector<polygon> _others;
    vector<vertex> _vertexes;
    vector<uint16_t> _indexes;
    
//    int _pol_count = 0;
    
    for (float j = 0; j < _n_ver; j++) {
        for (float i = 0; i < _n_hor; i++) {
            float _cur_left = _min_x + i;
            float _cur_right = _min_x + i + 1;
            float _cur_bottom = _min_y + j;
            float _cur_top = _min_y + j + 1;
            
            vec2 _cur_bottom_left = vec2(_cur_left, _cur_bottom);
            vec2 _cur_bottom_right = vec2(_cur_right, _cur_bottom);
            vec2 _cur_top_left = vec2(_cur_left, _cur_top);
            vec2 _cur_top_right = vec2(_cur_right, _cur_top);
            
            map.get_texture_space().from_space_to_base().transform_point(_cur_bottom_left);
            map.get_texture_space().from_space_to_base().transform_point(_cur_bottom_right);
            map.get_texture_space().from_space_to_base().transform_point(_cur_top_left);
            map.get_texture_space().from_space_to_base().transform_point(_cur_top_right);
            
            _rect.reset();
            _rect.add_point_after(_cur_bottom_left, 0);
            _rect.add_point_after(_cur_bottom_right, 0);
            _rect.add_point_after(_cur_top_right, 1);
            _rect.add_point_after(_cur_top_left, 2);
            _rect.close_polygon();
            
            _me = *this;
            
            _me.intersection(_rect, _others);
            
            _others.push_back(_me);
            
            //we have all our polygons
            for(auto& _pol : _others){
//                if(_pol.is_empty() || !_pol.area().has_value() || almost_equal(_pol.area().value(), 0))
//                    continue;
                
                if(_pol.is_empty() || !_pol.area().has_value())
                    continue;
                
//                if(_pol_count != 1){
//                    _pol_count++;
//                    continue;
//                }
                
                _pol._to_mesh(_vertexes, _indexes, subdivisions, map);
                
//                _pol_count++;
            }
        }
    }
    
    auto _max_vectors = numeric_limits<uint16_t>::max();
    if (_vertexes.size() > _max_vectors) {
        WARN("mesh is too big...");
        return storage;
    }
    
    vertex* _vb = (vertex*)malloc(sizeof(vertex) * _vertexes.size());
    uint16_t* _ib = (uint16_t*)malloc(sizeof(uint16_t) * _indexes.size());
    
    for (auto i = 0; i < _vertexes.size(); i++) {
        _vb[i] = _vertexes[i];
    }
    
    for (auto i = 0; i < _indexes.size(); i++) {
        _ib[i] = _indexes[i];
    }
    
    storage.set_buffers(_vb, (uint32_t)_vertexes.size(), _ib, (uint32_t)_indexes.size(), true);
    return storage;
}

float fract(const float value){
    auto _value = value;
    if(almost_equal(value, roundf(value)))
       _value = roundf(value); //we do this to avoid numerical errors...
    auto _f_value = floorf(_value);
    return _value - _f_value;
}

float wrap(const float bound_coord, const float bound_size, const float v1, const float v2, const float v3){
    auto _v1 = fract(v1);
    if(almost_equal(_v1, 0)) //we need to decide if 0 or 1
    {
        if(almost_greather(v2, v1) || almost_greather(v3, v1))
            _v1 = 0;
        else
            _v1 = 1;
    }
    
    return bound_coord + bound_size * _v1;
}

vec2 wrap(const rectangle& bounds, const vec2& v1, const vec2& v2, const vec2& v3){
    //wrap s
    auto _s = wrap(bounds.bottom_left().x(), bounds.size().x(), v1.x(), v2.x(), v3.x());
    //wrap t
    auto _t = wrap(bounds.bottom_left().y(), bounds.size().y(), v1.y(), v2.y(), v3.y());
    return vec2(_s, _t);
}

bool in_boundary(const rectangle& bounds, const vec2& v){
    edge e1(bounds.bottom_left(), bounds.bottom_right(), vec2::zero);
    edge e2(bounds.top_left(), bounds.top_right(), vec2::zero);
    edge e3(bounds.bottom_left(), bounds.top_left(), vec2::zero);
    edge e4(bounds.bottom_right(), bounds.top_right(), vec2::zero);
    
    if(almost_equal(e1.distance(v), 0))
        return true;
    
    if(almost_equal(e2.distance(v), 0))
        return true;
    
    if(almost_equal(e3.distance(v), 0))
        return true;
    
    if(almost_equal(e4.distance(v), 0))
        return true;
    
    return false;
}

void make_vertex(vertex& vt, const vec2& pos, const vec2& tx, const rectangle& bounds){
    vt.set_position(pos);
    vt.set_texture_coords(tx);
    vt.a = 1;
    vt.blend = 1;
    vt.set_texture_bounds(bounds);
    vt.set_color(color::from_rgba(1, 1, 1, 1));
}

void polygon::_to_mesh(vector<vertex>& vertexes, vector<uint16_t>& indexes, const uint32_t subdivisions, const texture_map& map){
    auto _subdv = subdivisions;
    
    vector<Vector2d> _v2d_input;
    vector<Vector2d> _v2d_result;
    vector<vec2> _v_buffer;
    if(get_ordering() == point_ordering::cw){
        for(auto it = _points.rbegin(); it != _points.rend(); it++){
            auto& v = *it;
            _v2d_input.push_back(Vector2d(v.x(), v.y()));
        }
    }
    else {
        for(auto it = _points.begin(); it != _points.end(); it++){
            auto& v = *it;
            _v2d_input.push_back(Vector2d(v.x(), v.y()));
        }
    }
    Triangulate::Process(_v2d_input, _v2d_result);
    vector<uint16_t> _indexes;
    std::map<std::pair<vec2, vec2>, uint16_t> _pos_tx_map;
    auto _n_triangles = _v2d_result.size() / 3;
    polygon _triangle;
    
    for (int32_t i = 0; i < _n_triangles; i++) {
        const Vector2d &_v2d_1 = _v2d_result[i*3+0];
        const Vector2d &_v2d_2 = _v2d_result[i*3+1];
        const Vector2d &_v2d_3 = _v2d_result[i*3+2];
        
        auto _v1 = vec2(_v2d_1.GetX(), _v2d_1.GetY());
        auto _v2 = vec2(_v2d_2.GetX(), _v2d_2.GetY());
        auto _v3 = vec2(_v2d_3.GetX(), _v2d_3.GetY());
        
        //we test if the points are ccw
        _v_buffer.clear();
        _v_buffer.push_back(_v1);
        _v_buffer.push_back(_v2);
        _v_buffer.push_back(_v3);
        build_closed_polygon(_v_buffer, _triangle);
        _v_buffer.clear();
        if(_triangle.get_ordering() == point_ordering::ccw) //is ccw?
            subdivide_triangle(_subdv, _v_buffer, _v1, _v2, _v3); //we subdivide...
        else //is cw!
            subdivide_triangle(_subdv, _v_buffer, _v3, _v2, _v1); //we subdivide...
        
        //populate the vertex buffers
        auto _n_sub_triangles = _v_buffer.size() / 3;
        
        for (uint32_t j = 0; j < _n_sub_triangles; j++) {
            auto& _v1 = _v_buffer[j * 3 + 0];
            auto& _v2 = _v_buffer[j * 3 + 1];
            auto& _v3 = _v_buffer[j * 3 + 2];
            
            vertex _vtx1, _vtx2, _vtx3;
            make_vertex(_vtx1, _v1, vec2::zero, rectangle::unit);
            map.set_texture_coords(_vtx1);
            
            make_vertex(_vtx2, _v2, vec2::zero, rectangle::unit);
            map.set_texture_coords(_vtx2);
            
            make_vertex(_vtx3, _v3, vec2::zero, rectangle::unit);
            map.set_texture_coords(_vtx3);
            
            auto _t1 = _vtx1.get_texture_coords();
            auto _t2 = _vtx2.get_texture_coords();
            auto _t3 = _vtx3.get_texture_coords();
            _vtx1.set_texture_coords(wrap(map.bounds(), _t1, _t2, _t3));
            _vtx2.set_texture_coords(wrap(map.bounds(), _t2, _t1, _t3));
            _vtx3.set_texture_coords(wrap(map.bounds(), _t3, _t2, _t1));
            
            auto _ptt1 = std::make_pair(_vtx1.get_position(), _vtx1.get_texture_coords());
            auto _ptt2 = std::make_pair(_vtx2.get_position(), _vtx2.get_texture_coords());
            auto _ptt3 = std::make_pair(_vtx3.get_position(), _vtx3.get_texture_coords());
            
            if(_pos_tx_map.count(_ptt1) != 0){
                _indexes.push_back(_pos_tx_map[_ptt1]);
            }
            else {
                vertexes.push_back(_vtx1);
                _indexes.push_back(vertexes.size() - 1);
                _pos_tx_map.insert({_ptt1, vertexes.size() - 1});
            }
            
            if(_pos_tx_map.count(_ptt2) != 0){
                _indexes.push_back(_pos_tx_map[_ptt2]);
            }
            else {
                vertexes.push_back(_vtx2);
                _indexes.push_back(vertexes.size() - 1);
                _pos_tx_map.insert({_ptt2, vertexes.size() - 1});
            }
            
            if(_pos_tx_map.count(_ptt3) != 0){
                _indexes.push_back(_pos_tx_map[_ptt3]);
            }
            else {
                vertexes.push_back(_vtx3);
                _indexes.push_back(vertexes.size() - 1);
                _pos_tx_map.insert({_ptt3, vertexes.size() - 1});
            }
        }
    }
    
    auto _max_vectors = numeric_limits<uint16_t>::max();
    if(vertexes.size() > _max_vectors){
        WARN("mesh is too big...");
        return;
    }
    
    for (int32_t i = 0; i < _indexes.size(); ++i) {
        indexes.push_back(_indexes[i]);
    }
}

void divide_at_integer_boundaries(const edge& e, const float start, const float end, std::vector<edge>& edges){
    auto _len = e.length();
    auto _t_len = end - start;
//    if(_t_len <= 1)
//    {
//        edges.push_back(e);
//        return;
//    }
    
    auto _start = floorf(start);
    
    auto _last = start;
    auto _current = _start + 1;
    auto _last_pt = e.pt0();
    auto _e_vec = (e.pt1() - e.pt0()).normalize();
    
    while(true){
        auto _t_current_len = _current - _last;
        auto _t_prop = _t_current_len / _t_len;
        auto _current_len = _len * _t_prop;
        edges.push_back(edge(_last_pt, _last_pt + _e_vec * _current_len, e.normal()));
        
        if(almost_greather_or_equal(_current, end))
            break;
        
        _last = _current;
        _current = _last + 1;
        
        if(_current > end)
            _current = end;
        _last_pt = edges.back().pt1();
    }
}

void add_outline_section(const vector<vec2>& upper_points, const vector<vec2>& bottom_points, const rectangle& bounds, const float max_s, const float perimeter, const size_t position,  vector<uint16_t>& neighbours, vector<vertex>& vertexes, vector<uint16_t>& indexes, float& sum_distance){
    auto p = position;
    if (position != (upper_points.size() - 1)){
        auto delta_distance = vec2::distance(upper_points[p], upper_points[p + 1]);
        vertex v1, v2, v3;
        auto t1 = vec2((sum_distance / perimeter) * max_s, 1);
        auto t2 = vec2(t1.x(), 0);
        auto t3 = vec2(((sum_distance + delta_distance) / perimeter) * max_s, 0);
        auto tt1 = wrap(bounds, t1, t2, t3);
        auto tt2 = wrap(bounds, t2, t1, t3);
        auto tt3 = wrap(bounds, t3, t2, t1);
        make_vertex(v1, upper_points[p], tt1, bounds);
        make_vertex(v2, bottom_points[p], tt2, bounds);
        make_vertex(v3, bottom_points[p + 1], tt3, bounds);
        
        vertex v4, v5, v6;
        auto t4 = vec2((sum_distance / perimeter) * max_s, 1);
        auto t5 = vec2(((sum_distance + delta_distance) / perimeter) * max_s, 0);
        auto t6 = vec2(t5.x(), 1);
        auto tt4 = wrap(bounds, t4, t5, t6);
        auto tt5 = wrap(bounds, t5, t4, t6);
        auto tt6 = wrap(bounds, t6, t5, t4);
        make_vertex(v4, upper_points[p], tt4, bounds);
        make_vertex(v5, bottom_points[p + 1], tt5, bounds);
        make_vertex(v6, upper_points[p + 1], tt6, bounds);
        
        auto i1 = 0, i3 = 0, i5 = 0, i6 = 0;
        
        if(neighbours.size() != 0){
            if(v1.get_texture_coords() == vertexes[neighbours[0]].get_texture_coords()){
                indexes.push_back(neighbours[0]);
                i1 = neighbours[0];
            }
            else {
                vertexes.push_back(v1);
                indexes.push_back(vertexes.size() - 1);
                i1 = (uint32_t)vertexes.size() - 1;
            }
            
            if(v2.get_texture_coords() == vertexes[neighbours[1]].get_texture_coords()){
                indexes.push_back(neighbours[1]);
            }
            else {
                vertexes.push_back(v2);
                indexes.push_back(vertexes.size() - 1);
            }
        }
        else {
            vertexes.push_back(v1);
            indexes.push_back(vertexes.size() - 1);
            i1 = (uint32_t)vertexes.size() - 1;
            
            vertexes.push_back(v2);
            indexes.push_back(vertexes.size() - 1);
        }
        
        vertexes.push_back(v3);
        indexes.push_back(vertexes.size() - 1);
        i3 = (uint32_t)vertexes.size() - 1;
        
        if(v4.get_texture_coords() == v1.get_texture_coords()) {
            indexes.push_back(i1);
        }
        else {
            vertexes.push_back(v4);
            indexes.push_back(vertexes.size() - 1);
        }
        
        if(v5.get_texture_coords() == v3.get_texture_coords()) {
            indexes.push_back(i3);
            i5 = i3;
        }
        else {
            vertexes.push_back(v5);
            indexes.push_back(vertexes.size() - 1);
            i5 = (uint32_t)vertexes.size() - 1;
        }
        
        vertexes.push_back(v6);
        indexes.push_back(vertexes.size() - 1);
        i6 = (uint32_t)vertexes.size() - 1;
        
        neighbours.clear();
        neighbours.push_back(i6);
        neighbours.push_back(i5);
        sum_distance += delta_distance;
    }
    else {
        return; //we don't add data past the end point
    }
}

mesh& polygon::textured_outline(mesh& storage, const rectangle& texture_bounds, const float max_s, const float stroke_width){
    optimize();
    assert(!is_empty());
    assert(is_simple().has_value() && is_simple().value() == true);
    assert(area().has_value());
    assert(get_ordering() != point_ordering::unknown);
    assert(stroke_width > 0);
    
    vector<edge> _upper_edges;
    vector<edge> _bottom_edges;
    auto _ordering = get_ordering();
    vector<edge> _joined;
    
    auto& _edges = get_edges();
    
    for(int32_t i = 0; i < _edges.size(); i++){
        _bottom_edges.push_back(_edges[i]);
        
        if(i == 0){
            _upper_edges.push_back(_edges[i].translate(_edges[i].normal() * stroke_width, false));
        }
        else {
            auto _previous = _upper_edges.back();
            _upper_edges.pop_back();
            
            //join edges
            auto _current = _edges[i].translate(_edges[i].normal() * stroke_width, false);
            join_edges(stroke_width, _ordering, _previous, _current, corner_type::miter, _joined);
            assert(_joined.size() == 2 || _joined.size() == 3);
            if(_joined.size() == 3){
                auto _welded =  (_previous.pt1() + _current.pt0()) / 2.0f;
                _upper_edges.push_back(edge(_previous.pt0(), _welded, compute_normal(_previous.pt0(), _welded, _ordering)));
                _upper_edges.push_back(edge(_welded, _current.pt1(), compute_normal(_welded, _current.pt1(), _ordering)));
            }
            else {
                for(auto& _e : _joined)
                    _upper_edges.push_back(_e);
            }
        }
    }
    
    if(is_closed()) //connect first and last edges
    {
        auto _previous = _upper_edges.back();
        _upper_edges.pop_back();
        auto _current = _upper_edges.front();
        _upper_edges.erase(_upper_edges.begin());
        
        //join edges
        join_edges(stroke_width, _ordering, _previous, _current, corner_type::miter, _joined);
        if(_joined.size() == 3){
            auto _welded =  (_previous.pt1() + _current.pt0()) / 2.0f;
            _upper_edges.push_back(edge(_previous.pt0(), _welded, compute_normal(_previous.pt0(), _welded, _ordering)));
            _upper_edges.push_back(edge(_welded, _current.pt1(), compute_normal(_welded, _current.pt1(), _ordering)));
        }
        else {
            for(auto& _e : _joined)
                _upper_edges.push_back(_e);
        }
        
        //we do the same with the bottom edges to mantain symmetry
        _current = _bottom_edges.front();
        _bottom_edges.erase(_bottom_edges.begin());
        _bottom_edges.push_back(_current);
    }
    
    float _distance  = 0;
    float _perimeter = 0;
    
    for(auto& _e : _upper_edges)
        _perimeter += _e.length();
    
    //we subdividide at integer boundaries...
    assert(_upper_edges.size() == _bottom_edges.size());
    vector<edge> _temp_upper_edges;
    vector<edge> _temp_bottom_edges;
    for(int32_t i = 0; i < _upper_edges.size(); i++){
        float _t_start = (_distance / _perimeter) * max_s;
        _distance += _upper_edges[i].length();
        float _t_end = (_distance / _perimeter) * max_s;
        divide_at_integer_boundaries(_upper_edges[i], _t_start, _t_end, _temp_upper_edges);
        divide_at_integer_boundaries(_bottom_edges[i], _t_start, _t_end, _temp_bottom_edges);
    }
    
    _upper_edges = std::move(_temp_upper_edges);
    _bottom_edges = std::move(_temp_bottom_edges);
    _distance = 0;
    
    vector<vec2> _upper_points;
    vector<vec2> _bottom_points;
    vector<uint16_t> _neighbours;
    vector<vertex> _vertexes;
    vector<uint16_t> _indexes;
    
    for(auto& _e : _upper_edges)
        _upper_points.push_back(_e.pt0());
    _upper_points.push_back(_upper_edges.back().pt1());
    
    for(auto& _e : _bottom_edges)
        _bottom_points.push_back(_e.pt0());
    _bottom_points.push_back(_bottom_edges.back().pt1());
    
    for (auto i = 0; i < _upper_points.size(); i++) {
        add_outline_section(_upper_points, _bottom_points, texture_bounds, max_s, _perimeter, i, _neighbours, _vertexes, _indexes, _distance);
//        if(i >= 0)
//            break;
    }
    
    if(_vertexes.size() > numeric_limits<uint16_t>::max()){
        WARN("mesh is too big...");
        return storage;
    }
    
    //we now construct our buffers
    vertex* _vb = (vertex*)malloc(sizeof(vertex) * _vertexes.size());
    uint16_t* _ib = (uint16_t*)malloc(sizeof(uint16_t) * _indexes.size());
    
    for(int32_t i = 0; i < _vertexes.size(); ++i){
        _vb[i] = _vertexes[i];
    }
    
    for (int32_t i = 0; i < _indexes.size(); i++) {
        _ib[i] = _indexes[i];
    }
    
    storage.set_buffers(_vb, (uint32_t)_vertexes.size(), _ib, (uint32_t)_indexes.size(), true);
    
    return storage;
}

mesh& polygon::to_line_mesh(mesh& storage, const rectangle& texture_bounds, const float max_s){
    optimize();
    assert(!is_empty());
    assert(perimeter().has_value());
    assert(get_ordering() != point_ordering::unknown);
    auto& _edges = get_edges();
    
    uint32_t _pt_count = (uint32_t)(is_closed() ? _points.size() + 1 : _points.size());
    uint32_t _idx_count = (uint32_t)(2 * _edges.size());
    //we construct our buffers
    vertex* _vb = (vertex*)malloc(sizeof(vertex) * _pt_count);
    uint16_t* _ib = (uint16_t*)malloc(sizeof(uint16_t) * _idx_count);
    float _distance_sum = 0;
    float _perimeter = perimeter().value();
    
    //set vertex buffer
    for(int32_t i = 0; i < _pt_count; ++i){
        if(i != (_pt_count - 1)) {
            _vb[i].set_position(_points[i]);
            _vb[i].a = 1; //100% opaque
            _vb[i].blend = 1;
            _vb[i].set_color(color::from_rgba(1, 1, 1, 1)); //opaque white
            _vb[i].set_texture_bounds(texture_bounds);
            vec2 _tex_coords(0, 0);
            _tex_coords.x((_distance_sum / _perimeter) * max_s);
            _vb[i].set_texture_coords(_tex_coords);
            _distance_sum += vec2::distance(_points[i], _points[i + 1]);
        }
        else {
            _vb[i].set_position(_points[0]);
            _vb[i].a = 1; //100% opaque
            _vb[i].blend = 1;
            _vb[i].set_color(color::from_rgba(1, 1, 1, 1)); //opaque white
            _vb[i].set_texture_bounds(texture_bounds);
            vec2 _tex_coords(max_s, 0);
            _vb[i].set_texture_coords(_tex_coords);
        }
    }
    
    //set index buffer
    auto _v_sum = 0;
    for(int32_t i = 0; i < _edges.size(); ++i){
        _ib[i * 2 + 0] = _v_sum++;
        _ib[i * 2 + 1] = _v_sum;
    }
    
    storage.set_buffers(_vb, _pt_count, _ib, _idx_count, true);
    
    return storage;
}

buffer polygon::to_buffer() const {
    auto _mem_size = sizeof(uint32_t) + sizeof(uint32_t) + sizeof(vec2) * point_count();
    void * _mem = malloc(_mem_size);
    uint32_t* _bMem = (uint32_t*)_mem;
    *_bMem = _is_closed;
    _bMem++;
    uint32_t* _ui32Mem = (uint32_t*)_bMem;
    *_ui32Mem = point_count();
    _ui32Mem++;
    vec2* _vMem = (vec2*)_ui32Mem;
    for(uint32_t i = 0; i < point_count(); i++){
        _vMem[i] = get_point(i);
    }
    
    buffer _b(_mem, _mem_size);
    free(_mem);
    return _b;
}

polygon::polygon(const buffer b){
    const uint32_t* bMem = (const uint32_t*)b.internal_buffer();
    bool _closed = *bMem;
    bMem++;
    const uint32_t* ui32Mem = (const uint32_t*)bMem;
    auto _s = *ui32Mem;
    ui32Mem++;
    const vec2* vMem = (const vec2*)ui32Mem;
    this->reset();
    for (uint32_t i = 0; i < _s; i++)
        this->_points.push_back(vMem[i]);
    if(_closed)
        this->close_polygon();
    else
        this->open_polygon();
}
































