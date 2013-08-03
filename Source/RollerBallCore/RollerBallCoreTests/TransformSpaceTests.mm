//
//  TransformSpaceTests.m
//  RollerBallCore
//
//  Created by Danilo Carvalho on 06/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "TransformSpaceTests.h"
#include "base.h"
#include "transform_space.h"
#include "test_macros.h"

using namespace rb;

@implementation TransformSpaceTests
- (void) testEquality{
    const transform_space sp = transform_space(vec2::zero, vec2(1, 1), vec2(0, M_PI / 2 + 2 * M_PI));
    const transform_space sp2 = transform_space(vec2::zero, vec2(1, 1), vec2(0, M_PI / 2 + M_PI));
    transform_space _canonical = sp.canonical();
    transform_space _default;
    TEST_ASSERT_TRUE(sp == _canonical);
    TEST_ASSERT_TRUE(sp == _default);
    TEST_ASSERT_TRUE(sp != sp2);
}
-(void)testTransforms{
    const vec2 pt = vec2::right;
    const vec2 pt2 = vec2::up;
    const transform_space tt = transform_space(vec2::right * 2, vec2(1, 1), vec2(M_PI / 2, M_PI));
    const vec2 transformed = tt.from_space_to_base().transformed_point(pt);
    const vec2 transformed2 = tt.from_space_to_base().transformed_point(pt2);
    TEST_ASSERT_TRUE(transformed == vec2(2, 1));
    TEST_ASSERT_TRUE(transformed2 == vec2(1, 0));
    TEST_ASSERT_TRUE(tt.from_base_to_space().transformed_point(transformed) == pt);
    TEST_ASSERT_TRUE(tt.from_base_to_space().transformed_point(transformed2) == pt2);
}

-(void)testDirection{
    const transform_space tt1 = transform_space(vec2(-1, 9), vec2(1, 6), vec2(0, M_PI / 2));
    const transform_space tt2 = transform_space(vec2(-1, 9), vec2(0, 6), vec2(0, M_PI / 2));
    const transform_space tt3 = transform_space(vec2(-1, 9), vec2(1, 6), vec2(M_PI / 2, M_PI / 2));
    const transform_space tt4 = transform_space(vec2(-1, 9), vec2(1, 6), vec2(0, M_PI));
    
    TEST_ASSERT_TRUE(tt1.test_direction(transform_direction::from_base_to_space));
    TEST_ASSERT_TRUE(tt1.test_direction(transform_direction::from_space_to_base));
    TEST_ASSERT_TRUE(tt2.test_direction(transform_direction::from_space_to_base));
    TEST_ASSERT_TRUE(!tt2.test_direction(transform_direction::from_base_to_space));
    TEST_ASSERT_TRUE(tt3.test_direction(transform_direction::from_space_to_base));
    TEST_ASSERT_TRUE(!tt3.test_direction(transform_direction::from_base_to_space));
    TEST_ASSERT_TRUE(tt4.test_direction(transform_direction::from_space_to_base));
    TEST_ASSERT_TRUE(!tt4.test_direction(transform_direction::from_base_to_space));
}

-(void)testCanonical{
    const transform_space tt = transform_space(vec2(1, 2), vec2(-1, 1), vec2(0, M_PI / 2));
    const transform_space canonical = tt.canonical();
    TEST_ASSERT_TRUE(canonical.origin() == vec2(1, 2));
    TEST_ASSERT_TRUE(canonical.scale() == vec2(1, 1));
    TEST_ASSERT_TRUE(canonical.rotation() == vec2(M_PI, M_PI / 2));
}

-(void)testMultiplication{
    const transform_space tt1 = transform_space(vec2(-1, 9), vec2(1, 6), vec2(M_PI / 4, M_PI / 2));
    const transform_space tt2 = transform_space(vec2(3, 1), vec2(1.5f, 1), vec2(0, M_PI / 4));
    const transform_space ttr = tt1 * tt2;
    const vec2 pt = vec2(2, 4);
    const vec2 pt1 = tt2.from_base_to_space().transformed_point(tt1.from_base_to_space().transformed_point(pt));
    const vec2 pt2 = ttr.from_base_to_space().transformed_point(pt);
    const vec2 pt3 = tt1.from_space_to_base().transformed_point(tt2.from_space_to_base().transformed_point(pt1));
    const vec2 pt4 = ttr.from_space_to_base().transformed_point(pt1);
    TEST_ASSERT_TRUE(pt1 == pt2);
    TEST_ASSERT_TRUE(pt3 == pt4);
    TEST_ASSERT_TRUE(pt == pt4);
}
@end
