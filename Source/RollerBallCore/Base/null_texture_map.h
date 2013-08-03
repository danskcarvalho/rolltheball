//
//  null_texture_map.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 13/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__null_texture_map__
#define __RollerBallCore__null_texture_map__

#include "texture_map.h"
#include "transform_space.h"
#include "rectangle.h"

namespace rb {
    class null_texture_map : public texture_map  {
    private:
        rectangle _r;
        transform_space _t;
    public:
        null_texture_map();
        virtual void set_texture_coords(vertex& v) const;
        virtual const transform_space& get_texture_space() const;
        virtual const rectangle& bounds() const;
        virtual ~null_texture_map();
    };
}

#endif /* defined(__RollerBallCore__null_texture_map__) */
