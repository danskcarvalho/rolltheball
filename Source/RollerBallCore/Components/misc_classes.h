//
//  misc_classes.h
//  RollerBallCore
//
//  Created by Danilo Santos de Carvalho on 11/03/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__misc_classes__
#define __RollerBallCore__misc_classes__

#include "components_base.h"
#include "typed_object.h"
#include "observer.h"

namespace rb {
    //won't support objects that modifies arrays or object properties...
    class multiple_nodes : public typed_object, public observer {
    public:
        virtual void property_changed(typed_object* object, const rb_string& property_name);
        virtual void branch_tag_changed(typed_object * object);
        virtual void object_deleted(typed_object* object);
    private:
        struct path_component { //path component
            rb_string value;
            uint32_t index;
        };
        //this must be set at the constructor (when the user creates a new object) and when a representative object is created...
        class type_descriptor* _original;
        std::vector<typed_object*> _objects;
        std::vector<typed_object*> _cloned_objects;
        std::set<multiple_nodes*> _to_be_freed;
        multiple_nodes* _root; //the root...
        std::list<path_component> _path; //path from the root...
        bool _is_void;
    public:
        multiple_nodes(const std::vector<typed_object*>& objects);
        virtual ~multiple_nodes();
    private:
        multiple_nodes();
        multiple_nodes(const class type_descriptor* original);
        //tracking
        multiple_nodes* track_multiple_nodes(multiple_nodes* mn) const;
        //get-set property
        //set
        void set_property_rec(typed_object* site, int mn_index, std::list<path_component> &path, const std::list<path_component> &full_path, const rb::generic_object &value);
        void set_property_value(const std::list<path_component>& path, const generic_object& value); //local path
        //set branch tag
        void set_branch_tag(int mn_index, const rb_string& value);
        //get
        const class type_descriptor* get_type_descriptor_rec(const rb::type_descriptor *start, std::list<path_component>& path);
        generic_object get_property_rec(typed_object* site, std::list<path_component> &path);
        void get_autocomplete_rec(typed_object* site, std::list<path_component> &path, std::vector<rb_string>& suggestions);
        void call_action_rec(typed_object* site, std::list<path_component> &path, const rb_string& action_name);
        std::vector<generic_object> get_property_values(const std::list<path_component>& path);
        void get_autocomplete_suggestions(const std::list<path_component>& path, std::vector<std::vector<rb_string>>& suggestions_list);
        void call_actions(const std::list<path_component>& path, const rb_string& action_name);
        generic_object get_representative_primitive_property_value(const std::list<path_component>& path);
        generic_object get_representative_object_property_value(const std::list<path_component>& path);
        generic_object get_representative_vector_property_value(const std::list<path_component>& path);
        generic_object get_representative_property_value(const std::list<path_component>& path);
        void get_representative_autocomplete_suggestions(const std::list<path_component>& path, std::vector<rb_string>& suggestions);
        static std::list<path_component> create_local_path(const rb_string& value, const uint32_t index = -1);
        //cloning...
        typed_object* clone_object(typed_object* cloned, int mn_index, const std::list<path_component>& path);
        std::vector<typed_object*> clone_vector(const std::vector<typed_object*>& cloned, int mn_index, const std::list<path_component>& path);
        //utils
        rb_string to_string(const std::list<path_component>& path);
        void set_void(const rb::generic_object& obj, const bool value);
    private:
        void add_property(const class type_descriptor* td, const rb_string& p);
    protected:
        //Typed Object
        virtual void describe_type() override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
        //Branch
    protected:
        virtual void branch_tag_changed() override;
    };
}

#endif /* defined(__RollerBallCore__misc_classes__) */
