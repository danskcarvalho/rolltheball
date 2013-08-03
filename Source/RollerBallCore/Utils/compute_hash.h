//
//  compute_hash.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 02/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__compute_hash__
#define __RollerBallCore__compute_hash__

#ifndef _FINAL_APP_
#include "precompiled.h"
#endif

namespace rb {
    class compute_hash {
    public:
        std::size_t operator()(const char* buffer, std::size_t size);
    };
}

#define DEFINE_HASH(_cls_) \
namespace std { \
    template<> \
    struct hash<_cls_> { \
        size_t operator()(const _cls_ &v) const \
        { \
            return rb::compute_hash()(reinterpret_cast<const char*>(&v), sizeof(_cls_)); \
        } \
    }; \
}

#endif /* defined(__RollerBallCore__compute_hash__) */
