//
//  editor_delegate.h
//  RollerBallCore
//
//  Created by Danilo Santos de Carvalho on 20/05/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__editor_delegate__
#define __RollerBallCore__editor_delegate__

#include "components_base.h"

namespace rb {
    class node_container;
    class node;
    class type_descriptor;
    class typed_object;
    class editor_delegate {
    public:
        virtual ~editor_delegate();
        virtual void selection_changed(node_container* container) = 0;
        virtual void hierarchy_changed(node_container* container) = 0;
        virtual void current_changed() = 0;
        virtual void name_changed(node* n) = 0;
        virtual void creatable_type_was_registered(const type_descriptor* td) = 0;
        virtual void alert(const rb_string& message) = 0;
        virtual void confirm(const rb_string& message, std::function<void (bool)> result) = 0;
        virtual void push_object_in_property_inspector(typed_object* obj, const rb_string& title) = 0;
    };
}

#endif /* defined(__RollerBallCore__editor_delegate__) */
