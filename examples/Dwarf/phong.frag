#version 150 compatibility
#define LIGHTS 3

precision highp float;

uniform sampler2D dwarf;

uniform float shininess = 20.0;
uniform float specular_light = 0.1;

uniform float near, far;
uniform float mix_depth = 0.0;
uniform float gamma = 1.0;

in vec3 light_dir[LIGHTS];
in vec4 diffuse[LIGHTS];
in vec4 specular[LIGHTS];

in vec4 ambient;

in vec3 surface_normal;
in vec3 eye;

in vec2 uv;

out vec4 color;

void main() {
  color = ambient;
  vec3 surface_unit = normalize(surface_normal);
  vec4 tex = texture2D(dwarf, uv);
  vec3 eye_unit = normalize(eye);

  for (int light = 0; light < LIGHTS; light++) {
    vec3 light_unit = normalize(light_dir[light]);

    float dotv = max(0.0f, dot(light_unit, surface_unit));
    color += tex * diffuse[light] * dotv;

    vec3 reflection = reflect(-light_unit, surface_unit);
    dotv = max(0.0f, dot(reflection, eye_unit));
    //color += specular[light] * pow(dotv, gl_FrontMaterial.shininess);
    //color += specular[light] * pow(dotv, shininess);
    color += specular_light * pow(dotv, shininess);
  }
  float depth = pow((eye.z-near)/(far-near), gamma);
  color = mix(vec4(1.0-depth, depth, depth, 1.0), color, 1.0-mix_depth);
}