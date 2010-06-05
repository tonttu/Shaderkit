#version 150 compatibility

precision highp float;

out vec3 light_dir;
out vec3 surface_normal;
out vec3 eye;
out vec2 uv;

void main() {
  // This vertex in eye space
  vec4 point = gl_ModelViewMatrix * gl_Vertex;

  eye = -point.xyz / point.w;

  for (int light = 0; light < 1; ++light) {
    light_dir = gl_LightSource[light].position.xyz;
  }

  surface_normal = gl_NormalMatrix * gl_Normal;

  // Normal projection transform of this vertex
  gl_Position = gl_ProjectionMatrix * point;

  uv = (gl_TextureMatrix[0] * gl_MultiTexCoord0).xy;
}
