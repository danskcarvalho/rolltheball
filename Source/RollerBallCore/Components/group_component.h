//
//  group_component.h
//  RollerBallCore
//
//  Created by Danilo Santos de Carvalho on 28/05/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__group_component__
#define __RollerBallCore__group_component__

#include "components_base.h"
#include "node.h"

namespace rb {
    class group_component : public node {
    private:
        bool _copy_node;
    public:
        inline bool copy_node() const {
            return _copy_node;
        }
        inline bool copy_node(const bool value) {
            _copy_node = value;
            return _copy_node;
        }
    public:
        group_component();
        virtual ~group_component();
    protected:
        //Typed Object
        virtual void describe_type() override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
    public:
        void ungroup(bool select_ungrouped, bool delete_self = true);
        virtual std::vector<rb_string> transformables() override;
        virtual void start_transformation(long index) override;
    };
}

#endif /* defined(__RollerBallCore__group_component__) */
