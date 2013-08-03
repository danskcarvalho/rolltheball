//
//  scene_loader.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 23/06/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#include "scene_loader.h"
#include "xnode.h"
#include "typed_object.h"
#include "type_descriptor.h"
#include "generic_object.h"
#include "buffer.h"
#include "color.h"
#include "vec2.h"
#include "scene.h"

using namespace rb;

inline NSString* to_platform_string(const rb::rb_string& str){
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> _str_converter;
    auto _u8Str = _str_converter.to_bytes(str);
    NSString* _platform_str = [NSString stringWithUTF8String:_u8Str.c_str()];
    return _platform_str;
}

void scene_loader::serialize_to_xnode(const typed_object* obj, rb::xnode &node){
    if(!obj){
        node.set_attribute_value(u"is-null", u"true");
        return;
    }
    
    node.set_attribute_value(u"type-name", obj->type_descriptor()->name());
    
    if(obj->type_descriptor()->gtd() == general_type_descriptor::branch)
        node.set_attribute_value(u"branch-tag", obj->branch_tag());
    
    xnode properties;
    properties.name(u"properties");
    std::vector<rb_string> _properties;
    obj->type_descriptor()->get_all_properties(_properties);
    for (auto& _p : _properties){
        xnode prop;
        prop.name(u"property");
        prop.set_attribute_value(u"name", _p);
        auto _td = obj->type_descriptor()->property_type(_p);
        if(_td->gtd() == general_type_descriptor::action){
            //do nothing
        }
        else if(_td->gtd() == general_type_descriptor::angle){
            auto _v = obj->type_descriptor()->get_property(obj, _p);
            if(_v.is_null())
                prop.set_attribute_value(u"is-null", u"true");
            else
                prop.set_attribute_value(u"value", rb::to_string(_v.single_value()));
        }
        else if(_td->gtd() == general_type_descriptor::boolean){
            auto _v = obj->type_descriptor()->get_property(obj, _p);
            if(_v.is_null())
                prop.set_attribute_value(u"is-null", u"true");
            else
                prop.set_attribute_value(u"value", rb::to_string(_v.boolean_value() ? u"true" : u"false"));
        }
        else if(_td->gtd() == general_type_descriptor::branch){
            auto _v = obj->type_descriptor()->get_property(obj, _p);
            serialize_to_xnode(_v.object_value(), prop);
        }
        else if(_td->gtd() == general_type_descriptor::buffer){
            auto _v = obj->type_descriptor()->get_property(obj, _p);
            if(_v.is_null())
                prop.set_attribute_value(u"is-null", u"true");
            else
                prop.set_attribute_value(u"value", _v.buffer_value().encode());
        }
        else if(_td->gtd() == general_type_descriptor::color){
            auto _v = obj->type_descriptor()->get_property(obj, _p);
            if(_v.is_null())
                prop.set_attribute_value(u"is-null", u"true");
            else
            {
                auto _c = _v.color_value();
                prop.set_attribute_value(u"value-r", rb::to_string(_c.r()));
                prop.set_attribute_value(u"value-g", rb::to_string(_c.g()));
                prop.set_attribute_value(u"value-b", rb::to_string(_c.b()));
                prop.set_attribute_value(u"value-a", rb::to_string(_c.a()));
            }
        }
        else if(_td->gtd() == general_type_descriptor::enumeration){
            auto _v = obj->type_descriptor()->get_nullable_string_property(obj, _p);
            if(!_v.has_value())
                prop.set_attribute_value(u"is-null", u"true");
            else
                prop.set_attribute_value(u"value", _v.value());
        }
        else if(_td->gtd() == general_type_descriptor::flags){
            auto _v = obj->type_descriptor()->get_property(obj, _p);
            if(_v.is_null())
                prop.set_attribute_value(u"is-null", u"true");
            else
                prop.set_attribute_value(u"value", rb::to_string(_v.integer_value()));
        }
        else if(_td->gtd() == general_type_descriptor::image){
            auto _v = obj->type_descriptor()->get_nullable_string_property(obj, _p);
            if(!_v.has_value())
                prop.set_attribute_value(u"is-null", u"true");
            else
                prop.set_attribute_value(u"value", _v.value());
        }
        else if(_td->gtd() == general_type_descriptor::integer){
            auto _v = obj->type_descriptor()->get_property(obj, _p);
            if(_v.is_null())
                prop.set_attribute_value(u"is-null", u"true");
            else
                prop.set_attribute_value(u"value", rb::to_string(_v.integer_value()));
        }
        else if(_td->gtd() == general_type_descriptor::object){
            auto _v = obj->type_descriptor()->get_property(obj, _p);
            serialize_to_xnode(_v.object_value(), prop);
        }
        else if(_td->gtd() == general_type_descriptor::ranged){
            auto _v = obj->type_descriptor()->get_property(obj, _p);
            if(_v.is_null())
                prop.set_attribute_value(u"is-null", u"true");
            else
                prop.set_attribute_value(u"value", rb::to_string(_v.single_value()));
        }
        else if(_td->gtd() == general_type_descriptor::single){
            auto _v = obj->type_descriptor()->get_property(obj, _p);
            if(_v.is_null())
                prop.set_attribute_value(u"is-null", u"true");
            else
                prop.set_attribute_value(u"value", rb::to_string(_v.single_value()));
        }
        else if(_td->gtd() == general_type_descriptor::string){
            auto _v = obj->type_descriptor()->get_nullable_string_property(obj, _p);
            if(!_v.has_value())
                prop.set_attribute_value(u"is-null", u"true");
            else
                prop.set_attribute_value(u"value", _v.value());
        }
        else if(_td->gtd() == general_type_descriptor::string_with_autocompletion){
            auto _v = obj->type_descriptor()->get_nullable_string_property(obj, _p);
            if(!_v.has_value())
                prop.set_attribute_value(u"is-null", u"true");
            else
                prop.set_attribute_value(u"value", _v.value());
        }
        else if(_td->gtd() == general_type_descriptor::vec2){
            auto _v = obj->type_descriptor()->get_property(obj, _p);
            if(_v.is_null())
                prop.set_attribute_value(u"is-null", u"true");
            else {
                prop.set_attribute_value(u"value-x", rb::to_string(_v.vec2_value().x()));
                prop.set_attribute_value(u"value-y", rb::to_string(_v.vec2_value().y()));
            }
        }
        else { //vector
            auto _vector = obj->type_descriptor()->get_property(obj, _p).vector_value();
            for (auto _n : _vector){
                xnode _item;
                _item.name(u"item");
                serialize_to_xnode(_n, _item);
                prop.children().push_back(_item);
            }
        }
        properties.children().push_back(prop);
    }
    node.children().push_back(properties);
    
    xnode children;
    children.name(u"children");
    std::vector<typed_object*> _children;
    obj->fill_vector_with_children(_children);
    for (auto _n : _children){
        xnode child;
        child.name(u"child");
        serialize_to_xnode(_n, child);
        children.children().push_back(child);
    }
    node.children().push_back(children);
}

typed_object* scene_loader::deserialize_from_xnode(const rb::xnode &node, const type_descriptor* td){
    if(node.has_attribute(u"is-null"))
        return nullptr;
    
    if(!td)
        td = type_descriptor::with_name(node.get_attribute_value(u"type-name"));
    
    typed_object* _to = td->new_object();
    
    if(node.has_attribute(u"branch-tag"))
        _to->branch_tag(node.get_attribute_value(u"branch-tag"));
    
    xnode _properties;
    xnode _children;
    for (auto& _n : node.children()){
        if(_n.name() == u"properties")
            _properties = _n;
        else if(_n.name() == u"children")
            _children = _n;
    }
    
    std::vector<typed_object*> _children_v;
    for(auto& _n : _children.children()){
        _children_v.push_back(deserialize_from_xnode(_n, nullptr));
    }
    
    if(_children_v.size() != 0)
        _to->set_children(_children_v);
    
    NSString* someStr = nil;
    
    for(auto& _n : _properties.children()){
        auto _p = _n.get_attribute_value(u"name");
        auto _tdp = _to->type_descriptor()->property_type(_p);
        
        if(_tdp->gtd() == general_type_descriptor::angle || _tdp->gtd() == general_type_descriptor::ranged || _tdp->gtd() == general_type_descriptor::single){
            if(_n.has_attribute(u"is-null"))
                _to->type_descriptor()->set_property(_to, _p, nullptr);
            else {
                someStr = to_platform_string(_n.get_attribute_value(u"value"));
                _to->type_descriptor()->set_property(_to, _p, [someStr floatValue]);
            }
        }
        else if(_tdp->gtd() == general_type_descriptor::boolean){
            if(_n.has_attribute(u"is-null"))
                _to->type_descriptor()->set_property(_to, _p, nullptr);
            else {
                auto _r = _n.get_attribute_value(u"value");
                _to->type_descriptor()->set_property(_to, _p, _r == u"true");
            }
        }
        else if(_tdp->gtd() == general_type_descriptor::branch || _tdp->gtd() == general_type_descriptor::object){
            if(_n.has_attribute(u"is-null"))
                _to->type_descriptor()->set_property(_to, _p, nullptr);
            else {
                _to->type_descriptor()->set_property(_to, _p, deserialize_from_xnode(_n, _tdp));
            }
        }
        else if(_tdp->gtd() == general_type_descriptor::buffer){
            if(_n.has_attribute(u"is-null"))
                _to->type_descriptor()->set_property(_to, _p, nullptr);
            else {
                auto _r = _n.get_attribute_value(u"value");
                _to->type_descriptor()->set_property(_to, _p, buffer(_r));
            }
        }
        else if(_tdp->gtd() == general_type_descriptor::color){
            if(_n.has_attribute(u"is-null"))
                _to->type_descriptor()->set_property(_to, _p, nullptr);
            else {
                someStr = to_platform_string(_n.get_attribute_value(u"value-r"));
                auto _r = [someStr floatValue];
                someStr = to_platform_string(_n.get_attribute_value(u"value-g"));
                auto _g = [someStr floatValue];
                someStr = to_platform_string(_n.get_attribute_value(u"value-b"));
                auto _b = [someStr floatValue];
                someStr = to_platform_string(_n.get_attribute_value(u"value-a"));
                auto _a = [someStr floatValue];
                _to->type_descriptor()->set_property(_to, _p, color::from_rgba(_r, _g, _b, _a));
            }
        }
        else if(_tdp->gtd() == general_type_descriptor::enumeration || _tdp->gtd() == general_type_descriptor::integer || _tdp->gtd() == general_type_descriptor::flags){
            if(_n.has_attribute(u"is-null"))
                _to->type_descriptor()->set_property(_to, _p, nullptr);
            else {
                someStr = to_platform_string(_n.get_attribute_value(u"value"));
                _to->type_descriptor()->set_property(_to, _p, (long)[someStr longLongValue]);
            }
        }
        else if(_tdp->gtd() == general_type_descriptor::image || _tdp->gtd() == general_type_descriptor::string || _tdp->gtd() == general_type_descriptor::string_with_autocompletion){
            if(_n.has_attribute(u"is-null"))
                _to->type_descriptor()->set_property(_to, _p, nullptr);
            else {
                auto _r = _n.get_attribute_value(u"value");
                _to->type_descriptor()->set_property(_to, _p, _r);
            }
        }
        else if(_tdp->gtd() == general_type_descriptor::vec2){
            if(_n.has_attribute(u"is-null"))
                _to->type_descriptor()->set_property(_to, _p, nullptr);
            else {
                someStr = to_platform_string(_n.get_attribute_value(u"value-x"));
                auto _x = [someStr floatValue];
                someStr = to_platform_string(_n.get_attribute_value(u"value-y"));
                auto _y = [someStr floatValue];
                _to->type_descriptor()->set_property(_to, _p, vec2(_x, _y));
            }
        }
        else { //vector
            std::vector<typed_object*> _vector;
            for(auto& _vn : _n.children()){
                _vector.push_back(deserialize_from_xnode(_vn, _tdp->element_type()));
            }
            _to->type_descriptor()->set_property(_to, _p, _vector);
        }
    }
    return _to;
}

void scene_loader::serialize_to_url(const rb::typed_object *obj, const rb_string &url){
    xnode _serialized;
    _serialized.name(u"object");
    if(obj)
        const_cast<typed_object*>(obj)->internal_begin_serialization();
    serialize_to_xnode(obj, _serialized);
    NSURL* furl = [NSURL fileURLWithPath:to_platform_string(url) isDirectory:NO];
    _serialized.write_to_url(furl);
    if(obj)
        const_cast<typed_object*>(obj)->internal_end_serialization();
}

rb_string scene_loader::serialize_to_string(const rb::typed_object *obj){
    xnode _serialized;
    _serialized.name(u"object");
    if(obj)
        const_cast<typed_object*>(obj)->internal_begin_serialization();
    serialize_to_xnode(obj, _serialized);
    rb_string _v = _serialized.to_string();
    if(obj)
        const_cast<typed_object*>(obj)->internal_end_serialization();
    return _v;
}

typed_object* scene_loader::deserialize_from_url(const rb_string &url){
    NSURL* furl = [NSURL fileURLWithPath:to_platform_string(url) isDirectory:NO];
    xnode _xnode = xnode::read_from_url(furl);
    auto _obj = deserialize_from_xnode(_xnode, nullptr);
    if(_obj)
        const_cast<typed_object*>(_obj)->internal_was_deserialized();
    return _obj;
}

typed_object* scene_loader::deserialize_from_string(const rb_string &str){
    xnode _xnode = xnode::read_from_string(str);
    auto _obj = deserialize_from_xnode(_xnode, nullptr);
    if(_obj)
        const_cast<typed_object*>(_obj)->internal_was_deserialized();
    return _obj;
}

scene* scene_loader::load(const rb_string &name){
    NSString* sceneName = to_platform_string(name);
    NSURL* url = [[NSBundle mainBundle] URLForResource:sceneName withExtension:@"scene"];
    xnode _xnode = xnode::read_from_url(url);
    auto _obj = deserialize_from_xnode(_xnode, nullptr);
    if(_obj)
        const_cast<typed_object*>(_obj)->internal_was_deserialized();
    return dynamic_cast<scene*>(_obj);
}




































