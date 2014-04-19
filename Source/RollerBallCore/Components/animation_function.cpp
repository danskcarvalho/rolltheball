//
//  animation_function.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 01/01/14.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#include "animation_function.h"

using namespace rb;

#define M_PI_X_2 (float)M_PI * 2.0f

animation_function::~animation_function(){
}

rate_animation_function::rate_animation_function(){
    _rate = 0;
}

rate_animation_function::rate_animation_function(float rate){
    _rate = rate;
}

float rate_animation_function::rate() const {
    return _rate;
}

float rate_animation_function::rate(float value){
    _rate = value;
    return _rate;
}

ease_in_function::ease_in_function()
: rate_animation_function(2) {
    
}

ease_in_function::ease_in_function(float rate)
: rate_animation_function(rate){
    
}

float ease_in_function::operator()(float time) const {
    return powf(time, rate());
}

ease_out_function::ease_out_function()
: rate_animation_function(2){
    
}

ease_out_function::ease_out_function(float rate)
: rate_animation_function(rate){

}

float ease_out_function::operator()(float time) const{
    return powf(time, 1 / rate());
}

ease_in_out_function::ease_in_out_function()
: rate_animation_function(2){
    
}

ease_in_out_function::ease_in_out_function(float rate)
: rate_animation_function(rate){
    
}

float ease_in_out_function::operator()(float t) const {
    float f = rate();
    
    t *= 2;
	if (t < 1) {
		return 0.5f * powf (t, f);
    }
	else {
		return 1.0f - 0.5f * powf(2-t, f);
    }
}

ease_elastic::ease_elastic(){
    _period = 0;
}

ease_elastic::ease_elastic(float period){
    _period = period;
}

float ease_elastic::period() const {
    return _period;
}

float ease_elastic::period(float value){
    _period = value;
    return _period;
}

ease_elastic_in::ease_elastic_in()
: ease_elastic(0.3f){
    
}

ease_elastic_in::ease_elastic_in(float period)
: ease_elastic(period){
    
}

float ease_elastic_in::operator()(float t) const{
    float f = period();
    float newT = 0;
    if (t == 0 || t == 1)
    {
        newT = t;
    }
    else
    {
        float s = f / 4;
        t = t - 1;
        newT = -powf(2, 10 * t) * sinf((t - s) * M_PI_X_2 / f);
    }
    
    return newT;
}

ease_elastic_out::ease_elastic_out()
: ease_elastic(0.3f){
    
}

ease_elastic_out::ease_elastic_out(float period)
: ease_elastic(period){
    
}

float ease_elastic_out::operator()(float t) const {
    float f = period();
    float newT = 0;
    if (t == 0 || t == 1)
    {
        newT = t;
    }
    else
    {
        float s = f / 4;
        newT = powf(2, -10 * t) * sinf((t - s) * M_PI_X_2 / f) + 1;
    }
    
    return newT;
}

ease_elastic_in_out::ease_elastic_in_out()
: ease_elastic(0.3f){
    
}

ease_elastic_in_out::ease_elastic_in_out(float period)
: ease_elastic(period){
    
}

float ease_elastic_in_out::operator()(float t) const {
    float newT = 0;
    
    if (t == 0 || t == 1)
    {
        newT = t;
    }
    else
    {
        float _period = period();
        t = t * 2;
        if (! _period)
        {
            _period = 0.3f * 1.5f;
        }
        
        float s = _period / 4;
        
        t = t - 1;
        if (t < 0)
        {
            newT = -0.5f * powf(2, 10 * t) * sinf((t - s) * M_PI_X_2 / _period);
        }
        else
        {
            newT = powf(2, -10 * t) * sinf((t - s) * M_PI_X_2 / _period) * 0.5f + 1;
        }
    }
    
    return newT;
}

float ease_bounce::bounce_time(float time) const {
    if (time < 1 / 2.75)
    {
        return 7.5625f * time * time;
    } else
		if (time < 2 / 2.75)
		{
			time -= 1.5f / 2.75f;
			return 7.5625f * time * time + 0.75f;
		} else
            if(time < 2.5 / 2.75)
            {
                time -= 2.25f / 2.75f;
                return 7.5625f * time * time + 0.9375f;
            }
    
    time -= 2.625f / 2.75f;
    return 7.5625f * time * time + 0.984375f;
}

float ease_bounce_in::operator()(float t) const {
    float newT = t;
	// prevents rounding errors
	if( t !=0 && t!=1)
		newT = 1 - bounce_time(1-t);
    
	return newT;
}

float ease_bounce_out::operator()(float t) const {
    float newT = t;
	// prevents rounding errors
	if( t !=0 && t!=1)
		newT = bounce_time(t);
    
	return newT;
}

float ease_bounce_in_out::operator()(float t) const {
    float newT;
	// prevents possible rounding errors
	if( t ==0 || t==1)
		newT = t;
	else if (t < 0.5) {
		t = t * 2;
		newT = (1 - bounce_time(1-t)) * 0.5f;
	} else
		newT = bounce_time(t * 2 - 1) * 0.5f + 0.5f;
    
	return newT;
}

float ease_back_in::operator()(float time) const {
    float overshoot = 1.70158f;
    return time * time * ((overshoot + 1) * time - overshoot);
}

float ease_back_out::operator()(float time) const {
    float overshoot = 1.70158f;
    
    time = time - 1;
    return time * time * ((overshoot + 1) * time + overshoot) + 1;
}

float ease_back_in_out::operator()(float time) const {
    float overshoot = 1.70158f * 1.525f;
    
    time = time * 2;
    if (time < 1)
    {
        return ((time * time * ((overshoot + 1) * time - overshoot)) / 2);
    }
    else
    {
        time = time - 2;
        return (time * time * ((overshoot + 1) * time + overshoot)) / 2 + 1;
    }
}













