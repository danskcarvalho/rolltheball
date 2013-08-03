//
//  base64.h
//  RollerBallCore
//
//  Created by Danilo Santos de Carvalho on 21/06/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#ifndef RollerBallCore_base64_h
#define RollerBallCore_base64_h

#ifndef _FINAL_APP_
#include "precompiled.h"
#endif

std::basic_string<char> base64Encode(const std::vector<uint8_t>& inputBuffer);
std::vector<uint8_t> base64Decode(const std::basic_string<char>& input);

#endif
