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
#include <Box2D/Box2D.h>

namespace rb {
    class sprite_component;
    class saw : public node, public base_enemy {
    private:
        bool _initialized;
        sprite_component* _sprite;
        b2World* _world;
        b2Body* _body;
    protected:
        //Typed Object
        virtual void describe_type() override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
    public:
        virtual void playing() override;
    public:
        saw();
    };
}

#endif /* defined(__RollerBallCore__saw__) */
