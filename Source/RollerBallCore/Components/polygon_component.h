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
    class mesh;
    class texture_map;
    class polygon_point_component : public node {
        
    };
    class polygon_component : public node {
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
            
            //renderable
            bool renderable : 1;
        } _flags;
        polygon _polygon;
        mesh* _m;
        mesh* _m_copy;
        mesh* _b; //border mesh
        
        texture_map* _map;
        transform_space _before;
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
        corner_type _border_corner_type;
        nullable<color> _border_color;
        rb_string _border_texture;
        float _max_s;
        //methods
        void destroy_polygon(bool destroy_map);
        void update_polygon();
        void transform_mesh();
        void update_collapsed_mesh();
        void update_collapsed_flag();
        void recreate_polygon();
        void recreate_border();
    public:
        polygon_component();
        ~polygon_component();
    };
}

#endif /* defined(__RollerBallCore__polygon_component__) */
