//
//  traits.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 01/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef RollerBallCore_traits__
#define RollerBallCore_traits__


#define HAS_MEMBER(_n_, _s_) \
template<class T, bool b> class has_correct_signature_##_n_ { \
public: \
    enum { value = false }; \
}; \
template<class T> class has_correct_signature_##_n_<T, true> { \
    typedef char yes[1]; \
    typedef char no[2]; \
    template <class> \
    static no& test(...); \
    template <class U> \
    static yes& test(_s_); \
public: \
    enum { value = sizeof(test<T>(&T::_n_)) == sizeof(yes) }; \
}; \
template <class T> \
class has_member_##_n_ { \
    typedef char yes[1]; \
    typedef char no[2]; \
    template <class U> \
    static yes& test(decltype(U::_n_)*); \
    template <class> \
    static no& test(...); \
public: \
    enum { has_member = sizeof(test<T>(0)) == sizeof(yes), value = has_member && has_correct_signature_##_n_<T, has_member>::value }; \
}; \
template <class T> \
class has_member_##_n_<T*> { \
    typedef char yes[1]; \
    typedef char no[2]; \
    template <class U> \
    static yes& test(decltype(U::_n_)*); \
    template <class> \
    static no& test(...); \
public: \
    enum { has_member = sizeof(test<T>(0)) == sizeof(yes), value = has_member && has_correct_signature_##_n_<T, has_member>::value }; \
}

#endif
