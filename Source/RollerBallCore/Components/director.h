//
//  director.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 07/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__director__
#define __RollerBallCore__director__

#include "components_base.h"

namespace rb {
    class scene;
    class responder;
    class editor_delegate;
    class type_descriptor;
    class typed_object;
    class director {
    private:
        static bool _in_editor;
        static scene* _active_scene;
        static editor_delegate* _delegate;
        static std::unordered_set<type_descriptor*> _registered_creatable_types;
        //register types
        static void register_creatable_types();
    public:
        static bool in_editor();
        static bool in_editor(bool value);
        static scene* active_scene();
        static void active_scene(scene* scene, bool cleanup);
        static responder* active_responder();
        static double ticks();
        static void editor_delegate(class editor_delegate* delegate);
        static class editor_delegate* editor_delegate();
        static void register_creatable_type(const type_descriptor* td);
        template<class T>
        static void register_serializable_type(){
            T* _t = new T();
            _t->type_descriptor(); //we force type registration here...
            delete _t;
        }
        static void fill_with_registered_creatable_types(std::vector<type_descriptor*>& list);
        static bool is_registered(const type_descriptor* td);
        static void edit_object_properties(typed_object* obj, const rb_string& title);
    };
}

#endif /* defined(__RollerBallCore__director__) */
