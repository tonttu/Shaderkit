#version 150 core

precision highp float;

uniform vec4 solidColor;

out vec4 color;

void main() {
  color = solidColor;
}
