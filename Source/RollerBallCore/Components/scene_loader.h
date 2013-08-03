//
//  scene_loader.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 23/06/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__scene_loader__
#define __RollerBallCore__scene_loader__

#include "components_base.h"

namespace rb {
    class typed_object;
    class scene;
    class xnode;
    class type_descriptor;
    class scene_loader {
    private:
        static void serialize_to_xnode(const typed_object* obj, xnode& node);
        static typed_object* deserialize_from_xnode(const xnode& node, const type_descriptor* td);
    public:
        static scene* load(const rb_string& name);
        static void serialize_to_url(const typed_object* obj, const rb_string& url);
        static rb_string serialize_to_string(const typed_object* obj);
        static typed_object* deserialize_from_url(const rb_string& url);
        static typed_object* deserialize_from_string(const rb_string& str);
    };
}

#endif /* defined(__RollerBallCore__scene_loader__) */
