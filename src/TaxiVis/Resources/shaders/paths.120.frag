#version 120
varying vec3 tex;
varying float progress;

void main(void)
{
  if (tex.x>1 || tex.x<0) discard;
  float alpha = 1-sqrt(tex.y*tex.y+tex.z*tex.z);
  vec4 srcColor = vec4(0.1, 0.3, 0.5, alpha);
  vec4 dstColor = vec4(1.0, 0.2, 0.1, alpha);
  gl_FragColor = mix(srcColor, dstColor, progress);
}
