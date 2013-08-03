//
//  platform.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 01/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//


#include "platform.h"


inline NSString* to_platform_string(const rb::rb_string& str){
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> _str_converter;
    auto _u8Str = _str_converter.to_bytes(str);
    NSString* _platform_str = [NSString stringWithUTF8String:_u8Str.c_str()];
    return _platform_str;
}

void rb::platform::error(const rb_string &str){
    NSString* _platform = to_platform_string(str);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
    NSLog(_platform);
#pragma clang diagnostic pop
}

void rb::platform::msg(const rb_string &str){
    NSString* _platform = to_platform_string(str);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
    NSLog(_platform);
#pragma clang diagnostic pop
}

void rb::platform::warn(const rb_string &str){
    NSString* _platform = to_platform_string(str);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
    NSLog(_platform);
#pragma clang diagnostic pop
}

