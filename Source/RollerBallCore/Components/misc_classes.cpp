//
//  misc_classes.cpp
//  RollerBallCore
//
//  Created by Danilo Santos de Carvalho on 11/03/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#include "misc_classes.h"
#include "generic_object.h"

using namespace rb;

namespace rb {
    bool is_node(const typed_object*);
}

//Multiple nodes...
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void multiple_nodes::property_changed(rb::typed_object *object, const rb_string &property_name){
    
}

void multiple_nodes::branch_tag_changed(rb::typed_object *object){
    
}

void multiple_nodes::object_deleted(rb::typed_object *object){
    size_t index = -1;
    for (size_t i = 0; i < _objects.size(); i++){
        if(_objects[i] == object){
            index = i;
            break;
        }
    }
    
    assert(index != -1);
    _objects.erase(_objects.cbegin() + index);
    _cloned_objects.erase(_cloned_objects.cbegin() + index);
}

rb_string multiple_nodes::to_string(const std::list<rb::multiple_nodes::path_component>& path){
    rb_string _p;
    int i = 0;
    for(auto& _c : path){
        auto _continuation = (i == (path.size() - 1)) ? u"" : u"->";
        if(_c.index != -1){
            _p += _c.value + u"[" + to_string(_c.index) + u"]" + _continuation;
        }
        else
            _p += _c.value + _continuation;
        i++;
    }
    
    return _p;
}

multiple_nodes::multiple_nodes(const std::vector<typed_object*>& objects){
    assert(objects.size() >= 2); //at least two objects
    _objects = objects;
    
    for (auto _n : objects)
        _n->add_observer(this);
    
    //i am the root
    _original = nullptr;
    _root = this;
    _is_void = false;
    auto _any_list = std::list<path_component>();
    auto _i = 0;
    std::vector<typed_object*> _temp_cloned;
    for (auto _o : _objects){
        _temp_cloned.push_back(clone_object(_o, _i, _any_list));
        _i++;
    }
    _cloned_objects = _temp_cloned;
}

multiple_nodes::multiple_nodes(){
    _original = nullptr;
    _root = nullptr;
    _is_void = false;
}

multiple_nodes::multiple_nodes(const class type_descriptor* original){
    _original = const_cast<class type_descriptor*>(original);
    _root = nullptr;
    _is_void = true;
}

multiple_nodes::~multiple_nodes(){
    for(auto _n : _objects)
        _n->remove_observer(this);
    
    for(auto _mn : _to_be_freed)
        delete _mn;
    
    for(auto _co : _cloned_objects)
        delete _co;
}

rb_string multiple_nodes::type_name() const {
    //we need to generate a unique name
    if(_original)
        return rb_string(u"rb::multiple_nodes-") + _original->name();
    else {
        if(std::all_of(_objects.begin(), _objects.end(), [=](const typed_object* obj){ return obj->type_descriptor()->name() == _objects[0]->type_descriptor()->name(); }))
            return rb_string(u"rb::multiple_nodes-") + _objects[0]->type_descriptor()->name();
        else
            return u"rb::multiple_nodes-@#78_mixed";
    }
    
}

void multiple_nodes::branch_tag_changed() {
    if(this->_root && !this->_is_void){
        for (auto i = 0; i < this->_root->_objects.size(); i++) {
            this->set_branch_tag(i, branch_tag());
        }
    }
}

void multiple_nodes::set_branch_tag(int mn_index, const rb_string &value){
    auto _new_path = this->_path;
    auto _real = get_property_rec(_root->_objects[mn_index], _new_path);
    _new_path = this->_path;
    auto _original = get_property_rec(_root->_cloned_objects[mn_index], _new_path);
    if(value != u"<NULL>")
        _real.object_value()->branch_tag(value);
    else {
        //eu preciso restaurar o valor original...
        if(_original.object_value())
            _real.object_value()->branch_tag(_original.object_value()->branch_tag());
    }
}

rb_string multiple_nodes::displayable_type_name() const {
    if(_original)
        return _original->display_name();
    else {
        if(std::all_of(_objects.begin(), _objects.end(), [=](const typed_object* obj){ return obj->type_descriptor()->name() == _objects[0]->type_descriptor()->name(); }))
            return _objects[0]->type_descriptor()->name();
        else
            return u"Mixed";
    }
}

void multiple_nodes::describe_type() {
    bool _all_of_same_type;
    bool _all_nodes;
    const class type_descriptor* _td = _original ? _original : _objects[0]->type_descriptor();
    
    if(_original)
        _all_of_same_type = true;
    else
        _all_of_same_type = std::all_of(_objects.begin(), _objects.end(), [=](const typed_object* obj){ return obj->type_descriptor()->name() == _objects[0]->type_descriptor()->name(); });
    
    if(_original)
        _all_nodes = false;
    else
        _all_nodes = std::all_of(_objects.begin(), _objects.end(), [=](const typed_object* obj){ return is_node(obj); });
    
    if(_all_of_same_type) //everyone of the same type...
    {
        if(_td->gtd() == general_type_descriptor::branch){ //branch
            start_branch_type<multiple_nodes>(_td->selector_name(), [=]() {
                return new multiple_nodes((class type_descriptor*)_td);
            });
            start_branch(u"<NULL>");
            end_branch();
            for(auto& _p : _td->branches()){
                start_branch(_p.first);
                std::vector<rb_string> _properties;
                _p.second->get_all_properties(_properties);
                for (auto& _p : _properties){
                    add_property(_td, _p); //_td is the right value
                }
                end_branch();
            }
            end_type();
        }
        else if(_td->gtd() == general_type_descriptor::object){ //object
            start_type<multiple_nodes>([=]() {
                return new multiple_nodes((class type_descriptor*)_td);
            });
            std::vector<rb_string> _properties;
            _td->get_all_properties(_properties);
            for (auto& _p : _properties){
                add_property(_td, _p);
            }
            end_type();
        }
        else {
            start_type<multiple_nodes>([]() { return nullptr; }); //unsupported
            end_type();
        }
    }
    else if(_all_nodes){ //if they're all nodes...
        start_type<multiple_nodes>([]() { return nullptr; }); //only name and classes...
        add_property(_td, u"rb::node_id");
        end_type();
    }
    else {
        start_type<multiple_nodes>([]() { return nullptr; }); //unsupported
        end_type();
    }
}

template<class T>
bool nullable_equal(const nullable<T>& t1, const nullable<T>& t2){
    if(!t1.has_value() && !t2.has_value())
        return true;
    
    if(!t1.has_value() || !t2.has_value())
        return false;
    
    return t1.value() == t2.value();
}

bool compare_vectors(const std::vector<typed_object*>& v1, const std::vector<typed_object*>& v2);

//we assume that obj1 is of the same type of obj2
bool compare_objects(const typed_object* obj1, const typed_object* obj2){
    if(obj1 == obj2)
        return true;
    
    if(obj1 == nullptr || obj2 == nullptr)
        return false;
    
    auto _td = obj1 != nullptr ? obj1->type_descriptor() : obj2->type_descriptor();
    
    if(_td->gtd() == general_type_descriptor::branch){
        if(obj1->branch_tag() != obj2->branch_tag())
            return false;
    }
    
    std::vector<rb_string> _properties;
    _td->get_all_properties(_properties);
    for (auto _p : _properties){
        if(_td->property_private(_p))
            continue;
        
        auto _td_p = _td->property_type(_p);
        if(_td_p->gtd() == general_type_descriptor::action)
            continue;
        bool _equals = true;
        if(_td_p->gtd() == general_type_descriptor::angle)
            _equals = nullable_equal(_td->get_nullable_single_property(obj1, _p), _td->get_nullable_single_property(obj2, _p));
        else if(_td_p->gtd() == general_type_descriptor::buffer)
            _equals = nullable_equal(_td->get_nullable_buffer_property(obj1, _p), _td->get_nullable_buffer_property(obj2, _p));
        else if(_td_p->gtd() == general_type_descriptor::boolean)
            _equals = nullable_equal(_td->get_nullable_boolean_property(obj1, _p), _td->get_nullable_boolean_property(obj2, _p));
        else if(_td_p->gtd() == general_type_descriptor::branch)
            _equals = compare_objects(_td->get_object_property<typed_object>(obj1, _p), _td->get_object_property<typed_object>(obj2, _p));
        else if(_td_p->gtd() == general_type_descriptor::color)
            _equals = nullable_equal(_td->get_nullable_color_property(obj1, _p), _td->get_nullable_color_property(obj2, _p));
        else if(_td_p->gtd() == general_type_descriptor::enumeration)
            _equals = nullable_equal(_td->get_nullable_integer_property(obj1, _p), _td->get_nullable_integer_property(obj2, _p));
        else if(_td_p->gtd() == general_type_descriptor::image)
            _equals = nullable_equal(_td->get_nullable_string_property(obj1, _p), _td->get_nullable_string_property(obj2, _p));
        else if(_td_p->gtd() == general_type_descriptor::integer)
            _equals = nullable_equal(_td->get_nullable_integer_property(obj1, _p), _td->get_nullable_integer_property(obj2, _p));
        else if(_td_p->gtd() == general_type_descriptor::object)
            _equals = compare_objects(_td->get_object_property<typed_object>(obj1, _p), _td->get_object_property<typed_object>(obj2, _p));
        else if(_td_p->gtd() == general_type_descriptor::flags)
            _equals = nullable_equal(_td->get_nullable_integer_property(obj1, _p), _td->get_nullable_integer_property(obj2, _p));
        else if(_td_p->gtd() == general_type_descriptor::ranged)
            _equals = nullable_equal(_td->get_nullable_single_property(obj1, _p), _td->get_nullable_single_property(obj2, _p));
        else if(_td_p->gtd() == general_type_descriptor::single)
            _equals = nullable_equal(_td->get_nullable_single_property(obj1, _p), _td->get_nullable_single_property(obj2, _p));
        else if(_td_p->gtd() == general_type_descriptor::string)
            _equals = nullable_equal(_td->get_nullable_string_property(obj1, _p), _td->get_nullable_string_property(obj2, _p));
        else if(_td_p->gtd() == general_type_descriptor::string_with_autocompletion)
            _equals = nullable_equal(_td->get_nullable_string_property(obj1, _p), _td->get_nullable_string_property(obj2, _p));
        else if(_td_p->gtd() == general_type_descriptor::vec2)
            _equals = nullable_equal(_td->get_nullable_vec2_property(obj1, _p), _td->get_nullable_vec2_property(obj2, _p));
        else { //vector
            auto _v1 = _td->get_vector_property<typed_object>(obj1, _p);
            auto _v2 = _td->get_vector_property<typed_object>(obj2, _p);
            
            _equals = compare_vectors(_v1, _v2);
        }
        
        if(!_equals)
            return false;
    }
    
    return true;
}

void multiple_nodes::set_property_rec(typed_object* site, int mn_index, std::list<path_component> &path, const std::list<path_component> &full_path, const rb::generic_object &value){
    assert(path.size() != 0);
    
    if(path.size() == 1){
        assert(path.front().index == -1);
        if(path.front().index == -1){
            auto _pgtd = site->type_descriptor()->property_type(path.front().value)->gtd();
            if(_pgtd == general_type_descriptor::object || _pgtd == general_type_descriptor::branch){
                if(value.is_null() && !site->type_descriptor()->property_type(path.front().value)->nullable())
                    return;
                
                site->type_descriptor()->set_property(site, path.front().value, value);
            }
            else if(_pgtd == general_type_descriptor::vector)
                site->type_descriptor()->set_property(site, path.front().value, value);
            else {
                if(value.type() != generic_object_type::null)
                    site->type_descriptor()->set_property(site, path.front().value, value);
                else {
                    auto _new_path = full_path;
                    auto _new_value = get_property_rec(_root->_cloned_objects[mn_index], _new_path);
                    if(_new_value.is_null() && !site->type_descriptor()->property_type(path.front().value)->nullable())
                        return;
                    site->type_descriptor()->set_property(site, path.front().value, _new_value);
                }
            }
        }
        else
        {
            throw std::exception(); //unsupported
        }
    }
    else {
        if(path.front().index == -1){
            typed_object* _obj = site->type_descriptor()->get_object_property<typed_object>(site, path.front().value);
            if(!_obj)
                return; //we don't set if the object is null...
            path.erase(path.begin());
            set_property_rec(_obj, mn_index, path, full_path, value);
        }
        else {
            std::vector<typed_object*> _v = site->type_descriptor()->get_vector_property<typed_object>(site, path.front().value);
            if(path.front().index >= _v.size())
                return; //we don't set if the index is invalid
            
            auto _obj = _v[path.front().index];
            
            if(!_obj)
                return; //unnecessary but we do the test...
            
            path.erase(path.begin());
            set_property_rec(_obj, mn_index, path, full_path, value);
        }
    }
}

typed_object* multiple_nodes::clone_object(rb::typed_object *cloned,  int mn_index, const std::list<path_component>& path){ //path contém o caminho completo a partir do root onde esse objeto poderá ser encontrado...
    if(!cloned){
        if(_root->_cloned_objects.size() == 0)
            return nullptr;
        //eu preciso restaurar o valor original...
        auto _new_path = path;
        auto _res = get_property_rec(_root->_cloned_objects[mn_index], _new_path);
        if(!_res.object_value())
            return nullptr;
        else
            return clone_object(_res.object_value(), mn_index, path);
    }
    
    typed_object* _new = nullptr;
    auto _mn = dynamic_cast<multiple_nodes*>(cloned);
    bool _void_mn = false; //if _mn was created by the user (in the property inspector)
    if(_mn){
        assert(_mn->_original);
        track_multiple_nodes(_mn);
        _new = _mn->_original->new_object<typed_object>();
        if(_mn->_is_void){
            if(_new->type_descriptor()->gtd() == general_type_descriptor::branch)
                _mn->branch_tag(_new->branch_tag());
            _void_mn = true;
        }
        _mn->_root = this->_root;
        _mn->_path = path;
    }
    else
        _new = cloned->type_descriptor()->new_object<typed_object>();
    
    if(_void_mn){
        return _new;
    }
    
    if(_new->type_descriptor()->gtd() == general_type_descriptor::branch)
        _new->branch_tag(cloned->branch_tag());
    
    std::vector<rb_string> _properties;
    _new->type_descriptor()->get_all_properties(_properties);
    for (auto _p : _properties){
        auto _new_path = path;
        _new_path.push_back({_p, static_cast<uint32_t>(-1)});
        auto _pgtd = cloned->type_descriptor()->property_type(_p)->gtd();
        
        if(cloned->type_descriptor()->property_private(_p))
            continue;
        if(_pgtd == general_type_descriptor::action)
            continue;
        auto _pv = cloned->type_descriptor()->get_property(cloned, _p);
        
        if(_pgtd == general_type_descriptor::branch || _pgtd == general_type_descriptor::object)
            _new->type_descriptor()->set_property(_new, _p, clone_object(_pv, mn_index, _new_path));
        else if(_pgtd == general_type_descriptor::vector)
            _new->type_descriptor()->set_property(_new, _p, clone_vector(_pv, mn_index, _new_path));
        else
            _new->type_descriptor()->set_property(_new, _p, _pv);
    }
    
    
    return _new;
        
}

std::vector<typed_object*> multiple_nodes::clone_vector(const std::vector<typed_object*>& cloned, int mn_index, const std::list<path_component>& path){
    if(cloned.size() == 0){
        if(_root->_cloned_objects.size() == 0)
            return cloned;
        
        auto _new_path = path;
        auto _res = get_property_rec(_root->_cloned_objects[mn_index], _new_path); //.vector_value();
        if(_res.is_null() || _res.vector_value().size() == 0)
            return cloned; //return the sized 0 vector
        else
            return clone_vector(_res.vector_value(), mn_index, path);
    }
    else {
        std::vector<typed_object*> _new_objs;
        auto _i = 0;
        for (auto _p : cloned){
            auto _mn = dynamic_cast<multiple_nodes*>(_p);
            if(_mn){
                track_multiple_nodes(_mn);
                _mn->_root = this->_root;
                if(!_mn->_is_void)
                    _new_objs.push_back(clone_object(_p, mn_index, _mn->_path));
                else {
                    auto _new = _mn->_original->new_object<typed_object>();
                    if(_new->type_descriptor()->gtd() == general_type_descriptor::branch)
                        _mn->branch_tag(_new->branch_tag());
                    _new_objs.push_back(_new);
                }
                //we don't update the path...
                //_mn->_path = path;
                //_mn->_path.back().index = _i;
            }
            else
                _new_objs.push_back(clone_object(_p, mn_index, path));
            
            _i++;
        }
        return _new_objs;
    }
}

void multiple_nodes::set_property_value(const std::list<path_component> &path, const rb::generic_object &value){ //this path is local to the object
    if(!_root || _root->_objects.size() == 0)
        return;
    
    auto _i = 0;
    std::list<path_component> _new_path = this->_path;
    for (auto _pc : path)
        _new_path.push_back(_pc);
    
    //we clone the values
    std::list<path_component> __p = _new_path;
    auto _pgtd = get_type_descriptor_rec(this->_root->_objects[0]->type_descriptor(), __p)->gtd();
    std::vector<generic_object> _cloned_values;
    for (int i = 0; i < this->_root->_objects.size(); i++) {
        if(_pgtd == general_type_descriptor::object || _pgtd == general_type_descriptor::branch)
            _cloned_values.push_back(clone_object(value, i, _new_path));
        else if(_pgtd == general_type_descriptor::vector)
            _cloned_values.push_back(clone_vector(value, i, _new_path));
        else
            _cloned_values.push_back(value);
    }
    
    for(auto _obj : this->_root->_objects){
        std::list<path_component> _p = _new_path;
        set_property_rec(_obj, _i, _p, _new_path, _cloned_values[_i]);
        _i++;
    }
    
    set_void(value, false);
    //if vector, set the path...
    if(value.type() == generic_object_type::vector){
        auto _v = value.vector_value();
        int _i = 0;
        for (auto _obj : _v){
            auto _obj2 = dynamic_cast<multiple_nodes*>(_obj);
            if(_obj2){
                _obj2->_path = _new_path;
                _obj2->_path.back().index = _i;
            }
            _i++;
        }
    }
}

void multiple_nodes::set_void(const rb::generic_object& obj, const bool value){
    if(obj.type() == generic_object_type::object){
        auto _obj = dynamic_cast<multiple_nodes*>(obj.object_value());
        if(_obj)
            _obj->_is_void = value;
    }
    else if(obj.type() == generic_object_type::vector){
        auto _v = obj.vector_value();
        for (auto _obj : _v){
            auto _obj2 = dynamic_cast<multiple_nodes*>(_obj);
            if(_obj2)
                _obj2->_is_void = value;
        }
    }
}

bool compare_vectors(const std::vector<typed_object*>& v1, const std::vector<typed_object*>& v2){
    if(v1.size() != v2.size())
        return false;
    else {
        for (auto i = 0; i < v1.size(); i++){
            if(!compare_objects(v1[i], v2[i])){
                return false;
            }
        }
    }
    
    return true;
}

generic_object multiple_nodes::get_property_rec(typed_object* site, std::list<path_component> &path){
    if(path.size() == 0){
        throw std::exception(); //should not get there...
    }
    else if(path.size() == 1){
        if(path.front().index == -1){
            return site->type_descriptor()->get_property(site, path.front().value);
        }
        else
        {
            std::vector<typed_object*> _v = site->type_descriptor()->get_vector_property<typed_object>(site, path.front().value);
            if(path.front().index < _v.size()){
                return _v[path.front().index];
            }
            else
                return nullptr;
        }
    }
    else {
        if(path.front().index == -1){
            typed_object* _obj = site->type_descriptor()->get_object_property<typed_object>(site, path.front().value);
            if(!_obj)
                return nullptr; //we don't set if the object is null...
            path.erase(path.begin());
            return get_property_rec(_obj, path);
        }
        else {
            std::vector<typed_object*> _v = site->type_descriptor()->get_vector_property<typed_object>(site, path.front().value);
            if(path.front().index >= _v.size())
                return nullptr; //we don't set if the index is invalid
            
            auto _obj = _v[path.front().index];
            
            if(!_obj)
                return nullptr; //unnecessary but we do the test...
            
            path.erase(path.begin());
            return get_property_rec(_obj, path);
        }
    }
}

void multiple_nodes::call_action_rec(typed_object* site, std::list<path_component> &path, const rb_string& action_name){
    if(path.size() == 0){
        throw std::exception(); //should not get there...
    }
    else if(path.size() == 1){
        if(path.front().index == -1){
            site->type_descriptor()->call_action(site, path.front().value, action_name);
        }
        else
        {
            throw std::exception(); //should not get there...
        }
    }
    else {
        if(path.front().index == -1){
            typed_object* _obj = site->type_descriptor()->get_object_property<typed_object>(site, path.front().value);
            if(!_obj)
                return; //we don't set if the object is null...
            path.erase(path.begin());
            get_property_rec(_obj, path);
        }
        else {
            std::vector<typed_object*> _v = site->type_descriptor()->get_vector_property<typed_object>(site, path.front().value);
            if(path.front().index >= _v.size())
                return; //we don't set if the index is invalid
            
            auto _obj = _v[path.front().index];
            
            if(!_obj)
                return; //unnecessary but we do the test...
            
            path.erase(path.begin());
            get_property_rec(_obj, path);
        }
    }
}

void multiple_nodes::get_autocomplete_rec(typed_object* site, std::list<path_component> &path, std::vector<rb_string>& suggestions){
    if(path.size() == 0){
        throw std::exception(); //should not get there...
    }
    else if(path.size() == 1){
        if(path.front().index == -1){
            suggestions = site->type_descriptor()->get_autocomplete_suggestions(site, path.front().value);
            return;
        }
        else
        {
            throw std::exception(); //should not get there...
        }
    }
    else {
        if(path.front().index == -1){
            typed_object* _obj = site->type_descriptor()->get_object_property<typed_object>(site, path.front().value);
            if(!_obj)
                return; //we don't set if the object is null...
            path.erase(path.begin());
            return get_autocomplete_rec(_obj, path, suggestions);
        }
        else {
            std::vector<typed_object*> _v = site->type_descriptor()->get_vector_property<typed_object>(site, path.front().value);
            if(path.front().index >= _v.size())
                return; //we don't set if the index is invalid
            
            auto _obj = _v[path.front().index];
            
            if(!_obj)
                return; //unnecessary but we do the test...
            
            path.erase(path.begin());
            return get_autocomplete_rec(_obj, path, suggestions);
        }
    }
}

std::vector<generic_object> multiple_nodes::get_property_values(const std::list<path_component>& path){ //this path is local to the object
    std::list<path_component> _new_path = this->_path;
    for (auto _pc : path)
        _new_path.push_back(_pc);
    
    std::vector<generic_object> _values;
    
    for(auto _obj : this->_root->_objects){
        std::list<path_component> _p = _new_path;
        _values.push_back(get_property_rec(_obj, _p));
    }
    
    return _values;
}

void multiple_nodes::get_autocomplete_suggestions(const std::list<path_component>& path, std::vector<std::vector<rb_string>>& suggestions_list){
    suggestions_list.clear();
    
    std::list<path_component> _new_path = this->_path;
    for (auto _pc : path)
        _new_path.push_back(_pc);
    
    for(auto _obj : this->_root->_objects){
        std::list<path_component> _p = _new_path;
        std::vector<rb_string> _s;
        get_autocomplete_rec(_obj, _p, _s);
        suggestions_list.push_back(_s);
    }
}

void multiple_nodes::get_representative_autocomplete_suggestions(const std::list<path_component> &path, std::vector<rb_string> &suggestions) {
    std::vector<std::vector<rb_string>> _suggestions_list;
    get_autocomplete_suggestions(path, _suggestions_list);
    
    std::unordered_set<rb_string> _already;
    
    suggestions.clear();
    for (auto& _v : _suggestions_list){
        for (auto& _s : _v){
            if(!_already.count(_s)){
                suggestions.push_back(_s);
                _already.insert(_s);
            }
        }
    }
}

void multiple_nodes::call_actions(const std::list<path_component>& path, const rb_string& action_name){
    std::list<path_component> _new_path = this->_path;
    for (auto _pc : path)
        _new_path.push_back(_pc);
    
    //type descriptor...
    const class type_descriptor* _start = this->_root->_objects[0]->type_descriptor();
    const class type_descriptor* _action_type = get_type_descriptor_rec(_start, _new_path);
    
    _new_path = this->_path;
    for (auto _pc : path)
        _new_path.push_back(_pc);
    
    for(auto _obj : this->_root->_objects){
        std::list<path_component> _p = _new_path;
        call_action_rec(_obj, _p, action_name);
        
        if((action_flags)((long)_action_type->action_flags() & (long)action_flags::dispatch_mask) == action_flags::single_dispatch)
            return;
    } 
}

generic_object multiple_nodes::get_representative_primitive_property_value(const std::list<path_component> &path){
    //we assume the property type is primitive...
    auto _values = get_property_values(path);
    
    for (auto i = 1; i < _values.size(); i++) {
        if (_values[i] != _values[0])
            return nullptr;
    }
    
    return _values[0];
}

const class type_descriptor* multiple_nodes::get_type_descriptor_rec(const rb::type_descriptor *start, std::list<path_component> &path){
    assert(path.size() != 0);
    
    auto _td = start->property_type(path.front().value);
    if(path.front().index != -1)
        _td = _td->element_type();
    
    if(path.size() == 1)
        return _td;
    
    if(path.size() > 1 && _td->gtd() == general_type_descriptor::vector)
        assert(path.front().index != -1);
    
    path.pop_front();
    return get_type_descriptor_rec(_td, path);
}

generic_object multiple_nodes::get_representative_object_property_value(const std::list<path_component> &path){
    auto _new_path = path;
    auto _start_td = this->_root == this ? _objects[0]->type_descriptor() : _original;
    auto _td = get_type_descriptor_rec(_start_td, _new_path);
    //we assume the property is an object...
    
    if(_td->nullable()) //is nullable
    {
        //we just compare the objects...
        auto _objs = get_property_values(path);
        
        for (auto i = 1; i < _objs.size(); i++) {
            if (!compare_objects(_objs[i], _objs[0]))
                return nullptr;
        }
        
        if(!_objs[0].object_value())
            return nullptr; //is null
        
        //we create a new multiple node...
        auto _mn = track_multiple_nodes(new multiple_nodes());
        _mn->_original = const_cast<class type_descriptor*>(_td);
        _mn->_root = this->_root;
        _mn->_path = this->_path;
        for (auto _pc : path)
            _mn->_path.push_back(_pc); //we point to the path of the object...
        if(_td->gtd() == general_type_descriptor::branch)
            _mn->branch_tag(_objs[0].object_value()->branch_tag());
        return _mn;
    }
    else {
        auto _objs = get_property_values(path);
        
        //we just return the object...
        auto _mn = track_multiple_nodes(new multiple_nodes());
        _mn->_original = const_cast<class type_descriptor*>(_td);
        _mn->_root = this->_root;
        _mn->_path = this->_path;
        for (auto _pc : path)
            _mn->_path.push_back(_pc); //we point to the path of the object...
        
        if(_td->gtd() == general_type_descriptor::branch){
            //compare the tags
            bool _tag_set = false;
            for (auto i = 1; i < _objs.size(); i++) {
                if (_objs[i].object_value()->branch_tag() != _objs[0].object_value()->branch_tag())
                {
                    _mn->branch_tag(u"<NULL>");
                    _tag_set = true;
                    break;
                }
            }
            
            if(!_tag_set)
                _mn->branch_tag(_objs[0].object_value()->branch_tag());
        }
        
        return _mn;
    }
}

generic_object multiple_nodes::get_representative_vector_property_value(const std::list<path_component> &path){
    //we assume the property is a vector...
    auto _new_path = path;
    auto _start_td = this->_root == this ? _objects[0]->type_descriptor() : _original;
    auto _td = get_type_descriptor_rec(_start_td, _new_path)->element_type();
    //we just compare the objects...
    auto _objs = get_property_values(path);
    
    for (auto i = 1; i < _objs.size(); i++) {
        if (!compare_vectors(_objs[i], _objs[0]))
            return std::vector<typed_object*>();
    }
    
    _new_path = this->_path;
    for (auto _pc : path)
        _new_path.push_back(_pc); //we point to the path of the object...
    
    std::vector<typed_object*> _result;
    auto _v0 = _objs[0].vector_value();
    for (auto i = 0; i < _v0.size(); i++) {
        auto _mn = track_multiple_nodes(new multiple_nodes());
        
        _mn->_original = const_cast<class type_descriptor*>(_td);
        _mn->_root = this->_root;
        _mn->_path = _new_path;
        _mn->_path.back().index = i;
        
        if(_td->gtd() == general_type_descriptor::branch)
            _mn->branch_tag(_v0[i]->branch_tag());
        
        _result.push_back(_mn);
    }
    
    return _result;
}

generic_object multiple_nodes::get_representative_property_value(const std::list<path_component> &path){
    if(!_root || _root->_objects.size() == 0)
        return nullptr;
    
    auto _new_path = path;
    auto _td = get_type_descriptor_rec(this->type_descriptor(), _new_path);
    
    if(_td->gtd() == general_type_descriptor::object || _td->gtd() == general_type_descriptor::branch)
        return get_representative_object_property_value(path);
    else if(_td->gtd() == general_type_descriptor::vector)
        return get_representative_vector_property_value(path);
    else
        return get_representative_primitive_property_value(path);
}

multiple_nodes* multiple_nodes::track_multiple_nodes(rb::multiple_nodes *mn) const{
    const_cast<multiple_nodes*>(this)->_root->_to_be_freed.insert(mn);
    return mn;
}

std::list<multiple_nodes::path_component> multiple_nodes::create_local_path(const rb_string &value, const uint32_t index){
    std::list<multiple_nodes::path_component> _path;
    _path.push_back({value, (uint32_t)index});
    return _path;
}

void multiple_nodes::add_property(const class type_descriptor *td, const rb_string &p){
    if(p == u"rb::transformation")
        return; //No transformation
    if(!td->property_editable(p))
        return;
    if(td->property_private(p))
        return;
    
    auto _gtd = td->property_type(p)->gtd();
    if(_gtd == general_type_descriptor::angle){
        nullable_angle_property<multiple_nodes>(p, td->property_display_name(p), td->property_editable(p), td->property_continuous(p), td->property_precision(p), {
            [=](const multiple_nodes* site){
                return const_cast<multiple_nodes*>(site)->get_representative_property_value(create_local_path(p));
            },
            [=](multiple_nodes* site, const nullable<float> value){
                const_cast<multiple_nodes*>(site)->set_property_value(create_local_path(p), value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::boolean){
        nullable_boolean_property<multiple_nodes>(p, td->property_display_name(p), td->property_editable(p), {
            [=](const multiple_nodes* site){
                return const_cast<multiple_nodes*>(site)->get_representative_property_value(create_local_path(p));
            },
            [=](multiple_nodes* site, const nullable<bool> value){
                const_cast<multiple_nodes*>(site)->set_property_value(create_local_path(p), value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::color){
        nullable_color_property<multiple_nodes>(p, td->property_display_name(p), td->property_editable(p), td->property_continuous(p), {
            [=](const multiple_nodes* site){
                return const_cast<multiple_nodes*>(site)->get_representative_property_value(create_local_path(p));
            },
            [=](multiple_nodes* site, const nullable<color> value){
                const_cast<multiple_nodes*>(site)->set_property_value(create_local_path(p), value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::enumeration){
        nullable_enumeration_property<multiple_nodes, long>(p, td->property_display_name(p), td->property_type(p)->enumeration_values(), td->property_editable(p), {
            [=](const multiple_nodes* site){
                return const_cast<multiple_nodes*>(site)->get_representative_property_value(create_local_path(p));
            },
            [=](multiple_nodes* site, const nullable<long> value){
                const_cast<multiple_nodes*>(site)->set_property_value(create_local_path(p), value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::image){
        nullable_image_property<multiple_nodes>(p, td->property_display_name(p), td->property_editable(p), {
            [=](const multiple_nodes* site){
                return const_cast<multiple_nodes*>(site)->get_representative_property_value(create_local_path(p));
            },
            [=](multiple_nodes* site, const nullable<rb_string> value){
                const_cast<multiple_nodes*>(site)->set_property_value(create_local_path(p), value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::integer){
        nullable_integer_property<multiple_nodes>(p, td->property_display_name(p), td->property_editable(p), {
            [=](const multiple_nodes* site){
                return const_cast<multiple_nodes*>(site)->get_representative_property_value(create_local_path(p));
            },
            [=](multiple_nodes* site, const nullable<long> value){
                const_cast<multiple_nodes*>(site)->set_property_value(create_local_path(p), value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::flags){
        nullable_flags_property<multiple_nodes, long>(p, td->property_display_name(p), td->property_type(p)->flag_names(), td->property_editable(p), {
            [=](const multiple_nodes* site){
                return const_cast<multiple_nodes*>(site)->get_representative_property_value(create_local_path(p));
            },
            [=](multiple_nodes* site, const nullable<long> value){
                const_cast<multiple_nodes*>(site)->set_property_value(create_local_path(p), value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::ranged){
        nullable_ranged_property<multiple_nodes>(p, td->property_display_name(p), td->property_editable(p), td->property_continuous(p), td->property_precision(p), {
            [=](const multiple_nodes* site){
                return const_cast<multiple_nodes*>(site)->get_representative_property_value(create_local_path(p));
            },
            [=](multiple_nodes* site, const nullable<float> value){
                const_cast<multiple_nodes*>(site)->set_property_value(create_local_path(p), value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::single){
        nullable_single_property<multiple_nodes>(p, td->property_display_name(p), td->property_editable(p), {
            [=](const multiple_nodes* site){
                return const_cast<multiple_nodes*>(site)->get_representative_property_value(create_local_path(p));
            },
            [=](multiple_nodes* site, const nullable<float> value){
                const_cast<multiple_nodes*>(site)->set_property_value(create_local_path(p), value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::string){
        nullable_string_property<multiple_nodes>(p, td->property_display_name(p), td->property_editable(p), td->property_continuous(p), {
            [=](const multiple_nodes* site){
                return const_cast<multiple_nodes*>(site)->get_representative_property_value(create_local_path(p));
            },
            [=](multiple_nodes* site, const nullable<rb_string> value){
                const_cast<multiple_nodes*>(site)->set_property_value(create_local_path(p), value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::string_with_autocompletion){
        nullable_string_with_autocompletion_property<multiple_nodes>(p, td->property_display_name(p), td->property_editable(p), {
            [=](const multiple_nodes* site){
                return const_cast<multiple_nodes*>(site)->get_representative_property_value(create_local_path(p));
            },
            [=](multiple_nodes* site, const nullable<rb_string> value){
                const_cast<multiple_nodes*>(site)->set_property_value(create_local_path(p), value);
            }
        },
            [=](const multiple_nodes* site){
                std::vector<rb_string> _suggestions;
                const_cast<multiple_nodes*>(site)->get_representative_autocomplete_suggestions(create_local_path(p), _suggestions);
                return _suggestions;
            }
        );
    }
    else if(_gtd == general_type_descriptor::vec2){
        nullable_vec2_property<multiple_nodes>(p, td->property_display_name(p), td->property_editable(p), {
            [=](const multiple_nodes* site){
                return const_cast<multiple_nodes*>(site)->get_representative_property_value(create_local_path(p));
            },
            [=](multiple_nodes* site, const nullable<vec2> value){
                const_cast<multiple_nodes*>(site)->set_property_value(create_local_path(p), value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::vector){
        auto _tmn = new multiple_nodes(td->property_type(p)->element_type());
        auto _new_td = _tmn->vector_type_descriptor();
        delete _tmn;
        
        vector_property<multiple_nodes>(p, td->property_display_name(p), _new_td, {
            [=](const multiple_nodes* site){
                return const_cast<multiple_nodes*>(site)->get_representative_property_value(create_local_path(p));
            },
            [=](multiple_nodes* site, const std::vector<typed_object*>& value){
                site->set_property_value(create_local_path(p), value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::action){
        action<multiple_nodes>(p, td->property_display_name(p), td->property_type(p)->action_flags(), td->property_type(p)->action_names(),
           [=](multiple_nodes* site, const rb_string& action_name){
               const_cast<multiple_nodes*>(site)->call_actions(create_local_path(p), action_name);
           });
    }
    else { //branch or object
        auto _tmn = new multiple_nodes(td->property_type(p));
        auto _new_td = _tmn->type_descriptor();
        auto _null_new_td = _tmn->nullable_type_descriptor();
        delete _tmn;
        
        if(td->property_type(p)->nullable()){
            nullable_object_property<multiple_nodes>(p, td->property_display_name(p), _null_new_td, {
                [=](const multiple_nodes* site){
                    return const_cast<multiple_nodes*>(site)->get_representative_property_value(create_local_path(p));
                },
                [=](multiple_nodes* site, typed_object* value){
                    site->set_property_value(create_local_path(p), value);
                }
            });
        }
        else {
            object_property<multiple_nodes>(p, td->property_display_name(p), _new_td, td->property_inlined(p), {
                [=](const multiple_nodes* site){
                    return const_cast<multiple_nodes*>(site)->get_representative_property_value(create_local_path(p));
                },
                [=](multiple_nodes* site, typed_object* value){
                    site->set_property_value(create_local_path(p), value);
                }
            });
        }
    }
}