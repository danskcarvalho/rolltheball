//
//  nullable.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 02/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef RollerBallCore_nullable__
#define RollerBallCore_nullable__

namespace rb {
    template<class T>
    class nullable {
    private:
        bool _has_value;
        T _value;
    public:
        nullable(){
            _has_value = false;
        }
        
        nullable(const std::nullptr_t value){
            _has_value = false;
        }
        
        nullable(const T& value){
            _has_value = true;
            _value = value;
        }
        
        nullable(const nullable& other){
            _has_value = other._has_value;
            _value = other._value;
        }
        
        nullable(nullable&& other){
            _has_value = other._has_value;
            _value = other._value;
        }
        
        const nullable& operator=(const std::nullptr_t other){
            _has_value = false;
            _value = T();
            return *this;
        }
        
        const nullable& operator=(const nullable& other){
            _has_value = other._has_value;
            _value = other._value;
            return *this;
        }
        
        const nullable& operator=(const T& other){
            _has_value = true;
            _value = other;
            return *this;
        }
        
        explicit operator T() const{
            assert(_has_value);
            return _value;
        }
        
        inline bool has_value() const {
            return _has_value;
        }
        
        T& value() const {
            assert(_has_value);
            return const_cast<T&>(_value);
        }
        
        const T& value(const T& v) {
            _has_value = true;
            _value = v;
            return _value;
        }
        
        rb_string to_string() const {
            if(_has_value)
                return rb::to_string(_value);
            else
                return u"nullptr";
        }
        
        T& operator->() const {
            assert(_has_value);
            return const_cast<T&>(_value);
        }
    };
}

#endif
