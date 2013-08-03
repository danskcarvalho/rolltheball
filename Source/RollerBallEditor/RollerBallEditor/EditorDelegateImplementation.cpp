//
//  EditorDelegateImplementation.cpp
//  RollerBallEditor
//
//  Created by Danilo Santos de Carvalho on 20/05/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#include "EditorDelegateImplementation.h"
#import "DSEditorDelegate.h"

using namespace rb;

inline NSString* to_platform_string(const rb::rb_string& str){
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> _str_converter;
    auto _u8Str = _str_converter.to_bytes(str);
    NSString* _platform_str = [NSString stringWithUTF8String:_u8Str.c_str()];
    return _platform_str;
}

EditorDelegateImplementation::EditorDelegateImplementation(DSEditorDelegate* delegate){
    _delegate = delegate;
}

void EditorDelegateImplementation::selection_changed(rb::node_container *container){
    [_delegate delegateSelectionChanged: container];
}

void EditorDelegateImplementation::hierarchy_changed(rb::node_container *container){
    [_delegate delegateHierarchyChanged: container];
}

void EditorDelegateImplementation::current_changed(){
    [_delegate delegateCurrentChanged];
}

void EditorDelegateImplementation::name_changed(rb::node *n){
    [_delegate delegateNameChanged: n];
}

void EditorDelegateImplementation::creatable_type_was_registered(const type_descriptor* td){
    [_delegate delegateCreatableTypeWasRegistered:const_cast<type_descriptor*>(td)];
}

void EditorDelegateImplementation::alert(const rb_string &message){
    [_delegate alert:to_platform_string(message)];
}

void EditorDelegateImplementation::confirm(const rb_string &message, std::function<void (bool)> result){
    [_delegate confirm:to_platform_string(message) confirmationBlock:^(BOOL value) {
        result(value);
    }];
}

void EditorDelegateImplementation::push_object_in_property_inspector(rb::typed_object *obj, const rb_string& title){
    [_delegate delegatePushObjectInPropertyInspector:obj withTitle:to_platform_string(title)];
}









