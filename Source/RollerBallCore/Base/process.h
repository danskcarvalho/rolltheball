//
//  program.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 21/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__process__
#define __RollerBallCore__process__

#include "base.h"

namespace rb {
    class process {
    protected:
        virtual bool equals(const process& other) const = 0;
    public:
        process();
        virtual ~process();
        virtual void begin_draw() = 0;
        virtual void end_draw() = 0;
        
        friend bool operator==(const process& p1, const process& p2);
        friend bool operator!=(const process& p1, const process& p2);
    };
    
    bool operator==(const process& p1, const process& p2);
    bool operator!=(const process& p1, const process& p2);
}

#endif /* defined(__RollerBallCore__process__) */
