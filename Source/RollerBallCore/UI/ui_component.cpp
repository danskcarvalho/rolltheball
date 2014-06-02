//
//  ui_component.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 2014-05-26.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#include "ui_component.h"
#include "scene.h"
#include "layer.h"
#include "sprite_component.h"
#include "ui_controller.h"
#include "ui_number.h"
#include "texture_atlas.h"
#include "director.h"
#include "sound_player.h"

using namespace rb;

#define UI_BACK u"uiBack"
#define UI_COIN u"uiCoin"
#define UI_COINSIGN u"uiCoinSign"
#define UI_DEATH u"uiDeath"
#define UI_DEATHSIGN u"uiDeathSign"
#define UI_8 u"ui8"
#define UI_5 u"ui5"
#define UI_4 u"ui4"
#define UI_PLAY u"uiPlay"
#define UI_HEART u"uiHeart"
#define UI_LEADERBOARDS u"uiLeaderboards"
#define UI_MINUS u"uiMinus"
#define UI_9 u"ui9"
#define UI_1 u"ui1"
#define UI_PAUSE u"uiPause"
#define UI_PLUS u"uiPlus"
#define UI_COLON u"uiColon"
#define UI_SET1 u"uiSet1"
#define UI_SET2 u"uiSet2"
#define UI_7 u"ui7"
#define UI_6 u"ui6"
#define UI_3 u"ui3"
#define UI_TIME u"uiTime"
#define UI_TIMESIGN u"uiTimeSign"
#define UI_TOTALSIGN u"uiTotalSign"
#define UI_TUTORIAL u"uiTutorial"
#define UI_2 u"ui2"
#define UI_0 u"ui0"

#define UI_INTRO_SPACING 0.1f
#define UI_INTRO_BTN_SIZE 0.5f
#define UI_INTRO_HEARTS_Y -0.75f
#define UI_INTRO_HEARTS_SIZE 0.25f
#define UI_PLAY_SCORE_SIZE 0.25f
#define UI_PLAY_BTN_SIZE 0.125f

ui_component::ui_component(){
    _initialized = false;
    _intro_play = true;
    _intro_select = false;
    classes(u"ui");
    name(u"ui");
    _play_btn = nullptr;
    _tutorial_btn = nullptr;
    _set1_btn = nullptr;
    _set2_btn = nullptr;
    _leaderboards_btn = nullptr;
    _hearts_btn = nullptr;
    _addhearts_btn = nullptr;
    _num_hearts = nullptr;
    _coins_btn = nullptr;
    _coins_num = nullptr;
    _time_btn = nullptr;
    _time_num = nullptr;
    _death_btn = nullptr;
    _death_num = nullptr;
    _total_btn = nullptr;
    _total_num = nullptr;
    _finish_btn = nullptr;
    _pause_btn = nullptr;
    _phearts_btn = nullptr;
    _phearts_num = nullptr;
    _ptime_btn = nullptr;
    _ptime_num = nullptr;
    _pcoins_btn = nullptr;
    _pcoins_num = nullptr;
    _unpause_btn = nullptr;
    _back_btn = nullptr;
    _scores_posted = false;
}

void ui_component::describe_type(){
    nvnode::describe_type();
    start_type<ui_component>([]() { return new ui_component(); });
    
    end_type();
}

rb_string ui_component::type_name() const{
    return u"rb::ui_component";
}

rb_string ui_component::displayable_type_name() const{
    return u"UI";
}

void ui_component::after_becoming_active(bool node_was_moved){
    register_for(registrable_event::update, 0);
    register_for(registrable_event::touch_events, -1);
}

vec2 ui_component::get_aspect_correction() const {
    const vec2& _viewport_size = parent_scene()->viewport_size();
    float _aspect = _viewport_size.x() / _viewport_size.y();
    if(!parent_scene()->camera_aspect_correction())
        _aspect = 1;
    return vec2(_aspect, 1);
}

void ui_component::update_statistics(){
    if(_pcoins_num){ //max 999
        auto _num = ui_controller::coins();
        if(_num > 999)
            _num = 999;
        _pcoins_num->number(_num);
    }
    if(_phearts_num){ //max 999
        auto _num = ui_controller::hearts();
        if(_num > 999)
            _num = 999;
        _phearts_num->number(_num);
    }
    if(_ptime_num){ //max 00:00
        auto _num = ui_controller::time();
        _ptime_num->number(_num);
    }
    if(_coins_num){ //max 999999
        auto _num = ui_controller::coins() * 10.0f;
        if(_num > 999999)
            _num = 999999;
        _coins_num->number(_num);
    }
    if(_time_num){ //max 999999
        auto _num = ui_controller::time() * (-2.0f);
        if(_num > 999999)
            _num = 999999;
        if(_num < -99999)
            _num = -99999;
        _time_num->number(_num);
    }
    if(_death_num){ //max 999999
        auto _num = ui_controller::deaths() * (-5.0f);
        if(_num > 999999)
            _num = 999999;
        if(_num < -99999)
            _num = -99999;
        _death_num->number(_num);
    }
    if(_total_num){ //max 999999
        auto _coin_num = ui_controller::coins() * 10.0f;
        auto _time_num = ui_controller::time() * (-2.0f);
        auto _deaths_num = ui_controller::deaths() * (-5.0f);
        auto _total = _coin_num + _time_num + _deaths_num;
        if(_total > 999999)
            _total = 999999;
        if(_total < -99999)
            _total = -99999;
        _total_num->number(_total);
    }
    if(_num_hearts){
        auto _num = ui_controller::hearts();
        if(_num > 999)
            _num = 999;
        _num_hearts->number(_num);
    }
}

void ui_component::update(float dt){
    update_statistics();
}

void ui_component::intro_layout(){
    _play_btn = new sprite_component();
    _play_btn->image_name(UI_PLAY);
    _play_btn->old_transform(transform_space(vec2::zero, vec2(1, 1), 0));
    parent_scene()->layer(9)->add_node(_play_btn);
    
    auto _total_size = UI_INTRO_BTN_SIZE * 3 + UI_INTRO_SPACING * 3;
    auto _initial_pos = -(_total_size / 2.0f);
    auto _inc = _total_size / 3.0f;
    
    _tutorial_btn = new sprite_component();
    _tutorial_btn->image_name(UI_TUTORIAL);
    _tutorial_btn->old_transform(transform_space(vec2(_initial_pos, 0), vec2(UI_INTRO_BTN_SIZE, UI_INTRO_BTN_SIZE), 0));
    _tutorial_btn->opacity(0);
    parent_scene()->layer(9)->add_node(_tutorial_btn);
    
    _initial_pos += _inc;
    _set1_btn = new sprite_component();
    _set1_btn->image_name(UI_SET1);
    _set1_btn->old_transform(transform_space(vec2(_initial_pos, 0), vec2(UI_INTRO_BTN_SIZE, UI_INTRO_BTN_SIZE), 0));
    _set1_btn->opacity(0);
    parent_scene()->layer(9)->add_node(_set1_btn);
    
    _initial_pos += _inc;
    _set2_btn = new sprite_component();
    _set2_btn->image_name(UI_SET2);
    _set2_btn->old_transform(transform_space(vec2(_initial_pos, 0), vec2(UI_INTRO_BTN_SIZE, UI_INTRO_BTN_SIZE), 0));
    _set2_btn->opacity(0);
    parent_scene()->layer(9)->add_node(_set2_btn);
    
    _initial_pos += _inc;
    _leaderboards_btn = new sprite_component();
    _leaderboards_btn->image_name(UI_LEADERBOARDS);
    _leaderboards_btn->old_transform(transform_space(vec2(_initial_pos, 0), vec2(UI_INTRO_BTN_SIZE, UI_INTRO_BTN_SIZE), 0));
    _leaderboards_btn->opacity(0);
    parent_scene()->layer(9)->add_node(_leaderboards_btn);
    
    _hearts_btn = new sprite_component();
    _hearts_btn->image_name(UI_HEART);
    _hearts_btn->old_transform(transform_space(vec2(-0.4f, UI_INTRO_HEARTS_Y), vec2(UI_INTRO_HEARTS_SIZE, UI_INTRO_HEARTS_SIZE), 0));
    _hearts_btn->opacity(0);
    parent_scene()->layer(9)->add_node(_hearts_btn);
    
    _num_hearts = new ui_number();
    _num_hearts->old_transform(transform_space(vec2(0, UI_INTRO_HEARTS_Y), vec2(UI_INTRO_HEARTS_SIZE, UI_INTRO_HEARTS_SIZE), 0));
    _num_hearts->visible(false);
    _num_hearts->alignment(text_alignment::center);
    _num_hearts->number(999);
    _num_hearts->_digits = 3;
    parent_scene()->layer(9)->add_node(_num_hearts);
    
    _addhearts_btn = new sprite_component();
    _addhearts_btn->image_name(UI_PLUS);
    _addhearts_btn->old_transform(transform_space(vec2(0.4f, UI_INTRO_HEARTS_Y), vec2(UI_INTRO_HEARTS_SIZE, UI_INTRO_HEARTS_SIZE), 0));
    _addhearts_btn->opacity(0);
    parent_scene()->layer(9)->add_node(_addhearts_btn);
}

void ui_component::update_sizes(){
    if(_sizes.size() == 0 || _sizes[u'0'] == 0.0f){
        _sizes.clear();
        _sizes[u'0'] = aspect_correction_factor(UI_0).x();
        _sizes[u'1'] = aspect_correction_factor(UI_1).x();
        _sizes[u'2'] = aspect_correction_factor(UI_2).x();
        _sizes[u'3'] = aspect_correction_factor(UI_3).x();
        _sizes[u'4'] = aspect_correction_factor(UI_4).x();
        _sizes[u'5'] = aspect_correction_factor(UI_5).x();
        _sizes[u'6'] = aspect_correction_factor(UI_6).x();
        _sizes[u'7'] = aspect_correction_factor(UI_7).x();
        _sizes[u'8'] = aspect_correction_factor(UI_8).x();
        _sizes[u'9'] = aspect_correction_factor(UI_9).x();
        _sizes[u'-'] = 0.5f;
        _sizes[u':'] = aspect_correction_factor(UI_COLON).x();
    }
}

vec2 ui_component::aspect_correction_factor(const rb_string& tex_name) const {
    vec2 _tex_s = size_of_tex(tex_name);
    if(_tex_s.x() == 0.0f || _tex_s.y() == 0.0f)
        return vec2::zero;
    if(almost_equal(_tex_s.x(), 0))
        _tex_s.x(1);
    if(almost_equal(_tex_s.y(), 0))
        _tex_s.y(1);
    auto _tx = _tex_s.x() / _tex_s.y();
    
    return  vec2(_tx, 1);
}

vec2 ui_component::size_of_tex(const rb_string& tex_name) const {
    if(parent_scene() && parent_scene()->atlas()){
        if(parent_scene()->atlas()->contains_texture(tex_name))
        {
            std::vector<rb_string> _groups;
            parent_scene()->atlas()->get_groups(tex_name, _groups);
            return parent_scene()->atlas()->get_bounds_in_pixels(_groups[0], tex_name).size();
            
        }
        else
            return vec2::zero;
    }
    else
        return vec2::zero;
}

float calc_size_text(const rb_string& str, float spacing, const std::unordered_map<char16_t, float> sizes){
    float size_chars = 0;
    for(size_t i = 0; i < str.size(); i++){
        size_chars += sizes.at(str[i]);
    }
    float total_size = ((float)str.size() - 1.0f) * spacing + size_chars;
    return total_size;
}

void ui_component::score_layout(){
    update_sizes();
    vec2 _pos_coin_btn, _pos_coin_num, _pos_time_btn, _pos_time_num, _pos_death_btn, _pos_death_num, _pos_total_btn, _pos_total_num;
    float _total_size = 0;
    auto _text_size = calc_size_text(u"100000", 0, _sizes) * UI_PLAY_SCORE_SIZE + 0.1f;
#define UI_PLAY_SCORE_SIZE_1 (UI_PLAY_SCORE_SIZE + 0.1f)
    
    _pos_coin_btn = vec2::right * UI_PLAY_SCORE_SIZE_1 / 2.0f;
    _total_size += UI_PLAY_SCORE_SIZE_1;
    
    _pos_coin_num = _pos_coin_btn + UI_PLAY_SCORE_SIZE_1 * vec2::right / 2.0f;
    _total_size += _text_size;
    ////
    _pos_time_btn = _pos_coin_num + (_text_size + 0.125f + 0.1f) * vec2::right;
    _total_size += UI_PLAY_SCORE_SIZE_1;
    
    _pos_time_num = _pos_time_btn + UI_PLAY_SCORE_SIZE_1 * vec2::right / 2.0f;
    _total_size += _text_size;
    _total_size += 0.1f;
    //recenter
    vec2 _recenter = vec2::left * (_total_size / 2.0f);
    _pos_coin_btn += _recenter;
    _pos_coin_num += _recenter;
    _pos_time_btn += _recenter;
    _pos_time_num += _recenter;
    //second row
    _pos_death_btn = _pos_coin_btn + vec2::down * UI_PLAY_SCORE_SIZE_1;
    _pos_death_num = _pos_coin_num + vec2::down * UI_PLAY_SCORE_SIZE_1;
    _pos_total_btn = _pos_time_btn + vec2::down * UI_PLAY_SCORE_SIZE_1;
    _pos_total_num = _pos_time_num + vec2::down * UI_PLAY_SCORE_SIZE_1;
    _pos_coin_btn += vec2::up * 0.5f;
    _pos_coin_num += vec2::up * 0.5f;
    _pos_time_btn += vec2::up * 0.5f;
    _pos_time_num += vec2::up * 0.5f;
    _pos_death_btn += vec2::up * 0.5f;
    _pos_death_num += vec2::up * 0.5f;
    _pos_total_btn += vec2::up * 0.5f;
    _pos_total_num += vec2::up * 0.5f;
    
    //creating first line of score
    //buttons
    _coins_btn = new sprite_component();
    _coins_btn->image_name(UI_COIN);
    _coins_btn->old_transform(transform_space(_pos_coin_btn, vec2(UI_PLAY_SCORE_SIZE, UI_PLAY_SCORE_SIZE), 0));
    _coins_btn->opacity(0);
    parent_scene()->layer(9)->add_node(_coins_btn);
    
    _time_btn = new sprite_component();
    _time_btn->image_name(UI_TIME);
    _time_btn->old_transform(transform_space(_pos_time_btn, vec2(UI_PLAY_SCORE_SIZE, UI_PLAY_SCORE_SIZE), 0));
    _time_btn->opacity(0);
    parent_scene()->layer(9)->add_node(_time_btn);
    
    _death_btn = new sprite_component();
    _death_btn->image_name(UI_DEATH);
    _death_btn->old_transform(transform_space(_pos_death_btn, vec2(UI_PLAY_SCORE_SIZE, UI_PLAY_SCORE_SIZE), 0));
    _death_btn->opacity(0);
    parent_scene()->layer(9)->add_node(_death_btn);
    
    _total_btn = new sprite_component();
    _total_btn->image_name(UI_TUTORIAL);
    _total_btn->old_transform(transform_space(_pos_total_btn, vec2(UI_PLAY_SCORE_SIZE, UI_PLAY_SCORE_SIZE), 0));
    _total_btn->opacity(0);
    parent_scene()->layer(9)->add_node(_total_btn);
    
    //texts
    _coins_num = new ui_number();
    parent_scene()->layer(9)->add_node(_coins_num);
    _coins_num->old_transform(transform_space(_pos_coin_num, vec2(UI_PLAY_SCORE_SIZE, UI_PLAY_SCORE_SIZE), 0));
    _coins_num->visible(false);
    _coins_num->alignment(text_alignment::right);
    _coins_num->number(999999);
    
    _time_num = new ui_number();
    parent_scene()->layer(9)->add_node(_time_num);
    _time_num->old_transform(transform_space(_pos_time_num, vec2(UI_PLAY_SCORE_SIZE, UI_PLAY_SCORE_SIZE), 0));
    _time_num->visible(false);
    _time_num->alignment(text_alignment::right);
    _time_num->number(999999);
    
    _death_num = new ui_number();
    parent_scene()->layer(9)->add_node(_death_num);
    _death_num->old_transform(transform_space(_pos_death_num, vec2(UI_PLAY_SCORE_SIZE, UI_PLAY_SCORE_SIZE), 0));
    _death_num->visible(false);
    _death_num->alignment(text_alignment::right);
    _death_num->number(-99999);
    
    _total_num = new ui_number();
    parent_scene()->layer(9)->add_node(_total_num);
    _total_num->old_transform(transform_space(_pos_total_num, vec2(UI_PLAY_SCORE_SIZE, UI_PLAY_SCORE_SIZE), 0));
    _total_num->visible(false);
    _total_num->alignment(text_alignment::right);
    _total_num->number(999999);
    
    //return button
    _finish_btn = new sprite_component();
    _finish_btn->image_name(UI_BACK);
    _finish_btn->old_transform(transform_space(vec2(0, -0.5), vec2(UI_INTRO_BTN_SIZE, UI_INTRO_BTN_SIZE), 0));
    _finish_btn->opacity(0);
    parent_scene()->layer(9)->add_node(_finish_btn);
}

void ui_component::play_layout(){
    update_sizes();
    vec2 _pos_coin_btn, _pos_coin_num, _pos_time_btn, _pos_time_num, _pos_hearts_btn, _pos_hearts_num;
    auto _text_size1 = calc_size_text(u"999", 0, _sizes) * UI_PLAY_BTN_SIZE + 0.05f;
    auto _text_size2 = calc_size_text(u"99:99", 0.1, _sizes) * UI_PLAY_BTN_SIZE + 0.05f;
#define UI_PLAY_BTN_SIZE_1 (UI_PLAY_BTN_SIZE + 0.1f)
    
    _pos_hearts_btn = vec2::right * UI_PLAY_BTN_SIZE_1 / 2.0f;
    _pos_hearts_num = _pos_hearts_btn + UI_PLAY_BTN_SIZE_1 * vec2::right / 2.0f;
    ////
    _pos_time_btn = _pos_hearts_num + (_text_size1 + (0.125f + 0.1f) / 2.0f) * vec2::right;
    _pos_time_num = _pos_time_btn + UI_PLAY_BTN_SIZE_1 * vec2::right / 2.0f;
    ///
    _pos_coin_btn = _pos_time_num + (_text_size2 + (0.125f + 0.1f) / 2.0f) * vec2::right;
    _pos_coin_num = _pos_coin_btn + UI_PLAY_BTN_SIZE_1 * vec2::right / 2.0f;
    //recenter
    auto _aspx = get_aspect_correction().x() * 0.95f;
    vec2 _to_left = vec2::left * _aspx;
    _to_left += vec2::up * 0.85f;
    _pos_coin_btn += _to_left;
    _pos_coin_num += _to_left;
    _pos_time_btn += _to_left;
    _pos_time_num += _to_left;
    _pos_hearts_btn += _to_left;
    _pos_hearts_num += _to_left;
    
    //creating hud
    //buttons
    _phearts_btn = new sprite_component();
    _phearts_btn->image_name(UI_HEART);
    _phearts_btn->old_transform(transform_space(_pos_hearts_btn, vec2(UI_PLAY_BTN_SIZE, UI_PLAY_BTN_SIZE), 0));
    _phearts_btn->opacity(1);
    parent_scene()->layer(9)->add_node(_phearts_btn);
    
    _ptime_btn = new sprite_component();
    _ptime_btn->image_name(UI_TIME);
    _ptime_btn->old_transform(transform_space(_pos_time_btn, vec2(UI_PLAY_BTN_SIZE, UI_PLAY_BTN_SIZE), 0));
    _ptime_btn->opacity(1);
    parent_scene()->layer(9)->add_node(_ptime_btn);
    
    _pcoins_btn = new sprite_component();
    _pcoins_btn->image_name(UI_COIN);
    _pcoins_btn->old_transform(transform_space(_pos_coin_btn, vec2(UI_PLAY_BTN_SIZE, UI_PLAY_BTN_SIZE), 0));
    _pcoins_btn->opacity(1);
    parent_scene()->layer(9)->add_node(_pcoins_btn);
    
    //texts
    _phearts_num = new ui_number();
    parent_scene()->layer(9)->add_node(_phearts_num);
    _phearts_num->old_transform(transform_space(_pos_hearts_num, vec2(UI_PLAY_BTN_SIZE, UI_PLAY_BTN_SIZE), 0));
    _phearts_num->visible(true);
    _phearts_num->alignment(text_alignment::right);
    _phearts_num->_digits = 3;
    _phearts_num->number(999);
    
    _ptime_num = new ui_number();
    parent_scene()->layer(9)->add_node(_ptime_num);
    _ptime_num->is_date(true);
    _ptime_num->spacing(0.1f);
    _ptime_num->old_transform(transform_space(_pos_time_num, vec2(UI_PLAY_BTN_SIZE, UI_PLAY_BTN_SIZE), 0));
    _ptime_num->visible(true);
    _ptime_num->alignment(text_alignment::right);
    _ptime_num->number(1290);
    
    _pcoins_num = new ui_number();
    parent_scene()->layer(9)->add_node(_pcoins_num);
    _pcoins_num->old_transform(transform_space(_pos_coin_num, vec2(UI_PLAY_BTN_SIZE, UI_PLAY_BTN_SIZE), 0));
    _pcoins_num->visible(true);
    _pcoins_num->alignment(text_alignment::right);
    _pcoins_num->_digits = 4;
    _pcoins_num->number(9999);
    
    //pause button
    _pause_btn = new sprite_component();
    _pause_btn->image_name(UI_PAUSE);
    _pause_btn->old_transform(transform_space(vec2::right * _aspx * 0.92f + vec2::up * 0.82f, vec2(0.25, 0.25), 0));
    _pause_btn->opacity(1);
    parent_scene()->layer(9)->add_node(_pause_btn);
    
    //paused state
    auto _total_size = UI_INTRO_BTN_SIZE + UI_INTRO_SPACING;
    auto _initial_pos = -(_total_size / 2.0f);
    auto _inc = _total_size;
    
    _unpause_btn = new sprite_component();
    _unpause_btn->image_name(UI_PLAY);
    _unpause_btn->old_transform(transform_space(vec2(_initial_pos, 0), vec2(UI_INTRO_BTN_SIZE, UI_INTRO_BTN_SIZE), 0));
    _unpause_btn->opacity(0);
    parent_scene()->layer(9)->add_node(_unpause_btn);
    
    _initial_pos += _inc;
    _back_btn = new sprite_component();
    _back_btn->image_name(UI_BACK);
    _back_btn->old_transform(transform_space(vec2(_initial_pos, 0), vec2(UI_INTRO_BTN_SIZE, UI_INTRO_BTN_SIZE), 0));
    _back_btn->opacity(0);
    parent_scene()->layer(9)->add_node(_back_btn);
}

void ui_component::playing(){
    if(!_initialized && !director::in_editor()){
        if(ui_controller::is_intro()){
            intro_layout();
        }
        else
        {
            play_layout();
            if(!ui_controller::is_tutorial())
                score_layout();
        }
        if(!ui_controller::is_intro() && ui_controller::get_level_number() == 0){ //first level
            ui_controller::coins(0);
            ui_controller::deaths(0);
            ui_controller::time(0);
        }
        update_statistics();
    }
}

bool intersects_circle(const vec2& center, float radius, const vec2& pos){
    auto _d = vec2::distance(center, pos);
    return _d <= radius;
}

void ui_component::touches_began(const std::vector<touch> &touches, bool &swallow){
    if(director::in_editor())
        return;
    for(auto& _t : touches){
        auto _np = parent_scene()->layer(9)->from_layer_space_to(space::normalized_screen).invert().transformed_point(_t.normalized_position());
        
        if(ui_controller::is_intro() && _intro_play){
            auto _touches = intersects_circle(vec2::zero, 0.5f, _np);
            if(_touches){
                sound_player::play_click();
                intro_play_clicked();
                return;
            }
        }
        else if(ui_controller::is_intro() && _intro_select) {
            auto _touches = intersects_circle(_tutorial_btn->old_transform().origin(), UI_INTRO_BTN_SIZE / 2.0f, _np);
            if(_touches){
                sound_player::play_click();
                intro_tutorial_clicked();
                return;
            }
            _touches = intersects_circle(_set1_btn->old_transform().origin(), UI_INTRO_BTN_SIZE / 2.0f, _np);
            if(_touches){
                sound_player::play_click();
                intro_set1_clicked();
                return;
            }
            _touches = intersects_circle(_set2_btn->old_transform().origin(), UI_INTRO_BTN_SIZE / 2.0f, _np);
            if(_touches){
                sound_player::play_click();
                intro_set2_clicked();
                return;
            }
            _touches = intersects_circle(_leaderboards_btn->old_transform().origin(), UI_INTRO_HEARTS_SIZE / 2.0f, _np);
            if(_touches){
                sound_player::play_click();
                intro_leaderboards_clicked();
                return;
            }
            _touches = intersects_circle(_addhearts_btn->old_transform().origin(), UI_INTRO_HEARTS_SIZE / 2.0f, _np);
            if(_touches){
                sound_player::play_click();
                intro_addhearts_clicked();
                return;
            }
        }
        else {
            if(_pause_btn->opacity() != 0.0f){
                auto _touches = intersects_circle(_pause_btn->old_transform().origin(), 0.25f, _np);
                if(_touches){
                    swallow = true;
                    sound_player::play_click();
                    play_pause_clicked();
                    return;
                }
            }
            if(_unpause_btn->opacity() != 0.0f){
                auto _touches = intersects_circle(_unpause_btn->old_transform().origin(), UI_INTRO_BTN_SIZE / 2.0f, _np);
                if(_touches){
                    swallow = true;
                    sound_player::play_click();
                    play_unpause_clicked();
                    return;
                }
            }
            if(_back_btn->opacity() != 0.0f){
                auto _touches = intersects_circle(_back_btn->old_transform().origin(), UI_INTRO_BTN_SIZE / 2.0f, _np);
                if(_touches){
                    swallow = true;
                    sound_player::play_click();
                    play_return_clicked();
                    return;
                }
            }
            if(_finish_btn && _finish_btn->opacity() != 0.0f){
                auto _touches = intersects_circle(_finish_btn->old_transform().origin(), UI_INTRO_BTN_SIZE / 2.0f, _np);
                if(_touches){
                    swallow = true;
                    sound_player::play_click();
                    play_finish_clicked();
                    return;
                }
            }
        }
    }
}

void ui_component::intro_leaderboards_clicked(){
    if(_leaderboards_btn->opacity() < 1)
        return;
    _tutorial_btn->image_name(UI_BACK);
    _leaderboards_btn->opacity(0.5f);
}

void ui_component::intro_addhearts_clicked(){
    ui_controller::hearts(ui_controller::hearts() + 15);
}

void ui_component::show_scores(){
    //hide playing
    _phearts_btn->opacity(0.0f);
    _pcoins_btn->opacity(0.0f);
    _ptime_btn->opacity(0.0f);
    _phearts_num->visible(false);
    _pcoins_num->visible(false);
    _ptime_num->visible(false);
    _pause_btn->opacity(0.0f);
    _unpause_btn->opacity(0.0f);
    _back_btn->opacity(0.0f);
    //show scores
    _coins_btn->opacity(1.0f);
    _time_btn->opacity(1.0f);
    _death_btn->opacity(1.0f);
    _total_btn->opacity(1.0f);
    _coins_num->visible(true);
    _time_num->visible(true);
    _death_num->visible(true);
    _total_num->visible(true);
    _finish_btn->opacity(1.0f);
    if(!_scores_posted){
        auto _coin_num = ui_controller::coins() * 10.0f;
        auto _time_num = ui_controller::time() * (-2.0f);
        auto _deaths_num = ui_controller::deaths() * (-5.0f);
        auto _total = _coin_num + _time_num + _deaths_num;
        ui_controller::scores().push_back({ui_controller::get_set(), _total});
        _scores_posted = true;
    }
}

void ui_component::play_pause_clicked(){
    if(_pause_btn->opacity() == 0.0f)
        return;
    for(size_t i = 0; i < 9; ++i)
        parent_scene()->layer((uint32_t)i)->playing(false);
    _pause_btn->opacity(0.0f);
    _unpause_btn->opacity(1.0f);
    _back_btn->opacity(1.0f);
}

void ui_component::play_unpause_clicked(){
    for(size_t i = 0; i < 9; ++i)
        parent_scene()->layer((uint32_t)i)->playing(true);
    _pause_btn->opacity(1.0f);
    _unpause_btn->opacity(0.0f);
    _back_btn->opacity(0.0f);
}

void ui_component::play_return_clicked(){
    parent_scene()->animated_fade(color::from_rgba(1, 1, 1, 1), 0.25f, [](){
        ui_controller::restore_hearts();
        ui_controller::set_intro(true);
        ui_controller::set_playing(false);
        ui_controller::set_tutorial(false);
        ui_controller::set_force_load_level(true);
        ui_controller::goto_first_level();
    });
}

void ui_component::play_finish_clicked(){
    //for now do the same thing as return_clicked
    parent_scene()->animated_fade(color::from_rgba(1, 1, 1, 1), 0.25f, [](){
        ui_controller::set_intro(true);
        ui_controller::set_playing(false);
        ui_controller::set_tutorial(false);
        ui_controller::set_force_load_level(true);
        ui_controller::goto_first_level();
    });
}

void ui_component::intro_play_clicked(){
    _intro_play = false;
    _intro_select = true;
    _play_btn->opacity(0);
    _set1_btn->opacity(1);
    _set2_btn->opacity(1);
    _tutorial_btn->opacity(1);
    _leaderboards_btn->opacity(1);
    _hearts_btn->opacity(1);
    _num_hearts->visible(true);
    _addhearts_btn->opacity(1);
}

void ui_component::intro_tutorial_clicked(){
    if(_tutorial_btn->image_name() == UI_BACK){
        _tutorial_btn->image_name(UI_TUTORIAL);
        _leaderboards_btn->opacity(1);
        return;
    }
    _intro_select = false;
    parent_scene()->animated_fade(color::from_rgba(1, 1, 1, 1), 0.25f, [](){
        ui_controller::set_intro(false);
        ui_controller::set_playing(false);
        ui_controller::set_tutorial(true);
        ui_controller::set_force_load_level(true);
        ui_controller::goto_first_level();
    });
}

void ui_component::intro_set1_clicked(){
    if(_tutorial_btn->image_name() == UI_BACK){
        ui_controller::leaderboard_to_show() = 1;
        return;
    }
    _intro_select = false;
    parent_scene()->animated_fade(color::from_rgba(1, 1, 1, 1), 0.25f, [](){
        ui_controller::set_intro(false);
        ui_controller::set_playing(true);
        ui_controller::set_tutorial(false);
        ui_controller::set_set(1);
        ui_controller::goto_first_level();
        ui_controller::set_force_load_level(true);
    });
}

void ui_component::intro_set2_clicked(){
    if(_tutorial_btn->image_name() == UI_BACK){
        ui_controller::leaderboard_to_show() = 2;
        return;
    }
    _intro_select = false;
    parent_scene()->animated_fade(color::from_rgba(1, 1, 1, 1), 0.25f, [](){
        ui_controller::set_intro(false);
        ui_controller::set_playing(true);
        ui_controller::set_tutorial(false);
        ui_controller::set_set(2);
        ui_controller::goto_first_level();
        ui_controller::set_force_load_level(true);
    });
}

void ui_component::touches_moved(const std::vector<touch> &touches, bool &swallow){
    
}

void ui_component::touches_ended(const std::vector<touch> &touches, bool &swallow){
    
}

void ui_component::touches_cancelled(const std::vector<touch> &touches, bool &swallow){
    
}

bool ui_component::should_serialize_children() const{
    return false;
}












