//
//  typed_object.h
//  RollerBallCore
//
//  Created by Danilo Santos de Carvalho on 27/02/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__typed_object__
#define __RollerBallCore__typed_object__

#include "components_base.h"
#include "type_descriptor.h"
#include "vec2.h"
#include "color.h"
#include "buffer.h"

#if defined(__OBJC__)
#import "CppBridgeProtocol.h"
#define BRIGE_TYPE id<CppBridgeProtocol>
#else
#define BRIGE_TYPE void*
#endif

namespace rb {
    class vec2;
    class color;
    class observer;
    
    class typed_object {
    protected:
        //Typedefs
        template<class Y, class T>
        using get_function = std::function<T (const Y*)>;
        template<class Y, class T>
        using set_function = std::function<void (Y*, T)>;
        template<class Y, class T>
        using accessors = std::pair<get_function<Y, T>, set_function<Y, T>>;
        template<class T>
        using constructor = std::function<T* (void)>;
        template<class T>
        using autocompletion_accessor = std::function<std::vector<rb_string> (const T*)>;
        template<class T>
        using action_accessor = std::function<void (T*, const rb_string&)>;
    private:
        //Types
        enum type_construction_stage {
            tsStartedType,
            tsStartedBranchType,
            tsStartedBranch,
            tsEnded
        };
        struct type_construction_state {
            type_descriptor* cached_type_descriptor;
            type_descriptor* current_branch;
            type_construction_stage stage;
            int private_properties;
        };
        //Fields
        BRIGE_TYPE _cpp_bridge;
        std::vector<type_descriptor*> _stack_inheritance;
        type_construction_state* _current_type_constr;
        std::set<observer*> _observers;
        std::set<class type_descriptor*> _associated_types;
        type_descriptor* _cached_type_descriptor;
        type_descriptor* _cached_nullable_type_descriptor;
        type_descriptor* _cached_vector_type_descriptor;
        static std::set<rb_string> _types_being_described;
        rb_string _cbranch_tag;
        bool _update_silently;
    private:
        void create_type_descriptor();
    protected:
        virtual void describe_type() = 0;
    public:
        virtual rb_string type_name() const = 0;
        virtual rb_string displayable_type_name() const = 0;
        virtual bool dynamic_type() const;
    protected:
        void begin_private_properties();
        void end_private_properties();
        template<class T>
        void start_type(constructor<T> constructor){ //can be called twice in inheritance scenarios
            assert(!_current_type_constr);
            assert(_types_being_described.count(type_name()) == 0);
            assert(!type_descriptor::with_name(type_name()));
            _types_being_described.insert(type_name());
            _current_type_constr = new type_construction_state();
            _current_type_constr->stage = tsStartedType;
            _current_type_constr->cached_type_descriptor = new class type_descriptor();
            _current_type_constr->private_properties = 0;
            _current_type_constr->cached_type_descriptor->_constructor = [=](){
                return (typed_object*)constructor();
            };
            _current_type_constr->cached_type_descriptor->_display_name = displayable_type_name();
            _current_type_constr->cached_type_descriptor->_gtd = general_type_descriptor::object;
            _current_type_constr->cached_type_descriptor->_name = type_name();
        }
        template<class T>
        void start_branch_type(const rb_string& selector_name, constructor<T> constructor){
            assert(!_current_type_constr);
            _current_type_constr = new type_construction_state();
            _current_type_constr->stage = tsStartedBranchType;
            _current_type_constr->cached_type_descriptor = new class type_descriptor();
            _current_type_constr->private_properties = 0;
            _current_type_constr->cached_type_descriptor->_constructor = [=](){
                return (typed_object*)constructor();
            };
            _current_type_constr->cached_type_descriptor->_display_name = displayable_type_name();
            _current_type_constr->cached_type_descriptor->_gtd = general_type_descriptor::branch;
            _current_type_constr->cached_type_descriptor->_name = type_name();
            _current_type_constr->cached_type_descriptor->_selector_name = selector_name;
        }
        void start_branch(const rb_string& name);
        void end_branch();
        void end_type();
        //adding properties
    private:
        template<class Y>
        void new_property(const type_descriptor* property_type, const rb_string& name, const rb_string& display_name, const bool editable, const bool continuous, const uint32_t precision, const bool inlined, const std::function<void (class type_descriptor::property_info&)> set_accessors){
            assert(_current_type_constr);
            if(property_type->gtd() == general_type_descriptor::buffer)
                assert(_current_type_constr->private_properties != 0);
            class type_descriptor::property_info _p_info;
            _p_info.display_name = display_name;
            _p_info.type = const_cast<class type_descriptor*>(property_type);
            _p_info.editable = editable;
            _p_info.continuous = continuous;
            _p_info.precision = precision;
            _p_info.inlined = inlined;
            _p_info.is_private = (_current_type_constr->private_properties != 0);
            set_accessors(_p_info);
            if(_current_type_constr->current_branch){
                _current_type_constr->current_branch->_properties.insert({name, _p_info});
                _current_type_constr->current_branch->_properties_decl_order.push_back(name);
            }
            else {
                _current_type_constr->cached_type_descriptor->_properties.insert({name, _p_info});
                _current_type_constr->cached_type_descriptor->_properties_decl_order.push_back(name);
            }
        }
#define NEW_PROPERTY(Y, property_type, name, display_name, editable, continuous, precision, inlined, field, type) \
        new_property<Y>(property_type, name, display_name, editable, continuous, precision, inlined, [=](class type_descriptor::property_info& p){ \
            p.field = { \
                [=](const typed_object* site){ \
                    return code.first(dynamic_cast<const Y*>(site)); \
                }, \
                [=](typed_object* site, type value){ \
                    code.second(dynamic_cast<Y*>(site), value); \
                } \
            }; \
        })
        
    protected:
        template<class Y>
        void buffer_property(const rb_string& name, const rb_string& display_name, const accessors<Y, buffer>& code){
            NEW_PROPERTY(Y, type_descriptor::buffer(), name, display_name, false, false, 0, true, buff_accs, buffer);
        }
        template<class Y>
        void string_property(const rb_string& name, const rb_string& display_name, const bool editable, const bool continuous, const accessors<Y, rb_string>& code){
            NEW_PROPERTY(Y, type_descriptor::string(), name, display_name, editable, continuous, 0, true, str_accs, rb_string);
        }
        template<class Y>
        void integer_property(const rb_string& name, const rb_string& display_name, const bool editable, const accessors<Y, long> code){
            NEW_PROPERTY(Y, type_descriptor::integer(), name, display_name, editable, false, 0, true, int_accs, long);
        }
        template<class Y>
        void single_property(const rb_string& name, const rb_string& display_name, const bool editable, const accessors<Y, float> code){
            NEW_PROPERTY(Y, type_descriptor::single(), name, display_name, editable, false, 5, true, float_accs, float);
        }
        template<class Y>
        void vec2_property(const rb_string& name, const rb_string& display_name, const bool editable, const accessors<Y, vec2> code){
            NEW_PROPERTY(Y, type_descriptor::vec2(), name, display_name, editable, false, 5, true, vec2_accs, vec2);
        }
        template<class Y>
        void boolean_property(const rb_string& name, const rb_string& display_name, const bool editable, const accessors<Y, bool> code){
            NEW_PROPERTY(Y, type_descriptor::boolean(), name, display_name, editable, false, 0, true, bool_accs, bool);
        }
        template<class Y>
        void ranged_property(const rb_string& name, const rb_string& display_name, const bool editable, const bool continuous, const uint32_t precision, const accessors<Y, float> code){
            NEW_PROPERTY(Y, type_descriptor::ranged(), name, display_name, editable, continuous, precision, true, float_accs, float);
        }
        template<class Y>
        void angle_property(const rb_string& name, const rb_string& display_name, const bool editable, const bool continuous, const uint32_t precision, const accessors<Y, float> code){
            NEW_PROPERTY(Y, type_descriptor::angle(), name, display_name, editable, continuous, precision, true, float_accs, float);
        }
        template<class Y>
        void color_property(const rb_string& name, const rb_string& display_name, const bool editable, const bool continuous, const accessors<Y, color> code){
            NEW_PROPERTY(Y, type_descriptor::color(), name, display_name, editable, continuous, 0, true, color_accs, color);
        }
        template<class Y>
        void image_property(const rb_string& name, const rb_string& display_name, const bool editable, const accessors<Y, rb_string> code){
            NEW_PROPERTY(Y, type_descriptor::image(), name, display_name, editable, false, 0, true, str_accs, rb_string);
        }
        template<class Y, class T>
        void enumeration_property(const rb_string& name, const rb_string& display_name, const std::map<rb_string, T>& enumeration_map, const bool editable, const accessors<Y, T> code){
            //create the enumeration type descriptor...
            class type_descriptor* _td_enum = new class type_descriptor();
            if(dynamic_type())
                _associated_types.insert(_td_enum);
            _td_enum->_display_name = display_name;
            for (auto& _p : enumeration_map){
                _td_enum->_enum_map[_p.first] = (uint32_t)_p.second;
            }
            _td_enum->_gtd = general_type_descriptor::enumeration;
            _td_enum->_name = type_name() + rb_string(u"_%_enum_") + name;
            _td_enum->_nullable = false;
            new_property<Y>(_td_enum, name, display_name, editable, false, 0, true, [=](class type_descriptor::property_info& p){
                p.int_accs = {
                    [=](const typed_object* site){
                        return (long)code.first(dynamic_cast<const Y*>(site));
                    },
                    [=](typed_object* site, long value){
                        code.second(dynamic_cast<Y*>(site), (T)value);
                    }
                };
            });
        }
        template<class Y>
        void string_with_autocompletion_property(const rb_string& name, const rb_string& display_name, const bool editable, const accessors<Y, rb_string>& code, const autocompletion_accessor<Y>& autocompletion){
            NEW_PROPERTY(Y, type_descriptor::string_with_autocompletion(), name, display_name, editable, false, 0, true, str_accs, rb_string);
            if(_current_type_constr->current_branch){
                _current_type_constr->current_branch->_properties[name].autocompletion_function = [=](const typed_object* obj){
                    return autocompletion(dynamic_cast<const Y*>(obj));
                };
            }
            else {
                _current_type_constr->cached_type_descriptor->_properties[name].autocompletion_function = [=](const typed_object* obj){
                    return autocompletion(dynamic_cast<const Y*>(obj));
                };
            }
        }
        template<class Y, class T>
        void flags_property(const rb_string& name, const rb_string& display_name, const std::vector<rb_string>& flags, const bool editable, const accessors<Y, T> code){
            assert(flags.size() <= 4 && flags.size() != 0);
            class type_descriptor* _td_flags = new class type_descriptor();
            if(dynamic_type())
                _associated_types.insert(_td_flags);
            _td_flags->_display_name = display_name;
            long _order = 0;
            for (auto& _p : flags){
                _td_flags->_enum_map[_p] = _order;
                _order++;
            }
            _td_flags->_gtd = general_type_descriptor::flags;
            _td_flags->_name = type_name() + rb_string(u"_%_flags_") + name;
            _td_flags->_nullable = false;
            new_property<Y>(_td_flags, name, display_name, editable, false, 0, true, [=](class type_descriptor::property_info& p){
                p.int_accs = {
                    [=](const typed_object* site){
                        return (long)code.first(dynamic_cast<const Y*>(site));
                    },
                    [=](typed_object* site, long value){
                        code.second(dynamic_cast<Y*>(site), (T)value);
                    }
                };
            });
        }
        template<class Y>
        void action(const rb_string& name, const rb_string& display_name, const action_flags flags, const std::vector<rb_string>& action_names, const action_accessor<Y>& handler){
            assert(action_names.size() <= 4 && action_names.size() != 0);
            class type_descriptor* _td_action = new class type_descriptor();
            if(dynamic_type())
                _associated_types.insert(_td_action);
            _td_action->_display_name = display_name;
            _td_action->_action_flags = flags;
            long _order = 0;
            for (auto& _p : action_names){
                _td_action->_enum_map[_p] = _order;
                _order++;
            }
            _td_action->_gtd = general_type_descriptor::action;
            _td_action->_name = type_name() + rb_string(u"_%_action_") + name;
            _td_action->_nullable = false;
            new_property<Y>(_td_action, name, display_name, true, false, 0, true, [=](class type_descriptor::property_info& p){
                p.action_accss = [=](typed_object* obj, const rb_string& action_name){
                    handler(dynamic_cast<Y*>(obj), action_name);
                };
            });
        }
        //nullable ones
        template<class Y>
        void nullable_buffer_property(const rb_string& name, const rb_string& display_name, const accessors<Y, nullable<buffer>>& code){
            NEW_PROPERTY(Y, type_descriptor::nullable_buffer(), name, display_name, false, false, 0, true, null_buff_accs, nullable<buffer>);
        }
        template<class Y>
        void nullable_string_property(const rb_string& name, const rb_string& display_name, const bool editable, const bool continuous, const accessors<Y, nullable<rb_string>>& code){
            NEW_PROPERTY(Y, type_descriptor::nullable_string(), name, display_name, editable, continuous, 0, true, null_str_accs, nullable<rb_string>);
        }
        template<class Y>
        void nullable_integer_property(const rb_string& name, const rb_string& display_name, const bool editable, const accessors<Y, nullable<long>> code){
            NEW_PROPERTY(Y, type_descriptor::nullable_integer(), name, display_name, editable, false, 0, true, null_int_accs, nullable<long>);
        }
        template<class Y>
        void nullable_single_property(const rb_string& name, const rb_string& display_name, const bool editable, const accessors<Y, nullable<float>> code){
            NEW_PROPERTY(Y, type_descriptor::nullable_single(), name, display_name, editable, false, 5, true, null_float_accs, nullable<float>);
        }
        template<class Y>
        void nullable_vec2_property(const rb_string& name, const rb_string& display_name, const bool editable, const accessors<Y, nullable<vec2>> code){
            NEW_PROPERTY(Y, type_descriptor::nullable_vec2(), name, display_name, editable, false, 5, true, null_vec2_accs, nullable<vec2>);
        }
        template<class Y>
        void nullable_boolean_property(const rb_string& name, const rb_string& display_name, const bool editable, const accessors<Y, nullable<bool>> code){
            NEW_PROPERTY(Y, type_descriptor::nullable_boolean(), name, display_name, editable, false, 0, true, null_bool_accs, nullable<bool>);
        }
        template<class Y>
        void nullable_ranged_property(const rb_string& name, const rb_string& display_name, const bool editable, const bool continuous, const uint32_t precision, const accessors<Y, nullable<float>> code){
            NEW_PROPERTY(Y, type_descriptor::nullable_ranged(), name, display_name, editable, continuous, precision, true, null_float_accs, nullable<float>);
        }
        template<class Y>
        void nullable_angle_property(const rb_string& name, const rb_string& display_name, const bool editable, const bool continuous, const uint32_t precision, const accessors<Y, nullable<float>> code){
            NEW_PROPERTY(Y, type_descriptor::nullable_angle(), name, display_name, editable, continuous, precision, true, null_float_accs, nullable<float>);
        }
        template<class Y>
        void nullable_color_property(const rb_string& name, const rb_string& display_name, const bool editable, const bool continuous, const accessors<Y, nullable<color>> code){
            NEW_PROPERTY(Y, type_descriptor::nullable_color(), name, display_name, editable, continuous, 0, true, null_color_accs, nullable<color>);
        }
        template<class Y>
        void nullable_image_property(const rb_string& name, const rb_string& display_name, const bool editable, const accessors<Y, nullable<rb_string>> code){
            NEW_PROPERTY(Y, type_descriptor::nullable_image(), name, display_name, editable, false, 0, true, null_str_accs, nullable<rb_string>);
        }
        template<class Y, class T>
        void nullable_enumeration_property(const rb_string& name, const rb_string& display_name, const std::map<rb_string, T>& enumeration_map, const bool editable, const accessors<Y, nullable<T>> code){
            //create the enumeration type descriptor...
            class type_descriptor* _td_enum = new class type_descriptor();
            _td_enum->_display_name = display_name;
            if(dynamic_type())
                _associated_types.insert(_td_enum);
            for (auto& _p : enumeration_map){
                _td_enum->_enum_map[_p.first] = (long)_p.second;
            }
            _td_enum->_gtd = general_type_descriptor::enumeration;
            _td_enum->_name = type_name() + rb_string(u"_%_n_enum_") + name;
            _td_enum->_nullable = true;
            new_property<Y>(_td_enum, name, display_name, editable, false, 0, true, [=](class type_descriptor::property_info& p){
                p.null_int_accs = {
                    [=](const typed_object* site){
                        rb::nullable<T> _r = code.first(dynamic_cast<const Y*>(site));
                        if(!_r.has_value())
                            return (rb::nullable<long>)nullptr;
                        else
                            return rb::nullable<long>((long)_r.value());
                    },
                    [=](typed_object* site, rb::nullable<long> value){
                        if(!value.has_value())
                            code.second(dynamic_cast<Y*>(site), nullptr);
                        else
                            code.second(dynamic_cast<Y*>(site), rb::nullable<T>((T)value.value()));
                    }
                };
            });
        }
        template<class Y>
        void nullable_string_with_autocompletion_property(const rb_string& name, const rb_string& display_name, const bool editable, const accessors<Y, nullable<rb_string>>& code, const autocompletion_accessor<Y>& autocompletion){
            NEW_PROPERTY(Y, type_descriptor::nullable_string_with_autocompletion(), name, display_name, editable, false, 0, true, null_str_accs, nullable<rb_string>);
            if(_current_type_constr->current_branch){
                _current_type_constr->current_branch->_properties[name].autocompletion_function = [=](const typed_object* obj){
                    return autocompletion(dynamic_cast<const Y*>(obj));
                };
            }
            else {
                _current_type_constr->cached_type_descriptor->_properties[name].autocompletion_function = [=](const typed_object* obj){
                    return autocompletion(dynamic_cast<const Y*>(obj));
                };
            }
        }
        template<class Y, class T>
        void nullable_flags_property(const rb_string& name, const rb_string& display_name, const std::vector<rb_string>& flags, const bool editable, const accessors<Y, nullable<T>> code){
            assert(flags.size() <= 4 && flags.size() != 0);
            class type_descriptor* _td_flags = new class type_descriptor();
            if(dynamic_type())
                _associated_types.insert(_td_flags);
            _td_flags->_display_name = display_name;
            long _order = 0;
            for (auto& _p : flags){
                _td_flags->_enum_map[_p] = _order;
                _order++;
            }
            _td_flags->_gtd = general_type_descriptor::flags;
            _td_flags->_name = type_name() + rb_string(u"_%_n_flags_") + name;
            _td_flags->_nullable = true;
            new_property<Y>(_td_flags, name, display_name, editable, false, 0, true, [=](class type_descriptor::property_info& p){
                p.null_int_accs = {
                    [=](const typed_object* site){
                        rb::nullable<T> _r = code.first(dynamic_cast<const Y*>(site));
                        if(!_r.has_value())
                            return (rb::nullable<long>)nullptr;
                        else
                            return rb::nullable<long>((long)_r.value());
                    },
                    [=](typed_object* site, rb::nullable<long> value){
                        if(!value.has_value())
                            code.second(dynamic_cast<Y*>(site), nullptr);
                        else
                            code.second(dynamic_cast<Y*>(site), rb::nullable<T>((T)value.value()));
                    }
                };
            });
        }
        //object
        template<class Y, class T>
        void object_property(const rb_string& name, const rb_string& display_name, bool inlined, accessors<Y, T*> code){
            //get type descriptor from object...
            auto _obj = new T();
            const class type_descriptor* _td = _obj->type_descriptor();
            delete _obj;
            new_property<Y>(_td, name, display_name, true, false, 0, inlined, [=](class type_descriptor::property_info& p){
                p.null_obj_accss = {
                    [=](const typed_object* site){
                        return code.first(dynamic_cast<const Y*>(site));
                    }, 
                    [=](typed_object* site, typed_object* value){
                        code.second(dynamic_cast<Y*>(site), dynamic_cast<T*>(value));
                    } 
                }; 
            });
        }
        template<class Y, class T>
        void nullable_object_property(const rb_string& name, const rb_string& display_name, accessors<Y, T*> code){
            //get type descriptor from object...
            auto _obj = new T();
            const class type_descriptor* _td = _obj->nullable_type_descriptor();
            delete _obj;
            new_property<Y>(_td, name, display_name, true, false, 0, false, [=](class type_descriptor::property_info& p){
                p.null_obj_accss = {
                    [=](const typed_object* site){
                        return code.first(dynamic_cast<const Y*>(site));
                    },
                    [=](typed_object* site, typed_object* value){
                        code.second(dynamic_cast<Y*>(site), dynamic_cast<T*>(value));
                    }
                };
            });
        }
        //non generic function
        template<class Y>
        void object_property(const rb_string& name, const rb_string& display_name, const class type_descriptor* object_type,
                             bool inlined, accessors<Y, typed_object*> code){
            assert(object_type);
            assert(!object_type->nullable());
            //get type descriptor from object...
            const class type_descriptor* _td = object_type;
            
            new_property<Y>(_td, name, display_name, true, false, 0, inlined, [=](class type_descriptor::property_info& p){
                p.null_obj_accss = {
                    [=](const typed_object* site){
                        return code.first(dynamic_cast<const Y*>(site));
                    },
                    [=](typed_object* site, typed_object* value){
                        code.second(dynamic_cast<Y*>(site), dynamic_cast<typed_object*>(value));
                    }
                };
            });
        }
        template<class Y>
        void nullable_object_property(const rb_string& name, const rb_string& display_name, const class type_descriptor* object_type, accessors<Y, typed_object*> code){
            assert(object_type);
            assert(object_type->nullable());
            //get type descriptor from object...
            const class type_descriptor* _td = object_type;

            new_property<Y>(_td, name, display_name, true, false, 0, false, [=](class type_descriptor::property_info& p){
                p.null_obj_accss = {
                    [=](const typed_object* site){
                        return code.first(dynamic_cast<const Y*>(site));
                    },
                    [=](typed_object* site, typed_object* value){
                        code.second(dynamic_cast<Y*>(site), dynamic_cast<typed_object*>(value));
                    }
                };
            });
        }
        //vector
        //it's implementor responsibility to clear removed objects...
        template<class Y, class T>
        void vector_property(const rb_string& name, const rb_string& display_name, accessors<Y, std::vector<T*>> code){
            //get type descriptor from object...
            auto _obj = new T();
            const class type_descriptor* _td = _obj->vector_type_descriptor();
            delete _obj;
            new_property<Y>(_td, name, display_name, true, false, 0, false, [=](class type_descriptor::property_info& p){
                p.vector_accs = {
                    [=](const typed_object* site){
                        std::vector<T*> _pointers = code.first(dynamic_cast<const Y*>(site));
                        
                        std::vector<typed_object*> _new;
                        for (auto item : _pointers){
                            _new.push_back((typed_object*)item);
                        }
                        
                        return _new;
                    },
                    [=](typed_object* site, const std::vector<typed_object*> value){
                        std::vector<T*> _new;
                        
                        for (auto item : value){
                            _new.push_back(dynamic_cast<T*>(item));
                        }
                        code.second(dynamic_cast<Y*>(site), _new);
                    }
                };
            });
        }
        //non generic function
        template<class Y>
        void vector_property(const rb_string& name, const rb_string& display_name, const class type_descriptor* vector_type, accessors<Y, std::vector<typed_object*>> code){
            assert(vector_type);
            assert(vector_type->gtd() == rb::general_type_descriptor::vector);
            //get type descriptor from object...
            const class type_descriptor* _td = vector_type;
            new_property<Y>(_td, name, display_name, true, false, 0, false, [=](class type_descriptor::property_info& p){
                p.vector_accs = {
                    [=](const typed_object* site){
                        return code.first(dynamic_cast<const Y*>(site));
                    },
                    [=](typed_object* site, const std::vector<typed_object*>& value){
                        code.second(dynamic_cast<Y*>(site), value);
                    }
                };
            });
        }
    public:
        //Function to changed and set tag
        virtual const rb_string& branch_tag() const;
        virtual const rb_string& branch_tag(const rb_string& value);
        void shutdown_notifications(std::function<void (void)> region);
    protected:
        //property changed
        virtual void branch_tag_changed();
        void notify_property_changed(const rb_string& name);
        void force_notify_property_changed(const rb_string& name);
    protected:
        //vector changed
        static std::vector<typed_object*> removed_objects(const std::vector<typed_object*>& previous, const std::vector<typed_object*>& now);
        static std::vector<typed_object*> added_objects(const std::vector<typed_object*>& previous, const std::vector<typed_object*>& now);
        static void free_removed_objects(const std::vector<typed_object*>& previous, const std::vector<typed_object*>& now, std::function<void (typed_object*)> deleter = std::function<void (typed_object*)>());
        //object changed
        static void free_old_object(const typed_object* old_obj, const typed_object* new_obj);
    public:
        void notify_atlas_changed();
    public:
        //constructor
        typed_object();
        virtual ~typed_object();
        
        //type descriptor
        const type_descriptor* type_descriptor() const;
        const class type_descriptor* nullable_type_descriptor() const;
        const class type_descriptor* vector_type_descriptor() const;
    public:
        //adding observer
        void add_observer(observer* o);
        void remove_observer(observer* o);
        //clone
    private:
        typed_object* clone(std::unordered_map<typed_object*, typed_object*>& already_cloned) const;
    public:
        virtual typed_object* clone() const;
        
        //bridge
#ifdef _OBJC_BRIDGE_
        void set_bridge(BRIGE_TYPE bridge);
        BRIGE_TYPE get_bridge();
#endif
    protected:
        virtual void fill_vector_with_children(std::vector<typed_object*>& children) const;
        virtual void set_children(const std::vector<typed_object*>& children);
        virtual bool should_serialize_children() const;
        virtual void begin_serialization();
        virtual void end_serialization();
        virtual void was_deserialized();
    private:
        void internal_begin_serialization();
        void internal_end_serialization();
        void internal_was_deserialized();
    public:
        friend class scene_loader;
        void serialize_to_url(const rb_string& url);
        rb_string serialize_to_string();
        static typed_object* deserialize_from_url(const rb_string& url);
        static typed_object* deserialize_from_string(const rb_string& str);
    };
}

#endif /* defined(__RollerBallCore__typed_object__) */
