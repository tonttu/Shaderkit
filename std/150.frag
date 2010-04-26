in vec4 gl_FragCoord;
in bool gl_FrontFacing;
in float gl_ClipDistance[];
out vec4 gl_FragColor; // deprecated
out vec4 gl_FragData[gl_MaxDrawBuffers]; // deprecated
out float gl_FragDepth;
in vec2 gl_PointCoord;
in int gl_PrimitiveID;

// Returns the derivative in x using local differencing for the input argument p.
genType dFdx(genType p);
// Returns the derivative in y using local differencing for the input argument p.
genType dFdy(genType p);
// Returns the sum of the absolute derivative in x and y using local differencing for the input argument p.
genType fwidth(genType p);
