//
//  observer.h
//  RollerBallCore
//
//  Created by Danilo Santos de Carvalho on 27/02/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__observer__
#define __RollerBallCore__observer__

#include "components_base.h"

namespace rb {
    class typed_object;
    class observer {
    public:
        virtual void property_changed(typed_object* object, const rb_string& property_name) = 0;
        virtual void branch_tag_changed(typed_object * object) = 0;
        virtual void object_deleted(typed_object* object) = 0;
    };
}

#endif /* defined(__RollerBallCore__observer__) */
