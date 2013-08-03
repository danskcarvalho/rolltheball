//
//  tokenizer.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 02/08/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#include "base.h"
using namespace rb;

std::vector<rb_string> rb::tokenize(const rb_string& source){
    std::vector<rb_string> _tokens;
    rb_string _acc = u"";
    
    for (size_t i = 0; i < source.size(); i++) {
        if(source[i] == u' '){
            if (_acc == u"")
                continue;
            _tokens.push_back(_acc);
            _acc = u"";
        }
        else
        {
            _acc += source[i];
        }
    }
    
    if(_acc != u"")
        _tokens.push_back(_acc);
    
    return _tokens;
}
