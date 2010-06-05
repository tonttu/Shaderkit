#version 150 compatibility

precision highp float;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 light_dir[];
in vec3 surface_normal[];
in vec3 eye[];
in vec2 uv[];

out vec3 light_dir2;
out vec3 surface_normal2;
out vec3 eye2;
out vec2 uv2;

void main() {
  for (int i = 0; i < gl_in.length(); ++i) {
    gl_Position = gl_in[i].gl_Position;
    gl_PointSize = gl_in[i].gl_PointSize;
    gl_ClipDistance = gl_in[i].gl_ClipDistance;
    light_dir2 = light_dir[i];
    surface_normal2 = surface_normal[i];
    eye2 = eye[i];
    uv2 = uv[i];
    EmitVertex();
  }
  EndPrimitive();
}
