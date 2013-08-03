//
//  responder.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 07/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__responder__
#define __RollerBallCore__responder__

#include "components_base.h"

namespace rb {
    class vec2;
    class touch;
    class responder {
    public:
        //called every frame
        virtual void update() = 0;
        
        //render renders the actual data after each update (ideal situation).
        virtual void render() = 0;
        
        //viewport resized
        virtual void viewport_resized() = 0;
        
        //when a key is pressed on the keyboard
        //never delivered in iOS...
        virtual void keydown(const uint32_t keycode, const keyboard_modifier modifier) = 0;
        virtual void keyup(const uint32_t keycode, const keyboard_modifier modifier) = 0;
        
        virtual void begin_gesture() = 0;
        virtual void end_gesture() = 0;
        
        //gestures
        virtual void magnify(const float delta) = 0;
        virtual void rotate(const float delta) = 0;
        virtual void swipe(const vec2& delta) = 0;
        virtual void scroll(const vec2& delta) = 0;
        
        //touches
        virtual void touches_began(const std::vector<touch>& touches) = 0;
        virtual void touches_moved(const std::vector<touch>& touches) = 0;
        virtual void touches_ended(const std::vector<touch>& touches) = 0;
        virtual void touches_cancelled(const std::vector<touch>& touches) = 0;
        
        //basic mouse events
        //those events isn't delivered in iOS...
        //the normalized position where 0 is the center of the screen...
        virtual void mouse_down(const vec2& normalized_position) = 0;
        virtual void mouse_up(const vec2& normalized_position) = 0;
        virtual void mouse_dragged(const vec2& normalized_position) = 0;
        
        //this throws an exception if called in iOS...
        static keyboard_modifier modifiers();
        
        //destructor
        virtual ~responder();
    };
}

#endif /* defined(__RollerBallCore__responder__) */
