//
//  animation_manager_component.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 01/01/14.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__animation_manager_component__
#define __RollerBallCore__animation_manager_component__

#include "components_base.h"
#include "nvnode.h"

#define INFINITE_LOOP -1
#define ANIM_ENGINE_UPDATE_PRIORITY -40000

namespace rb {
    class animation_function;
    struct animation_info {
        float initial_delay = 0;
        float delay = 0;
        float duration = 0;
        animation_state state;
        int loop_count = 0;
        bool mirror = false;
        bool auto_destroy = false;
        animation_function* time_function = nullptr;
        std::function<void (float, animation_info*)> update_function;
        std::function<void (animation_info*)> destroy_function;
        void* user_data = nullptr;
        animation_info();
    };
    typedef void* animation_id;
    class animation_manager_component : public nvnode {
    private:
        struct ex_animation_info {
            animation_info ai;
            float initial_delay_remaining;
            float delay_remaining;
            float duration_remaining;
            bool mirroring;
            int current_loop_count;
            ex_animation_info();
            ~ex_animation_info();
        };
        std::list<ex_animation_info*> _animations;
        std::list<ex_animation_info*>::iterator _current_iterator;
        bool _iterator_changed;
        void run_animation(ex_animation_info* ai, float dt);
    protected:
        //Typed Object
        virtual void describe_type() override;
        virtual void after_becoming_active(bool node_was_moved) override;
        virtual void before_becoming_inactive(bool node_was_moved) override;
        //Update
        virtual void update(float dt) override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
    public:
        animation_manager_component();
        virtual ~animation_manager_component();
        animation_id add_animation(animation_info* ai);
        animation_info* animation(animation_id aid);
        void reset_animation(animation_id aid);
        void destroy_animation(animation_id aid);
    };
}

#endif /* defined(__RollerBallCore__animation_manager_component__) */
