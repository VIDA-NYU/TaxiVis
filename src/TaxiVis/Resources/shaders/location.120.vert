#version 130
#extension GL_EXT_gpu_shader4 : enable

uniform float zoom;
uniform vec2  center;
uniform vec2  size;

#define M_PI 3.14159265358979323846
#define lat2worldY(lat) (log(tan(M_PI/4+lat*M_PI/360)))
#define lat2y(lat) ((M_PI-lat2worldY(lat))/M_PI*128)
#define lon2x(lon) ((lon+180)/360.0*256)

void main(void) 
{
  float unit = exp2(zoom);
  vec4  screenPos = vec4((lon2x(gl_Vertex.y)-lon2x(center.y))*unit+size.x*0.5,
                         (lat2y(gl_Vertex.x)-lat2y(center.x))*unit+size.y*0.5,
                         0.0, 1.0);
  gl_Position = gl_ModelViewProjectionMatrix*screenPos;
  gl_FrontColor = gl_Color;
}
