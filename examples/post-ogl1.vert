varying vec2 uv;

void main() {
  // This vertex in eye space
  vec4 point = gl_ModelViewMatrix * gl_Vertex;

  // Normal projection transform of this vertex
  gl_Position = gl_ProjectionMatrix * point;

  uv = (gl_TextureMatrix[0] * gl_MultiTexCoord0).xy;
}
