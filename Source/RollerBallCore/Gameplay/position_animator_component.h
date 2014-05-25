//
//  position_animator_component.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 03/03/14.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__position_animator_component__
#define __RollerBallCore__position_animator_component__

#include "components_base.h"
#include "nvnode.h"
#include "resettable_component.h"
#include "polygon_path.h"
#include "action_target.h"

namespace rb {
    class position_animator_component : public nvnode, public resettable_component, public action_target {
    private:
        std::vector<node*> _nodes;
        std::vector<float> _lengths;
        std::vector<float> _dirs;
        std::unordered_map<node*, polygon_path> _paths;
        std::unordered_map<node*, node*> _attached_path;
        std::vector<matrix3x3> _saved_transforms;
        std::vector<float> _saved_lengths;
        std::vector<float> _saved_dirs;
        rb_string _class;
        rb_string _path_class;
        
        bool _clamp;
        bool _invert;
        float _velocity;
        float _asleep_duration;
        float _awake_duration;
        float _start_asleep_duration;
        float _endpoint_asleep_duration;
        //bookkepping
        bool _paused;
        std::vector<float> _current_asleep;
        std::vector<float> _current_awake;
        std::vector<float> _current_asleep_endpoint;
        float _current_asleep_start;
        //saved
        bool _saved_paused;
    protected:
        //Reset component
        virtual void reset_component();
        //Update
        virtual void update(float dt);
        //Typed Object
        virtual void describe_type() override;
        virtual void after_becoming_active(bool node_was_moved) override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
    public:
        position_animator_component();
        void reset_animation(bool initial_delay);
        bool paused_animation() const;
        bool paused_animation(bool value);
        const rb_string& animated_class() const;
        const rb_string& animated_class(const rb_string& value);
        const rb_string& path_class() const;
        const rb_string& path_class(const rb_string& value);
        bool clamp() const;
        bool clamp(bool value);
        bool invert() const;
        bool invert(bool value);
        float velocity() const;
        float velocity(float value);
        float asleep_duration() const;
        float asleep_duration(float value);
        float awake_duration() const;
        float awake_duration(float value);
        float start_asleep_duration() const;
        float start_asleep_duration(float value);
        float endpoint_asleep_duration() const;
        float endpoint_asleep_duration(float value);
        void adjust_objects_to_path();
    public:
        virtual void playing() override;
    public:
        virtual void do_action(const rb_string& action_name, const rb_string& arg) override;
    };
}

#endif /* defined(__RollerBallCore__position_animator_component__) */
