//
//  ui_component.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 2014-05-26.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__ui_component__
#define __RollerBallCore__ui_component__

#include "components_base.h"
#include "nvnode.h"

namespace rb {
    class sprite_component;
    class ui_number;
    class ui_component : public nvnode {
    private:
        void update_sizes();
    private:
        vec2 size_of_tex(const rb_string& tex_name) const;
        vec2 aspect_correction_factor(const rb_string& tex_name) const;
    private:
        void intro_layout();
        void play_layout();
        void score_layout();
    private:
        std::unordered_map<char16_t, float> _sizes;
        
        bool _initialized;
        //intro screen
        sprite_component* _play_btn;
        sprite_component* _tutorial_btn;
        sprite_component* _set1_btn;
        sprite_component* _set2_btn;
        sprite_component* _leaderboards_btn;
        sprite_component* _hearts_btn;
        sprite_component* _addhearts_btn;
        ui_number* _num_hearts;
        //score screen
        sprite_component* _coins_btn;
        ui_number* _coins_num;
        sprite_component* _time_btn;
        ui_number* _time_num;
        sprite_component* _death_btn;
        ui_number* _death_num;
        sprite_component* _total_btn;
        ui_number* _total_num;
        sprite_component* _finish_btn;
        //play screen
        sprite_component* _pause_btn;
        sprite_component* _phearts_btn;
        ui_number* _phearts_num;
        sprite_component* _ptime_btn;
        ui_number* _ptime_num;
        sprite_component* _pcoins_btn;
        ui_number* _pcoins_num;
        sprite_component* _unpause_btn;
        sprite_component* _back_btn;
        
        vec2 get_aspect_correction() const;
        bool _intro_play;
        bool _intro_select;
    private:
        void intro_play_clicked();
        void intro_tutorial_clicked();
        void intro_set1_clicked();
        void intro_set2_clicked();
    protected:
        virtual bool should_serialize_children() const override;
        //Update
        virtual void update(float dt);
        //Typed Object
        virtual void describe_type() override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
        virtual void after_becoming_active(bool node_was_moved) override;
    public:
        ui_component();
    public:
        virtual void playing() override;
        //touches
    protected:
        virtual void touches_began(const std::vector<touch>& touches, bool& swallow) override;
        virtual void touches_moved(const std::vector<touch>& touches, bool& swallow) override;
        virtual void touches_ended(const std::vector<touch>& touches, bool& swallow) override;
        virtual void touches_cancelled(const std::vector<touch>& touches, bool& swallow) override;
    };
}

#endif /* defined(__RollerBallCore__ui_component__) */
