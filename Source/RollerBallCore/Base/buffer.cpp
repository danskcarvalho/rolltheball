//
//  buffer.cpp
//  RollerBallCore
//
//  Created by Danilo Santos de Carvalho on 21/06/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#include "buffer.h"
#include "rb_base64.h"
#include "compute_hash.h"

using namespace rb;

buffer::buffer_internals::buffer_internals(){
    _bytes = nullptr;
    _size = 0;
    _refCount = 1;
}

buffer::buffer_internals::~buffer_internals(){
    if(_bytes)
        free(_bytes);
    _bytes = nullptr;
}

buffer::buffer_internals::buffer_internals(const uint8_t* bytes, size_t size){
    assert(size > 0);
    assert(bytes);
    _bytes = (uint8_t*)malloc(size);
    memcpy(_bytes, bytes, size);
    _size = size;
    _refCount = 1;
}
buffer::buffer_internals::buffer_internals(const std::vector<uint8_t>& bytes){
    assert(bytes.size() > 0);
    _bytes = (uint8_t*)malloc(bytes.size());
    _size = bytes.size();
    for (size_t i = 0; i < bytes.size(); i++) {
        _bytes[i] = bytes[i];
    }
    _refCount = 1;
}

static std::string to_utf8_string(const rb_string& str){
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> str_converter;
    return str_converter.to_bytes(str);
}

template <class T>
static rb_string _to_rb_string(const T val){
    std::stringstream ss;
    ss << val;
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> str_converter;
    return str_converter.from_bytes(ss.str());
}

buffer::buffer_internals::buffer_internals(const rb_string& encoded){
    if(encoded == u""){
        _bytes = nullptr;
        _size = 0;
        _refCount = 1;
        return;
    }
    std::string _str = to_utf8_string(encoded);
    auto _b_v = rb_base64Decode(_str);
    _size = _b_v.size();
    _bytes = (uint8_t*)malloc(_size);
    for (size_t i = 0; i < _b_v.size(); i++) {
        _bytes[i] = _b_v[i];
    }
    _refCount = 1;
}

void buffer::buffer_internals::add_ref() {
    _refCount++;
}

void buffer::buffer_internals::remove_ref(rb::buffer::buffer_internals **pointer){
    auto _previous = _refCount.fetch_sub(1);
    
    if(_previous == 1){
        *pointer = nullptr;
        delete this;
    }
}

buffer::buffer()
{
    _internals = nullptr;
}

buffer::~buffer(){
    if(_internals)
        _internals->remove_ref(&_internals);
    _internals = nullptr;
}

buffer::buffer(const void* bytes, size_t size){
    assert(size > 0);
    assert(bytes);
    _internals = new buffer_internals((uint8_t*)bytes, size);
}

buffer::buffer(const rb_string& encoded){
    _internals = new buffer_internals(encoded);
}

buffer::buffer(const std::vector<uint8_t>& bytes){
    _internals = new buffer_internals(bytes);
}

buffer::buffer(const buffer& other){
    _internals = other._internals;
    if(_internals)
        _internals->add_ref();
}

buffer::buffer(buffer&& other){
    _internals = other._internals;
    other._internals = nullptr;
}

buffer& buffer::operator=(const buffer& other){
    if(other._internals)
        other._internals->add_ref();
    if(_internals)
        _internals->remove_ref(&_internals);
    _internals = other._internals;
    return *this;
}

buffer& buffer::operator=(buffer&& other){
    if(_internals)
        _internals->remove_ref(&_internals);
    _internals = other._internals;
    other._internals = nullptr;
    return *this;
}

size_t buffer::size() const {
    if(!_internals)
        return 0;
    return _internals->_size;
}

uint8_t buffer::operator[](size_t index) const {
    assert(_internals);
    assert(index < _internals->_size);
    return _internals->_bytes[index];
}

void buffer::copy_to_vector(std::vector<uint8_t> &bytes) const {
    bytes.clear();
    if(!_internals)
        return;
    for (size_t i = 0; i < _internals->_size; i++) {
        bytes.push_back(_internals->_bytes[i]);
    }
}

void buffer::copy_to_cbuffer(void *cbuffer) const {
    if(!_internals)
        return;
    memcpy(cbuffer, _internals->_bytes, _internals->_size);
}

rb_string buffer::encode() const {
    std::vector<uint8_t> _b;
    copy_to_vector(_b);
    auto _str = rb_base64Encode(_b);
    return _to_rb_string(_str);
}

const void* buffer::internal_buffer() const {
    return _internals->_bytes;
}

bool rb::operator==(const buffer& o1, const buffer& o2){
    if(o1.size() != o2.size())
        return false;
    
    for (size_t i = 0; i < o1.size(); i++) {
        if(o1[i] != o2[i])
            return false;
    }
    
    return true;
}
bool rb::operator!=(const buffer& o1, const buffer& o2){
    if(o1.size() != o2.size())
        return true;
    
    for (size_t i = 0; i < o1.size(); i++) {
        if(o1[i] != o2[i])
            return true;
    }
    
    return false;
}
bool rb::operator<(const buffer& o1, const buffer& o2){
    if(o1.size() != o2.size())
        return o1.size() < o2.size();
    
    for (size_t i = 0; i < o1.size(); i++) {
        if(o1[i] < o2[i])
            return true;
        else if(o1[i] > o2[i])
            return false;
    }
    
    return false;
}
bool rb::operator>(const buffer& o1, const buffer& o2){
    if(o1.size() != o2.size())
        return o1.size() > o2.size();
    
    for (size_t i = 0; i < o1.size(); i++) {
        if(o1[i] > o2[i])
            return true;
        else if(o1[i] < o2[i])
            return false;
    }
    
    return false;
}
bool rb::operator<=(const buffer& o1, const buffer& o2){
    if(o1.size() != o2.size())
        return o1.size() < o2.size();
    
    for (size_t i = 0; i < o1.size(); i++) {
        if(o1[i] < o2[i])
            return true;
        else if(o1[i] > o2[i])
            return false;
    }
    
    return true;
}
bool rb::operator>=(const buffer& o1, const buffer& o2){
    if(o1.size() != o2.size())
        return o1.size() > o2.size();
    
    for (size_t i = 0; i < o1.size(); i++) {
        if(o1[i] > o2[i])
            return true;
        else if(o1[i] < o2[i])
            return false;
    }
    
    return true;
}

size_t std::hash<rb::buffer>::operator()(const rb::buffer &obj) const {
    return compute_hash()((const char*)obj.internal_buffer(), obj.size());
}
































