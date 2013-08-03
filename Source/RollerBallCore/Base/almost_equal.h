//
//  almost_equal.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 02/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef RollerBallCore_almost_equal__
#define RollerBallCore_almost_equal__

namespace rb {
    inline bool almost_equal(float n1, float n2){
        float epsilon = C_EPSILON;
        if (fabsf(n2 - n1) <= epsilon)
            return true;
        if (fabsf(n2 - n1) <= epsilon * fabsf(n2))
            return true;
        
        return false;
    }
    
    inline bool almost_greather(float n1, float n2){
        if(almost_equal(n1, n2))
            return false;
        
        return n1 > n2;
    }
    
    inline bool almost_less(float n1, float n2){
        if(almost_equal(n1, n2))
            return false;
        
        return n1 < n2;
    }
    
    inline bool almost_greather_or_equal(float n1, float n2){
        if(almost_equal(n1, n2))
            return true;
        
        return n1 > n2;
    }
    
    inline bool almost_less_or_equal(float n1, float n2){
        if(almost_equal(n1, n2))
            return true;
        
        return n1 < n2;
    }
}

#endif
