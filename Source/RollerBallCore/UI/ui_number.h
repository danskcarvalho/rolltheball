//
//  ui_number.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 2014-05-26.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__ui_number__
#define __RollerBallCore__ui_number__

#include "components_base.h"
#include "node.h"

namespace rb {
    class sprite_component;
    class ui_component;
    enum class text_alignment {
        center,
        left,
        right
    };
    class ui_number : public node {
    public:
        friend class ui_component;
    private:
        int _digits;
        float _number;
        bool _is_date;
        bool _visible;
        text_alignment _alignment;
        sprite_component* _sprites[10];
        float _spacing;
        std::unordered_map<char16_t, float> _sizes;
    private:
        void update_sprites();
        void update_sizes();
    private:
        vec2 size_of_tex(const rb_string& tex_name) const;
        vec2 aspect_correction_factor(const rb_string& tex_name) const;
    protected:
        virtual bool should_serialize_children() const override;
        //Typed Object
        virtual void describe_type() override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
    public:
        ui_number();
        float number() const;
        float number(float value);
        bool is_date() const;
        bool is_date(bool value);
        bool visible() const;
        bool visible(bool value);
        text_alignment alignment() const;
        text_alignment alignment(text_alignment value);
        float spacing() const;
        float spacing(float value);
    };
}

#endif /* defined(__RollerBallCore__ui_number__) */
