//
//  VectorTests.m
//  RollerBallCore
//
//  Created by Danilo Carvalho on 02/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "VectorTests.h"
#include "base.h"
#include "vec2.h"
#include "test_macros.h"

using namespace rb;

@implementation VectorTests
- (void) testTests{
    TEST_ASSERT_TRUE(vec2(0, 0).is_zero());
    TEST_ASSERT_TRUE(vec2(1, 0).is_unit());
    TEST_ASSERT_TRUE(vec2(2, 3).normalized().is_unit());
    TEST_ASSERT_TRUE(!vec2(1,0).is_perpendicular_to(vec2(1,1)));
    TEST_ASSERT_TRUE(vec2::up.is_perpendicular_to(vec2::right));
    TEST_ASSERT_TRUE(!vec2::up.is_parallel_to(vec2::left));
    TEST_ASSERT_TRUE(vec2::up.is_parallel_to(vec2::up));
    TEST_ASSERT_TRUE(vec2::up.is_parallel_to(vec2::down));
    TEST_ASSERT_TRUE(vec2::up.points_at_same_direction(vec2::up * 10));
    TEST_ASSERT_TRUE(!vec2::up.points_at_same_direction(vec2::up * (-1)));
    TEST_ASSERT_TRUE(!vec2::up.points_at_same_direction(vec2::left));
}
- (void) testAngleBetween{
    TEST_ASSERT_TRUE(almost_equal(vec2::right.angle_between(vec2::up), M_PI / 2));
    TEST_ASSERT_TRUE(almost_equal(vec2::right.angle_between(vec2::up, rotation_direction::ccw), M_PI / 2));
    TEST_ASSERT_TRUE(almost_equal(vec2::right.angle_between(vec2::up, rotation_direction::cw), 3 * (M_PI / 2)));
    TEST_ASSERT_TRUE(almost_equal(vec2::right.angle_between(vec2::left), M_PI));
    TEST_ASSERT_TRUE(almost_equal(vec2::right.angle_between(vec2::down), M_PI / 2));
    TEST_ASSERT_TRUE(almost_equal(vec2::right.angle_between(vec2::down, rotation_direction::cw), M_PI / 2));
    TEST_ASSERT_TRUE(almost_equal(vec2::right.angle_between(vec2::down, rotation_direction::ccw), 3 * (M_PI / 2)));
    TEST_ASSERT_TRUE(almost_equal(vec2::right.angle_between(vec2(1, -1)), M_PI / 4));
}
- (void) testProject{
    TEST_ASSERT_TRUE(vec2(3, -1).projection(vec2::right) == vec2(3, 0));
    TEST_ASSERT_TRUE(vec2(3, -1).projection(vec2::left) == vec2(3, 0));
    
}
- (void) testOperators{
    vec2 v1 = vec2(1, 4);
    vec2 v2 = vec2(-3, 8);
    TEST_ASSERT_TRUE(v1 + v2 == vec2(1 - 3, 4 + 8));
    TEST_ASSERT_TRUE(v1 - v2 == vec2(1 + 3, 4 - 8));
    TEST_ASSERT_TRUE(-v1 == vec2(-1, -4));
    TEST_ASSERT_TRUE(v1*2 == vec2(2, 8));
    TEST_ASSERT_TRUE(v1*v2 == vec2(1 * (-3), 4 * 8));
}
- (void) testDistance {
    vec2 v1 = vec2(0, 0);
    vec2 v2 = vec2(1, 1);
    TEST_ASSERT_TRUE(vec2::distance(v1, v2) == sqrtf(2));
}
@end
