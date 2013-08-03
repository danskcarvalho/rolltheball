//
//  basic_shader.fsh
//  RollerBallCore
//
//  Created by Danilo Carvalho on 22/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifdef GL_ES
precision highp float;
#define LOWP lowp
#define MEDIUMP mediump
#define HIGHP highp
#else
#define LOWP
#define MEDIUMP
#define HIGHP
#endif

varying LOWP vec4 color_varying;
varying LOWP float blend_varying;
varying HIGHP vec2 texture_varying;
varying HIGHP vec4 texture_bounds_varying;

uniform sampler2D texture_sampler;

void main()
{
    gl_FragColor = (1.0 - blend_varying) * color_varying + blend_varying * texture2D(texture_sampler, texture_varying) * color_varying;
}
