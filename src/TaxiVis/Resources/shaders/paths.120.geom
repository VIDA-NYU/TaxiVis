#version 120
#extension GL_EXT_geometry_shader4 : enable

uniform int   zoom;
uniform vec2  scale;
uniform float time;
uniform vec2  glyphArea;

varying vec3 tex;
varying float progress;

void main(void) 
{    
  vec4 p0 = gl_PositionIn[0];
  vec4 p1 = gl_PositionIn[1];
  vec4 p2 = gl_PositionIn[2];
  float prevTime = time - glyphArea.x;
  bool stopped = ((p2.x==p2.z) && (time<p2.x) ||
                  (p2.y==p2.w) && (time>p2.y));
  float curTime = stopped?clamp(time, p2.x, p2.y) : time;
  if (stopped || (curTime>=p2.x && prevTime<=p2.y)) {
    float w = exp2(-zoom-8.0);
    float width = exp2(min(zoom,18)-21.5+glyphArea.y)*w;
    float aspect = radians(scale.x/scale.y);
    vec4 dir = p1-p0;
    vec2 dir2 = normalize(vec2(dir.x*aspect, dir.y/aspect))*width;
    vec2 normal = vec2(-dir2.y, dir2.x);
    vec4 normalX = vec4(dir2.x/aspect, dir2.y*aspect, 0, 0);
    vec4 o0 = vec4(p0.xy-normal, 0, w);
    vec4 o1 = vec4(p0.xy+normal, 0, w);
    vec4 ds;
    float timeref;

    progress = (curTime-p2.z)/(p2.w-p2.z);
    
    timeref = (curTime-p2.x)/(p2.y-p2.x);
    ds = dir*timeref;
    gl_Position = o0 + ds + normalX;
    tex = vec3(timeref, 1, -1);
    EmitVertex();
    gl_Position = o1 + ds + normalX;
    tex = vec3(timeref, 1, 1);
    EmitVertex();

    gl_Position = o0 + ds;
    tex = vec3(timeref, 0, -1);
    EmitVertex();
    gl_Position = o1 + ds;
    tex = vec3(timeref, 0, 1);
    EmitVertex();

    if (stopped)
      ds -=normalX;
    else {
      timeref = (prevTime-p2.x)/(p2.y-p2.x);
      ds = dir*timeref;
    }
    gl_Position = o0 + ds;
    tex = vec3(timeref, -1, -1);
    EmitVertex();
    gl_Position = o1 + ds;
    tex = vec3(timeref, -1, 1);
    EmitVertex();

    EndPrimitive();
  }
}
