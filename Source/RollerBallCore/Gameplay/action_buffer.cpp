//
//  action_buffer.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 2014-04-20.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#include "action_buffer.h"
#include "action_target.h"
#include "scene.h"

using namespace rb;

static void align4(size_t* s){
    if((*s) == 0)
        return;
    auto _mod = (*s) % 4;
    if(_mod == 0)
        return;
    
    *s = ((*s) / 4) * 4 + 4;
}

static void* sto_buffer(void* buffer, const rb_string& str){
    auto _size = 8 + str.size() * sizeof(rb_string::value_type);
    align4(&_size);
    auto _mem = (uint32_t*)buffer;
    _mem[0] = (uint32_t)_size;
    _mem[1] = (uint32_t)str.size();
    auto _chars = (char16_t*)(_mem + 2);
    for (size_t i = 0; i < str.size(); i++) {
        _chars[i] = str[i];
    }
    char* _cmem = (char*)buffer;
    _cmem += _size;
    return _cmem;
}

static size_t get_size_string(const rb_string& str){
    auto _size = 8 + str.size() * sizeof(rb_string::value_type);
    align4(&_size);
    return _size;
}

static rb_string sfrom_buffer(void* buffer, void** next){
    uint32_t* _u32mem = (uint32_t*)buffer;
    uint32_t _sizeBytes = _u32mem[0];
    uint32_t _countItems = _u32mem[1];
    rb_string _res = u"";
    char16_t* _tmem = (char16_t*)(_u32mem + 2);
    
    for (uint32_t i = 0; i < _countItems; i++) {
        _res.push_back(*_tmem);
        _tmem += 1;
    }
    *next = (((char*)buffer) + _sizeBytes);
    return _res;
}

static size_t get_size_action_distach_info(const action_dispatch_info& adi){
    return 4 + get_size_string(adi.target) + get_size_string(adi.action_name) + get_size_string(adi.action_arg);
}

static void* adito_buffer(void* buffer, const action_dispatch_info& adi){
    uint32_t* _ui32mem = (uint32_t*)buffer;
    _ui32mem[0] = (uint32_t)adi.by_class;
    _ui32mem += 1;
    char* _cmem = (char*)_ui32mem;
    _cmem = (char*)sto_buffer(_cmem, adi.target);
    _cmem = (char*)sto_buffer(_cmem, adi.action_name);
    _cmem = (char*)sto_buffer(_cmem, adi.action_arg);
    return _cmem;
}

static action_dispatch_info adifrom_buffer(void* buffer, void** next){
    uint32_t* _u32mem = (uint32_t*)buffer;
    action_dispatch_info adi;
    adi.by_class = (bool)_u32mem[0];
    void* _cmem = _u32mem + 1;
    adi.target = sfrom_buffer(_cmem, &_cmem);
    adi.action_name = sfrom_buffer(_cmem, &_cmem);
    adi.action_arg = sfrom_buffer(_cmem, &_cmem);
    *next = _cmem;
    return adi;
}

static size_t get_size_action_info(const action_info& ai){
    auto _s = get_size_string(ai.name) + 4;
    for (auto _a : ai.actions){
        _s += get_size_action_distach_info(_a);
    }
    return _s;
}

static void* aito_buffer(void* buffer, const action_info& ai){
    buffer = sto_buffer(buffer, ai.name);
    uint32_t* _u32mem = (uint32_t*)buffer;
    _u32mem[0] = (uint32_t)ai.actions.size();
    buffer = _u32mem + 1;
    for (auto _a : ai.actions){
        buffer = adito_buffer(buffer, _a);
    }
    return buffer;
}

static action_info aifrom_buffer(void* buffer, void** next){
    action_info ai;
    ai.name = sfrom_buffer(buffer, &buffer);
    auto _u32mem = (uint32_t*)buffer;
    auto _size = _u32mem[0];
    buffer = _u32mem + 1;
    for (uint32_t i = 0; i < _size; i++) {
        ai.actions.push_back(adifrom_buffer(buffer, &buffer));
    }
    *next = buffer;
    if(ai.actions.size() > 0)
        ai.current_action = ai.actions.begin();
    else
        ai.current_action = nullptr;
    return ai;
}

static size_t get_size_action_info_vector(const std::list<action_info> actions){
    size_t _s = 4;
    for (auto _a : actions){
        _s += get_size_action_info(_a);
    }
    return _s;
}

static void* aivto_buffer(void* buffer, const std::list<action_info> actions){
    uint32_t* _u32mem = (uint32_t*)buffer;
    _u32mem[0] = (uint32_t)actions.size();
    buffer = _u32mem + 1;
    for (auto _a : actions){
        buffer = aito_buffer(buffer, _a);
    }
    return buffer;
}

static std::list<action_info> aivfrom_buffer(void* buffer){
    uint32_t* _u32mem = (uint32_t*)buffer;
    auto _size = _u32mem[0];
    buffer = _u32mem + 1;
    std::list<action_info> _ai;
    for (uint32_t i = 0; i < _size; i++) {
        _ai.push_back(aifrom_buffer(buffer, &buffer));
    }
    return _ai;
}

buffer action_buffer::save_state() const {
    auto _size = get_size_action_info_vector(_actions);
    auto _mem = malloc(_size);
    aivto_buffer(_mem, _actions);
    buffer _b = buffer(_mem, _size);
    free(_mem);
    return _b;
}

void action_buffer::restore_state(rb::buffer buff){
    _actions = aivfrom_buffer(const_cast<void*>(buff.internal_buffer()));
    if(_actions.size() > 0)
        _current_action = _actions.begin();
    else
        _current_action = nullptr;
}

action_buffer::action_buffer(){
    _current_action = nullptr;
}

void action_buffer::goto_first_action(){
    if(_actions.size() == 0)
        _current_action = nullptr;
    else
        _current_action = _actions.begin();
    goto_first_subaction();
    notify_properties_changed();
}

void action_buffer::goto_previous_action(){
    if(_actions.size() == 0)
        _current_action = nullptr;
    else {
        if(_current_action.has_value())
        {
            if(_current_action.value() == _actions.begin())
                return;
            _current_action = std::prev(_current_action.value());
        }
        else
            _current_action = _actions.begin();
    }
    goto_first_subaction();
    notify_properties_changed();
}

void action_buffer::goto_next_action(){
    if(_actions.size() == 0)
        _current_action = nullptr;
    else {
        if(_current_action.has_value())
        {
            auto _end = std::prev(_actions.end());
            if(_current_action.value() == _end)
                return;
            _current_action = std::next(_current_action.value());
        }
        else
            _current_action = std::prev(_actions.end());
    }
    goto_first_subaction();
    notify_properties_changed();
}

void action_buffer::goto_last_action(){
    if(_actions.size() == 0)
        _current_action = nullptr;
    else
        _current_action = std::prev(_actions.end());
    goto_first_subaction();
    notify_properties_changed();
}

void action_buffer::goto_first_subaction(){
    if(!_current_action.has_value())
        return;
    if(_current_action->actions.size() == 0)
        _current_action->current_action = nullptr;
    else
        _current_action->current_action = _current_action->actions.begin();
    notify_properties_changed();
}

void action_buffer::goto_previous_subaction(){
    if(!_current_action.has_value())
        return;
    if(_current_action->actions.size() == 0)
        _current_action->current_action = nullptr;
    else {
        if(_current_action->current_action.has_value())
        {
            if(_current_action->current_action.value() == _current_action->actions.begin())
                return;
            _current_action->current_action = std::prev(_current_action->current_action.value());
        }
        else
            _current_action->current_action = _current_action->actions.begin();
    }
    notify_properties_changed();
}

void action_buffer::goto_next_subaction(){
    if(!_current_action.has_value())
        return;
    if(_current_action->actions.size() == 0)
        _current_action->current_action = nullptr;
    else {
        if(_current_action->current_action.has_value())
        {
            auto _end = std::prev(_current_action->actions.end());
            if(_current_action->current_action.value() == _end)
                return;
            _current_action->current_action = std::next(_current_action->current_action.value());
        }
        else
            _current_action->current_action = std::prev(_current_action->actions.end());
    }
    notify_properties_changed();
}

void action_buffer::goto_last_subaction(){
    if(!_current_action.has_value())
        return;
    if(_current_action->actions.size() == 0)
        _current_action->current_action = nullptr;
    else
        _current_action->current_action = std::prev(_current_action->actions.end());
    notify_properties_changed();
}


void action_buffer::notify_properties_changed(){
    notify_property_changed(u"current_action_name");
    notify_property_changed(u"current_target");
    notify_property_changed(u"current_by_class");
    notify_property_changed(u"current_action_to_call");
    notify_property_changed(u"current_action_arg");
}

rb_string action_buffer::current_action_name() const {
    if(!_current_action.has_value())
        return u"";
    return _current_action->name;
}

void action_buffer::current_action_name(const rb_string &value){
    if(!_current_action.has_value())
        return;
    _current_action->name = value;
}

rb_string action_buffer::current_target() const {
    if(!_current_action.has_value())
        return u"";
    if(!_current_action->current_action.has_value())
        return u"";
    return _current_action->current_action->target;
}

void action_buffer::current_target(const rb_string &value){
    if(!_current_action.has_value())
        return;
    if(!_current_action->current_action.has_value())
        return;
    _current_action->current_action->target = value;
}

bool action_buffer::current_by_class() const {
    if(!_current_action.has_value())
        return u"";
    if(!_current_action->current_action.has_value())
        return u"";
    return _current_action->current_action->by_class;
}

void action_buffer::current_by_class(bool value){
    if(!_current_action.has_value())
        return;
    if(!_current_action->current_action.has_value())
        return;
    _current_action->current_action->by_class = value;
}

rb_string action_buffer::current_action_to_call() const {
    if(!_current_action.has_value())
        return u"";
    if(!_current_action->current_action.has_value())
        return u"";
    return _current_action->current_action->action_name;
}

void action_buffer::current_action_to_call(const rb_string &value){
    if(!_current_action.has_value())
        return;
    if(!_current_action->current_action.has_value())
        return;
    _current_action->current_action->action_name = value;
}

rb_string action_buffer::current_action_arg() const {
    if(!_current_action.has_value())
        return u"";
    if(!_current_action->current_action.has_value())
        return u"";
    return _current_action->current_action->action_arg;
}

void action_buffer::current_action_arg(const rb_string &value){
    if(!_current_action.has_value())
        return;
    if(!_current_action->current_action.has_value())
        return;
    _current_action->current_action->action_arg = value;
}

void action_buffer::create_new_action(const rb_string &name){
    action_info ai;
    ai.name = name;
    ai.current_action = nullptr;
    _actions.push_back(ai);
    _current_action = std::prev(_actions.end());
    notify_properties_changed();
}

void action_buffer::delete_current_action(){
    if(!_current_action.has_value())
        return;
    _current_action = _actions.erase(_current_action.value());
    if(_actions.size() == 0)
        _current_action = nullptr;
    else if(_current_action.value() == _actions.end())
        _current_action = _actions.begin();
    notify_properties_changed();
}

void action_buffer::insert_new_subaction(){
    if(!_current_action.has_value())
        return;
    action_dispatch_info adi;
    adi.by_class = true;
    adi.target = u"<placeholder>";
    _current_action->actions.push_back(adi);
    _current_action->current_action = std::prev(_current_action->actions.end());
    notify_properties_changed();
}

void action_buffer::delete_current_subaction(){
    if(!_current_action.has_value())
        return;
    if(!_current_action->current_action.has_value())
        return;
    _current_action->current_action = _current_action->actions.erase(_current_action->current_action.value());
    if(_current_action->actions.size() == 0)
        _current_action->current_action = nullptr;
    else if(_current_action->current_action.value() == _current_action->actions.end())
        _current_action->current_action = _current_action->actions.begin();
    notify_properties_changed();
}

void action_buffer::perform_action(const rb_string &name){
    action_info _ai;
    bool _found = false;
    
    for (auto _aii : _actions){
        if(_aii.name == name){
            _ai = _aii;
            _found = true;
            break;
        }
    }
    
    if(!_found)
        return;
    
    std::vector<node*> _nodes;
    for (auto _adi : _ai.actions){
        _nodes.clear();
        if(_adi.by_class)
            _nodes = parent_scene()->node_with_one_class(_adi.target);
        else {
            auto _n = parent_scene()->node_with_name(_adi.target);
            if(_n)
                _nodes.push_back(_n);
        }
        
        for (auto _n : _nodes){
            auto _at = dynamic_cast<action_target*>(_n);
            if(_at){
                _at->do_action(_adi.action_name, _adi.action_arg);
            }
        }
    }
}

rb_string action_buffer::type_name() const {
    return u"rb::action_buffer";
}

rb_string action_buffer::displayable_type_name() const {
    return u"Action Buffer";
}

void action_buffer::describe_type(){
    nvnode::describe_type();
    
    start_type<action_buffer>([](){return new action_buffer();});
    string_property<action_buffer>(u"current_new_action_name", u"New", true, false, {
        [](const action_buffer* site){
            return site->_new_action_name;
        },
        [](action_buffer* site, const rb_string& value){
            site->_new_action_name = value;
        }
    });
    action<action_buffer>(u"new", u"", action_flags::multi_dispatch, {u"New Action", u"New Subaction"}, [](action_buffer* site, const rb_string& action_name){
        if(action_name == u"New Action")
            site->create_new_action(site->_new_action_name);
        else
            site->insert_new_subaction();
    });
    action<action_buffer>(u"goto", u"Goto Act", action_flags::multi_dispatch, {u"First", u"Previous", u"Next", u"Last"}, [](action_buffer* site, const rb_string& action_name){
        if(action_name == u"First")
            site->goto_first_action();
        else if(action_name == u"Previous")
            site->goto_previous_action();
        else if(action_name == u"Next")
            site->goto_next_action();
        else
            site->goto_last_action();
    });
    action<action_buffer>(u"goto_subact", u"Goto Subact", action_flags::multi_dispatch, {u"First", u"Previous", u"Next", u"Last"}, [](action_buffer* site, const rb_string& action_name){
        if(action_name == u"First")
            site->goto_first_subaction();
        else if(action_name == u"Previous")
            site->goto_previous_subaction();
        else if(action_name == u"Next")
            site->goto_next_subaction();
        else
            site->goto_last_subaction();
    });
    string_property<action_buffer>(u"current_action_name", u"Act Name", true, false, {
        [](const action_buffer* site){
            return site->current_action_name();
        },
        [](action_buffer* site, const rb_string& value){
            site->current_action_name(value);
        }
    });
    boolean_property<action_buffer>(u"current_by_class", u"By Class", true, {
        [](const action_buffer* site){
            return site->current_by_class();
        },
        [](action_buffer* site, bool value){
            site->current_by_class(value);
        }
    });
    string_property<action_buffer>(u"current_target", u"Target", true, false, {
        [](const action_buffer* site){
            return site->current_target();
        },
        [](action_buffer* site, const rb_string& value){
            site->current_target(value);
        }
    });
    string_property<action_buffer>(u"current_action_to_call", u"Action", true, false, {
        [](const action_buffer* site){
            return site->current_action_to_call();
        },
        [](action_buffer* site, const rb_string& value){
            site->current_action_to_call(value);
        }
    });
    string_property<action_buffer>(u"current_action_arg", u"Arg", true, false, {
        [](const action_buffer* site){
            return site->current_action_arg();
        },
        [](action_buffer* site, const rb_string& value){
            site->current_action_arg(value);
        }
    });
    action<action_buffer>(u"delete", u"Delete", action_flags::multi_dispatch, {u"Action", u"Subaction"}, [](action_buffer* site, const rb_string& action_name){
        if(action_name == u"Action")
            site->delete_current_action();
        else
            site->delete_current_subaction();
    });
    begin_private_properties();
    buffer_property<action_buffer>(u"internal_data", u"Internal Data", {
        [](const action_buffer* site){
            return site->save_state();
        },
        [](action_buffer* site, buffer value){
            site->restore_state(value);
        }
    });
    end_private_properties();
    end_type();
}



























