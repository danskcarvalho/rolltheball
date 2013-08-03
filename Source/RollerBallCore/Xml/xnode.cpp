//
//  xnode.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 18/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "xnode.h"
#include "XNodeParser.h"
#include "XmlWriter.h"

inline NSString* to_platform_string(const rb::rb_string& str){
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> _str_converter;
    auto _u8Str = _str_converter.to_bytes(str);
    NSString* _platform_str = [NSString stringWithUTF8String:_u8Str.c_str()];
    return _platform_str;
}

inline rb::rb_string from_platform_string(NSString* str){
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> _str_converter;
    return _str_converter.from_bytes([str UTF8String]);
}

using namespace rb;

bool xnode::has_attribute(const rb_string &attr_name) const {
    return _attributes.count(attr_name) != 0;
}

void xnode::get_attributes(std::vector<rb_string>& attributes) const{
    attributes.clear();
    for(auto _p : _attributes){
        attributes.push_back(_p.first);
    }
}
rb_string xnode::get_attribute_value(const rb_string& attr_name) const{
    assert(_attributes.count(attr_name) != 0);
    return _attributes.at(attr_name);
}
void xnode::set_attribute_value(const rb_string& attr_name, const rb_string& attr_value){
    _attributes[attr_name] = attr_value;
}
void xnode::clear_attributes(){
    _attributes.clear();
}
const rb_string& xnode::name() const{
    return _name;
}
const rb_string& xnode::name(const rb_string& value) {
    _name = value;
    return _name;
}

const std::vector<xnode>& xnode::children() const{
    return _children;
}
std::vector<xnode>& xnode::children(){
    return _children;
}

xnode xnode::read_from_url(NSURL* url){
    xnode node;
    XNodeParser* parser = [[XNodeParser alloc] initWithNode: &node];
    [parser parseXml: url];
    return node;
}
xnode xnode::read_from_string(const rb_string &str){
    xnode node;
    XNodeParser* parser = [[XNodeParser alloc] initWithNode: &node];
    [parser parseXmlWithString:to_platform_string(str)];
    return node;
}
xnode& xnode::get_children_by_tag_name(const rb_string& name){
    auto _it = std::find_if(_children.begin(), _children.end(), [&](const xnode& n) {
        return n.name() == name;
    });
    assert(_it != _children.end());
    return *_it;
}
void xnode::write_to_url(NSURL* url){
    XMLWriter* _writer = [[XMLWriter alloc] init];
    [_writer writeStartDocumentWithEncodingAndVersion: @"UTF-8" version: @"1.0"];
    this->write_to_writer(_writer);
    [_writer writeEndDocument];
    [_writer flush];
    NSMutableString* _str = [_writer toString];
    [_str writeToURL:url atomically:YES encoding: NSUTF8StringEncoding error:NULL];
}

rb_string xnode::to_string() const {
    XMLWriter* _writer = [[XMLWriter alloc] init];
    [_writer writeStartDocumentWithEncodingAndVersion: @"UTF-8" version: @"1.0"];
    const_cast<xnode*>(this)->write_to_writer(_writer);
    [_writer writeEndDocument];
    [_writer flush];
    NSMutableString* _str = [_writer toString];
    return from_platform_string(_str);
}

void xnode::write_to_writer(XMLWriter* writer){
    NSString* _elem_name = to_platform_string(_name);
    [writer writeStartElement:_elem_name];
    for(auto& _p : _attributes){
        NSString* _attr_name = to_platform_string(_p.first);
        NSString* _attr_value = to_platform_string(_p.second);
        [writer writeAttribute: _attr_name value: _attr_value];
    }
    for(auto & _n : _children){
        _n.write_to_writer(writer);
    }
    [writer writeEndElement];
}