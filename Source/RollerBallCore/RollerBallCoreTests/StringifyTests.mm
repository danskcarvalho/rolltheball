//
//  StringifyTests.m
//  RollerBallCore
//
//  Created by Danilo Carvalho on 01/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "StringifyTests.h"
#include "stringification.h"
#include "test_macros.h"
#include "enumerations.h"

using namespace rb;

class test_class_01 {
public:
    int32_t value;
    test_class_01(int32_t value){
        this->value = value;
    }
    rb_string to_string() const {
        return rb::to_string(u"value: ", this->value);
    }
};

CLASS_NAME(test_class_01);

enum class test_enum_01 {
    A,
    B,
    C
};

enum class test_enum_02 {
    A = 1,
    B = 2,
    C = 4
};

BEGIN_STRINGIFY_ENUM(test_enum_01)
    STRINGIFY_ENUM_VALUE(test_enum_01::A)
    STRINGIFY_ENUM_VALUE(test_enum_01::B)
    STRINGIFY_ENUM_VALUE(test_enum_01::C)
END_STRINGIFY_ENUM();

BEGIN_STRINGIFY_FLAGS(test_enum_02)
    STRINGIFY_FLAG_VALUE(test_enum_02::A)
    STRINGIFY_FLAG_VALUE(test_enum_02::B)
    STRINGIFY_FLAG_VALUE(test_enum_02::C)
END_STRINGIFY_FLAGS()

@implementation StringifyTests

-(void)testPrimitiveStringification{
    rb_string str = to_string(10);
    TEST_STRING_EQUALS(str, "10");
    str = to_string(1.6);
    TEST_STRING_EQUALS(str, "1.6");
    str = to_string(1.2f);
    TEST_STRING_EQUALS(str, "1.2");
    str = to_string(-10);
    TEST_STRING_EQUALS(str, "-10");
    str = to_string(true);
    TEST_STRING_EQUALS(str, "true");
    str = to_string(u"test");
    TEST_STRING_EQUALS(str, "test");
    const char16_t* c_str = u"test";
    str = to_string(c_str);
    TEST_STRING_EQUALS(str, "test");
    str = to_string("test");
    TEST_STRING_EQUALS(str, "test");
    auto cpp_str = rb_string(u"test");
    char16_t* _m_c_str = new char16_t[5];
    memcpy(_m_c_str, cpp_str.c_str(), sizeof(char16_t) * 5);
    str = to_string(_m_c_str);
    TEST_STRING_EQUALS(str, "test");
    delete _m_c_str;
    int32_t i = 78;
    str = to_string(i);
    TEST_STRING_EQUALS(str, "78");
    std::u16string _u16_str = u"test";
    str = to_string(_u16_str);
    TEST_STRING_EQUALS(str, "test");
    const int32_t c_i = 90;
    str = to_string((const int32_t)c_i);
    TEST_STRING_EQUALS(str, "90");
}

-(void)testContainerStringification{
    std::vector<int> v1({0, 1, 2});
    rb_string str = to_string(v1);
    TEST_STRING_EQUALS(str, "[0, 1, 2]");
    std::vector<rb_string> v2({u"Danilo", u"Lorena", u"Kälil"});
    str = to_string(v2);
    TEST_STRING_EQUALS(str, "[Danilo, Lorena, Kälil]");
    std::unordered_set<int32_t> v3({0, 1, 2});
    str = to_string(v3);
    TEST_STRING_EQUALS(str, "[2, 1, 0]");
    std::map<rb_string, int32_t> v4({{u"Danilo", 1}, {u"Lorena", 2}});
    str = to_string(v4);
    TEST_STRING_EQUALS(str, "[Danilo: 1, Lorena: 2]");
}

@end
