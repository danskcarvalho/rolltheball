//
//  responder.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 07/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "responder.h"
#if defined(MACOSX_TARGET)
#import <Cocoa/Cocoa.h>
#endif

using namespace rb;

responder::~responder(){
    
}

keyboard_modifier responder::modifiers(){
#if defined(MACOSX_TARGET)
    NSUInteger modifiers = [NSEvent modifierFlags];
    return (keyboard_modifier)(modifiers & NSDeviceIndependentModifierFlagsMask);
#else
    return (keyboard_modifier)0;
#endif
}