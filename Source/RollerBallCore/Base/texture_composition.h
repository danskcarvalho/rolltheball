//
//  texture_composition.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 09/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__texture_composition__
#define __RollerBallCore__texture_composition__

#include "base.h"
#include "texture_layer.h"

namespace rb {
    class texture_composition {
    private:
        std::vector<texture_layer*> _layers;
        texture_source* _base_texture;
        float _scale;
        bool _mask_aggressively;
    public:
        texture_composition(const texture_source& base_texture, const float scale, const bool mask_aggressively, const std::vector<texture_layer*>& layers);
        ~texture_composition();
        
        inline const std::vector<texture_layer*>& layers() const{
            return _layers;
        }
        
        inline const texture_source*  base_texture() const{
            return _base_texture;
        }
        
        inline const float scale() const{
            return _scale;
        }
        
        inline const bool mask_aggressively() const {
            return _mask_aggressively;
        }
    };
}

#endif /* defined(__RollerBallCore__texture_composition__) */
