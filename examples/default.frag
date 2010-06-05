#version 150 compatibility

precision highp float;

uniform sampler2D texture;

uniform float shininess;

in vec3 light_dir2;
in vec3 surface_normal2;
in vec3 eye2;
in vec3 uv2;

out vec4 color;

void main() {
  vec3 color_sum = vec3(0);
  vec3 surface_unit = normalize(surface_normal2);

  for (int light = 0; light < 1; light++) {
    vec3 light_unit = normalize(light_dir2);

    // ambient
    vec3 color = (gl_FrontMaterial.ambient * gl_LightSource[light].ambient).xyz;

    float dotv = max(0.0f, dot(light_unit, surface_unit));
    color += (gl_FrontMaterial.diffuse * gl_LightSource[light].diffuse).xyz * dotv;

    vec3 reflection = reflect(-light_unit, surface_unit);
    dotv = max(0.0f, dot(reflection, normalize(eye2)));
        // color += (gl_FrontMaterial.specular * gl_LightSource[light].specular).xyz * pow(dotv, gl_FrontMaterial.shininess);
    color += pow(dotv, shininess);

    color_sum += color;
  }

  gl_FragColor = vec4((gl_LightModel.ambient * gl_FrontMaterial.ambient).xyz + color_sum, 1.0);
}
