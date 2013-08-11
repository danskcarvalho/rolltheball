//
//  base.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 01/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef RollerBallCore_base__
#define RollerBallCore_base__

#ifndef _FINAL_APP_
#include "precompiled.h"
#endif

#include "definitions.h"

#define TO_U_STR(_t_) u###_t_
#include "enumerations.h"
#include "traits.h"
#include "stringification.h"

#ifdef DEBUG
#define ENABLE_WARNINGS
#define ENABLE_MESSAGES
#endif

#define C_EPSILON 0.0001f
#define TO_RADIANS(_x_) ((float)((_x_)*(M_PI / 180.0f)))
#define TO_DEGREES(_x_) ((float)((_x_)*(180.0f / M_PI)))

#include "logging.h"
#include "almost_equal.h"
#include "base_enumerations.h"
#include "compute_hash.h"
#include "nullable.h"
#include "tokenizer.h"

#if defined(__OBJC__)
#define URL_TYPE NSURL*
#else
#define URL_TYPE void*
#endif

#define TEXTURE_BORDER 2

#endif
