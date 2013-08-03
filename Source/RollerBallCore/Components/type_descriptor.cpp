//
//  type_descriptor.cpp
//  RollerBallCore
//
//  Created by Danilo Santos de Carvalho on 27/02/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#include "type_descriptor.h"
#include "generic_object.h"
#include "typed_object.h"

using namespace rb;

std::map<rb_string, const type_descriptor*> type_descriptor::_registered_types;

const class rb::type_descriptor::property_info& type_descriptor::_property_info(const rb_string &name) const {
    if(_gtd == general_type_descriptor::branch){
        for(auto& _p : _union_map){
            if(_p.second->_properties.count(name) != 0)
                return _p.second->_properties.at(name);
        }
        
        throw std::exception(); //should not get there
    }
    else {
        return _properties.at(name);
    }
}

type_descriptor::type_descriptor(){
    _gtd = general_type_descriptor::string;
    _element_type = nullptr;
    _nullable = false;
    _constructor = []() { return nullptr; };
    _action_flags = action_flags::multi_dispatch;
    _dynamic_type = false;
}

void type_descriptor::get_all_properties(std::vector<rb_string> &properties) const {
    if(_gtd == general_type_descriptor::branch){
        for(auto& _p : _union_map){
            for (auto& _p_n : _p.second->_properties_decl_order){
                properties.push_back(_p_n);
            }
        }
    }
    else {
        for (auto& _p_n : _properties_decl_order){
            properties.push_back(_p_n);
        }
    }
}

bool type_descriptor::has_property(const rb_string &name) const {
    if(_gtd == general_type_descriptor::branch){
        return std::any_of(_union_map.begin(), _union_map.end(), [&](const std::pair<rb_string, type_descriptor*>& o){
            return o.second->has_property(name);
        });
    }
    else
        return _properties.count(name) != 0;
}

const rb_string& type_descriptor::property_display_name(const rb_string &name) const {
    assert(has_property(name));
    return _property_info(name).display_name;
}

const type_descriptor* type_descriptor::property_type(const rb_string &name) const {
    assert(has_property(name));
    return _property_info(name).type;
}

bool type_descriptor::property_editable(const rb_string &name) const {
    assert(has_property(name));
    return _property_info(name).editable;
}

bool type_descriptor::property_private(const rb_string &name) const {
    assert(has_property(name));
    return _property_info(name).is_private;
}

bool type_descriptor::property_continuous(const rb_string &name) const {
    assert(has_property(name));
    return _property_info(name).continuous;
}

bool type_descriptor::property_inlined(const rb_string &name) const {
    assert(has_property(name));
    return _property_info(name).inlined;
}

uint32_t type_descriptor::property_precision(const rb_string &name) const {
    assert(has_property(name));
    return _property_info(name).precision;
}

enum action_flags type_descriptor::action_flags() const {
    return _action_flags;
}

std::vector<rb_string> type_descriptor::get_autocomplete_suggestions(const typed_object* site, const rb_string& name) const {
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::string_with_autocompletion);
    
    return _property_info(name).autocompletion_function(site);
}

generic_object type_descriptor::get_property(const rb::typed_object *site, const rb_string &name) const {
    assert(has_property(name));
    
    auto _p_gtd = _property_info(name).type->gtd();
    assert(_p_gtd != general_type_descriptor::action);
    
    if(_p_gtd == general_type_descriptor::angle){
        return get_nullable_single_property(site, name);
    }
    else if(_p_gtd == general_type_descriptor::buffer){
        return get_nullable_buffer_property(site, name);
    }
    else if(_p_gtd == general_type_descriptor::boolean){
        return get_nullable_boolean_property(site, name);
    }
    else if(_p_gtd == general_type_descriptor::branch){
        return get_object_property<typed_object>(site, name);
    }
    else if(_p_gtd == general_type_descriptor::color){
        return get_nullable_color_property(site, name);
    }
    else if(_p_gtd == general_type_descriptor::enumeration){
        return get_nullable_integer_property(site, name);
    }
    else if(_p_gtd == general_type_descriptor::image){
        return get_nullable_string_property(site, name);
    }
    else if(_p_gtd == general_type_descriptor::integer){
        return get_nullable_integer_property(site, name);
    }
    else if(_p_gtd == general_type_descriptor::object){
        return get_object_property<typed_object>(site, name);
    }
    else if(_p_gtd == general_type_descriptor::ranged){
        return get_nullable_single_property(site, name);
    }
    else if(_p_gtd == general_type_descriptor::single){
        return get_nullable_single_property(site, name);
    }
    else if(_p_gtd == general_type_descriptor::string){
        return get_nullable_string_property(site, name);
    }
    else if(_p_gtd == general_type_descriptor::vec2){
        return get_nullable_vec2_property(site, name);
    }
    else if(_p_gtd == general_type_descriptor::flags){
        return get_nullable_integer_property(site, name);
    }
    else if(_p_gtd == general_type_descriptor::string_with_autocompletion){
        return get_nullable_string_property(site, name);
    }
    else { //vector
        return get_vector_property<typed_object>(site, name);
    }
}

buffer type_descriptor::get_buffer_property(const typed_object* site, const rb_string& name) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::buffer);
    
    if (_property_info(name).type->nullable()){
        auto _value = _property_info(name).null_buff_accs.first(site);
        assert(_value.has_value());
        return _value.value();
    }
    else {
        return _property_info(name).buff_accs.first(site);
    }
}

rb_string type_descriptor::get_string_property(const typed_object* site, const rb_string& name) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::string || _property_info(name).type->gtd() == general_type_descriptor::image || _property_info(name).type->gtd() == general_type_descriptor::enumeration || _property_info(name).type->gtd() == general_type_descriptor::string_with_autocompletion);
    
    if (_property_info(name).type->gtd() == general_type_descriptor::enumeration){
        long value = get_integer_property(site, name);
        for (auto& _m : _property_info(name).type->enumeration_values()){
            if(_m.second == value)
                return _m.first;
        }
        //never should get here...
        return u"";
    }
    else {
        if (_property_info(name).type->nullable()){
            auto _value = _property_info(name).null_str_accs.first(site);
            assert(_value.has_value());
            return _value.value();
        }
        else {
            return _property_info(name).str_accs.first(site);
        }
    }
}
long type_descriptor::get_integer_property(const typed_object* site, const rb_string& name) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::integer || _property_info(name).type->gtd() == general_type_descriptor::enumeration || _property_info(name).type->gtd() == general_type_descriptor::flags);
    
    if (_property_info(name).type->nullable()){
        auto _value = _property_info(name).null_int_accs.first(site);
        assert(_value.has_value());
        return _value.value();
    }
    else {
        return _property_info(name).int_accs.first(site);
    }
}
float type_descriptor::get_single_property(const typed_object* site, const rb_string& name) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::single || _property_info(name).type->gtd() == general_type_descriptor::ranged || _property_info(name).type->gtd() == general_type_descriptor::angle);
    
    if (_property_info(name).type->nullable()){
        auto _value = _property_info(name).null_float_accs.first(site);
        assert(_value.has_value());
        return _value.value();
    }
    else {
        return _property_info(name).float_accs.first(site);
    }
}
vec2 type_descriptor::get_vec2_property(const typed_object* site, const rb_string& name) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::vec2);
    
    if (_property_info(name).type->nullable()){
        auto _value = _property_info(name).null_vec2_accs.first(site);
        assert(_value.has_value());
        return _value.value();
    }
    else {
        return _property_info(name).vec2_accs.first(site);
    }
}
bool type_descriptor::get_boolean_property(const typed_object* site, const rb_string& name) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::boolean);
    
    if (_property_info(name).type->nullable()){
        auto _value = _property_info(name).null_bool_accs.first(site);
        assert(_value.has_value());
        return _value.value();
    }
    else {
        return _property_info(name).bool_accs.first(site);
    }
}
color type_descriptor::get_color_property(const typed_object* site, const rb_string& name) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::color);
    
    if (_property_info(name).type->nullable()){
        auto _value = _property_info(name).null_color_accs.first(site);
        assert(_value.has_value());
        return _value.value();
    }
    else {
        return _property_info(name).color_accs.first(site);
    }
}
rb::nullable<rb_string> type_descriptor::get_nullable_string_property(const typed_object* site, const rb_string& name) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::string || _property_info(name).type->gtd() == general_type_descriptor::image || _property_info(name).type->gtd() == general_type_descriptor::enumeration || _property_info(name).type->gtd() == general_type_descriptor::string_with_autocompletion);
    
    if(_property_info(name).type->gtd() == general_type_descriptor::enumeration){
        rb::nullable<long> value = get_nullable_integer_property(site, name);
        if(!value.has_value())
            return nullptr;
        
        for (auto& _m : _property_info(name).type->enumeration_values()){
            if(_m.second == value.value())
                return _m.first;
        }
        //never should get here...
        return rb_string(u"");
    }
    else {
        if (_property_info(name).type->nullable()){
            return _property_info(name).null_str_accs.first(site);
        }
        else {
            return _property_info(name).str_accs.first(site);
        }
    }
}
rb::nullable<long> type_descriptor::get_nullable_integer_property(const typed_object* site, const rb_string& name) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::integer || _property_info(name).type->gtd() == general_type_descriptor::enumeration || _property_info(name).type->gtd() == general_type_descriptor::flags);
    
    if (_property_info(name).type->nullable()){
        return _property_info(name).null_int_accs.first(site);
    }
    else {
        return _property_info(name).int_accs.first(site);
    }
}
rb::nullable<buffer> type_descriptor::get_nullable_buffer_property(const typed_object* site, const rb_string& name) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::buffer);
    
    if (_property_info(name).type->nullable()){
        return _property_info(name).null_buff_accs.first(site);
    }
    else {
        return _property_info(name).buff_accs.first(site);
    }
}
rb::nullable<float> type_descriptor::get_nullable_single_property(const typed_object* site, const rb_string& name) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::single || _property_info(name).type->gtd() == general_type_descriptor::angle || _property_info(name).type->gtd() == general_type_descriptor::ranged);
    
    if (_property_info(name).type->nullable()){
        return _property_info(name).null_float_accs.first(site);
    }
    else {
        return _property_info(name).float_accs.first(site);
    }
}
rb::nullable<vec2> type_descriptor::get_nullable_vec2_property(const typed_object* site, const rb_string& name) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::vec2);
    
    if (_property_info(name).type->nullable()){
        return _property_info(name).null_vec2_accs.first(site);
    }
    else {
        return _property_info(name).vec2_accs.first(site);
    }
}
rb::nullable<bool> type_descriptor::get_nullable_boolean_property(const typed_object* site, const rb_string& name) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::boolean);
    
    if (_property_info(name).type->nullable()){
        return _property_info(name).null_bool_accs.first(site);
    }
    else {
        return _property_info(name).bool_accs.first(site);
    }
}
rb::nullable<color> type_descriptor::get_nullable_color_property(const typed_object* site, const rb_string& name) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::color);
    
    if (_property_info(name).type->nullable()){
        return _property_info(name).null_color_accs.first(site);
    }
    else {
        return _property_info(name).color_accs.first(site);
    }
}

void type_descriptor::set_property(rb::typed_object *site, const rb_string &name, const rb::generic_object &value) const {
    assert(has_property(name));
    
    auto _p_gtd = _property_info(name).type->gtd();
    assert(_p_gtd != general_type_descriptor::action);
    
    if(_p_gtd == general_type_descriptor::angle){
        set_nullable_single_property(site, name, value);
    }
    else if(_p_gtd == general_type_descriptor::buffer){
        set_nullable_buffer_property(site, name, value);
    }
    else if(_p_gtd == general_type_descriptor::boolean){
        set_nullable_boolean_property(site, name, value);
    }
    else if(_p_gtd == general_type_descriptor::branch){
        set_object_property<typed_object>(site, name, value);
    }
    else if(_p_gtd == general_type_descriptor::color){
        set_nullable_color_property(site, name, value);
    }
    else if(_p_gtd == general_type_descriptor::enumeration){
        if(value.is_null() || value.type() == generic_object_type::integer)
            set_nullable_integer_property(site, name, value);
        else
            set_nullable_string_property(site, name, value);
    }
    else if(_p_gtd == general_type_descriptor::image){
        set_nullable_string_property(site, name, value);
    }
    else if(_p_gtd == general_type_descriptor::integer){
        set_nullable_integer_property(site, name, value);
    }
    else if(_p_gtd == general_type_descriptor::object){
        set_object_property<typed_object>(site, name, value);
    }
    else if(_p_gtd == general_type_descriptor::ranged){
        set_nullable_single_property(site, name, value);
    }
    else if(_p_gtd == general_type_descriptor::single){
        set_nullable_single_property(site, name, value);
    }
    else if(_p_gtd == general_type_descriptor::string){
        set_nullable_string_property(site, name, value);
    }
    else if(_p_gtd == general_type_descriptor::vec2){
        set_nullable_vec2_property(site, name, value);
    }
    else if(_p_gtd == general_type_descriptor::flags){
        set_nullable_integer_property(site, name, value);
    }
    else if(_p_gtd == general_type_descriptor::string_with_autocompletion){
        set_nullable_string_property(site, name, value);
    }
    else { //vector
        set_vector_property<typed_object>(site, name, value);
    }
}

void type_descriptor::set_string_property(typed_object* site, const rb_string& name, const rb_string& value) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::string || _property_info(name).type->gtd() == general_type_descriptor::image || _property_info(name).type->gtd() == general_type_descriptor::enumeration || _property_info(name).type->gtd() == general_type_descriptor::string_with_autocompletion);
    
    if(_property_info(name).type->gtd() == general_type_descriptor::enumeration){
        _property_info(name).int_accs.second(site, _property_info(name).type->enumeration_values().at(value));
    }
    else {
        if (_property_info(name).type->nullable())
            _property_info(name).null_str_accs.second(site, value);
        else
            _property_info(name).str_accs.second(site, value);
    }
}
void type_descriptor::set_buffer_property(typed_object* site, const rb_string& name, class buffer value) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::buffer);
    
    if (_property_info(name).type->nullable())
        _property_info(name).null_buff_accs.second(site, value);
    else
        _property_info(name).buff_accs.second(site, value);
}
void type_descriptor::set_integer_property(typed_object* site, const rb_string& name, const long value) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::integer || _property_info(name).type->gtd() == general_type_descriptor::enumeration || _property_info(name).type->gtd() == general_type_descriptor::flags);
    
    if (_property_info(name).type->nullable())
        _property_info(name).null_int_accs.second(site, value);
    else
        _property_info(name).int_accs.second(site, value);
}
void type_descriptor::set_single_property(typed_object* site, const rb_string& name, const float value) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::single || _property_info(name).type->gtd() == general_type_descriptor::ranged || _property_info(name).type->gtd() == general_type_descriptor::angle);
    
    if (_property_info(name).type->nullable())
        _property_info(name).null_float_accs.second(site, value);
    else
        _property_info(name).float_accs.second(site, value);
}
void type_descriptor::set_vec2_property(typed_object* site, const rb_string& name, const class vec2& value) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::vec2);
    
    if (_property_info(name).type->nullable())
        _property_info(name).null_vec2_accs.second(site, value);
    else
        _property_info(name).vec2_accs.second(site, value);
}
void type_descriptor::set_boolean_property(typed_object* site, const rb_string& name, const bool value) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::boolean);
    
    if (_property_info(name).type->nullable())
        _property_info(name).null_bool_accs.second(site, value);
    else
        _property_info(name).bool_accs.second(site, value);
}
void type_descriptor::set_color_property(typed_object* site, const rb_string& name, const class color& value) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::color);
    
    if (_property_info(name).type->nullable())
        _property_info(name).null_color_accs.second(site, value);
    else
        _property_info(name).color_accs.second(site, value);
}
void type_descriptor::set_nullable_string_property(typed_object* site, const rb_string& name, const rb::nullable<rb_string> value) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::string || _property_info(name).type->gtd() == general_type_descriptor::image || _property_info(name).type->gtd() == general_type_descriptor::enumeration || _property_info(name).type->gtd() == general_type_descriptor::string_with_autocompletion);
    
    if(_property_info(name).type->gtd() == general_type_descriptor::enumeration){
        if (_property_info(name).type->nullable()){
            if(value.has_value())
                _property_info(name).null_int_accs.second(site, _property_info(name).type->enumeration_values().at(value.value()));
            else
                _property_info(name).null_int_accs.second(site, nullptr);
        }
        else {
            assert(value.has_value());
            _property_info(name).int_accs.second(site, _property_info(name).type->enumeration_values().at(value.value()));
        }
    }
    else {
        if (_property_info(name).type->nullable())
            _property_info(name).null_str_accs.second(site, value);
        else {
            if(_property_info(name).type->gtd() == general_type_descriptor::image){
                if(value.has_value())
                    _property_info(name).str_accs.second(site, value.value());
                else
                    _property_info(name).str_accs.second(site, u"");
            }
            else {
                assert(value.has_value());
                _property_info(name).str_accs.second(site, value.value());
            }
        }
    }
}
void type_descriptor::set_nullable_buffer_property(typed_object* site, const rb_string& name, const rb::nullable<class buffer> value) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::buffer);
    
    if (_property_info(name).type->nullable())
        _property_info(name).null_buff_accs.second(site, value);
    else {
        assert(value.has_value());
        _property_info(name).buff_accs.second(site, value.value());
    }
}
void type_descriptor::set_nullable_integer_property(typed_object* site, const rb_string& name, const rb::nullable<long> value) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::integer || _property_info(name).type->gtd() == general_type_descriptor::enumeration || _property_info(name).type->gtd() == general_type_descriptor::flags);
    
    if (_property_info(name).type->nullable())
        _property_info(name).null_int_accs.second(site, value);
    else {
        assert(value.has_value());
        _property_info(name).int_accs.second(site, value.value());
    }
}
void type_descriptor::set_nullable_single_property(typed_object* site, const rb_string& name, const rb::nullable<float> value) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::single || _property_info(name).type->gtd() == general_type_descriptor::ranged || _property_info(name).type->gtd() == general_type_descriptor::angle);
    
    if (_property_info(name).type->nullable())
        _property_info(name).null_float_accs.second(site, value);
    else {
        assert(value.has_value());
        _property_info(name).float_accs.second(site, value.value());
    }
}
void type_descriptor::set_nullable_vec2_property(typed_object* site, const rb_string& name, const rb::nullable<class vec2> value) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::vec2);
    
    if (_property_info(name).type->nullable())
        _property_info(name).null_vec2_accs.second(site, value);
    else {
        assert(value.has_value());
        _property_info(name).vec2_accs.second(site, value.value());
    }
}
void type_descriptor::call_action(rb::typed_object *site, const rb_string& property_name, const rb_string &action_name) const {
    assert(has_property(property_name));
    assert(_property_info(property_name).type->gtd() == general_type_descriptor::action);
    
    _property_info(property_name).action_accss(site, action_name);
}
void type_descriptor::set_nullable_boolean_property(typed_object* site, const rb_string& name, const rb::nullable<bool> value) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::boolean);
    
    if (_property_info(name).type->nullable())
        _property_info(name).null_bool_accs.second(site, value);
    else {
        assert(value.has_value());
        _property_info(name).bool_accs.second(site, value.value());
    }
}
void type_descriptor::set_nullable_color_property(typed_object* site, const rb_string& name, const rb::nullable<class color> value) const{
    assert(has_property(name));
    assert(_property_info(name).type->gtd() == general_type_descriptor::color);
    
    if (_property_info(name).type->nullable())
        _property_info(name).null_color_accs.second(site, value);
    else {
        assert(value.has_value());
        _property_info(name).color_accs.second(site, value.value());
    }
}

type_descriptor* type_descriptor::create_basic_descriptor(const rb_string& name, const rb_string& display_name, const general_type_descriptor gtd, const bool nullable){
    type_descriptor* _n = new type_descriptor();
    _n->_gtd = gtd;
    _n->_name = name;
    _n->_display_name = display_name;
    _n->_nullable = nullable;
    return _n;
}

void type_descriptor::create_stock_types(){
    if(!with_name(u"_@_basic_string")){
        _registered_types.insert({u"_@_basic_buffer", create_basic_descriptor(u"_@_basic_buffer", u"Buffer", general_type_descriptor::buffer, false)});
        _registered_types.insert({u"_@_basic_string", create_basic_descriptor(u"_@_basic_string", u"Text", general_type_descriptor::string, false)});
        _registered_types.insert({u"_@_basic_integer", create_basic_descriptor(u"_@_basic_integer", u"Integer", general_type_descriptor::integer, false)});
        _registered_types.insert({u"_@_basic_single", create_basic_descriptor(u"_@_basic_single", u"Float", general_type_descriptor::single, false)});
        _registered_types.insert({u"_@_basic_vec2", create_basic_descriptor(u"_@_basic_vec2", u"2-D Vector", general_type_descriptor::vec2, false)});
        _registered_types.insert({u"_@_basic_boolean", create_basic_descriptor(u"_@_basic_boolean", u"Boolean", general_type_descriptor::boolean, false)});
        _registered_types.insert({u"_@_basic_ranged", create_basic_descriptor(u"_@_basic_ranged", u"Range", general_type_descriptor::ranged, false)});
        _registered_types.insert({u"_@_basic_angle", create_basic_descriptor(u"_@_basic_angle", u"Angle", general_type_descriptor::angle, false)});
        _registered_types.insert({u"_@_basic_color", create_basic_descriptor(u"_@_basic_color", u"Color", general_type_descriptor::color, false)});
        _registered_types.insert({u"_@_basic_image", create_basic_descriptor(u"_@_basic_image", u"Image", general_type_descriptor::image, false)});
        _registered_types.insert({u"_@_basic_string_with_autocompletion", create_basic_descriptor(u"_@_basic_string_with_autocompletion", u"Text", general_type_descriptor::string_with_autocompletion, false)});
        
        _registered_types.insert({u"_@_basic_nullable_buffer", create_basic_descriptor(u"_@_basic_nullable_buffer", u"Buffer", general_type_descriptor::buffer, true)});
        _registered_types.insert({u"_@_basic_nullable_string", create_basic_descriptor(u"_@_basic_nullable_string", u"Text", general_type_descriptor::string, true)});
        _registered_types.insert({u"_@_basic_nullable_integer", create_basic_descriptor(u"_@_basic_nullable_integer", u"Integer", general_type_descriptor::integer, true)});
        _registered_types.insert({u"_@_basic_nullable_single", create_basic_descriptor(u"_@_basic_nullable_single", u"Single", general_type_descriptor::single, true)});
        _registered_types.insert({u"_@_basic_nullable_vec2", create_basic_descriptor(u"_@_basic_nullable_vec2", u"2-D Vector", general_type_descriptor::vec2, true)});
        _registered_types.insert({u"_@_basic_nullable_boolean", create_basic_descriptor(u"_@_basic_nullable_boolean", u"Boolean", general_type_descriptor::boolean, true)});
        _registered_types.insert({u"_@_basic_nullable_ranged", create_basic_descriptor(u"_@_basic_nullable_ranged", u"Range", general_type_descriptor::ranged, true)});
        _registered_types.insert({u"_@_basic_nullable_angle", create_basic_descriptor(u"_@_basic_nullable_angle", u"Angle", general_type_descriptor::angle, true)});
        _registered_types.insert({u"_@_basic_nullable_color", create_basic_descriptor(u"_@_basic_nullable_color", u"Color", general_type_descriptor::color, true)});
        _registered_types.insert({u"_@_basic_nullable_image", create_basic_descriptor(u"_@_basic_nullable_image", u"Image", general_type_descriptor::image, true)});
        _registered_types.insert({u"_@_basic_nullable_string_with_autocompletion", create_basic_descriptor(u"_@_basic_nullable_string_with_autocompletion", u"Text", general_type_descriptor::string_with_autocompletion, true)});
    }
}

const type_descriptor* type_descriptor::buffer() {
    create_stock_types();
    return _registered_types.at(u"_@_basic_buffer");
}

const type_descriptor* type_descriptor::string() {
    create_stock_types();
    return _registered_types.at(u"_@_basic_string");
}

const type_descriptor* type_descriptor::string_with_autocompletion() {
    create_stock_types();
    return _registered_types.at(u"_@_basic_string_with_autocompletion");
}

const type_descriptor* type_descriptor::integer() {
    create_stock_types();
    return _registered_types.at(u"_@_basic_integer");
}

const type_descriptor* type_descriptor::single() {
    create_stock_types();
    return _registered_types.at(u"_@_basic_single");
}

const type_descriptor* type_descriptor::vec2() {
    create_stock_types();
    return _registered_types.at(u"_@_basic_vec2");
}

const type_descriptor* type_descriptor::boolean() {
    create_stock_types();
    return _registered_types.at(u"_@_basic_boolean");
}

const type_descriptor* type_descriptor::ranged() {
    create_stock_types();
    return _registered_types.at(u"_@_basic_ranged");
}

const type_descriptor* type_descriptor::angle() {
    create_stock_types();
    return _registered_types.at(u"_@_basic_angle");
}

const type_descriptor* type_descriptor::color() {
    create_stock_types();
    return _registered_types.at(u"_@_basic_color");
}

const type_descriptor* type_descriptor::image() {
    create_stock_types();
    return _registered_types.at(u"_@_basic_image");
}

const type_descriptor* type_descriptor::nullable_buffer(){
    create_stock_types();
    return _registered_types.at(u"_@_basic_nullable_buffer");
}

const type_descriptor* type_descriptor::nullable_string() {
    create_stock_types();
    return _registered_types.at(u"_@_basic_nullable_string");
}

const type_descriptor* type_descriptor::nullable_string_with_autocompletion() {
    create_stock_types();
    return _registered_types.at(u"_@_basic_nullable_string_with_autocompletion");
}

const type_descriptor* type_descriptor::nullable_integer() {
    create_stock_types();
    return _registered_types.at(u"_@_basic_nullable_integer");
}

const type_descriptor* type_descriptor::nullable_single() {
    create_stock_types();
    return _registered_types.at(u"_@_basic_nullable_single");
}

const type_descriptor* type_descriptor::nullable_vec2() {
    create_stock_types();
    return _registered_types.at(u"_@_basic_nullable_vec2");
}

const type_descriptor* type_descriptor::nullable_boolean() {
    create_stock_types();
    return _registered_types.at(u"_@_basic_nullable_boolean");
}

const type_descriptor* type_descriptor::nullable_ranged() {
    create_stock_types();
    return _registered_types.at(u"_@_basic_nullable_ranged");
}

const type_descriptor* type_descriptor::nullable_angle() {
    create_stock_types();
    return _registered_types.at(u"_@_basic_nullable_angle");
}

const type_descriptor* type_descriptor::nullable_color() {
    create_stock_types();
    return _registered_types.at(u"_@_basic_nullable_color");
}

const type_descriptor* type_descriptor::nullable_image() {
    create_stock_types();
    return _registered_types.at(u"_@_basic_nullable_image");
}

void type_descriptor::register_type(const rb::type_descriptor *descriptor){
    assert(descriptor);
    assert(_registered_types.count(descriptor->name()) == 0);
    _registered_types.insert({descriptor->name(), descriptor});
}

const type_descriptor* type_descriptor::with_name(const rb_string &name){
    if(_registered_types.count(name) == 0)
        return nullptr;
    return _registered_types.at(name);
}















