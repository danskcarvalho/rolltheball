//
//  vertex.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 09/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "vertex.h"
using namespace rb;

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

void vertex::setup_attribute_descriptors(){
    //POSITION
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), BUFFER_OFFSET(0));
    //TEXTURE
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), BUFFER_OFFSET(8));
    //ALPHA
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(vertex), BUFFER_OFFSET(16));
    //BLEND
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(vertex), BUFFER_OFFSET(20));
    //TEXTURE BOUNDS
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), BUFFER_OFFSET(24));
    //COLOR
    glEnableVertexAttribArray(5);
#if defined(VERTEX_PACKED_COLOR)
    glVertexAttribPointer(5, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(vertex), BUFFER_OFFSET(40));
#else
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), BUFFER_OFFSET(40));
#endif
}

void vertex::setup_attribute_bindings(uint32_t program){
    glBindAttribLocation(program, 0, "position");
    glBindAttribLocation(program, 1, "texture");
    glBindAttribLocation(program, 2, "alpha");
    glBindAttribLocation(program, 3, "blend");
    glBindAttribLocation(program, 4, "texture_bounds");
    glBindAttribLocation(program, 5, "color");
}