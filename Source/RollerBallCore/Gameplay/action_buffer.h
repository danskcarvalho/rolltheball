//
//  action_buffer.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 2014-04-20.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__action_buffer__
#define __RollerBallCore__action_buffer__

#include "nvnode.h"

namespace rb {
    struct action_dispatch_info {
        bool by_class;
        rb_string target;
        rb_string action_name;
        rb_string action_arg;
    };
    struct action_info {
        rb_string name;
        std::list<action_dispatch_info> actions;
        nullable<std::list<action_dispatch_info>::iterator> current_action;
    };
    class action_buffer : public nvnode {
    private:
        std::list<action_info> _actions;
        nullable<std::list<action_info>::iterator> _current_action;
        rb_string _new_action_name;
        //properties change
        void notify_properties_changed();
    public:
        //constructor
        action_buffer();
        //methods
        //move between action
        void goto_first_action();
        void goto_previous_action();
        void goto_next_action();
        void goto_last_action();
        //move between subactions
        void goto_first_subaction();
        void goto_previous_subaction();
        void goto_next_subaction();
        void goto_last_subaction();
        //change currents
        rb_string current_action_name() const;
        void current_action_name(const rb_string& value);
        rb_string current_target() const;
        void current_target(const rb_string& value);
        bool current_by_class() const;
        void current_by_class(bool value);
        rb_string current_action_to_call() const;
        void current_action_to_call(const rb_string& value);
        rb_string current_action_arg() const;
        void current_action_arg(const rb_string& value);
        //creation and deletion
        void create_new_action(const rb_string& name);
        void delete_current_action();
        void insert_new_subaction();
        void delete_current_subaction();
        //perform action
        void perform_action(const rb_string& name);
        //serialization/deserialization
    private:
        buffer save_state() const;
        void restore_state(buffer buff);
    protected:
        //Typed Object
        virtual void describe_type() override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
    };
}

#endif /* defined(__RollerBallCore__action_buffer__) */
