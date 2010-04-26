out gl_PerVertex { // part of the gl_PerVertex block described in 7.1
  // in addition to other gl_PerVertex members...
  vec4 gl_ClipVertex;
};
out gl_PerVertex {
  // in addition to other gl_PerVertex members...
  vec4 gl_FrontColor;
  vec4 gl_BackColor;
  vec4 gl_FrontSecondaryColor;
  vec4 gl_BackSecondaryColor;
  vec4 gl_TexCoord[];
  float gl_FogFragCoord;
};

in vec4 gl_Color;
in vec4 gl_SecondaryColor;
in vec3 gl_Normal;
in vec4 gl_Vertex;
in vec4 gl_MultiTexCoord0;
in vec4 gl_MultiTexCoord1;
in vec4 gl_MultiTexCoord2;
in vec4 gl_MultiTexCoord3;
in vec4 gl_MultiTexCoord4;
in vec4 gl_MultiTexCoord5;
in vec4 gl_MultiTexCoord6;
in vec4 gl_MultiTexCoord7;
in float gl_FogCoord;

// This function will ensure that the incoming vertex value will be transformed
// in a way that produces exactly the same result as would be produced by
// OpenGL’s fixed functionality transform.
vec4 ftransform();
