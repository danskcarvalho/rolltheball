//
//  type_descriptor.h
//  RollerBallCore
//
//  Created by Danilo Santos de Carvalho on 27/02/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__type_descriptor__
#define __RollerBallCore__type_descriptor__

#include "components_base.h"
#include "vec2.h"
#include "color.h"
#include "buffer.h"

namespace rb {
    class typed_object;
    class vec2;
    class color;
    class generic_object;
    class type_descriptor {
    public:
        friend class typed_object;
        typedef std::function<typed_object* (void)> ctor;
    private:
        //accessors
        template<class T>
        using get_function = std::function<T (const typed_object*)>;
        template<class T>
        using set_function = std::function<void (typed_object*, T)>;
        template<class T>
        using accessors = std::pair<get_function<T>, set_function<T>>;
        //fields
        general_type_descriptor _gtd;
        std::map<rb_string, type_descriptor*> _union_map;
        std::map<rb_string, long> _enum_map;
        rb_string _name;
        rb_string _display_name;
        rb_string _selector_name;
        type_descriptor* _element_type;
        bool _dynamic_type;
        bool _nullable;
        static std::map<rb_string, const type_descriptor*> _registered_types;
        action_flags _action_flags;
        struct property_info {
            bool is_private;
            rb_string display_name;
            type_descriptor* type;
            bool editable;
            bool continuous;
            uint32_t precision;
            bool inlined;
            //non-nullable
            accessors<buffer> buff_accs;
            accessors<rb_string> str_accs;
            accessors<long> int_accs;
            accessors<float> float_accs;
            accessors<vec2> vec2_accs;
            accessors<bool> bool_accs;
            accessors<color> color_accs;
            accessors<std::vector<typed_object*>> vector_accs;
            //nullable
            accessors<nullable<buffer>> null_buff_accs;
            accessors<nullable<rb_string>> null_str_accs;
            accessors<nullable<long>> null_int_accs;
            accessors<nullable<float>> null_float_accs;
            accessors<nullable<vec2>> null_vec2_accs;
            accessors<nullable<bool>> null_bool_accs;
            accessors<nullable<color>> null_color_accs;
            accessors<typed_object*> null_obj_accss;
            //action
            std::function<void (typed_object* site, const rb_string&)> action_accss;
            //autocompletion
            std::function<std::vector<rb_string> (const typed_object* site)> autocompletion_function;
        };
        std::vector<rb_string> _properties_decl_order;
        std::map<rb_string, property_info> _properties;
        ctor _constructor;
    private:
        const property_info& _property_info(const rb_string& name) const;
    public:
        //Properties Of The Type
        inline ctor constructor() const {
            return _constructor;
        }
        inline general_type_descriptor gtd() const {
            return _gtd;
        }
        inline const std::map<rb_string, type_descriptor*>& branches() const {
            return _union_map;
        }
        inline const std::map<rb_string, long>& enumeration_values() const {
            assert(_gtd == general_type_descriptor::enumeration);
            return _enum_map;
        }
        inline std::vector<rb_string> action_names() const {
            assert(_gtd == general_type_descriptor::action);
            std::vector<rb_string> _actions;
            for (auto& _k : _enum_map){
                _actions.push_back(_k.first);
            }
            std::sort(_actions.begin(), _actions.end(), [&](const rb_string& n1, const rb_string& n2) {
                return _enum_map.at(n1) < _enum_map.at(n2); });
            return _actions;
        }
        inline std::vector<rb_string> flag_names() const {
            assert(_gtd == general_type_descriptor::flags);
            std::vector<rb_string> _actions;
            for (auto& _k : _enum_map){
                _actions.push_back(_k.first);
            }
            std::sort(_actions.begin(), _actions.end(), [&](const rb_string& n1, const rb_string& n2) {
                return _enum_map.at(n1) < _enum_map.at(n2); });
            return _actions;
        }
        inline bool dynamic_type() const {
            return _dynamic_type;
        }
        inline const rb_string& name() const {
            return _name;
        }
        inline const rb_string& display_name() const {
            return _display_name;
        }
        inline const rb_string& selector_name() const {
            return _selector_name;
        }
        inline const type_descriptor* element_type() const {
            return _element_type;
        }
        inline bool nullable() const {
            return _nullable;
        }
        enum action_flags action_flags() const;
        //About the Properties of this type
        void get_all_properties(std::vector<rb_string>& properties) const;
        bool has_property(const rb_string& name) const;
        const rb_string& property_display_name(const rb_string& name) const;
        const type_descriptor* property_type(const rb_string& name) const;
        bool property_editable(const rb_string& name) const;
        bool property_continuous(const rb_string& name) const;
        bool property_inlined(const rb_string& name) const;
        bool property_private(const rb_string& name) const;
        uint32_t property_precision(const rb_string& name) const;
        //get property values
        std::vector<rb_string> get_autocomplete_suggestions(const typed_object* site, const rb_string& name) const;
        generic_object get_property(const typed_object* site, const rb_string& name) const;
        buffer get_buffer_property(const typed_object* site, const rb_string& name) const;
        rb_string get_string_property(const typed_object* site, const rb_string& name) const;
        long get_integer_property(const typed_object* site, const rb_string& name) const;
        float get_single_property(const typed_object* site, const rb_string& name) const;
        vec2 get_vec2_property(const typed_object* site, const rb_string& name) const;
        bool get_boolean_property(const typed_object* site, const rb_string& name) const;
        color get_color_property(const typed_object* site, const rb_string& name) const;
        template<class T>
        T get_enumeration_property(const typed_object* site, const rb_string& name) const {
            return (T)get_integer_property(site, name);
        }
        rb::nullable<buffer> get_nullable_buffer_property(const typed_object* site, const rb_string& name) const;
        rb::nullable<rb_string> get_nullable_string_property(const typed_object* site, const rb_string& name) const;
        rb::nullable<long> get_nullable_integer_property(const typed_object* site, const rb_string& name) const;
        rb::nullable<float> get_nullable_single_property(const typed_object* site, const rb_string& name) const;
        rb::nullable<vec2> get_nullable_vec2_property(const typed_object* site, const rb_string& name) const;
        rb::nullable<bool> get_nullable_boolean_property(const typed_object* site, const rb_string& name) const;
        rb::nullable<color> get_nullable_color_property(const typed_object* site, const rb_string& name) const;
        template<class T>
        rb::nullable<T> get_nullable_enumeration_property(const typed_object* site, const rb_string& name) const {
            rb::nullable<long> _e = get_integer_property(site, name);
            if(!_e.has_value())
                return nullptr;
            else
                return rb::nullable<T>((T)_e.value());
        }
        template<class T>
        T* get_object_property(const typed_object* site, const rb_string& name) const{
            assert(has_property(name));
            assert(_property_info(name).type->gtd() == general_type_descriptor::object || _property_info(name).type->gtd() == general_type_descriptor::branch);
            return dynamic_cast<T*>(_property_info(name).null_obj_accss.first(site));
        }
        template<class T>
        std::vector<T*> get_vector_property(const typed_object* site, const rb_string& name) const{
            assert(has_property(name));
            assert(_property_info(name).type->gtd() == general_type_descriptor::vector);
            
            std::vector<T*> _new;
            auto _old = (std::vector<typed_object*>)_property_info(name).vector_accs.first(site);
            
            for (auto item : _old){
                _new.push_back(dynamic_cast<T*>(item));
            }
            return _new;
        }
        //set properties
        void call_action(typed_object* site, const rb_string& property_name, const rb_string& action_name) const;
        void set_property(typed_object* site, const rb_string& name, const generic_object& value) const;
        void set_buffer_property(typed_object* site, const rb_string& name, buffer value) const;
        void set_string_property(typed_object* site, const rb_string& name, const rb_string& value) const;
        void set_integer_property(typed_object* site, const rb_string& name, const long value) const;
        void set_single_property(typed_object* site, const rb_string& name, const float value) const;
        void set_vec2_property(typed_object* site, const rb_string& name, const vec2& value) const;
        void set_boolean_property(typed_object* site, const rb_string& name, const bool value) const;
        void set_color_property(typed_object* site, const rb_string& name, const color& value) const;
        template<class T>
        void set_enumeration_property(typed_object* site, const rb_string& name, const T value) const{
            set_integer_property(site, name, (long)value);
        }
        void set_nullable_buffer_property(typed_object* site, const rb_string& name, const rb::nullable<buffer> value) const;
        void set_nullable_string_property(typed_object* site, const rb_string& name, const rb::nullable<rb_string> value) const;
        void set_nullable_integer_property(typed_object* site, const rb_string& name, const rb::nullable<long> value) const;
        void set_nullable_single_property(typed_object* site, const rb_string& name, const rb::nullable<float> value) const;
        void set_nullable_vec2_property(typed_object* site, const rb_string& name, const rb::nullable<vec2> value) const;
        void set_nullable_boolean_property(typed_object* site, const rb_string& name, const rb::nullable<bool> value) const;
        void set_nullable_color_property(typed_object* site, const rb_string& name, const rb::nullable<color> value) const;
        template<class T>
        void set_nullable_enumeration_property(typed_object* site, const rb_string& name, const rb::nullable<T> value) const{
            if(!value.has_value())
                set_nullable_integer_property(site, name, nullptr);
            else
                set_integer_property(site, name, (long)value.value());
        }
        template<class T>
        void set_object_property(typed_object* site, const rb_string& name, const T* value) const{
            assert(has_property(name));
            assert(_property_info(name).type->gtd() == general_type_descriptor::object || _property_info(name).type->gtd() == general_type_descriptor::branch);
            _property_info(name).null_obj_accss.second(site, (typed_object*)value);
        }
        template<class T>
        void set_vector_property(typed_object* site, const rb_string& name, const std::vector<T*>& value) const{
            assert(has_property(name));
            assert(_property_info(name).type->gtd() == general_type_descriptor::vector);
            
            std::vector<typed_object*> _new;
            for (auto item : value){
                _new.push_back((typed_object*)item);
            }
            _property_info(name).vector_accs.second(site, _new);
        }
        //constructor
    private:
        type_descriptor();
        //registering types
        static void register_type(const type_descriptor* descriptor);
    public:
        static const type_descriptor* with_name(const rb_string& name);
        //new object
        template<class T = typed_object>
        T* new_object() const{
            return dynamic_cast<T*>(_constructor());
        }
    public:
        template<class T>
        static const type_descriptor* get(){
            auto _value = new T();
            auto _td = _value->type_descriptor();
            delete _value;
            return _td;
        }
    private:
        static void create_stock_types();
        static type_descriptor* create_basic_descriptor(const rb_string& name, const rb_string& display_name, const general_type_descriptor gtd, const bool nullable);
    public:
        //stock types
        static const type_descriptor* string();
        static const type_descriptor* integer();
        static const type_descriptor* single();
        static const type_descriptor* vec2();
        static const type_descriptor* boolean();
        static const type_descriptor* ranged();
        static const type_descriptor* angle();
        static const type_descriptor* color();
        static const type_descriptor* image();
        static const type_descriptor* string_with_autocompletion();
        static const type_descriptor* buffer();
        //nullable stock types
        static const type_descriptor* nullable_string();
        static const type_descriptor* nullable_integer();
        static const type_descriptor* nullable_single();
        static const type_descriptor* nullable_vec2();
        static const type_descriptor* nullable_boolean();
        static const type_descriptor* nullable_ranged();
        static const type_descriptor* nullable_angle();
        static const type_descriptor* nullable_color();
        static const type_descriptor* nullable_image();
        static const type_descriptor* nullable_string_with_autocompletion();
        static const type_descriptor* nullable_buffer();
    };
}

#endif /* defined(__RollerBallCore__type_descriptor__) */
