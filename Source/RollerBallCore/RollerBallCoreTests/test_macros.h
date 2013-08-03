//
//  Macros.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 01/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef RollerBallCore_test_macros__
#define RollerBallCore_test_macros__

#include "base.h"
#include <locale>
#include <codecvt>

#define TEST_ASSERT_TRUE(_t_) STAssertTrue(_t_, @"Assertion Failed...")
#define TEST_STRING_EQUALS(_t_, _v_) TEST_ASSERT_TRUE(_t_ == u##_v_)

inline NSString* to_platform_string(const rb::rb_string& str){
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> _str_converter;
    auto _u8Str = _str_converter.to_bytes(str);
    NSString* _platform_str = [NSString stringWithUTF8String:_u8Str.c_str()];
    return _platform_str;
}

#endif
