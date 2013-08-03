//
//  SmoothCurveTests.m
//  RollerBallCore
//
//  Created by Danilo Carvalho on 13/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "SmoothCurveTests.h"
#include "test_macros.h"
#include "smooth_curve.h"

using namespace rb;

@implementation SmoothCurveTests
-(void)testOpenSmoothCurve{
    std::vector<vec2> points;
    points.push_back(vec2(0, 0.5));
    points.push_back(vec2(0, 1));
    points.push_back(vec2(1, 1));
    points.push_back(vec2(1, 0));
    smooth_curve curve;
    smooth_curve::build_open_curve(points, curve);
    auto pt = curve.sample_along_path(0.0);
    TEST_ASSERT_TRUE(pt == vec2(0, 0.5));
    pt = curve.sample_along_path(1);
    TEST_ASSERT_TRUE(pt == vec2(1, 0));
}

-(void)testClosedSmoothCurve{
    std::vector<vec2> points;
    points.push_back(vec2(0, 0));
    points.push_back(vec2(0.5, 0.5));
    points.push_back(vec2(0, 1));
    points.push_back(vec2(1, 1));
    points.push_back(vec2(1, 0));
    smooth_curve curve;
    smooth_curve::build_closed_curve(points, curve);
    auto ptS = curve.sample_along_path(0.0);
    auto ptE = curve.sample_along_path(1.0);
    TEST_ASSERT_TRUE(ptS == ptE);
}
@end
