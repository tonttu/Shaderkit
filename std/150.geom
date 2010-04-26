in gl_PerVertex {
  vec4 gl_Position;
  float gl_PointSize;
  float gl_ClipDistance[];
} gl_in[];

in int gl_PrimitiveIDIn;

out gl_PerVertex {
  vec4 gl_Position;
  float gl_PointSize;
  float gl_ClipDistance[];
};

out int gl_PrimitiveID;
out int gl_Layer;

// Emit the current values of output variables to the current output primitive.
void EmitVertex();
// Completes the current output primitive and starts a new one. No vertex is emitted.
void EndPrimitive();
