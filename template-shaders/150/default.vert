#version 150 core

precision highp float;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

in vec3 vertex;

void main() {
  gl_Position = projectionMatrix * modelViewMatrix * vec4(vertex, 1);
}
