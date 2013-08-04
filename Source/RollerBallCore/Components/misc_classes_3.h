//
//  misc_classes_3.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 04/08/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__misc_classes_3__
#define __RollerBallCore__misc_classes_3__

#include "components_base.h"
#include "typed_object.h"

namespace rb {
    class node_without_transform : public typed_object {
    private:
        class typed_object* _node;
        void add_nullable_property(const class type_descriptor *td, const rb_string &p);
        void add_non_nullable_property(const class type_descriptor *td, const rb_string &p);
        void add_property(const class type_descriptor *td, const rb_string &p);
    public:
        node_without_transform(class typed_object* n);
        virtual ~node_without_transform();
        class typed_object* node();
    protected:
        //Typed Object
        virtual void describe_type() override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
    };
}

#endif /* defined(__RollerBallCore__misc_classes_3__) */
