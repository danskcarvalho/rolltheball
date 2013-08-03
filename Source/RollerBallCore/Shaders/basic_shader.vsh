//
//  basic_shader.vsh
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

attribute HIGHP vec2 position;
attribute HIGHP vec2 texture;
attribute LOWP float alpha;
attribute HIGHP vec4 texture_bounds;
attribute LOWP vec4 color;
attribute LOWP float blend;

varying LOWP vec4 color_varying;
varying LOWP float blend_varying;
varying HIGHP vec2 texture_varying;
varying HIGHP vec4 texture_bounds_varying;

uniform HIGHP mat3 position_transform;
uniform HIGHP mat3 texture_transform;
uniform LOWP vec4 ambient_color;

void main()
{
    vec3 tex_coords = texture_transform * vec3(texture, 1);
    vec3 pos_coords = position_transform * vec3(position, 1);
    color_varying = alpha * color * ambient_color;
    texture_varying = tex_coords.xy;
    texture_bounds_varying = texture_bounds;
    blend_varying = blend;
    gl_Position = vec4(pos_coords.xy, 0, 1);
}
