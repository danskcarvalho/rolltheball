//
//  sprite_component.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 21/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__sprite_component__
#define __RollerBallCore__sprite_component__

#include "components_base.h"
#include "node.h"

namespace rb {
    class mesh;
    class texture_map;
    class example_class;
    class sprite_component : public node {
    private:
        vec2 _matrix;
        mesh* _m;
        mesh* _m_copy;
        texture_map* _map;
        transform_space _before;
        rb_string _image;
        bool _reapply_mapping;
        float _opacity;
        color _tint;
        float _blend;
        bool _collapsed;
        bool _visible;
        bool _aspect_correction;
        //methods
        void destroy();
        void create();
        void reapply_mapping();
        void transform_mesh(const bool refill_buffers);
        //collapse methods
        void update_collapsed_mesh();
        void update_collapsed_flag();
    protected:
        virtual void after_becoming_active(bool node_was_moved) override;
        virtual void before_becoming_inactive(bool node_was_moved) override;
        virtual void render(const bool refill_buffers) override;
        virtual rectangle bounds() const override;
    public:
        sprite_component();
        virtual ~sprite_component();
    protected:
        //Typed Object
        virtual void describe_type() override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
        //properties
    public:
        const rb_string& image_name() const;
        const rb_string& image_name(const rb_string& value);
        bool aspect_correction() const;
        bool aspect_correction(const bool value);
        float opacity() const;
        float opacity(float value);
        bool visible() const;
        bool visible(bool value);
        const color& tint() const;
        const color& tint(const color& value);
        float blend() const;
        float blend(float value);
        const vec2& matrix() const;
        const vec2& matrix(const vec2& value);
    private:
        vec2 size_of_tex() const;
        transform_space aspect_correction_factor() const;
    };
}

#endif /* defined(__RollerBallCore__sprite_component__) */
