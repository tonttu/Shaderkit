#version 150 compatibility
#define LIGHTS 3

precision highp float;

uniform float time;

out vec3 light_dir[LIGHTS];
out vec4 diffuse[LIGHTS];
out vec4 specular[LIGHTS];

out vec4 ambient;

out vec3 surface_normal;
out vec3 eye;
out vec2 uv;

void main() {
  // This vertex in eye space
  vec4 tmp = gl_Vertex;

  tmp.x += 0.03*sin(time*0.5)*tmp.y;
  tmp.z += 0.03*cos(time*2.1243)*tmp.y;

  vec4 point = gl_ModelViewMatrix * tmp;

  eye = -point.xyz / point.w;

  ambient = gl_LightModel.ambient * gl_FrontMaterial.ambient;

  for (int light = 0; light < LIGHTS; ++light) {
    light_dir[light] = gl_LightSource[light].position.xyz;
    ambient += gl_FrontMaterial.ambient * gl_LightSource[light].ambient;
    diffuse[light] = gl_FrontMaterial.diffuse * gl_LightSource[light].diffuse;
    specular[light] = gl_FrontMaterial.specular * gl_LightSource[light].specular;
  }

  surface_normal = gl_NormalMatrix * gl_Normal;

  // Normal projection transform of this vertex
  gl_Position = gl_ProjectionMatrix * point;

  uv = (gl_TextureMatrix[0] * gl_MultiTexCoord0).xy;
}