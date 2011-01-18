uniform sampler2D texture;

uniform float shininess;

varying vec3 light_dir;
varying vec3 surface_normal;
varying vec3 eye;
varying vec3 uv;

void main() {
  vec3 color_sum;
  color_sum = vec3(0, 0, 0);
  vec3 surface_unit; surface_unit = normalize(surface_normal2);

  int light;
  for (light = 0; light < 1; light++) {
    vec3 light_unit, color, reflection;
    float dotv;

    light_unit = normalize(light_dir2);

    // ambient
    color = (gl_FrontMaterial.ambient * gl_LightSource[light].ambient).xyz;

    dotv = max(0.0f, dot(light_unit, surface_unit));
    color += (gl_FrontMaterial.diffuse * gl_LightSource[light].diffuse).xyz * dotv;

    reflection = reflect(-light_unit, surface_unit);
    dotv = max(0.0f, dot(reflection, normalize(eye2)));
        // color += (gl_FrontMaterial.specular * gl_LightSource[light].specular).xyz * pow(dotv, gl_FrontMaterial.shininess);
    color += pow(dotv, shininess);

    color_sum += color;
  }

  gl_FragColor = vec4((gl_LightModel.ambient * gl_FrontMaterial.ambient).xyz + color_sum, 1.0);
}
