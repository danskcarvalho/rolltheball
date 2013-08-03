//
//  texture_map.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 10/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__texture_map__
#define __RollerBallCore__texture_map__

namespace rb {
    class rectangle;
    class vertex;
    class transform_space;
    class texture_map {
    public:
        texture_map();
        virtual void set_texture_coords(vertex& v) const = 0;
        virtual const transform_space& get_texture_space() const = 0;
        virtual const rectangle& bounds() const = 0;
        virtual ~texture_map();
    };
}

#endif /* defined(__RollerBallCore__texture_map__) */
