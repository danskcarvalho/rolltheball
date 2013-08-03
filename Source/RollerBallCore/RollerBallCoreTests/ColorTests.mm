//
//  ColorTests.m
//  RollerBallCore
//
//  Created by Danilo Carvalho on 13/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "ColorTests.h"
#include "color.h"
#include "test_macros.h"

using namespace rb;

@implementation ColorTests
-(void)testInitialization{
    color c1 = color::from_rgb(0.5, 0.6, 0.7);
    TEST_ASSERT_TRUE(almost_equal(c1.r(), 0.5));
    TEST_ASSERT_TRUE(almost_equal(c1.g(), 0.6));
    TEST_ASSERT_TRUE(almost_equal(c1.b(), 0.7));
    TEST_ASSERT_TRUE(almost_equal(c1.a(), 1.0));
    
    c1 = color::from_rgba(0.1, 0.2, 0.3, 0.4);
    TEST_ASSERT_TRUE(almost_equal(c1.r(), 0.1));
    TEST_ASSERT_TRUE(almost_equal(c1.g(), 0.2));
    TEST_ASSERT_TRUE(almost_equal(c1.b(), 0.3));
    TEST_ASSERT_TRUE(almost_equal(c1.a(), 0.4));
    
    c1 = color::from_packed_rgba(0x7FFFFF7F);
    TEST_ASSERT_TRUE(almost_equal(c1.r(), 0.498039));
    TEST_ASSERT_TRUE(almost_equal(c1.g(), 1.0));
    TEST_ASSERT_TRUE(almost_equal(c1.b(), 1.0));
    TEST_ASSERT_TRUE(almost_equal(c1.a(), 0.498039));
    TEST_ASSERT_TRUE(c1.packed_rgba() == 0x7FFFFF7F);
    
    c1 = color::from_hsla(TO_RADIANS(100), 0.2, 0.5, 0.8);
    auto h = c1.h();
    auto r = (uint32_t)(c1.r() * 255);
    auto g = (uint32_t)(c1.g() * 255);
    auto b = (uint32_t)(c1.b() * 255);
    TEST_ASSERT_TRUE(almost_equal(r, 119));
    TEST_ASSERT_TRUE(almost_equal(g, 153));
    TEST_ASSERT_TRUE(almost_equal(b, 102));
    TEST_ASSERT_TRUE(almost_equal(c1.a(), 0.8));
    
    c1 = color::from_rgba(119.0 / 255, 153.0 / 255, 102.0 / 255, 0.8);
    h = c1.h();
    auto s = c1.sl();
    auto l = c1.l();
    TEST_ASSERT_TRUE(almost_equal(h, 1.76092));
    TEST_ASSERT_TRUE(almost_equal(s, 0.2));
    TEST_ASSERT_TRUE(almost_equal(l, 0.5));
    
    c1 = color::from_hsva(TO_RADIANS(100), 0.2, 0.5, 0.8);
    r = (uint32_t)(c1.r() * 255);
    g = (uint32_t)(c1.g() * 255);
    b = (uint32_t)(c1.b() * 255);
    TEST_ASSERT_TRUE(almost_equal(r, 110));
    TEST_ASSERT_TRUE(almost_equal(g, 127));
    TEST_ASSERT_TRUE(almost_equal(b, 102));
    TEST_ASSERT_TRUE(almost_equal(c1.a(), 0.8));
    
    c1 = color::from_rgba(110 / 255.0, 127 / 255.0, 102 / 255.0, 0.8);
    h = c1.h();
    s = c1.sv();
    auto v = c1.v();
    TEST_ASSERT_TRUE(almost_equal(h, 1.77572));
    TEST_ASSERT_TRUE(almost_equal(s, 0.19685));
    TEST_ASSERT_TRUE(almost_equal(v, 0.498039));
}
@end
