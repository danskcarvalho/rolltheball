//
//  NullableTests.m
//  RollerBallCore
//
//  Created by Danilo Carvalho on 02/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "NullableTests.h"
#include "test_macros.h"
#include "base.h"
#include "nullable.h"

using namespace rb;

@implementation NullableTests
- (void) testNullable{
    nullable<int> i(20);
    TEST_ASSERT_TRUE(i.has_value());
    TEST_ASSERT_TRUE((int)i == 20);
    nullable<double> d;
    TEST_ASSERT_TRUE(!d.has_value());
    d = 10.2;
    TEST_ASSERT_TRUE(d.has_value());
    TEST_ASSERT_TRUE((double)d == 10.2);
    d = nullptr;
    TEST_ASSERT_TRUE(!d.has_value());
    nullable<float> f;
    TEST_ASSERT_TRUE(!f.has_value());
    f = 12.1f;
    TEST_ASSERT_TRUE(f.has_value());
    TEST_ASSERT_TRUE((float)f == 12.1f);
    f = nullptr;
    TEST_ASSERT_TRUE(!f.has_value());
    f = 23.1f;
}
@end
