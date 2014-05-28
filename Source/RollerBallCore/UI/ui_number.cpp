//
//  ui_number.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 2014-05-26.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#include "ui_number.h"
#include "sprite_component.h"
#include "scene.h"
#include "texture_atlas.h"

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

rb_string get_string(int digits, float number, bool is_date){
    if(is_date && number < 0)
        number *= -1;
    std::string s;
    if(is_date){
        float _minutes = floorf(number / 60.0f);
        float _seconds = floorf(number - _minutes * 60.0f);
        if(_minutes > 99)
            _minutes = 99;
        std::string s1 = std::to_string((int)_minutes);
        std::string s2 = std::to_string((int)_seconds);
        if(s1.size() == 1)
            s1 = "0" + s1;
        if(s2.size() == 1)
            s2 = "0" + s2;
        s = s1 + ":" + s2;
    }
    else {
        if(number > 2000000000.0f)
            number = 2000000000.0f;
        if(number < -2000000000.0f)
            number = -2000000000.0f;
        auto _n = (int)number;
        s = std::to_string(_n);
        if(digits != 0)
        {
            while(s.size() != digits)
                s = "0" + s;
        }
    }
    
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert; // ... filled in with a codecvt to do UTF-8 <-> UTF-16
    return convert.from_bytes(s);
}

vec2 ui_number::aspect_correction_factor(const rb_string& tex_name) const {
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

vec2 ui_number::size_of_tex(const rb_string& tex_name) const {
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

rb_string get_texture(char16_t c){
    switch(c){
        case u'0':
            return UI_0;
        case u'1':
            return UI_1;
        case u'2':
            return UI_2;
        case u'3':
            return UI_3;
        case u'4':
            return UI_4;
        case u'5':
            return UI_5;
        case u'6':
            return UI_6;
        case u'7':
            return UI_7;
        case u'8':
            return UI_8;
        case u'9':
            return UI_9;
        case u'-':
            return UI_MINUS;
        case u':':
            return UI_COLON;
        default:
            throw "shouldn't get here!";
    }
}

void ui_number::update_sizes(){
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

ui_number::ui_number(){
    _digits = 0;
    _number = 0;
    _is_date = false;
    _visible = true;
    _alignment = text_alignment::center;
    _spacing = 0;
    for (auto i = 0; i < 10; i++){
        _sprites[i] = new sprite_component();
        _sprites[i]->image_name(UI_0);
        _sprites[i]->visible(i == 0);
        _sprites[i]->opacity(i == 0 ? 1.0f : 0.0f);
        add_node(_sprites[i]);
    }
}

void calc_pos(const rb_string& str, float spacing, const std::unordered_map<char16_t, float> sizes, text_alignment ta, std::vector<float>& positions){
    if(ta == text_alignment::center){
        if(str.size() == 1)
        {
            positions.push_back(0);
            return;
        }
        float size_chars = 0;
        for(size_t i = 0; i < str.size(); i++){
            if(i == 0 || i == (str.size() - 1))
                size_chars += sizes.at(str[i]) / 2.0f;
            else
                size_chars += sizes.at(str[i]);
        }
        float total_size = ((float)str.size() - 1.0f) * spacing + size_chars;
        float a1 = -(total_size / 2.0f);
        auto inc = total_size / (str.size() - 1.0f);
        float start = a1;
        for(size_t i = 0; i < str.size(); i++){
            positions.push_back(start);
            start += inc;
        }
    }
    else if(ta == text_alignment::left){
        float start = 0;
        for(int i = ((int)str.size() - 1); i >= 0; i--){
            start -= sizes.at(str[i]) / 2.0f;
            positions.push_back(start);
            start -= sizes.at(str[i]) / 2.0f;
            start -= spacing;
        }
        std::reverse(positions.begin(), positions.end());
    }
    else {
        float start = 0;
        for(size_t i = 0; i < str.size(); i++){
            start += sizes.at(str[i]) / 2.0f;
            positions.push_back(start);
            start += sizes.at(str[i]) / 2.0f;
            start += spacing;
        }
    }
}

void ui_number::update_sprites(){
    update_sizes();
    if(!_visible){
        for (auto i = 0; i < 10; i++){
            _sprites[i]->visible(false);
            _sprites[i]->opacity(0.0f);
            _sprites[i]->old_transform(transform_space());
        }
        
        return;
    }
    auto _str = get_string(_digits, _number, _is_date);
    for (auto i = 0; i < 10; i++){
        _sprites[i]->visible(false);
        _sprites[i]->opacity(0.0f);
        _sprites[i]->old_transform(transform_space());
    }
    
    for (size_t i = 0; i < _str.size(); i++){
        _sprites[i]->visible(true);
        _sprites[i]->opacity(1.0f);
        _sprites[i]->image_name(get_texture(_str[i]));
    }
    std::vector<float> _positions;
    calc_pos(_str, _spacing, _sizes, _alignment, _positions);
    for (size_t i = 0; i < _positions.size(); i++){
        _sprites[i]->old_transform(_sprites[i]->old_transform().moved(vec2(_positions[i], 0.0f)));
        if(_str[i] == u'-')
            _sprites[i]->old_transform(_sprites[i]->old_transform().scaled(0.5f, 0.5f));
    }
}

bool ui_number::should_serialize_children() const {
    return false;
}

void ui_number::describe_type() {
    node::describe_type();
    
    start_type<ui_number>([](){return new ui_number();});
    single_property<ui_number>(u"number", u"Number", true, {
        [](const ui_number* site){
            return site->number();
        },
        [](ui_number* site, float value){
            site->number(value);
        }
    });
    boolean_property<ui_number>(u"is_date",u"Is Date", true, {
        [](const ui_number* site){
            return site->is_date();
        },
        [](ui_number* site, bool value){
            site->is_date(value);
        }
    });
    enumeration_property<ui_number, text_alignment>(u"alignment", u"Align", {{u"Center", text_alignment::center}, {u"Left", text_alignment::left}, {u"Right", text_alignment::right}}, true, {
        [](const ui_number* site){
            return site->alignment();
        },
        [](ui_number* site, text_alignment value){
            site->alignment(value);
        }
    });
    single_property<ui_number>(u"spacing", u"Spacing", true, {
        [](const ui_number* site){
            return site->spacing();
        },
        [](ui_number* site, float value){
            site->spacing(value);
        }
    });
    end_type();
}

rb_string ui_number::type_name() const {
    return u"rb::ui_number";
}

rb_string ui_number::displayable_type_name() const {
    return u"UI Number";
}

float ui_number::number() const{
    return _number;
}
float ui_number::number(float value){
    _number = value;
    update_sprites();
    return _number;
}
bool ui_number::is_date() const{
    return _is_date;
}
bool ui_number::is_date(bool value){
    _is_date = value;
    update_sprites();
    return _is_date;
}
bool ui_number::visible() const{
    return _visible;
}
bool ui_number::visible(bool value){
    _visible = value;
    update_sprites();
    return _visible;
}
text_alignment ui_number::alignment() const{
    return _alignment;
}
text_alignment ui_number::alignment(text_alignment value){
    _alignment = value;
    update_sprites();
    return _alignment;
}
float ui_number::spacing() const{
    return _spacing;
}
float ui_number::spacing(float value){
    _spacing = value;
    update_sprites();
    return _spacing;
}




























