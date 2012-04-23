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

in vec3 normal;
in vec3 vertex;
in vec4 uv0;

void main() {
  // This vertex in eye space
  vec4 tmp = vec4(vertex, 1);

  tmp.x += 0.8*sin(time*0.5);
  tmp.z += 0.8*cos(time*2.1243);

  vec4 point = gl_ModelViewMatrix * tmp;

  eye = -point.xyz / point.w;

  ambient = gl_LightModel.ambient * gl_FrontMaterial.ambient;

  for (int light = 0; light < LIGHTS; ++light) {
    light_dir[light] = gl_LightSource[light].position.xyz;
    ambient += gl_FrontMaterial.ambient * gl_LightSource[light].ambient;
    diffuse[light] = gl_FrontMaterial.diffuse * gl_LightSource[light].diffuse;
    specular[light] = gl_FrontMaterial.specular * gl_LightSource[light].specular;
  }

  surface_normal = gl_NormalMatrix * normal;

  // Normal projection transform of this vertex
  gl_Position = gl_ProjectionMatrix * point;

  uv = (gl_TextureMatrix[0] * uv0).xy;
}
