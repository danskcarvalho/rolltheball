//
//  nvnode.h
//  RollerBallCore
//
//  Created by Danilo Santos de Carvalho on 20/05/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__nvnode__
#define __RollerBallCore__nvnode__

#include "node.h"

namespace rb {
    class nvnode : public node {
    public:
        virtual ~nvnode();
    public:
        virtual bool renderable() const override;
    protected:
        //Typed Object
        virtual void describe_type() override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
        virtual bool add_node_at(node* n, uint32_t at) override;
    };
}

#endif /* defined(__RollerBallCore__nvnode__) */
