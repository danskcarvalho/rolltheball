//
//  resettable_component.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 05/01/14.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__resettable_component__
#define __RollerBallCore__resettable_component__

#include "components_base.h"

namespace rb {
    class scene;
    class node;
    class resettable_component {
    private:
        static void reset_component(node* n);
        static void reset_physics(node* n);
    protected:
        virtual void reset_component();
        virtual void reset_physics();
    public:
        static void reset_components(scene* s);
        virtual ~resettable_component();
    };
}

#endif /* defined(__RollerBallCore__resettable_component__) */
