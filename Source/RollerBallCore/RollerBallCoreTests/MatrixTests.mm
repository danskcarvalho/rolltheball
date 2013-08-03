//
//  MatrixTests.m
//  RollerBallCore
//
//  Created by Danilo Carvalho on 02/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "MatrixTests.h"
#include "matrix3x3.h"
#include "test_macros.h"
#include "logging.h"

using namespace rb;

@implementation MatrixTests
- (void) testIdentity{
    matrix3x3 identity = matrix3x3::identity;
    TEST_ASSERT_TRUE(identity.is_identity());
    TEST_ASSERT_TRUE(identity.transformed_point(vec2::up) == vec2::up);
    TEST_ASSERT_TRUE(identity.transformed_vector(vec2::left) == vec2::left);
}

- (void) testReversibility{
    matrix3x3 identity = matrix3x3::identity;
    TEST_ASSERT_TRUE(identity == identity.inverse());
    matrix3x3 otherMatrix = matrix3x3::build_scale(2, 1) * matrix3x3::build_translation(0, -3);
    matrix3x3 inverse = otherMatrix.inverse();
    matrix3x3 combination = otherMatrix * inverse;
    TEST_ASSERT_TRUE(combination == identity);
    TEST_ASSERT_TRUE(inverse.transformed_point(otherMatrix.transformed_point(vec2(1, 2))) == vec2(1, 2));
}

- (void) testEquality {
    matrix3x3 identity = matrix3x3::identity;
    matrix3x3 another = matrix3x3::build_rotation(1.0f);
    TEST_ASSERT_TRUE(identity != another);
    TEST_ASSERT_TRUE(identity == matrix3x3::identity);
    TEST_ASSERT_TRUE(another != matrix3x3::build_rotation(2.0f));
    TEST_ASSERT_TRUE(another == another);
}

- (void) testRotation {
    matrix3x3 rotation = matrix3x3::build_rotation(M_PI / 2);
    vec2 result = rotation.transformed_point(vec2(1, 0));
    TEST_ASSERT_TRUE(result == vec2(0, 1));
    rotation = matrix3x3::build_rotation(-M_PI / 2);
    result = rotation.transformed_vector(vec2(1, 0));
    TEST_ASSERT_TRUE(result == vec2(0, -1));
}

- (void) testTranslation {
    matrix3x3 rotation = matrix3x3::build_translation(2, 3);
    vec2 result = rotation.transformed_point(vec2(1, 0));
    TEST_ASSERT_TRUE(result == vec2(3, 3));
}

- (void) testScale {
    matrix3x3 rotation = matrix3x3::build_scale(2, 3);
    vec2 result = rotation.transformed_point(vec2(4, 5));
    TEST_ASSERT_TRUE(result == vec2(8, 15));
}

- (void) testMultiplication {
    matrix3x3 m1 = matrix3x3::build_rotation(M_PI / 4);
    matrix3x3 m2 = matrix3x3::build_scale(1.4f, -0.8f);
    matrix3x3 res = m1 * m2;
    const vec2 pt = vec2(2, 1);
    const vec2 pt1 = m1.transformed_point(m2.transformed_point(pt));
    const vec2 pt2 = res.transformed_point(pt);
    TEST_ASSERT_TRUE(pt1 == pt2);
}

@end
