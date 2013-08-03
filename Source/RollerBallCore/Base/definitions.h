//
//  definitions.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 01/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef RollerBallCore_definitions__
#define RollerBallCore_definitions__

#define TO_U_STR(_t_) u###_t_

#ifdef _FINAL_APP_
#include <cstddef>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <string>
#include <memory>
#include <sstream>
#include <locale>
#include <codecvt>
#include <array>
#include <vector>
#include <deque>
#include <forward_list>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#endif

//Definiremos aqui os tipos básicos
namespace rb {
    typedef char16_t rb_char;
    typedef std::u16string rb_string;
    template<class T>
    using ptr = std::shared_ptr<T>;
    template<class T>
    using wk = std::weak_ptr<T>;
    template<class T>
    using uptr = std::unique_ptr<T>;
}

#endif
