//
//  PolygonTests.m
//  RollerBallCore
//
//  Created by Danilo Carvalho on 07/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "PolygonTests.h"
#include "test_macros.h"
#include "edge.h"
#include "polygon.h"
#include "matrix3x3.h"
#include "ray.h"
#include "null_texture_map.h"
#include "mesh.h"

using namespace rb;
using namespace std;

@implementation PolygonTests
-(void)testRemoveDuplicatedPoints{
    polygon ps;
    vector<vec2> points = {vec2(0, 0), vec2(0, 0), vec2(0, 1), vec2(0, 2), vec2(0, 2), vec2(0, 0)};
    auto p = polygon::build_closed_polygon(points, ps);
    p.remove_duplicated_points();
    TEST_ASSERT_TRUE(p.point_count() == 3);
    TEST_ASSERT_TRUE(p.get_point(0) == vec2(0, 1));
    TEST_ASSERT_TRUE(p.get_point(1) == vec2(0, 2));
    TEST_ASSERT_TRUE(p.get_point(2) == vec2(0, 0));
}
-(void)testOrdering{
    polygon ps;
    vector<vec2> points {{0, 0}, {0, 1}, {1, 1}, {1, 0}};
    ps = polygon::build_closed_polygon(points, ps);
    TEST_ASSERT_TRUE(ps.get_ordering() == point_ordering::cw);
    ps.revert();
    TEST_ASSERT_TRUE(ps.get_ordering() == point_ordering::ccw);
}
-(void)testArea{
    polygon ps;
    vector<vec2> points {{0, 0}, {0, 1}, {1, 1}, {1, 0}};
    ps = polygon::build_closed_polygon(points, ps);
    TEST_ASSERT_TRUE(ps.area().value() == 1);
    TEST_ASSERT_TRUE(ps.signed_area().value() == -1);
    TEST_ASSERT_TRUE(ps.bounds().value().size().x() == 1);
    TEST_ASSERT_TRUE(ps.bounds().value().size().y() == 1);
    TEST_ASSERT_TRUE(ps.bounds().value().center().x() == 0.5);
    TEST_ASSERT_TRUE(ps.bounds().value().center().y() == 0.5);
}
-(void)testPerimeter{
    polygon ps;
    vector<vec2> points {{0, 0}, {0, 1}, {1, 1}, {1, 0}};
    ps = polygon::build_closed_polygon(points, ps);
    TEST_ASSERT_TRUE(ps.perimeter().value() == 4);
    TEST_ASSERT_TRUE(ps.sample_along_path(0.5) == vec2(0, 0.5));
    TEST_ASSERT_TRUE(ps.sample_along_path(1) == vec2(0, 1));
    TEST_ASSERT_TRUE(ps.sample_along_path(1.5) == vec2(0.5, 1));
    TEST_ASSERT_TRUE(ps.sample_along_path(4) == vec2(0, 0));
    TEST_ASSERT_TRUE(ps.sample_along_path(4.5) == vec2(0, 0.5));
    ps = polygon::build_open_polygon(points, ps);
    TEST_ASSERT_TRUE(ps.perimeter().value() == 3);
    TEST_ASSERT_TRUE(ps.sample_along_path(0.5) == vec2(0, 0.5));
    TEST_ASSERT_TRUE(ps.sample_along_path(1) == vec2(0, 1));
    TEST_ASSERT_TRUE(ps.sample_along_path(1.5) == vec2(0.5, 1));
    TEST_ASSERT_TRUE(ps.sample_along_path(3) == vec2(1, 0));
}
-(void)testConvex{
    polygon ps;
    vector<vec2> ps1 {{0, 0}, {0, 1}, {1, 1}, {1, 0}};
    vector<vec2> ps2 {{0, 0}, {0, 1}, {0.5, 1}, {1, 1}, {1, 0}};
    vector<vec2> ps3 {{0, 0}, {0, 1}, {0.5, 2}, {1, 1}, {1, 0}};
    vector<vec2> ps4 {{0, 0}, {0, 1}, {0.5, 2}, {1, 1}, {2, 0.5}, {1, 0}};
    vector<vec2> ps5 {{0, 0}, {0, 1}, {0.5, 0.5}, {1, 1}, {2, 0.5}, {1, 0}};
    ps = polygon::build_closed_polygon(ps1, ps);
    TEST_ASSERT_TRUE(ps.is_convex().value());
    ps = polygon::build_closed_polygon(ps2, ps);
    TEST_ASSERT_TRUE(ps.is_convex().value());
    ps = polygon::build_closed_polygon(ps3, ps);
    TEST_ASSERT_TRUE(ps.is_convex().value());
    ps = polygon::build_closed_polygon(ps4, ps);
    TEST_ASSERT_TRUE(!ps.is_convex().value());
    ps = polygon::build_closed_polygon(ps5, ps);
    TEST_ASSERT_TRUE(!ps.is_convex().value());
}
-(void)testSimple{
    polygon ps;
    vector<vec2> points {{0, 0}, {0, 1}, {1, 1}, {1, 0}};
    ps = polygon::build_closed_polygon(points, ps);
    TEST_ASSERT_TRUE(ps.is_simple().value());
        
    points = {{0, 0}, {0, 1}, {1, 1}, {1, 0}};
    ps = polygon::build_open_polygon(points, ps);
    TEST_ASSERT_TRUE(ps.is_simple().value());
    
    points = {{0, 0}, {1, 0}, {1, 1}, {1, 0}, {2, 0}};
    ps = polygon::build_open_polygon(points, ps);
    TEST_ASSERT_TRUE(!ps.is_simple().value());
    
    points = {{0, 0}, {0, 1}, {0.5, 1}, {1, 1}, {1, 0}};
    ps = polygon::build_closed_polygon(points, ps);
    TEST_ASSERT_TRUE(ps.is_simple().value());
    
    points = {{0, 0}, {0, 1}, {0.5, 0.5}, {1, 1}, {1, 0}};
    ps = polygon::build_closed_polygon(points, ps);
    TEST_ASSERT_TRUE(ps.is_simple().value());
    
    points = {{0, 0}, {0, 1}, {0.5, 0}, {1, 1}, {1, 0}};
    ps = polygon::build_closed_polygon(points, ps);
    TEST_ASSERT_TRUE(!ps.is_simple().value());
    
    points = {{0, 0}, {0, 1}, {0.5, 0.1}, {1, 1}, {1, 0}};
    ps = polygon::build_closed_polygon(points, ps);
    TEST_ASSERT_TRUE(ps.is_simple().value());
    
    points = {{0, 0}, {0, 1}, {0.5, -0.25}, {1, 1}, {1, 0}};
    ps = polygon::build_closed_polygon(points, ps);
    TEST_ASSERT_TRUE(!ps.is_simple().value());
    
    points = {{0, 0}, {0, 1}, {0.5, -1000}, {1, 1}, {1, 0}};
    ps = polygon::build_closed_polygon(points, ps);
    TEST_ASSERT_TRUE(!ps.is_simple().value());
    
    points = {{0, 0}, {0, 1}, {0.5, 10}, {1, 1}, {1, 0}};
    ps = polygon::build_closed_polygon(points, ps);
    TEST_ASSERT_TRUE(ps.is_simple().value());
    
    points = {{0, 0}, {0, 1}, {1, 0}, {1, 1}, {1, 0}, {0, 0}};
    ps = polygon::build_closed_polygon(points, ps);
    TEST_ASSERT_TRUE(!ps.is_simple().value());
    
    points = {{0, 0}, {0, 1}, {1, 1}, {2, 1}, {2, 0.5}, {3, 1}, {2, -1}, {-1, 2}};
    ps = polygon::build_closed_polygon(points, ps);
    TEST_ASSERT_TRUE(!ps.is_simple().value());
    
    points = {{0, 0}, {1, 0}, {1, 1},  {0.5, 0}, {0, 1}};
    ps = polygon::build_closed_polygon(points, ps);
    TEST_ASSERT_TRUE(!ps.is_simple().value());
}

-(void)testConvexHull{
    polygon ps;
    vector<vec2> points {{0, 0}, {0, 1}, {1, 1}, {1, 0}};
    ps = polygon::build_closed_polygon(points, ps);
    polygon hull;
    hull = ps.convex_hull(hull);
    TEST_ASSERT_TRUE(hull.point_count() == 4);
    TEST_ASSERT_TRUE(hull.get_point(0) == vec2(1, 0));
    
    points = {{0, 0}, {0, 1}, {0.5, 0.5}, {1, 1}, {1, 0}};
    ps = polygon::build_closed_polygon(points, ps);
    hull = ps.convex_hull(hull);
    TEST_ASSERT_TRUE(hull.point_count() == 4);
    TEST_ASSERT_TRUE(hull.get_point(0) == vec2(1, 0));
    
    points = {{0, 0}, {0, 1}, {0.5, 0.5}, {1, 1}, {2, 0.5}, {1, 0}};
    ps = polygon::build_closed_polygon(points, ps);
    hull = ps.convex_hull(hull);
    TEST_ASSERT_TRUE(hull.point_count() == 5);
    TEST_ASSERT_TRUE(hull.get_point(0) == vec2(1, 0));
}

-(void)testJoin{
    polygon ps_0, ps_1;
    vector<vec2> points {{0, 0}, {0, 1}, {1, 1}, {1, 0}};
    ps_0 = polygon::build_closed_polygon(points, ps_0);
    ps_1 = polygon::build_closed_polygon(points, ps_1);
    matrix3x3::build_translation(2, 0).transform_polygon(ps_1);
    ps_0.join(ps_1);
    TEST_ASSERT_TRUE(ps_0.point_count() == 4);
    TEST_ASSERT_TRUE(ps_0.get_point(0) == vec2(0, 0));
    ps_0 = polygon::build_closed_polygon(points, ps_0);
    ps_1 = polygon::build_closed_polygon(points, ps_1);
    matrix3x3::build_translation(0.5, 0).transform_polygon(ps_1);
    ps_0.join(ps_1);
    TEST_ASSERT_TRUE(ps_0.point_count() == 4);
    TEST_ASSERT_TRUE(ps_0.area().value() == (1 * 1.5));
    ps_0 = polygon::build_closed_polygon(points, ps_0);
    ps_1 = polygon::build_closed_polygon(points, ps_1);
    matrix3x3::build_scale(1.5, 0.5).transform_polygon(ps_1);
    ps_0.join(ps_1);
    TEST_ASSERT_TRUE(ps_0.point_count() == 6);
    TEST_ASSERT_TRUE(ps_0.bounds().value().top_right() == vec2(1.5, 1));
}

-(void)testSplit{
    polygon ps_0;
    std::vector<polygon> other;
    vector<vec2> points {{0, 0}, {0, 1}, {1, 1}, {1, 0}};
    ps_0 = polygon::build_closed_polygon(points, ps_0);
    other.clear();
    ps_0.split(ray(vec2::up * 10, vec2::right), other);
    TEST_ASSERT_TRUE(other.size() == 0);
    
    ps_0 = polygon::build_closed_polygon(points, ps_0);
    other.clear();
    ps_0.split(ray(vec2::up * 0.5, vec2::right), other);
    TEST_ASSERT_TRUE(other.size() == 1);
    TEST_ASSERT_TRUE(ps_0.area().value() == 0.5f);
    TEST_ASSERT_TRUE(other[0].area().value() == 0.5f);
    
    ps_0 = polygon::build_closed_polygon(points, ps_0);
    other.clear();
    ps_0.split(ray(vec2::up * 0.5, vec2(1, 1)), other);
    TEST_ASSERT_TRUE(other.size() == 1);
    TEST_ASSERT_TRUE(ps_0.point_count() == 3);
    TEST_ASSERT_TRUE(other[0].point_count() == 5);
    
}

-(void)testPolygonToMesh {
    vector<vec2> points {{0, 0}, {0, 1}, {1, 1}, {1, 0}};
    polygon ps;
    polygon::build_closed_polygon(points, ps);
    mesh ms;
    null_texture_map ntm;
    ps.to_mesh(ms, 0, ntm);
    TEST_ASSERT_TRUE(ms.index_count() == 6);
    TEST_ASSERT_TRUE(ms.vertex_count() == 4);
    
    points = {{0, 0}, {0, 1}, {0.5, 0.5}, {1, 1}, {1, 0}};
    polygon::build_closed_polygon(points, ps);
    mesh ms2;
    ps.to_mesh(ms2, 0, ntm);
    TEST_ASSERT_TRUE(ms2.vertex_count() == 5);
    TEST_ASSERT_TRUE(ms2.index_count() == 9);

    points = {{0, 0}, {0, 1}, {1, 1}, {1, 0}};
    polygon::build_closed_polygon(points, ps);
    mesh ms3;
    ps.to_mesh(ms3, 1, ntm);
    TEST_ASSERT_TRUE(ms3.vertex_count() == 8);
    TEST_ASSERT_TRUE(ms3.index_count() == 24);
}
@end







