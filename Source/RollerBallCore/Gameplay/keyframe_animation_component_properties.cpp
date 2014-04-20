//
//  keyframe_animation_component_properties.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 2014-04-14.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#include "keyframe_animation_component.h"
#include "polygon_component.h"
#include "smooth_curve.h"
#include "polygon_path.h"
#include "scene.h"
#include "buffer.h"

using namespace rb;

void keyframe_animation_component::describe_type(){
    std::map<rb_string, easing_function> _easings = {
        {u"Linear", easing_function::linear},
        {u"In", easing_function::ease_in},
        {u"Out", easing_function::ease_out},
        {u"In/Out", easing_function::ease_in_out},
        {u"Elastic In", easing_function::ease_elastic_in},
        {u"Elastic Out", easing_function::ease_elastic_out},
        {u"Elastic In/Out", easing_function::ease_elastic_in_out},
        {u"Bounce In", easing_function::ease_bounce_in},
        {u"Bounce Out", easing_function::ease_bounce_out},
        {u"Bounce In/Out", easing_function::ease_bounce_in_out},
        {u"Back In", easing_function::ease_back_in},
        {u"Back Out", easing_function::ease_back_out},
        {u"Back In/Out", easing_function::ease_back_in_out}
    };
    nvnode::describe_type();
    
    start_type<keyframe_animation_component>([](){return new keyframe_animation_component();});
    boolean_property<keyframe_animation_component>(u"is_playing_animation", u"Playing", true, {
        [](const keyframe_animation_component* site){
            return site->_playing_anim;
        },
        [](keyframe_animation_component* site, bool value){
            site->_playing_anim = value;
        }
    });
    boolean_property<keyframe_animation_component>(u"loop", u"Loop", true, {
        [](const keyframe_animation_component* site){
            return site->_loop;
        },
        [](keyframe_animation_component* site, bool value){
            site->_loop = value;
        }
    });
    boolean_property<keyframe_animation_component>(u"mirror", u"Mirror", true, {
        [](const keyframe_animation_component* site){
            return site->_mirror;
        },
        [](keyframe_animation_component* site, bool value){
            site->_mirror = value;
        }
    });
    action<keyframe_animation_component>(u"dirty", u"", action_flags::multi_dispatch, {u"Make Dirty"}, [](keyframe_animation_component* site, const rb_string& actionName){
        site->_dirty_anim = true;
    });
    action<keyframe_animation_component>(u"see", u"See", action_flags::multi_dispatch, {u"Select", u"Preview"}, [](keyframe_animation_component* site, const rb_string& actionName){
        if(actionName == u"Select")
            site->reselect_animated();
        else
            site->preview_current_keyframe();
    });
    action<keyframe_animation_component>(u"goto", u"GoTo", action_flags::multi_dispatch, {u"First", u"Previous", u"Next", u"Last"}, [](keyframe_animation_component* site, const rb_string& actionName){
        if(actionName == u"First")
            site->goto_first();
        else if(actionName == u"Previous")
            site->goto_previous();
        else if(actionName == u"Next")
            site->goto_next();
        else
            site->goto_last();
    });
    action<keyframe_animation_component>(u"goto_placeholder", u"", action_flags::multi_dispatch, {u"Placeholder"}, [](keyframe_animation_component* site, const rb_string& actionName){
        site->goto_placeholder();
    });
    integer_property<keyframe_animation_component>(u"current_index", u"Index", false, {
        [](const keyframe_animation_component* site){
            return site->current_index();
        },
        [](keyframe_animation_component* site, long value){
            
        }
    });
    single_property<keyframe_animation_component>(u"current_delay", u"Delay", true, {
        [](const keyframe_animation_component* site){
            return site->current_delay();
        },
        [](keyframe_animation_component* site, float value){
            site->current_delay(value);
        }
    });
    action<keyframe_animation_component>(u"set_delay", u"", action_flags::multi_dispatch, {u"Set Delay"}, [](keyframe_animation_component* site, const rb_string& actionName){
        site->set_delay();
    });
    enumeration_property<keyframe_animation_component, easing_function>(u"current_position_x_easing", u"X Easing", _easings, true, {
        [](const keyframe_animation_component* site){
            return site->current_position_x_easing();
        },
        [](keyframe_animation_component* site, easing_function value){
            site->current_position_x_easing(value);
        }
    });
    single_property<keyframe_animation_component>(u"current_position_x_factor", u"X Factor", true, {
        [](const keyframe_animation_component* site){
            return site->current_position_x_factor();
        },
        [](keyframe_animation_component* site, float value){
            site->current_position_x_factor(value);
        }
    });
    enumeration_property<keyframe_animation_component, easing_function>(u"current_position_y_easing", u"Y Easing", _easings, true, {
        [](const keyframe_animation_component* site){
            return site->current_position_y_easing();
        },
        [](keyframe_animation_component* site, easing_function value){
            site->current_position_y_easing(value);
        }
    });
    single_property<keyframe_animation_component>(u"current_position_y_factor", u"Y Factor", true, {
        [](const keyframe_animation_component* site){
            return site->current_position_y_factor();
        },
        [](keyframe_animation_component* site, float value){
            site->current_position_y_factor(value);
        }
    });
    enumeration_property<keyframe_animation_component, easing_function>(u"current_rotation_easing", u"Rot Easing", _easings, true, {
        [](const keyframe_animation_component* site){
            return site->current_rotation_easing();
        },
        [](keyframe_animation_component* site, easing_function value){
            site->current_rotation_easing(value);
        }
    });
    single_property<keyframe_animation_component>(u"current_rotation_factor", u"Rot Factor", true, {
        [](const keyframe_animation_component* site){
            return site->current_rotation_factor();
        },
        [](keyframe_animation_component* site, float value){
            site->current_rotation_factor(value);
        }
    });
    action<keyframe_animation_component>(u"set_keyframe", u"Set Keyframe", action_flags::multi_dispatch, {u"Record", u"Record Add"}, [](keyframe_animation_component* site, const rb_string& action){
        if(action == u"Update")
            site->update_transforms();
        else if(action == u"Record")
            site->record_keyframe();
        else if(action == u"Delete")
            site->delete_current();
        else
            site->record_add_to_keyframe();
    });
    action<keyframe_animation_component>(u"set_keyframe_2", u"", action_flags::multi_dispatch, {u"Update", u"Delete"}, [](keyframe_animation_component* site, const rb_string& action){
        if(action == u"Update")
            site->update_transforms();
        else if(action == u"Record")
            site->record_keyframe();
        else if(action == u"Delete")
            site->delete_current();
        else
            site->record_add_to_keyframe();
    });
    action<keyframe_animation_component>(u"start", u"Start", action_flags::multi_dispatch, {u"Record", u"Reset"}, [](keyframe_animation_component* site, const rb_string& actionName){
        if(actionName == u"Record")
            site->record_start();
        else
            site->reset_transforms();
    });
    string_property<keyframe_animation_component>(u"attachment_id", u"Pol ID", true, false, {
        [](const keyframe_animation_component* site){
            return site->_attachment_id;
        },
        [](keyframe_animation_component* site, const rb_string& value){
            site->_attachment_id = value;
        }
    });
    string_property<keyframe_animation_component>(u"attachment_class", u"Att Class", true, false, {
        [](const keyframe_animation_component* site){
            return site->_attachment_class;
        },
        [](keyframe_animation_component* site, const rb_string& value){
            site->_attachment_class = value;
        }
    });
    action<keyframe_animation_component>(u"attach", u"Attach", action_flags::multi_dispatch, {u"Setup", u"Remove For", u"Remove All"}, [](keyframe_animation_component* site, const rb_string& actionName){
        if(actionName == u"Setup")
            site->setup_attachment(site->_attachment_class, site->_attachment_id);
        else if(actionName == u"Remove For")
            site->remove_attachment_for(site->_attachment_id);
        else
            site->remove_attachment_for_all();
    });
    boolean_property<keyframe_animation_component>(u"sync_attached", u"Sync Att", true, {
        [](const keyframe_animation_component* site){
            return site->_sync_attachable;
        },
        [](keyframe_animation_component* site, bool value){
            site->_sync_attachable = value;
        }
    });
    action<keyframe_animation_component>(u"play", u"Play", action_flags::multi_dispatch, {u"Play", u"Resume", u"Pause"}, [](keyframe_animation_component* site, const rb_string& actionName){
        if(actionName == u"Play")
            site->editor_play_animation();
        else if(actionName == u"Resume")
            site->editor_resume_animation();
        else
            site->editor_pause_animation();
    });
    begin_private_properties();
    integer_property<keyframe_animation_component>(u"n_frames", u"N Frames", true, {
        [](const keyframe_animation_component* site){
            return site->_n_frames;
        },
        [](keyframe_animation_component* site, long value){
            site->_n_frames = (uint32_t)value;
        }
    });
    boolean_property<keyframe_animation_component>(u"dirty_anim", u"Dirty", true, {
        [](const keyframe_animation_component* site){
            return site->_dirty_anim;
        },
        [](keyframe_animation_component* site, bool value){
            site->_dirty_anim = value;
        }
    });
    buffer_property<keyframe_animation_component>(u"internal_state", u"Internal State", {
        [](const keyframe_animation_component* site){
            return site->save_state();
        },
        [](keyframe_animation_component* site, buffer value){
            site->_pending_buffer = value;
        }
    });
    end_private_properties();
    end_type();
}

































