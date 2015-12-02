#version 120
#extension GL_EXT_gpu_shader4 : enable

uniform int  nVert;
uniform int  zoom;
uniform vec2 center;
uniform vec2 scale;

attribute vec4 pos;

void main(void) 
{
  gl_Position = (gl_VertexID<nVert) ?
    vec4((pos.xy-center)/scale, pos.zw) : pos;
}
