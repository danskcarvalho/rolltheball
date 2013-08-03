//
//  EdgeTests.m
//  RollerBallCore
//
//  Created by Danilo Carvalho on 07/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "EdgeTests.h"
#include "test_macros.h"
#include "edge.h"

using namespace rb;

@implementation EdgeTests
-(void)testDistance{
    edge e1 = edge(vec2::zero, vec2::right, vec2::up);
    TEST_ASSERT_TRUE(e1.distance(vec2(0.5f, 0)) == 0);
    TEST_ASSERT_TRUE(e1.distance(vec2(0.5f, 1)) == 1);
    TEST_ASSERT_TRUE(e1.distance(vec2(0, 0)) == 0);
    TEST_ASSERT_TRUE(e1.distance(vec2(1, 0)) == 0);
    TEST_ASSERT_TRUE(e1.distance(vec2(2, 0)) == 1);
    TEST_ASSERT_TRUE(e1.distance(vec2(-1, 0)) == 1);
    TEST_ASSERT_TRUE(e1.distance(vec2(-1, 1)) == sqrtf(2));
}
-(void)testIntersection{
    edge e1 = edge(vec2::zero, vec2::right, vec2::up);
    edge e2 = edge(vec2::zero, vec2::right, vec2::up);;
    edge e3 = edge(vec2::zero, vec2::right * 0.5, vec2::up);
    edge e4 = edge(vec2::zero + vec2(0.5, 0), vec2::right, vec2::up);
    edge e5 = edge(vec2::zero - vec2(0.5, 0), vec2::right, vec2::up);
    edge e6 = edge(vec2::zero + vec2(0.2, 0), vec2::right * 0.5, vec2::up);
    edge e7 = edge(vec2::right, vec2::right * 2, vec2::up);
    edge e8 = edge(-vec2::right, vec2::zero, vec2::up);
    edge e9 = edge(vec2(0,1), vec2(0,0), vec2::up);
    edge e10 = edge(vec2::up, -vec2::up, vec2::right);
    edge e11 = edge(vec2::up - vec2::right, -vec2::up - vec2::right, vec2::right);
    edge e12 = edge(vec2::up + vec2::right, -vec2::up + vec2::right, vec2::right);
    edge e13 = edge(vec2::up + 1.01 * vec2::right, -vec2::up + 1.01 * vec2::right, vec2::right);
    edge e14 = edge(vec2::up + vec2::right * 0.5, -vec2::up + vec2::right * 0.5, vec2::right);
    TEST_ASSERT_TRUE(edge::test_intersection(e1, e2));
    TEST_ASSERT_TRUE(edge::test_intersection(e1, e3));
    TEST_ASSERT_TRUE(edge::test_intersection(e1, e4));
    TEST_ASSERT_TRUE(edge::test_intersection(e1, e5));
    TEST_ASSERT_TRUE(edge::test_intersection(e1, e6));
    TEST_ASSERT_TRUE(!edge::test_intersection(e1, e7));
    TEST_ASSERT_TRUE(!edge::test_intersection(e1, e8));
    TEST_ASSERT_TRUE(!edge::test_intersection(e1, e9));
    TEST_ASSERT_TRUE(edge::test_intersection(e1, e10));
    TEST_ASSERT_TRUE(!edge::test_intersection(e1, e11));
    TEST_ASSERT_TRUE(edge::test_intersection(e1, e12));
    TEST_ASSERT_TRUE(!edge::test_intersection(e1, e13));
    TEST_ASSERT_TRUE(edge::test_intersection(e1, e14));
}
@end
