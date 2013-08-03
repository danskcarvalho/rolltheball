//
//  basic_shader_with_wrapping.fsh
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
varying HIGHP vec2 texture_varying;
varying HIGHP vec4 texture_bounds_varying;
varying LOWP float blend_varying;

uniform sampler2D texture_sampler;

void main()
{
    vec2 tex_coords = texture_bounds_varying.xy + fract(texture_varying) * texture_bounds_varying.zw;
    vec4 tex_color = texture2D(texture_sampler, tex_coords);
    gl_FragColor = (1.0 - blend_varying) * tex_color.a * color_varying + blend_varying * tex_color * color_varying;
}
