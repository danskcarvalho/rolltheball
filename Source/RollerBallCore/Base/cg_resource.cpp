//
//  cg_resource.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 15/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "cg_resource.h"

using namespace rb;

CFTypeRef cg_resource::resource() const{
    return _cf_ref;
}

cg_resource::cg_resource(CFTypeRef ref){
    this->_cf_ref = ref;
}

void cg_resource::delete_resource(){
    if(this->_cf_ref){
        CFRelease(this->_cf_ref);
        this->_cf_ref = nullptr;
    }
}

cg_resource::~cg_resource(){
    if(this->_cf_ref){
        CFRelease(this->_cf_ref);
        this->_cf_ref = nullptr;
    }
}