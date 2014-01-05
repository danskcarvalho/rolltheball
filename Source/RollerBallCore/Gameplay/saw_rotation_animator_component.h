//
//  saw_rotation_animator.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 05/01/14.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__saw_rotation_animator__
#define __RollerBallCore__saw_rotation_animator__

#include "components_base.h"
#include "nvnode.h"

namespace rb {
    class saw_rotation_animator_component : public nvnode {
    private:
        std::vector<node*> _nodes;
        float _r_velocity;
    protected:
        //Update
        virtual void update(float dt);
        //Typed Object
        virtual void describe_type() override;
        virtual void after_becoming_active(bool node_was_moved) override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
    public:
        float rotation_velocity() const;
        float rotation_velocity(float value);
        saw_rotation_animator_component();
    };
}

#endif /* defined(__RollerBallCore__saw_rotation_animator__) */
