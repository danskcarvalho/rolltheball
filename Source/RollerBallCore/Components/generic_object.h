//
//  generic_object.h
//  RollerBallCore
//
//  Created by Danilo Santos de Carvalho on 27/03/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__generic_object__
#define __RollerBallCore__generic_object__

#include "components_base.h"
#include "vec2.h"
#include "color.h"
#include "buffer.h"

namespace rb {
    class typed_object;
    class generic_object {
    private:
        union {
            long lValue;
            float fValue;
        } _value01;
        rb_string _value02;
        std::vector<typed_object*> _value03;
        vec2 _value04;
        color _value05;
        generic_object_type _type;
        class buffer _buffer;
    public:
        inline generic_object_type type() const {
            return _type;
        }
        //Test
        bool is_null() const;
        //Constructors
        generic_object();
        generic_object(const bool value);
        generic_object(const long value);
        generic_object(const float value);
        generic_object(const vec2& value);
        generic_object(const color& value);
        generic_object(const rb_string& value);
        generic_object(typed_object* value);
        generic_object(const nullable<bool>& value);
        generic_object(const nullable<long>& value);
        generic_object(const nullable<float>& value);
        generic_object(const nullable<vec2>& value);
        generic_object(const nullable<color>& value);
        generic_object(const nullable<rb_string>& value);
        generic_object(class buffer value);
        generic_object(const nullable<class buffer>& value);
        generic_object(const std::vector<typed_object*>& value);
        template<class T>
        generic_object(const std::vector<T*>& value){
            _type = generic_object_type::vector;
            _value03.clear();
            for (auto _v : value)
                _value03.push_back((typed_object*)_v);
        }
        
        //Values
        buffer buffer_value() const;
        bool boolean_value() const;
        long integer_value() const;
        float single_value() const;
        vec2 vec2_value() const;
        color color_value() const;
        rb_string string_value() const;
        nullable<buffer> nullable_buffer_value() const;
        nullable<bool> nullable_boolean_value() const;
        nullable<long> nullable_integer_value() const;
        nullable<float> nullable_single_value() const;
        nullable<vec2> nullable_vec2_value() const;
        nullable<color> nullable_color_value() const;
        nullable<rb_string> nullable_string_value() const;
        typed_object* object_value() const;
        std::vector<typed_object*> vector_value() const;
        template<class T>
        std::vector<T*> vector_value() const {
            std::vector<T*> _r;
            auto _vector = vector_value();
            for (auto _v : _vector)
                _r.push_back((T*)_v);
            return _r;
        }
        
        operator buffer() const;
        operator bool() const;
        operator long() const;
        operator float() const;
        operator vec2() const;
        operator color() const;
        operator rb_string() const;
        operator nullable<buffer>() const;
        operator nullable<bool>() const;
        operator nullable<long>() const;
        operator nullable<float>() const;
        operator nullable<vec2>() const;
        operator nullable<color>() const;
        operator nullable<rb_string>() const;
        operator typed_object*() const;
        operator std::vector<typed_object*>() const;
        template<class T>
        operator std::vector<T*>() const {
            std::vector<T*> _r;
            auto _vector = vector_value();
            for (auto _v : _vector)
                _r.push_back((T*)_v);
            return _r;
        }
        
        rb_string to_string() const;
    };
    
    bool operator==(const generic_object& o1, const generic_object& o2);
    bool operator!=(const generic_object& o1, const generic_object& o2);
    bool operator<(const generic_object& o1, const generic_object& o2);
    bool operator>(const generic_object& o1, const generic_object& o2);
    bool operator<=(const generic_object& o1, const generic_object& o2);
    bool operator>=(const generic_object& o1, const generic_object& o2);
}

namespace std {
    template<>
    struct hash<rb::generic_object>{
        size_t operator()(const rb::generic_object& obj) const;
    };
}

#endif /* defined(__RollerBallCore__generic_object__) */
