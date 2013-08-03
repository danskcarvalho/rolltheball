//
//  apple_program_manager.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 24/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "apple_program_manager.h"
#include "program.h"

using namespace rb;

apple_program_manager::apple_program_manager(){
    _basic_program = nullptr;
    _basic_program_with_wrapping = nullptr;
    load_basic_program();
    load_basic_program_with_wrapping();
}
void apple_program_manager::load_basic_program(){
    NSBundle* mainBundle = [NSBundle mainBundle];
    NSString* vertShaderPathname = [mainBundle pathForResource:@"basic_shader" ofType:@"vsh"];
    assert(vertShaderPathname);
    char* vert_source = (char *)[[NSString stringWithContentsOfFile:vertShaderPathname encoding:NSUTF8StringEncoding error:nil] UTF8String];
    
    NSString* fragShaderPathname = [mainBundle pathForResource:@"basic_shader" ofType:@"fsh"];
    assert(fragShaderPathname);
    char* frag_source = (char *)[[NSString stringWithContentsOfFile:fragShaderPathname encoding:NSUTF8StringEncoding error:nil] UTF8String];
    
    _basic_program = new program(vert_source, frag_source);
}
void apple_program_manager::load_basic_program_with_wrapping(){
    NSBundle* mainBundle = [NSBundle mainBundle];
    NSString* vertShaderPathname = [mainBundle pathForResource:@"basic_shader" ofType:@"vsh"];
    assert(vertShaderPathname);
    char* vert_source = (char *)[[NSString stringWithContentsOfFile:vertShaderPathname encoding:NSUTF8StringEncoding error:nil] UTF8String];
    
    NSString* fragShaderPathname = [mainBundle pathForResource:@"basic_shader_with_wrapping" ofType:@"fsh"];
    assert(fragShaderPathname);
    char* frag_source = (char *)[[NSString stringWithContentsOfFile:fragShaderPathname encoding:NSUTF8StringEncoding error:nil] UTF8String];
    
    _basic_program_with_wrapping = new program(vert_source, frag_source);
}
apple_program_manager::~apple_program_manager(){
    delete _basic_program;
    delete _basic_program_with_wrapping;
}
const program* apple_program_manager::get_basic_program() const{
    return _basic_program;
}
const program* apple_program_manager::get_basic_program_with_wrapping() const{
    return _basic_program_with_wrapping;
}





















