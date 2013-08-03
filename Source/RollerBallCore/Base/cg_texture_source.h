//
//  cg_texture_source.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 22/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__cg_texture_source__
#define __RollerBallCore__cg_texture_source__

#include "base.h"
#include "texture_source.h"
#include <CoreGraphics/CGContext.h>
#include <ImageIO/ImageIO.h>

namespace rb {
    class cg_texture_source : public texture_source {
    private:
        rb_string _path_str;
        URL_TYPE _path_ref;
        cg_texture_source();
    public:
        cg_texture_source(NSURL* path);
        virtual const URL_TYPE image_url() const;
        virtual rb_string to_string() const;
        virtual texture_source* duplicate() const;
        
        virtual ~cg_texture_source();
    };
}

#endif /* defined(__RollerBallCore__cg_texture_source__) */
