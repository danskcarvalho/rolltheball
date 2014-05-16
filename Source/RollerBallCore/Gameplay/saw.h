//
//  saw.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 05/01/14.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__saw__
#define __RollerBallCore__saw__

#include "components_base.h"
#include "node.h"
#include "base_enemy.h"
#include "resettable_component.h"
#include "action_target.h"
#include <Box2D/Box2D.h>

namespace rb {
    class sprite_component;
    class animation_manager_component;
    class saw : public node, public base_enemy, public resettable_component, public action_target {
    protected:
        virtual void reset_component() override;
        virtual void reset_physics() override;
    private:
        bool _initialized;
        bool _animatable;
        sprite_component* _sprite;
        b2World* _world;
        b2Body* _body;
        transform_space _saved_transform;
        //destruction
        bool _destructible;
        bool _destroyed;
        animation_manager_component* _an_manager;
    protected:
        virtual bool should_serialize_children() const override;
        virtual void after_becoming_active(bool node_was_moved) override;
    protected:
        //Update
        virtual void update(float dt);
        //Typed Object
        virtual void describe_type() override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
        bool animatable() const;
        bool animatable(bool value);
    public:
        virtual void playing() override;
    public:
        saw();
    public:
        virtual void do_action(const rb_string& action_name, const rb_string& arg) override;
        //destruction
    public:
        bool destructible() const;
        bool destructible(bool value);
        bool destroyed() const;
        void destroy_saw();
    };
}

#endif /* defined(__RollerBallCore__saw__) */
