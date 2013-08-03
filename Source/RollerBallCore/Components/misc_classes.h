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
    class node;
    class transformation_values;
    class component_id : public typed_object {
    private:
        rb_string _name;
        rb_string _classes;
        transformation_values* _internal;
    public:
        node* named_node;
        inline const rb_string& name() const {
            return _name;
        }
        const rb_string& name(const rb_string& value);
        inline const rb_string& classes() const {
            return _classes;
        }
        const rb_string& classes(const rb_string& value);
        //Constructors
        component_id();
    protected:
        //Typed Object
        virtual void describe_type() override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
    };
    
    class transformation_values : public typed_object {
    private:
        vec2 _translation;
        vec2 _scale;
        vec2 _rotation;
        bool _orthogonal;
        bool _uniform;
    public:
        node* transformed_node;
        inline const vec2& translation() const {
            return _translation;
        }
        
        inline const vec2& scale() const {
            return _scale;
        }
        
        inline const vec2& rotation() const {
            return _rotation;
        }
        
        inline const vec2& translation(const vec2& value) {
            _translation = value;
            notify_property_changed(u"translation");
            return _translation;
        }
        
        inline const vec2& scale(const vec2& value) {
            _scale = value;
            notify_property_changed(u"scale");
            //We won't enforce uniformity...
//            if(almost_equal(value.x(), value.y()))
//                uniform(true);
//            else
//                uniform(false);
            return _scale;
        }
        
    private:
        inline float normalize_angle(float angle){
            float angleValue = angle;
            if(angleValue > 0)
            {
                angleValue = angleValue - floorf(angleValue / (2 * 180)) * (2 * 180);
            }
            else if(angleValue < 0){
                angleValue = -angleValue;
                angleValue = angleValue - floorf(angleValue / (2 * 180)) * (2 * 180);
                angleValue = -angleValue;
                angleValue = (2 * 180) + angleValue;
            }
            return angleValue;
        }
    public:
        inline const vec2& rotation(const vec2& value) {
            _rotation = value;
            notify_property_changed(u"rotation");
            //we won't enforce orthogonality...
//            auto _diff = fabs(normalize_angle(_rotation.x()) - normalize_angle(_rotation.y()));
//            if(almost_equal(_diff, 90))
//                orthogonal(true);
//            else
//                orthogonal(false);
            return _rotation;
        }
        
        inline bool orthogonal() const {
            return _orthogonal;
        }
        
        inline bool orthogonal(const bool value){
            _orthogonal = value;
            notify_property_changed(u"orthogonal");
            return _orthogonal;
        }
        
        inline bool uniform() const {
            return _uniform;
        }
        
        inline bool uniform(const bool value){
            _uniform = value;
            notify_property_changed(u"uniform");
            return _uniform;
        }
        void sync_values();
        //Constructors
        transformation_values();
    protected:
        //Typed Object
        virtual void describe_type() override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
    };
    
    class node_without_transform : public typed_object {
    private:
        class node* _node;
        void add_nullable_property(const class type_descriptor *td, const rb_string &p);
        void add_non_nullable_property(const class type_descriptor *td, const rb_string &p);
        void add_property(const class type_descriptor *td, const rb_string &p);
    public:
        node_without_transform(class node* n);
        virtual ~node_without_transform();
        class node* node();
    protected:
        //Typed Object
        virtual void describe_type() override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
    };
    
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
