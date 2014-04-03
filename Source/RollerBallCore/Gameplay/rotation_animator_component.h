//
//  saw_rotation_animator.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 05/01/14.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore_rotation_animator__
#define __RollerBallCore_rotation_animator__

#include "components_base.h"
#include "nvnode.h"
#include "resettable_component.h"

namespace rb {
    class rotation_animator_component : public nvnode, public resettable_component {
    protected:
        virtual void reset_component();
    private:
        std::vector<node*> _nodes;
        std::vector<float> _directions;
        std::vector<transform_space> _saved_transforms;
        std::vector<nullable<float>> _current_min_angles;
        std::vector<nullable<float>> _current_max_angles;
        rb_string _class;
        float _r_velocity;
        float _asleep_duration;
        float _awake_duration;
        float _asleep_duration_start;
        float _asleep_duration_endpoint;
        nullable<float> _min_angle;
        nullable<float> _max_angle;
        bool _change_dir_endpoint;
        //bookkepping
        bool _paused;
        std::vector<float> _current_asleep_endpoint;
        float _current_asleep_start;
        std::vector<float> _current_asleep;
        std::vector<float> _current_awake;
        //saved
        bool _saved_paused;
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
        rotation_animator_component();
        void reset_animation(bool initial_delay);
        bool paused_animation() const;
        bool paused_animation(bool value);
        const rb_string& animated_class() const;
        const rb_string& animated_class(const rb_string& value);
        float asleep_duration() const;
        float asleep_duration(float value);
        float awake_duration() const;
        float awake_duration(float value);
        bool change_dir_endpoint() const;
        bool change_dir_endpoint(bool value);
        float start_asleep_duration() const;
        float start_asleep_duration(float value);
        float endpoint_asleep_duration() const;
        float endpoint_asleep_duration(float value);
        const nullable<float>& min_angle() const;
        const nullable<float>& min_angle(const nullable<float>& value);
        const nullable<float>& max_angle() const;
        const nullable<float>& max_angle(const nullable<float>& value);
    public:
        virtual void playing() override;
    };
}

#endif /* defined(__RollerBallCore__saw_rotation_animator__) */
