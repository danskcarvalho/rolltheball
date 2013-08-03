//
//  cg_resource.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 15/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__cg_resource__
#define __RollerBallCore__cg_resource__

#include "base.h"
#include <CoreGraphics/CGContext.h>

namespace rb {
    class cg_resource {
    private:
        CFTypeRef _cf_ref;
    public:
        CFTypeRef resource() const;
        cg_resource(CFTypeRef ref);
        cg_resource(const cg_resource& other) = delete;
        cg_resource(cg_resource&& other) = delete;
        cg_resource& operator=(const cg_resource& other) = delete;
        cg_resource& operator=(cg_resource&& other) = delete;
        void delete_resource();
        ~cg_resource();
    };
}

#endif /* defined(__RollerBallCore__cg_resource__) */
