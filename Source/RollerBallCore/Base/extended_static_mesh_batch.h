//
//  extended_static_mesh_batch.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 12/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__extended_static_mesh_batch__
#define __RollerBallCore__extended_static_mesh_batch__

#include "base.h"
#include "mesh_batch.h"

namespace rb {
    class extended_dynamic_mesh_batch;
    class process;
    class static_mesh_batch;
    class extended_static_mesh_batch : public mesh_batch {
    private:
        std::vector<static_mesh_batch*> _batches;
        
        //blend mode
        blend_mode _blend_mode;
        //line width
        float _line_width;
        
        geometry_type _geom_type;
        
        extended_static_mesh_batch();
    public:
        virtual ~extended_static_mesh_batch();
        virtual void draw() override;
        
        //line width
        inline float line_width() const {
            return _line_width;
        }
        
        inline float line_width(const float value) {
            assert(value >= 0);
            _line_width = value;
            return _line_width;
        }
        
        //geometry type
        inline enum geometry_type geometry_type() const {
            return _geom_type;
        }
        inline enum geometry_type geometry_type(const enum geometry_type value){
            _geom_type = value;
            return _geom_type;
        }
        
        //blend mode
        inline blend_mode blend_mode() const {
            return _blend_mode;
        }
        inline enum blend_mode blend_mode(const enum blend_mode value) {
            _blend_mode = value;
            return _blend_mode;
        }
        
        friend class extended_dynamic_mesh_batch;
    };
}

#endif /* defined(__RollerBallCore__extended_static_mesh_batch__) */
