//
//  generic_object.cpp
//  RollerBallCore
//
//  Created by Danilo Santos de Carvalho on 27/03/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#include "generic_object.h"
#include "typed_object.h"

using namespace rb;

generic_object::generic_object(){
    _type = generic_object_type::null;
}

generic_object::generic_object(class buffer value){
    _type = generic_object_type::buffer;
    _buffer = value;
}

generic_object::generic_object(const nullable<class buffer>& value){
    if (value.has_value()){
        _type = generic_object_type::buffer;
        _buffer = value.value();
    }
    else
        _type = generic_object_type::null;
}

generic_object::generic_object(const bool value){
    _type = generic_object_type::boolean;
    _value01.lValue = value;
}

generic_object::generic_object(const long value){
    _type = generic_object_type::integer;
    _value01.lValue = value;
}

generic_object::generic_object(const float value){
    _type = generic_object_type::single;
    _value01.fValue = value;
}

generic_object::generic_object(const vec2& value){
    _type = generic_object_type::vec2;
    _value04 = value;
}

generic_object::generic_object(const color& value){
    _type = generic_object_type::color;
    _value05 = value;
}

generic_object::generic_object(const rb_string& value){
    _type = generic_object_type::string;
    _value02 = value;
}

generic_object::generic_object(const nullable<bool>& value){
    if (value.has_value()){
        _type = generic_object_type::boolean;
        _value01.lValue = value.value();
    }
    else
        _type = generic_object_type::null;
}

generic_object::generic_object(const nullable<long>& value){
    if (value.has_value()){
        _type = generic_object_type::integer;
        _value01.lValue = value.value();
    }
    else
        _type = generic_object_type::null;
}

generic_object::generic_object(const nullable<float>& value){
    if (value.has_value()){
        _type = generic_object_type::single;
        _value01.fValue = value.value();
    }
    else
        _type = generic_object_type::null;
}

generic_object::generic_object(const nullable<vec2>& value){
    if (value.has_value()){
        _type = generic_object_type::vec2;
        _value04 = value.value();
    }
    else
        _type = generic_object_type::null;
}

generic_object::generic_object(const nullable<color>& value){
    if (value.has_value()){
        _type = generic_object_type::color;
        _value05 = value.value();
    }
    else
        _type = generic_object_type::null;
}

generic_object::generic_object(const nullable<rb_string>& value){
    if (value.has_value()){
        _type = generic_object_type::string;
        _value02 = value.value();
    }
    else
        _type = generic_object_type::null;
}

generic_object::generic_object(typed_object* value){
    if(value){
        _type = generic_object_type::object;
        _value03.push_back(value);
    }
    else {
        _type = generic_object_type::null;
    }
}

generic_object::generic_object(const std::vector<typed_object*>& value){
    _type = generic_object_type::vector;
    _value03 = value;
}

buffer generic_object::buffer_value() const {
    assert(_type == generic_object_type::buffer);
    return _buffer;
}

bool generic_object::boolean_value() const {
    assert(_type == generic_object_type::boolean);
    return _value01.lValue;
}

long generic_object::integer_value() const {
    assert(_type == generic_object_type::integer);
    return _value01.lValue;
}

float generic_object::single_value() const {
    assert(_type == generic_object_type::single);
    return _value01.fValue;
}

vec2 generic_object::vec2_value() const {
    assert(_type == generic_object_type::vec2);
    return _value04;
}

color generic_object::color_value() const {
    assert(_type == generic_object_type::color);
    return _value05;
}

rb_string generic_object::string_value() const {
    assert(_type == generic_object_type::string);
    return _value02;
}

nullable<buffer> generic_object::nullable_buffer_value() const {
    assert(_type == generic_object_type::buffer || _type == generic_object_type::null);
    return _type == generic_object_type::null ? nullptr : (nullable<buffer>)_buffer;
}
nullable<bool> generic_object::nullable_boolean_value() const{
    assert(_type == generic_object_type::boolean || _type == generic_object_type::null);
    return _type == generic_object_type::null ? nullptr : (nullable<bool>)_value01.lValue;
}
nullable<long> generic_object::nullable_integer_value() const{
    assert(_type == generic_object_type::integer || _type == generic_object_type::null);
    return _type == generic_object_type::null ? nullptr : (nullable<long>)_value01.lValue;
}
nullable<float> generic_object::nullable_single_value() const{
    assert(_type == generic_object_type::single || _type == generic_object_type::null);
    return _type == generic_object_type::null ? nullptr : (nullable<float>)_value01.fValue;
}
nullable<vec2> generic_object::nullable_vec2_value() const{
    assert(_type == generic_object_type::vec2 || _type == generic_object_type::null);
    return _type == generic_object_type::null ? nullptr : (nullable<vec2>)_value04;
}
nullable<color> generic_object::nullable_color_value() const{
    assert(_type == generic_object_type::color || _type == generic_object_type::null);
    return _type == generic_object_type::null ? nullptr : (nullable<color>)_value05;
}
nullable<rb_string> generic_object::nullable_string_value() const{
    assert(_type == generic_object_type::string || _type == generic_object_type::null);
    return _type == generic_object_type::null ? nullptr : (nullable<rb_string>)_value02;
}

typed_object* generic_object::object_value() const {
    assert(_type == generic_object_type::object || _type == generic_object_type::null);
    return _value03.size() == 0 ? nullptr : _value03[0];
}

std::vector<typed_object*> generic_object::vector_value() const {
    assert(_type == generic_object_type::vector);
    return _value03;
}

generic_object::operator bool() const {
    return boolean_value();
}

bool generic_object::is_null() const {
    return type() == generic_object_type::null;
}

generic_object::operator rb_string() const {
    return string_value();
}

generic_object::operator long() const {
    return integer_value();
}

generic_object::operator float() const {
    return single_value();
}

generic_object::operator vec2() const {
    return vec2_value();
}

generic_object::operator color() const {
    return color_value();
}

generic_object::operator typed_object*() const {
    return object_value();
}

generic_object::operator std::vector<typed_object*>() const {
    return vector_value();
}

generic_object::operator nullable<buffer>() const {
    return nullable_buffer_value();
}
generic_object::operator nullable<bool>() const{
    return nullable_boolean_value();
}
generic_object::operator nullable<long>() const{
    return nullable_integer_value();
}
generic_object::operator nullable<float>() const{
    return nullable_single_value();
}
generic_object::operator nullable<vec2>() const{
    return nullable_vec2_value();
}
generic_object::operator nullable<color>() const{
    return nullable_color_value();
}
generic_object::operator nullable<rb_string>() const{
    return nullable_string_value();
}
generic_object::operator buffer() const {
    return buffer_value();
}

bool rb::operator==(const rb::generic_object &o1, const rb::generic_object &o2){
    if(o1.type() != o2.type())
        return false;
    
    if(o1.type() == generic_object_type::null)
        return true;
    
    else if(o1.type() == generic_object_type::buffer)
        return o1.buffer_value() == o2.buffer_value();
    
    else if(o1.type() == generic_object_type::boolean)
        return o1.boolean_value() == o2.boolean_value();
    
    else if(o1.type() == generic_object_type::color)
        return exact_match(o1.color_value(), o2.color_value());
    
    else if(o1.type() == generic_object_type::integer)
        return o1.integer_value() == o2.integer_value();
    
    else if(o1.type() == generic_object_type::object)
        return o1.object_value() == o2.object_value();
    
    else if(o1.type() == generic_object_type::single)
        return o1.single_value() == o2.single_value();
    
    else if(o1.type() == generic_object_type::string)
        return o1.string_value() == o2.string_value();
    
    else if(o1.type() == generic_object_type::vec2)
        return exact_match(o1.vec2_value(), o2.vec2_value());
    
    else { //vector
        auto _v1 = o1.vector_value();
        auto _v2 = o2.vector_value();
        
        if(_v1.size() != _v2.size())
            return false;
        
        for (auto i = 0; i < _v1.size(); i++){
            if(_v1[i] != _v2[i])
                return false;
        }
        
        return true;
    }
}

bool rb::operator!=(const rb::generic_object &o1, const rb::generic_object &o2){
    if(o1.type() != o2.type())
        return true;
    
    if(o1.type() == generic_object_type::null)
        return false;
    
    else if(o1.type() == generic_object_type::buffer)
        return o1.buffer_value() != o2.buffer_value();
        
    else if(o1.type() == generic_object_type::boolean)
        return o1.boolean_value() != o2.boolean_value();
    
    else if(o1.type() == generic_object_type::color)
        return !exact_match(o1.color_value(), o2.color_value());
    
    else if(o1.type() == generic_object_type::integer)
        return o1.integer_value() != o2.integer_value();
    
    else if(o1.type() == generic_object_type::object)
        return o1.object_value() != o2.object_value();
    
    else if(o1.type() == generic_object_type::single)
        return o1.single_value() != o2.single_value();
    
    else if(o1.type() == generic_object_type::string)
        return o1.string_value() != o2.string_value();
    
    else if(o1.type() == generic_object_type::vec2)
        return !exact_match(o1.vec2_value(), o2.vec2_value());
    
    else { //vector
        auto _v1 = o1.vector_value();
        auto _v2 = o2.vector_value();
        
        if(_v1.size() != _v2.size())
            return true;
        
        for (auto i = 0; i < _v1.size(); i++){
            if(_v1[i] != _v2[i])
                return true;
        }
        
        return false;
    }
}

bool rb::operator<(const rb::generic_object &o1, const rb::generic_object &o2){
    if(o1.type() != o2.type())
        return ((long)o1.type()) < ((long)o2.type());
    
    if(o1.type() == generic_object_type::null)
        return false;
    
    else if(o1.type() == generic_object_type::boolean)
        return (long)o1.boolean_value() < (long)o2.boolean_value();
    
    else if(o1.type() == generic_object_type::buffer)
        return o1.buffer_value() < o2.buffer_value();
    
    else if(o1.type() == generic_object_type::color)
        return o1.color_value() <  o2.color_value();
    
    else if(o1.type() == generic_object_type::integer)
        return o1.integer_value() < o2.integer_value();
    
    else if(o1.type() == generic_object_type::object)
        return o1.object_value() < o2.object_value();
    
    else if(o1.type() == generic_object_type::single)
        return o1.single_value() < o2.single_value();
    
    else if(o1.type() == generic_object_type::string)
        return o1.string_value() < o2.string_value();
    
    else if(o1.type() == generic_object_type::vec2)
        return o1.vec2_value() < o2.vec2_value();
    
    else { //vector
        auto _v1 = o1.vector_value();
        auto _v2 = o2.vector_value();
        
        if(_v1.size() != _v2.size())
            return _v1.size() < _v2.size();
        
        for (auto i = 0; i < _v1.size(); i++){
            if(_v1[i] == _v2[i])
                continue;
            else
                return _v1[i] < _v2[i];
        }
        
        return false;
    }
}

bool rb::operator>(const rb::generic_object &o1, const rb::generic_object &o2){
    if(o1.type() != o2.type())
        return ((long)o1.type()) > ((long)o2.type());
    
    if(o1.type() == generic_object_type::null)
        return false;
    
    else if(o1.type() == generic_object_type::boolean)
        return (long)o1.boolean_value() > (long)o2.boolean_value();
    
    else if(o1.type() == generic_object_type::buffer)
        return o1.buffer_value() > o2.buffer_value();
    
    else if(o1.type() == generic_object_type::color)
        return o1.color_value() >  o2.color_value();
    
    else if(o1.type() == generic_object_type::integer)
        return o1.integer_value() > o2.integer_value();
    
    else if(o1.type() == generic_object_type::object)
        return o1.object_value() > o2.object_value();
    
    else if(o1.type() == generic_object_type::single)
        return o1.single_value() > o2.single_value();
    
    else if(o1.type() == generic_object_type::string)
        return o1.string_value() > o2.string_value();
    
    else if(o1.type() == generic_object_type::vec2)
        return o1.vec2_value() > o2.vec2_value();
    
    else { //vector
        auto _v1 = o1.vector_value();
        auto _v2 = o2.vector_value();
        
        if(_v1.size() != _v2.size())
            return _v1.size() > _v2.size();
        
        for (auto i = 0; i < _v1.size(); i++){
            if(_v1[i] == _v2[i])
                continue;
            else
                return _v1[i] > _v2[i];
        }
        
        return false;
    }
}

bool rb::operator<=(const generic_object& o1, const generic_object& o2){
    return o1 < o2 || o1 == o2;
}
bool rb::operator>=(const generic_object& o1, const generic_object& o2){
    return o1 > o2 || o1 == o2;
}

size_t std::hash<rb::generic_object>::operator()(const rb::generic_object &obj) const {
    if(obj.type() == generic_object_type::null){
        return 0;
    }
    else if(obj.type() == generic_object_type::boolean)
    {
        std::hash<bool> _h;
        return _h(obj.boolean_value());
    }
    
    else if(obj.type() == generic_object_type::buffer)
    {
        std::hash<buffer> _h;
        return _h(obj.buffer_value());
    }
    
    else if(obj.type() == generic_object_type::color)
    {
        std::hash<color> _h;
        return _h(obj.color_value());
    }
    
    else if(obj.type() == generic_object_type::integer)
    {
        std::hash<long> _h;
        return _h(obj.integer_value());
    }
    
    else if(obj.type() == generic_object_type::object)
    {
        std::hash<rb::typed_object*> _h;
        return _h(obj.object_value());
    }
    
    else if(obj.type() == generic_object_type::single)
    {
        std::hash<float> _h;
        return _h(obj.single_value());
    }
    
    else if(obj.type() == generic_object_type::string)
    {
        std::hash<rb_string> _h;
        return _h(obj.string_value());
    }
    
    else if(obj.type() == generic_object_type::vec2)
    {
        std::hash<vec2> _h;
        return _h(obj.vec2_value());
    }
    
    else { //vector
        auto _v = obj.vector_value();
        std::hash<rb::typed_object*> _h;
        const size_t _seedPrimeNumber = 691;
        const size_t _fieldPrimeNumber = 397;
        size_t _hash = _seedPrimeNumber;
        for (int i = 0; i < _v.size(); ++i) {
            _hash *= (_fieldPrimeNumber + _h(_v[i]));
        }
        return _hash;
    }
}

rb_string generic_object::to_string() const {
    if(type() == generic_object_type::null){
        return rb::to_string(nullptr);
    }
    else if(type() == generic_object_type::boolean)
    {
        return rb::to_string(boolean_value());
    }
    
    else if(type() == generic_object_type::color)
    {
        return rb::to_string(color_value());
    }
    
    else if(type() == generic_object_type::integer)
    {
        return rb::to_string(integer_value());
    }
    
    else if(type() == generic_object_type::object)
    {
        return rb::to_string((void*)object_value(), "[", object_value()->type_descriptor()->name(), "]");
    }
    
    else if(type() == generic_object_type::single)
    {
        return rb::to_string(single_value());
    }
    
    else if(type() == generic_object_type::string)
    {
        return rb::to_string(string_value());
    }
    
    else if(type() == generic_object_type::vec2)
    {
        return rb::to_string(vec2_value());
    }
    
    else if(type() == generic_object_type::buffer){
        return rb::to_string(_buffer.encode());
    }
    
    else { //vector
        rb_string _vStr = u"[";
        auto _vector = vector_value();
        for (size_t i = 0; i < _vector.size(); i++) {
            _vStr += rb::to_string((void*)_vector[i], "[", _vector[i]->type_descriptor()->name(), "]");
            if(i != (_vector.size() - 1))
                _vStr += u", ";
        }
        _vStr += u"]";
        return _vStr;
    }
}





































