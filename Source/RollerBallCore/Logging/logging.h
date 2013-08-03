//
//  logging.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 01/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef RollerBallCore_logging__
#define RollerBallCore_logging__

#include "stringification.h"
#include "platform.h"

namespace rb {
    template<class... Tps>
    inline void error(const Tps&... values){
        rb_string _str = rb::to_string(values...);
        rb::platform::error(_str);
    }
    
    template<class... Tps>
    void warn(const Tps&... values){
#ifdef ENABLE_WARNINGS
        auto _str = to_string(values...);
        rb::platform::warn(_str);
#endif
    }
    
    template<class... Tps>
    void msg(const Tps&... values){
#ifdef ENABLE_MESSAGES
        auto _str = to_string(values...);
        rb::platform::msg(_str);
#endif
    }
}

#ifdef DEBUG
#define ERROR(...) rb::error(__VA_ARGS__)
#define WARN(...) rb::warn(__VA_ARGS__)
#define MSG(...) rb::msg(__VA_ARGS__)
#else
#define ERROR(...) rb::error(__VA_ARGS__)
#define WARN(...) ((void)0)
#define MSG(...) ((void)0)
#endif


#endif
