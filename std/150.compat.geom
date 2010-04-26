in gl_PerVertex { // part of the gl_PerVertex block described in 7.1
  // in addition to other gl_PerVertex members...
  vec4 gl_ClipVertex;
} gl_in[];

in gl_PerVertex {
  // in addition to other gl_PerVertex members...
  vec4 gl_FrontColor;
  vec4 gl_BackColor;
  vec4 gl_FrontSecondaryColor;
  vec4 gl_BackSecondaryColor;
  vec4 gl_TexCoord[];
  float gl_FogFragCoord;
} gl_in[];

out vec4 gl_ClipVertex;
