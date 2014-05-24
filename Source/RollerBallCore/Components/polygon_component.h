//
//  polygon_component.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 08/08/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__polygon_component__
#define __RollerBallCore__polygon_component__

#include "components_base.h"
#include "transform_space.h"
#include "color.h"
#include "node.h"
#include "polygon.h"

namespace rb {
    class smooth_curve;
    class mesh;
    class texture_map;
    class polygon_point_component : public node {
    private:
        bool _in_live_edit;
    public:
        polygon_point_component();
    protected:
        virtual void after_becoming_active(bool node_was_moved) override;
    protected:
        virtual void transform_changed();
        //Live editing
    protected:
        virtual void begin_live_edit(rb::live_edit kind);
        virtual void end_live_edit();
        //Render Gizmo
    protected:
        virtual void render_gizmo() override;
        //Hit Test
    protected:
        //returns the bounds in self space...
        virtual rectangle bounds() const override;
    protected:
        //Typed Object
        virtual void describe_type() override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
        //Make it first
    public:
        void make_itself_first();
    private:
        void split();
    };
    class polygon_component : public node {
    public:
        friend class polygon_point_component;
    private:
        enum PolType : unsigned int {
            kPolQuad = 0,
            kPolCircle = 1,
            kPolFreeform = 2
        };
        struct {
            enum PolType type : 3; //Polygon type
            bool dirty_polygon : 1; //should recreate the entire polygon... we also have to invalidate buffers...
            bool in_live_edit : 1; //Moving points around
            bool dirty_map : 1; //Map changed
            bool dirty_opacity: 1; //Opacity changed
            bool dirty_tint : 1; //Tint changed
            bool dirty_blend : 1; //Blend changed
            bool dirty_transformable : 1; //We should create or change the polygon to be transformable (either because it's transforming or it's transformable [see 2 line bellow]...
            bool transformable : 1; //Is transformable?
            bool polygon_transformable : 1; //The polygon data is transformable...
            bool in_texture_transformation : 1; //Are we transforming the texture
            bool visible : 1; //visible?
            bool collapsed : 1; //collapsed?
            bool smooth : 1; //smooth
            bool open : 1; //open
            
            //border flags
            bool dirty_border_polygon : 1;
            bool dirty_border_color : 1;
            bool border_collapsed : 1;
            bool dirty_border_map : 1; //Used to adjust the texture mapping on the ipad...
            bool _new : 1;
            
            //renderable
            bool renderable : 1;
        } _flags;
        polygon _polygon;
        mesh* _m;
        mesh* _m_copy;
        mesh* _b; //border mesh
        mesh* _b_copy;
        mesh* _skeleton;
        
        texture_map* _map;
        matrix3x3 _before;
        matrix3x3 _before_b;
        rb_string _image;
        
        uint32_t _circle_sides;
        float _opacity;
        color _tint;
        float _blend;
        float _smooth_quality;
        float _smooth_divisions;
        transform_space _tx_space;
        //border
        float _border_size;
        border_placement _border_placement;
        corner_type _border_corner_type;
        nullable<color> _border_color;
        float _border_blend;
        rb_string _border_texture;
        vec2 _border_tx_scaling;
        float _border_rotation;
        float _max_s;
        //methods
        void reset_children(PolType pt);
        void create_polygon_data();
        void destroy_polygon();
        void update_polygon(bool refill_buffers);
        void transform_mesh();
        void update_collapsed_mesh();
        void update_collapsed_border_mesh();
        void update_collapsed_flag();
        void recreate_polygon();
        void recreate_border();
        //create skeleton
        mesh* create_skeleton(const polygon& p);
    public:
        polygon_component();
        ~polygon_component();
    protected:
        virtual void after_becoming_active(bool node_was_moved) override;
        virtual void before_becoming_inactive(bool node_was_moved) override;
        virtual void render(const bool refill_buffers) override;
    protected:
        //Typed Object
        virtual void describe_type() override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
        //Render Gizmo
    protected:
        virtual void render_gizmo() override;
        //Hit Test and Bounds
    protected:
        //returns the bounds in self space...
        virtual rectangle bounds() const override;
        //pt in screen coordinates
        virtual bool hit_test(const vec2& pt) const override;
        //rc in screen coordinates
        virtual bool hit_test(const rectangle& rc) const override;
    public:
        virtual bool is_degenerated() const;
        //Adding nodes
    public:
        virtual bool add_node_at(node* n, uint32_t at) override;
        virtual bool remove_node(node* n, bool cleanup) override;
        virtual bool bring_to_front(node* n) override;
        virtual bool send_to_back(node* n) override;
        virtual bool send_backward(node* n) override;
        virtual bool bring_forward(node* n) override;
    public:
        //properties
        const polygon& to_polygon() const;
        const rb_string& image_name() const;
        const rb_string& image_name(const rb_string& value);
        uint32_t circle_sides() const;
        uint32_t circle_sides(const uint32_t value);
        float opacity() const;
        float opacity(const float value);
        const color& tint() const;
        const color& tint(const color& value);
        float blend() const;
        float blend(const float value);
        float smooth_quality() const;
        float smooth_quality(const float value);
        float smooth_divisions() const;
        float smooth_divisions(const float value);
        const transform_space& texture_space() const;
        const transform_space& texture_space(const transform_space& value);
        float border_size() const;
        float border_size(const float value);
        corner_type border_corner_type() const;
        corner_type border_corner_type(const corner_type value);
        const nullable<color>& border_color() const;
        const nullable<color>& border_color(const nullable<color>& value);
        float border_blend() const;
        float border_blend(float value);
        const vec2& border_texture_scale() const;
        const vec2& border_texture_scale(const vec2& value);
        enum border_placement border_placement() const;
        enum border_placement border_placement(enum border_placement value);
        float border_texture_rotation() const;
        float border_texture_rotation(float value);
        const rb_string& border_image_name() const;
        const rb_string& border_image_name(const rb_string& value);
        float max_s() const;
        float max_s(const float value);
        bool transformable() const;
        bool transformable(const bool value);
        bool visible() const;
        bool visible(const bool value);
        bool smooth() const;
        bool smooth(const bool value);
        bool opened() const;
        bool opened(const bool value);
        bool marker() const;
        bool marker(const bool value);
        void reset_to_quad();
        void reset_to_circle();
        polygon to_smooth_polygon() const;
        virtual typed_object* clone() const override;
        void remove_scaling();
        //start transformation
    public:
        virtual std::vector<rb_string> transformables() override;
        virtual void start_transformation(long index) override;
    private:
        //actions
        void compute_union();
        void compute_intersection();
    public:
        void to_convex_hull();
        void to_ccw();
    };
}

#endif /* defined(__RollerBallCore__polygon_component__) */
