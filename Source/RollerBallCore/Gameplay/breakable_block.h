//
//  breakable_block.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 2014-04-06.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__breakable_block__
#define __RollerBallCore__breakable_block__

#include "components_base.h"
#include "node.h"
#include "resettable_component.h"
#include "vec2.h"
#include "action_target.h"

class b2World;
class b2Body;

namespace rb {
    class destructible_sprite_component;
    class animation_manager_component;
    typedef void* animation_id;
    class breakable_block : public node, public resettable_component, public action_target {
    protected:
        virtual void reset_component() override;
    private:
        //character
        bool _explode_character_contact;
        bool _can_kill_character;
        node* _main_character;
        //animation manager
        animation_manager_component* _an_manager;
        std::vector<vec2> _velocities;
        animation_id _break_an;
        animation_id _restore_an;
        float _last_t;
        //sprite
        destructible_sprite_component* _sprite;
        bool _broken;
        bool _saved_broken;
        bool _should_be_active;
        b2World* _world;
        b2Body* _body;
        //restoration
        float _restoration_left;
        float _restoration_time;
        bool _restoration_enabled;
        //on break
        rb_string _on_break_action_buffer;
        rb_string _on_break_action_name;
        rb_string _on_restore_action_buffer;
        rb_string _on_restore_action_name;
        bool _no_reentrancy;
    protected:
        virtual bool should_serialize_children() const override;
    protected:
        //Update
        virtual void after_becoming_active(bool node_was_moved) override;
        virtual void update(float dt) override;
    protected:
        //Typed Object
        virtual void describe_type() override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
    public:
        virtual void playing() override;
    private:
        bool is_touching_main_character();
        bool is_touching_saw();
        void generate_random_velocities();
        void animate_break(float t);
        void animate_restore(float t);
    public:
        breakable_block();
        void break_block(bool animation); //breaks the block
        void restore_block(bool animation); //restore the block
        float restoration_time() const;
        float restoration_time(float value);
        bool restoration_enabled() const;
        bool restoration_enabled(bool value);
        const rb_string& image_name() const;
        const rb_string& image_name(const rb_string& value);
        bool broken() const;
        bool broken(bool value);
        bool can_kill_character() const;
        bool can_kill_character(bool value);
        bool explode_character_contact() const;
        bool explode_character_contact(bool value);
    public:
        virtual void do_action(const rb_string& action_name, const rb_string& arg) override;
    };
}

#endif /* defined(__RollerBallCore__breakable_block__) */
