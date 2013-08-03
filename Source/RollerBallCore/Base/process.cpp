//
//  program.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 21/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "process.h"

using namespace rb;

process::process(){
    
}
process::~process(){
    
}

bool rb::operator==(const process& p1, const process& p2){
    return p1.equals(p2);
}
bool rb::operator!=(const process& p1, const process& p2){
    return !p1.equals(p2);
}