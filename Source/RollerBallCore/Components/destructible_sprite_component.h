//
//  destructible_sprite_component.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 17/11/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__destructible_sprite_component__
#define __RollerBallCore__destructible_sprite_component__

#include "components_base.h"
#include "node.h"
#include "transform_space.h"

namespace rb {
    class mesh;
    class texture_map;
    class texture_atlas;
    class example_class;
    class destructible_sprite_component : public node {
    private:
        //texture size
        nullable<vec2> _tex_size;
        texture_atlas* _last_atlas;
        //others
        vec2 _matrix;
        std::vector<mesh*> _m;
        std::vector<mesh*> _m_copy;
        texture_map* _map;
        std::vector<transform_space> _part_befores;
        std::vector<transform_space> _part_transforms;
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
        destructible_sprite_component();
        virtual ~destructible_sprite_component();
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
        const transform_space& transform(const uint32_t x, const uint32_t y) const;
        const transform_space& transform(const uint32_t x, const uint32_t y, const transform_space& value);
    private:
        vec2 size_of_tex() const;
        transform_space aspect_correction_factor() const;
    };
}

#endif /* defined(__RollerBallCore__destructible_sprite_component__) */
