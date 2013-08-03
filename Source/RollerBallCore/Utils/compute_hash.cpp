//
//  compute_hash.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 02/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "compute_hash.h"
#include "precompiled.h"

using namespace rb;

size_t compute_hash::operator()(const char *buffer, std::size_t size){
    const size_t _seedPrimeNumber = 691;
    const size_t _fieldPrimeNumber = 397;
    size_t _hash = _seedPrimeNumber;
    for (int i = 0; i < size; ++i) {
        _hash *= (_fieldPrimeNumber + buffer[i]);
    }
    return _hash;
}