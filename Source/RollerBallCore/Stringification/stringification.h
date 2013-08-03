//
//  stringification.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 01/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef RollerBallCore_stringification__
#define RollerBallCore_stringification__

#ifndef _FINAL_APP_
#include "precompiled.h"
#endif
#include "definitions.h"
#include "enumerations.h"
#include "traits.h"

#define CLASS_NAME(_n_) \
namespace rb { \
template <> \
class class_name<_n_> { \
public: \
    static rb_string value() { \
        return TO_U_STR(_n_); \
    } \
}; \
}

#define BEGIN_STRINGIFY_ENUM(_enum_) \
namespace rb { \
    template<> \
    class stringify<_enum_>{ \
    public: \
        static rb_string to_string(const _enum_ t){

#define STRINGIFY_ENUM_VALUE(_value_) \
            if(t == _value_) \
                return TO_U_STR(_value_);


#define END_STRINGIFY_ENUM() \
            return u"undefined"; \
        } \
    }; \
}

#define BEGIN_STRINGIFY_FLAGS(_flags_) \
namespace rb { \
    template<> \
    class stringify<_flags_>{ \
    public: \
        static rb_string to_string(const _flags_ t){ \
            rb_string _str = u"0"; \
            bool _first = true; \
            bool _hasFlag = false;

#define STRINGIFY_FLAG_VALUE(_value_) \
            _hasFlag = rb::has_flag(t, _value_); \
            if(_hasFlag && _first){ \
                _str = TO_U_STR(_value_); \
                _first = false; \
            } \
            else if(_hasFlag && !_first){ \
                _str += u" | "; \
                _str += TO_U_STR(_value_); \
            }


#define END_STRINGIFY_FLAGS() \
            return _str; \
        } \
    }; \
}

namespace rb {
    
    template <class T>
    class class_name {
    public:
        static rb_string value() {
            return u"no_name";
        }
    };
    
    
    //This is the default implementation... We assume type T has no to_string
    template <class T, bool hasToString>
    class custom_stringify {
    public:
        static rb_string to_string(const T& t) {
            return class_name<T>::value();
        }
    };
    
    //This is the default implementation... We assume type T has to_string
    template <class T>
    class custom_stringify<T, true> {
    public:
        //The default implementation calls to_string method. This should be the preferred way for classes to add stringify support...
        static rb_string to_string(const T& t) {
            rb_string str;
            str += u"{ ";
            str += t.to_string();
            str += u" }";
            return str;
        }
    };
    
    template <class T>
    static rb_string _to_rb_string(const T val){
        std::stringstream ss;
        ss << val;
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> str_converter;
        return str_converter.from_bytes(ss.str());
    }
    
    template<class T, bool hasToString>
    class custom_stringify<T*, hasToString> {
    public:
        static rb_string to_string(const T* t) {
            if(t == nullptr)
                return u"nullptr";
            else {
                rb_string str = u"*(";
                str += _to_rb_string(reinterpret_cast<const void*>(t));
                str += u") ";
                str += class_name<T>::value();
                return str;
            }
        }
    };
    
    template<class T>
    class custom_stringify<T*, true> {
    public:
        //The default implementation calls to_string method. This should be the preferred way for classes to add stringify support...
        static rb_string to_string(const T* t) {
            if(t == nullptr)
                return u"nullptr";
            else {
                rb_string str = u"*(";
                str += _to_rb_string(reinterpret_cast<const void*>(t));
                str += u") ";
                str += u"{ ";
                str += t->to_string();
                str += u" }";
                return str;
            }
        }
    };
    
    template<class T>
    class custom_stringify<const T*, true> {
    public:
        //The default implementation calls to_string method. This should be the preferred way for classes to add stringify support...
        static rb_string to_string(const T* t) {
            if(t == nullptr)
                return u"nullptr";
            else {
                rb_string str = u"*(";
                str += _to_rb_string(reinterpret_cast<const void*>(t));
                str += u") ";
                str += u"{ ";
                str += t->to_string();
                str += u" }";
                return str;
            }
        }
    };
    
    HAS_MEMBER(to_string,rb_string (U::*)(void) const);
    
    //This is the default implementation.
    template<class T>
    class stringify {
    public:
        static rb_string to_string(const T& t){
            return custom_stringify<T, has_member_to_string<T>::value>::to_string(t);
        }
    };
    
    template<class T>
    class stringify<T*> {
    public:
        static rb_string to_string(const T* t){
            return custom_stringify<T*, has_member_to_string<T*>::value>::to_string(t);
        }
    };
    
    template<class T>
    class stringify<const T*> {
    public:
        static rb_string to_string(const T* t){
            return custom_stringify<T*, has_member_to_string<T*>::value>::to_string(t);
        }
    };
    
    template<class T>
    class stringify<ptr<T>>{
    public:
        static rb_string to_string(const ptr<T>& t){
            auto p = t.get();
            return stringify<T*>::to_string(p);
        }
    };
    
    template<class T>
    class stringify<wk<T>>{
    public:
        static rb_string to_string(const wk<T>& t){
            auto p = t.lock();
            return stringify<ptr<T>>::to_string(p);
        }
    };
    
    template<class T>
    class stringify<uptr<T>>{
    public:
        static rb_string to_string(const wk<T>& t){
            auto p = t.get();
            return stringify<ptr<T>>::to_string(p);
        }
    };
    
    template<>
    class stringify<rb_string>{
    public:
        static rb_string to_string(const rb_string& t){
            return t;
        }
    };
    
    template<size_t n>
    class stringify<char16_t[n]>{
    public:
        static rb_string to_string(const char16_t t[n]){
            return t;
        }
    };
    
    template<size_t n>
    class stringify<char[n]>{
    public:
        static rb_string to_string(const char t[n]){
            return _to_rb_string(t);
        }
    };
    
    template<>
    class stringify<const char16_t*>{
    public:
        static rb_string to_string(const char16_t* t){
            return t;
        }
    };
    
    template<>
    class stringify<char16_t*>{
    public:
        static rb_string to_string(const char16_t* t){
            return t;
        }
    };
    
    template<>
    class stringify<const char*>{
    public:
        static rb_string to_string(const char* t){
            return _to_rb_string(t);
        }
    };
    
    template<>
    class stringify<char*>{
    public:
        static rb_string to_string(const char* t){
            return _to_rb_string(t);
        }
    };
    
#define STRINGIFY_PRIMITIVE(_p_) \
    template<> \
    class stringify<_p_>{ \
    public: \
        static rb_string to_string(const _p_ t){ \
            return _to_rb_string(t); \
        } \
    }
    
    //Funções primitivas
    STRINGIFY_PRIMITIVE(short);
    STRINGIFY_PRIMITIVE(unsigned short);
    STRINGIFY_PRIMITIVE(int);
    STRINGIFY_PRIMITIVE(unsigned int);
    STRINGIFY_PRIMITIVE(long);
    STRINGIFY_PRIMITIVE(unsigned long);
    STRINGIFY_PRIMITIVE(long long);
    STRINGIFY_PRIMITIVE(unsigned long long);
    STRINGIFY_PRIMITIVE(float);
    STRINGIFY_PRIMITIVE(double);
    STRINGIFY_PRIMITIVE(long double);
    STRINGIFY_PRIMITIVE(void*);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wduplicate-decl-specifier"
    STRINGIFY_PRIMITIVE(const void*);
#pragma clang diagnostic pop
    
    template<>
    class stringify<bool> {
    public:
        static rb_string to_string(const bool t){
            return t ? u"true" : u"false";
        }
    };
    
    //Containers STL
    template<class T, std::size_t size>
    class stringify<std::array<T, size>> {
    public:
        static rb_string to_string(const std::array<T, size>& t){
            rb_string _str = u"[";
            bool _first = true;
            
            for(auto& _v : t){
                auto _v_str = stringify<T>::to_string(_v);
                if(_first) {
                    _str += _v_str;
                    _first = false;
                }
                else {
                    _str += u", ";
                    _str += _v_str;
                }
            }
            
            _str += u"]";
            
            return _str;
        }
    };
    
#define STRINGIFY_SEQUENCE_CONTAINER(_type_) \
    template<class T> \
    class stringify<_type_<T>> { \
    public: \
        static rb_string to_string(const _type_<T>& t){ \
            rb_string _str = u"["; \
            bool _first = true; \
            for(auto& _v : t){ \
                auto _v_str = stringify<T>::to_string(_v); \
                if(_first) { \
                    _str += _v_str; \
                    _first = false; \
                } \
                else { \
                    _str += u", "; \
                    _str += _v_str; \
                } \
            } \
            _str += u"]"; \
            return _str; \
        } \
    }
    
    STRINGIFY_SEQUENCE_CONTAINER(std::vector);
    STRINGIFY_SEQUENCE_CONTAINER(std::deque);
    STRINGIFY_SEQUENCE_CONTAINER(std::forward_list);
    STRINGIFY_SEQUENCE_CONTAINER(std::list);
    STRINGIFY_SEQUENCE_CONTAINER(std::set);
    STRINGIFY_SEQUENCE_CONTAINER(std::multiset);
    STRINGIFY_SEQUENCE_CONTAINER(std::unordered_set);
    STRINGIFY_SEQUENCE_CONTAINER(std::unordered_multiset);
    
#define STRINGIFY_ASSOCIATIVE_CONTAINER(_type_) \
    template<class TKey, class TValue> \
    class stringify<_type_<TKey, TValue>> { \
    public: \
        static rb_string to_string(const _type_<TKey, TValue>& t){ \
            rb_string _str = u"["; \
            bool _first = true; \
            for(auto& _v : t){ \
                auto _v_first = stringify<TKey>::to_string(_v.first);\
                auto _v_second = stringify<TValue>::to_string(_v.second);\
                if(_first) { \
                    _str += _v_first;\
                    _str += u": ";\
                    _str += _v_second;\
                    _first = false; \
                } \
                else { \
                    _str += u", "; \
                    _str += _v_first;\
                    _str += u": ";\
                    _str += _v_second;\
                } \
            } \
            _str += u"]"; \
            return _str; \
        }\
    }
    
    STRINGIFY_ASSOCIATIVE_CONTAINER(std::map);
    STRINGIFY_ASSOCIATIVE_CONTAINER(std::multimap);
    STRINGIFY_ASSOCIATIVE_CONTAINER(std::unordered_map);
    STRINGIFY_ASSOCIATIVE_CONTAINER(std::unordered_multimap);
    
    inline rb_string _to_string(rb_string & _acc){
        return _acc;
    }
    
    template<class T, class... Tps>
    rb_string _to_string(rb_string& _acc,  const T& value){
        _acc += stringify<T>::to_string(value);
        return _acc;
    }

    template<class T, class... Tps>
    rb_string _to_string(rb_string _acc, const T& value, const Tps&... tail){
        _acc += stringify<T>::to_string(value);
        return _to_string(_acc, tail...);
    }

    template<class... Tps>
    rb_string to_string(const Tps&... values){
        rb_string _acc;
        return _to_string(_acc, values...);
    }
}

#endif
