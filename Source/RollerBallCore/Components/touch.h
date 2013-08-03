//
//  touch.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 07/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__touch__
#define __RollerBallCore__touch__

#include "components_base.h"
#include "vec2.h"

#if __OBJC__
#if defined(MACOSX_TARGET)
#import <Cocoa/Cocoa.h>
#elif defined(IOS_TARGET) //IOS
#import <UIKit/UIKit.h>
#endif
#endif

#if __OBJC__
#define NATIVE_TOUCH_TYPE id
#else
#define NATIVE_TOUCH_TYPE void*
#endif

namespace rb {
    class vec2;
    class touch {
    private:
        NATIVE_TOUCH_TYPE _native_touch;
        vec2 _normalized_pos;
    public:
#if __OBJC__
#if defined(MACOSX_TARGET)
        touch(NSTouch* native_touch);
#elif defined(IOS_TARGET) //IOS
        touch(UITouch* native_touch, const vec2& normalized_position);
#endif
#endif
        bool compare_identity(const touch& another) const;
        //the normalized position where 0 is the center of the screen...
        const vec2& normalized_position() const;
        float x() const;
        float y() const;
        
        //constructors
        touch();
        touch(const touch& another);
        touch(touch&& another);
        touch& operator=(const touch& another);
        touch& operator=(touch&& another);
        ~touch();
    };
}

#endif /* defined(__RollerBallCore__touch__) */
