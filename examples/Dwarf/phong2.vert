#version 150 compatibility
#define LIGHTS 3

precision highp float;

uniform float time;

out vec3 light_dir[LIGHTS];
out vec4 diffuse[LIGHTS];
out vec4 specular[LIGHTS];

out vec4 ambient;

in vec3 normal;
in vec4 vertex;
in vec2 uv0;

uniform mat4 modelview;
uniform mat4 projection;

out vec3 surface_normal;
out vec3 eye;
out vec2 uv;

void main() {
  // This vertex in eye space
  vec4 tmp = vertex;

  tmp.x += 0.03*sin(time*0.5)*tmp.y;
  tmp.z += 0.03*cos(time*2.1243)*tmp.y;

  vec4 point = modelview * tmp;

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
  gl_Position = projection * point;

  uv = uv0;
}