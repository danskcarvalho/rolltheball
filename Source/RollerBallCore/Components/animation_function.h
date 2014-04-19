//
//  animation_function.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 01/01/14.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__animation_function__
#define __RollerBallCore__animation_function__

#include "components_base.h"

namespace rb {
    class animation_function {
    public:
        virtual float operator()(float time) const = 0;
        virtual ~animation_function();
    };
    
    class rate_animation_function : public animation_function {
    private:
        float _rate;
    public:
        rate_animation_function();
        rate_animation_function(float rate);
        float rate() const;
        float rate(float value);
    };
    
    class ease_in_function : public rate_animation_function {
    public:
        ease_in_function();
        ease_in_function(float rate);
        virtual float operator()(float time) const override;
    };
    
    class ease_out_function : public rate_animation_function {
    public:
        ease_out_function();
        ease_out_function(float rate);
        virtual float operator()(float time) const override;
    };
    
    class ease_in_out_function : public rate_animation_function {
    public:
        ease_in_out_function();
        ease_in_out_function(float rate);
        virtual float operator()(float time) const override;
    };
    
    class ease_elastic : public animation_function {
    private:
        float _period;
    public:
        ease_elastic();
        ease_elastic(float period);
        float period() const;
        float period(float value);
    };
    
    class ease_elastic_in : public ease_elastic {
    public:
        ease_elastic_in();
        ease_elastic_in(float period);
        virtual float operator()(float time) const override;
    };
    
    class ease_elastic_out : public ease_elastic {
    public:
        ease_elastic_out();
        ease_elastic_out(float period);
        virtual float operator()(float time) const override;
    };
    
    class ease_elastic_in_out : public ease_elastic {
    public:
        ease_elastic_in_out();
        ease_elastic_in_out(float period);
        virtual float operator()(float time) const override;
    };
    
    class ease_bounce : public animation_function {
    protected:
        float bounce_time(float time) const;
    };
    
    class ease_bounce_in : public ease_bounce {
    public:
        virtual float operator()(float time) const override;
    };
    
    class ease_bounce_out : public ease_bounce {
    public:
        virtual float operator()(float time) const override;
    };
    
    class ease_bounce_in_out : public ease_bounce {
    public:
        virtual float operator()(float time) const override;
    };
    
    class ease_back_in : public animation_function {
    public:
        virtual float operator()(float time) const override;
    };
    
    class ease_back_out : public animation_function {
    public:
        virtual float operator()(float time) const override;
    };
    
    class ease_back_in_out : public animation_function {
    public:
        virtual float operator()(float time) const override;
    };
}

#endif /* defined(__RollerBallCore__animation_function__) */
