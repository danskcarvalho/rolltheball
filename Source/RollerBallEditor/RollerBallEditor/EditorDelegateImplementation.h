//
//  EditorDelegateImplementation.h
//  RollerBallEditor
//
//  Created by Danilo Santos de Carvalho on 20/05/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallEditor__EditorDelegateImplementation__
#define __RollerBallEditor__EditorDelegateImplementation__

#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#define MACOSX_TARGET
#include "components_external.h"

@class DSEditorDelegate;
namespace rb {
    class EditorDelegateImplementation : public editor_delegate {
    private:
        DSEditorDelegate* _delegate;
    public:
        EditorDelegateImplementation(DSEditorDelegate* delegate);
        virtual void selection_changed(node_container* container) override;
        virtual void hierarchy_changed(node_container* container) override;
        virtual void current_changed() override;
        virtual void name_changed(node* n) override;
        virtual void creatable_type_was_registered(const type_descriptor* td) override;
        virtual void alert(const rb_string& message) override;
        virtual void confirm(const rb_string& message, std::function<void (bool)> result) override;
        virtual void push_object_in_property_inspector(typed_object* obj, const rb_string& title) override;
    };
}

#endif /* defined(__RollerBallEditor__EditorDelegateImplementation__) */
