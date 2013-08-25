//
//  physics_shape.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 16/08/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__physics_shape__
#define __RollerBallCore__physics_shape__

#include "components_base.h"
#include "polygon_component.h"

class b2World;
class b2Body;

namespace rb {
    class main_character;
    class physics_shape : public polygon_component {
    public:
        enum type {
            kStaticGravityZone = 0,
            kStaticPlanet = 1
        };
    private:
        float _gravity;
        type _type;
        b2World* _world;
        b2Body* _body;
        bool _phys_initialized;
        rb_string _planet_name;
        physics_shape* _planet;
        polygon _cached_pol;
        uint32_t _priority;
        bool _active_gravity;
        bool _invert_velocity;
        nullable<rb_string> _gravity_ref;
        node* _gravity_ref2;
    public:
        friend class main_character;
        physics_shape();
        ~physics_shape();
    protected:
        //Typed Object
        virtual void describe_type() override;
        virtual void after_becoming_active(bool node_was_moved) override;
        virtual void before_becoming_inactive(bool node_was_moved) override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
    protected:
        virtual void playing() override;
    public:
        type shape_type() const;
        type shape_type(const type value);
        float gravity() const;
        float gravity(const float value);
        vec2 gravity_vector(const vec2& position, vec2& cam_gravity);
        const rb_string& planet_name() const;
        const rb_string& planet_name(const rb_string& value);
        physics_shape* planet() const;
        uint32_t priority() const;
        uint32_t priority(const uint32_t value);
        bool invert_velocity() const;
        bool invert_velocity(const bool value);
        const nullable<rb_string>& gravity_reference() const;
        const nullable<rb_string>& gravity_reference(const nullable<rb_string>& value);
    };
}


#endif /* defined(__RollerBallCore__physics_shape__) */
