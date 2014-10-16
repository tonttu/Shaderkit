#version 150 core

precision highp float;

uniform vec4 solidColor = vec4(1,0,0,1);

out vec4 color;

void main() {
  color = solidColor;
}
