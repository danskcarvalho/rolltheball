//
//  physics_engine.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 13/08/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__physics_engine__
#define __RollerBallCore__physics_engine__

#include "components_base.h"
#include "nvnode.h"

#define PHYS_ENGINE_UPDATE_PRIORITY -10000
#define PHYS_OBJECT_UPDATE_PRIORITY -20000
#define PHYS_ENGINE_TIME_STEP (1.0f / 60.0f)
#define PHYS_ENGINE_STEPS_PER_FRAME 2

class b2World;

namespace rb {
    class physics_engine : public nvnode {
    private:
        b2World* _world;
        vec2 _default_gravity;
    public:
        physics_engine();
        ~physics_engine();
    protected:
        //Typed Object
        virtual void describe_type() override;
        virtual void after_becoming_active(bool node_was_moved) override;
        virtual void before_becoming_inactive(bool node_was_moved) override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
        //Physics World
    public:
        b2World* world() const;
    protected:
        //Update
        virtual void update(float dt) override;
    public:
        const vec2& default_gravity() const;
        const vec2& default_gravity(const vec2& value);
    };
}

#endif /* defined(__RollerBallCore__physics_engine__) */
