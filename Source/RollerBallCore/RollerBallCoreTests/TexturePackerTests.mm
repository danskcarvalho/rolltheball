//
//  TexturePackerTests.m
//  RollerBallCore
//
//  Created by Danilo Carvalho on 14/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "TexturePackerTests.h"
#include "test_macros.h"
#include "texture_packer.h"

using namespace rb;

@implementation TexturePackerTests
-(void)testTexturePacker{
    std::vector<vec2> sizes;
    sizes.push_back(vec2(10, 10));
    sizes.push_back(vec2(5, 5));
    sizes.push_back(vec2(2, 1));
    
    auto packed = texture_packer::pack_textures(sizes, vec2(15, 15));
    TEST_ASSERT_TRUE(packed.has_been_packed(0));
    TEST_ASSERT_TRUE(packed.has_been_packed(1));
    TEST_ASSERT_TRUE(packed.has_been_packed(2));
    
    sizes = {{10, 10}, {9, 9},  {5, 5}, {2, 1}};
    packed = texture_packer::pack_textures(sizes, vec2(15, 15));
    TEST_ASSERT_TRUE(packed.has_been_packed(0));
    TEST_ASSERT_TRUE(!packed.has_been_packed(1));
    TEST_ASSERT_TRUE(packed.has_been_packed(2));
    TEST_ASSERT_TRUE(packed.has_been_packed(3));
    
    sizes = {{10, 10}, {2, 2}, {5, 5}, {2, 1}};
    packed = texture_packer::pack_textures(sizes, vec2(16, 16));
    TEST_ASSERT_TRUE(packed.has_been_packed(0));
    TEST_ASSERT_TRUE(packed.has_been_packed(1));
    TEST_ASSERT_TRUE(packed.has_been_packed(2));
    TEST_ASSERT_TRUE(packed.has_been_packed(3));
    
    sizes = {{10, 10}, {2, 2}, {5, 5}, {2, 1}, {15, 10}, {10, 20}};
    packed = texture_packer::pack_textures(sizes, vec2(50, 50));
    TEST_ASSERT_TRUE(packed.has_been_packed(0));
    TEST_ASSERT_TRUE(packed.has_been_packed(1));
    TEST_ASSERT_TRUE(packed.has_been_packed(2));
    TEST_ASSERT_TRUE(packed.has_been_packed(3));
    TEST_ASSERT_TRUE(packed.has_been_packed(4));
    TEST_ASSERT_TRUE(packed.has_been_packed(5));
}
@end
