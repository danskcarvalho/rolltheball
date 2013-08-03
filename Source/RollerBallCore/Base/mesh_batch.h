//
//  mesh_batch.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 16/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__mesh_batch__
#define __RollerBallCore__mesh_batch__

#include "base.h"

namespace rb {
    class mesh_batch {
    public:
        mesh_batch();
        virtual ~mesh_batch();
        virtual void draw() = 0;
    };
}

#endif /* defined(__RollerBallCore__mesh_batch__) */
