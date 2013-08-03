//
//  RayTests.m
//  RollerBallCore
//
//  Created by Danilo Carvalho on 07/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "RayTests.h"
#include "test_macros.h"
#include "ray.h"

using namespace rb;

@implementation RayTests
-(void)testTests{
    TEST_ASSERT_TRUE(ray::right.is_collinear_to(ray::left));
    TEST_ASSERT_TRUE(ray::right.is_collinear_to(ray(vec2(-2, 0), vec2::left)));
    TEST_ASSERT_TRUE(!ray::right.is_collinear_to(ray::up));
    TEST_ASSERT_TRUE(ray::right.is_perpendicular_to(ray::up));
    TEST_ASSERT_TRUE(!ray::right.is_perpendicular_to(ray::left));
    TEST_ASSERT_TRUE(ray::right.contains_point(vec2(0,0)));
    TEST_ASSERT_TRUE(ray::right.contains_point(vec2(10, 0)));
    TEST_ASSERT_TRUE(!ray::right.contains_point(vec2(-10, 0)));
    TEST_ASSERT_TRUE(!ray::right.contains_point(vec2(10, 0.1f)));
    TEST_ASSERT_TRUE(ray::right.is_in_line(vec2(2, 0)));
    TEST_ASSERT_TRUE(ray::right.is_in_line(vec2(-2, 0)));
    TEST_ASSERT_TRUE(!ray::right.is_in_line(vec2(1, 0.01f)));
}
-(void)testParameterRecovery{
    TEST_ASSERT_TRUE(ray::right.get_parameter(vec2(1, 0)) == 1);
    TEST_ASSERT_TRUE(ray::right.get_parameter(vec2(1, 1)) == 1);
    TEST_ASSERT_TRUE(ray::right.get_parameter(vec2(-2, 1)) == -2);
}

-(void)testDistance{
    TEST_ASSERT_TRUE(almost_equal(ray::right.distance(vec2(1, 0)), 0));
    TEST_ASSERT_TRUE(almost_equal(ray::right.distance(vec2(10, 0)), 0));
    TEST_ASSERT_TRUE(almost_equal(ray::right.distance(vec2(10, 10)), 10));
    TEST_ASSERT_TRUE(almost_equal(ray::right.distance(vec2(-10, 0)), 10));
    TEST_ASSERT_TRUE(almost_equal(ray::right.distance(vec2(-10, 10)), sqrtf(10*10 + 10*10)));
}

-(void)testIntersection{
    ray r1 = ray(vec2(1, 0), vec2::right);
    ray r2 = ray(vec2(-1, 0), vec2::left);
    ray r3 = ray(vec2(-1, 0), vec2::right);
    ray r4 = ray(vec2(1, 1), vec2::down);
    ray r5 = ray(vec2(2, 1), vec2::down);
    ray r6 = ray(vec2::zero, vec2(1, 1));
    ray r7 = ray(vec2(1, 0), vec2(1, 1).rotated90());
    TEST_ASSERT_TRUE(!ray::intersection(r1, r2).has_value());
    TEST_ASSERT_TRUE(!ray::intersection(r1, r3).has_value());
    TEST_ASSERT_TRUE(ray::intersection(r1, r4).has_value());
    vec2 r1r4 = ray::intersection(r1, r4).value();
    TEST_ASSERT_TRUE(r1r4 == vec2(1, 0));
    TEST_ASSERT_TRUE(ray::intersection(r1, r5).has_value());
    vec2 r1r5 = ray::intersection(r1, r5).value();
    TEST_ASSERT_TRUE(r1r5 == vec2(2, 0));
    TEST_ASSERT_TRUE(ray::intersection(r6, r7).has_value());
    vec2 r6r7 = ray::intersection(r6, r7).value();
    TEST_ASSERT_TRUE(r6r7 == vec2(0.5f, 0.5f));
}
@end
