#include <GL/glew.h>

void wrap_gl_extensions() {
  /* differs:
    GL_GEOMETRY_INPUT_TYPE
      0x8917
      0x8DDB
    GL_GEOMETRY_OUTPUT_TYPE
      0x8918
      0x8DDC
    GL_GEOMETRY_VERTICES_OUT
      0x8916
      0x8DDA
    GL_OBJECT_TYPE
      0x9112
      0x8B4E
   */

  /**
   * GL_ACTIVE_VERTEX_UNITS, GL_ALPHA16F, GL_ALPHA32F, GL_COMPRESSED_RGBA_BPTC_UNORM,
   * GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT, GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,
   * GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM, GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT, GL_CURRENT_MATRIX,
   * GL_CURRENT_MATRIX_INDEX, GL_CURRENT_MATRIX_STACK_DEPTH, GL_CURRENT_PALETTE_MATRIX,
   * GL_CURRENT_WEIGHT, GL_DEBUG_CALLBACK_FUNCTION, GL_DEBUG_CALLBACK_USER_PARAM,
   * GL_DEBUG_LOGGED_MESSAGES, GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH, GL_DEBUG_OUTPUT_SYNCHRONOUS,
   * GL_DEBUG_SEVERITY_HIGH, GL_DEBUG_SEVERITY_LOW, GL_DEBUG_SEVERITY_MEDIUM, GL_DEBUG_SOURCE_API,
   * GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_SOURCE_OTHER, GL_DEBUG_SOURCE_SHADER_COMPILER,
   * GL_DEBUG_SOURCE_THIRD_PARTY, GL_DEBUG_SOURCE_WINDOW_SYSTEM, GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR,
   * GL_DEBUG_TYPE_ERROR, GL_DEBUG_TYPE_OTHER, GL_DEBUG_TYPE_PERFORMANCE, GL_DEBUG_TYPE_PORTABILITY,
   * GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, GL_FRAGMENT_PROGRAM, GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT,
   * GL_GUILTY_CONTEXT_RESET, GL_INNOCENT_CONTEXT_RESET, GL_INTENSITY16F, GL_INTENSITY32F,
   * GL_LOSE_CONTEXT_ON_RESET, GL_LUMINANCE16F, GL_LUMINANCE32F, GL_LUMINANCE_ALPHA16F,
   * GL_LUMINANCE_ALPHA32F, GL_MATRIX0, GL_MATRIX1, GL_MATRIX10, GL_MATRIX11, GL_MATRIX12,
   * GL_MATRIX13, GL_MATRIX14, GL_MATRIX15, GL_MATRIX16, GL_MATRIX17, GL_MATRIX18, GL_MATRIX19,
   * GL_MATRIX2, GL_MATRIX20, GL_MATRIX21, GL_MATRIX22, GL_MATRIX23, GL_MATRIX24, GL_MATRIX25,
   * GL_MATRIX26, GL_MATRIX27, GL_MATRIX28, GL_MATRIX29, GL_MATRIX3, GL_MATRIX30, GL_MATRIX31,
   * GL_MATRIX4, GL_MATRIX5, GL_MATRIX6, GL_MATRIX7, GL_MATRIX8, GL_MATRIX9, GL_MATRIX_INDEX_ARRAY,
   * GL_MATRIX_INDEX_ARRAY_POINTER, GL_MATRIX_INDEX_ARRAY_SIZE, GL_MATRIX_INDEX_ARRAY_STRIDE,
   * GL_MATRIX_INDEX_ARRAY_TYPE, GL_MATRIX_PALETTE, GL_MAX_DEBUG_LOGGED_MESSAGES,
   * GL_MAX_DEBUG_MESSAGE_LENGTH, GL_MAX_GEOMETRY_VARYING_COMPONENTS,
   * GL_MAX_MATRIX_PALETTE_STACK_DEPTH, GL_MAX_PALETTE_MATRICES, GL_MAX_PROGRAM_ADDRESS_REGISTERS,
   * GL_MAX_PROGRAM_ALU_INSTRUCTIONS, GL_MAX_PROGRAM_ATTRIBS, GL_MAX_PROGRAM_ENV_PARAMETERS,
   * GL_MAX_PROGRAM_INSTRUCTIONS, GL_MAX_PROGRAM_LOCAL_PARAMETERS, GL_MAX_PROGRAM_MATRICES,
   * GL_MAX_PROGRAM_MATRIX_STACK_DEPTH, GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS,
   * GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS, GL_MAX_PROGRAM_NATIVE_ATTRIBS,
   * GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS, GL_MAX_PROGRAM_NATIVE_PARAMETERS,
   * GL_MAX_PROGRAM_NATIVE_TEMPORARIES, GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS,
   * GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS, GL_MAX_PROGRAM_PARAMETERS, GL_MAX_PROGRAM_TEMPORARIES,
   * GL_MAX_PROGRAM_TEX_INDIRECTIONS, GL_MAX_PROGRAM_TEX_INSTRUCTIONS, GL_MAX_VERTEX_UNITS,
   * GL_MAX_VERTEX_VARYING_COMPONENTS, GL_MODELVIEW0, GL_MODELVIEW1, GL_MODELVIEW10, GL_MODELVIEW11,
   * GL_MODELVIEW12, GL_MODELVIEW13, GL_MODELVIEW14, GL_MODELVIEW15, GL_MODELVIEW16, GL_MODELVIEW17,
   * GL_MODELVIEW18, GL_MODELVIEW19, GL_MODELVIEW2, GL_MODELVIEW20, GL_MODELVIEW21, GL_MODELVIEW22,
   * GL_MODELVIEW23, GL_MODELVIEW24, GL_MODELVIEW25, GL_MODELVIEW26, GL_MODELVIEW27, GL_MODELVIEW28,
   * GL_MODELVIEW29, GL_MODELVIEW3, GL_MODELVIEW30, GL_MODELVIEW31, GL_MODELVIEW4, GL_MODELVIEW5,
   * GL_MODELVIEW6, GL_MODELVIEW7, GL_MODELVIEW8, GL_MODELVIEW9, GL_NAMED_STRING_LENGTH,
   * GL_NAMED_STRING_TYPE, GL_NO_RESET_NOTIFICATION, GL_OBJECT_ACTIVE_ATTRIBUTES,
   * GL_OBJECT_ACTIVE_ATTRIBUTE_MAX_LENGTH, GL_OBJECT_ACTIVE_UNIFORMS,
   * GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH, GL_OBJECT_ATTACHED_OBJECTS, GL_OBJECT_COMPILE_STATUS,
   * GL_OBJECT_DELETE_STATUS, GL_OBJECT_INFO_LOG_LENGTH, GL_OBJECT_LINK_STATUS,
   * GL_OBJECT_SHADER_SOURCE_LENGTH, GL_OBJECT_SUBTYPE, GL_OBJECT_VALIDATE_STATUS,
   * GL_PROGRAM_ADDRESS_REGISTERS, GL_PROGRAM_ALU_INSTRUCTIONS, GL_PROGRAM_ATTRIBS,
   * GL_PROGRAM_BINDING, GL_PROGRAM_ERROR_POSITION, GL_PROGRAM_ERROR_STRING, GL_PROGRAM_FORMAT,
   * GL_PROGRAM_FORMAT_ASCII, GL_PROGRAM_INSTRUCTIONS, GL_PROGRAM_LENGTH,
   * GL_PROGRAM_NATIVE_ADDRESS_REGISTERS, GL_PROGRAM_NATIVE_ALU_INSTRUCTIONS,
   * GL_PROGRAM_NATIVE_ATTRIBS, GL_PROGRAM_NATIVE_INSTRUCTIONS, GL_PROGRAM_NATIVE_PARAMETERS,
   * GL_PROGRAM_NATIVE_TEMPORARIES, GL_PROGRAM_NATIVE_TEX_INDIRECTIONS,
   * GL_PROGRAM_NATIVE_TEX_INSTRUCTIONS, GL_PROGRAM_OBJECT, GL_PROGRAM_PARAMETERS,
   * GL_PROGRAM_STRING, GL_PROGRAM_TEMPORARIES, GL_PROGRAM_TEX_INDIRECTIONS,
   * GL_PROGRAM_TEX_INSTRUCTIONS, GL_PROGRAM_UNDER_NATIVE_LIMITS, GL_RESET_NOTIFICATION_STRATEGY,
   * GL_RGBA_FLOAT_MODE, GL_SHADER_INCLUDE, GL_SHADER_OBJECT, GL_SYNC_CL_EVENT,
   * GL_SYNC_CL_EVENT_COMPLETE, GL_TEXTURE_COMPARE_FAIL_VALUE, GL_TRANSPOSE_CURRENT_MATRIX,
   * GL_UNKNOWN_CONTEXT_RESET, GL_VERTEX_BLEND, GL_VERTEX_PROGRAM, GL_WEIGHT_ARRAY,
   * GL_WEIGHT_ARRAY_POINTER, GL_WEIGHT_ARRAY_SIZE, GL_WEIGHT_ARRAY_STRIDE, GL_WEIGHT_ARRAY_TYPE,
   * GL_WEIGHT_SUM_UNITY
   */

  if (!glActiveTexture) glActiveTexture = glActiveTextureARB;
  if (!glAttachShader) glAttachShader = glAttachObjectARB;
  if (!glBeginQuery) glBeginQuery = glBeginQueryARB;
  if (!glBindAttribLocation) glBindAttribLocation = glBindAttribLocationARB;
  if (!glBindBuffer) glBindBuffer = glBindBufferARB;
  // if (!glBindProgram) glBindProgram = glBindProgramARB;
  if (!glBlendEquationSeparatei) glBlendEquationSeparatei = glBlendEquationSeparateiARB;
  if (!glBlendEquationi) glBlendEquationi = glBlendEquationiARB;
  if (!glBlendFuncSeparatei) glBlendFuncSeparatei = glBlendFuncSeparateiARB;
  if (!glBlendFunci) glBlendFunci = glBlendFunciARB;
  if (!glBufferData) glBufferData = glBufferDataARB;
  if (!glBufferSubData) glBufferSubData = glBufferSubDataARB;
  if (!glClampColor) glClampColor = glClampColorARB;
  if (!glClientActiveTexture) glClientActiveTexture = glClientActiveTextureARB;
  if (!glCompileShader) glCompileShader = glCompileShaderARB;
  // if (!glCompileShaderInclude) glCompileShaderInclude = glCompileShaderIncludeARB;
  if (!glCompressedTexImage1D) glCompressedTexImage1D = glCompressedTexImage1DARB;
  if (!glCompressedTexImage2D) glCompressedTexImage2D = glCompressedTexImage2DARB;
  if (!glCompressedTexImage3D) glCompressedTexImage3D = glCompressedTexImage3DARB;
  if (!glCompressedTexSubImage1D) glCompressedTexSubImage1D = glCompressedTexSubImage1DARB;
  if (!glCompressedTexSubImage2D) glCompressedTexSubImage2D = glCompressedTexSubImage2DARB;
  if (!glCompressedTexSubImage3D) glCompressedTexSubImage3D = glCompressedTexSubImage3DARB;
  if (!glCreateProgram) glCreateProgram = glCreateProgramObjectARB;
  if (!glCreateShader) glCreateShader = glCreateShaderObjectARB;
  // if (!glCreateSyncFromCLevent) glCreateSyncFromCLevent = glCreateSyncFromCLeventARB;
  // if (!glCurrentPaletteMatrix) glCurrentPaletteMatrix = glCurrentPaletteMatrixARB;
  // if (!glDebugMessageCallback) glDebugMessageCallback = glDebugMessageCallbackARB;
  // if (!glDebugMessageControl) glDebugMessageControl = glDebugMessageControlARB;
  // if (!glDebugMessageInsert) glDebugMessageInsert = glDebugMessageInsertARB;
  if (!glDeleteBuffers) glDeleteBuffers = glDeleteBuffersARB;
  // if (!glDeleteNamedString) glDeleteNamedString = glDeleteNamedStringARB;
  if (!glDeleteProgram) glDeleteProgram = glDeleteObjectARB;
  if (!glDeleteShader) glDeleteShader = glDeleteObjectARB;
  if (!glDeleteQueries) glDeleteQueries = glDeleteQueriesARB;
  if (!glDetachShader) glDetachShader = glDetachObjectARB;
  if (!glDisableVertexAttribArray) glDisableVertexAttribArray = glDisableVertexAttribArrayARB;
  if (!glDrawArraysInstanced) glDrawArraysInstanced = glDrawArraysInstancedARB;
  if (!glDrawBuffers) glDrawBuffers = glDrawBuffersARB;
  if (!glDrawElementsInstanced) glDrawElementsInstanced = glDrawElementsInstancedARB;
  if (!glEnableVertexAttribArray) glEnableVertexAttribArray = glEnableVertexAttribArrayARB;
  if (!glEndQuery) glEndQuery = glEndQueryARB;
  if (!glFramebufferTexture) glFramebufferTexture = glFramebufferTextureARB;
  // if (!glFramebufferTextureFace) glFramebufferTextureFace = glFramebufferTextureFaceARB;
  if (!glFramebufferTextureLayer) glFramebufferTextureLayer = glFramebufferTextureLayerARB;
  if (!glGenBuffers) glGenBuffers = glGenBuffersARB;
  // if (!glGenPrograms) glGenPrograms = glGenProgramsARB;
  if (!glGenQueries) glGenQueries = glGenQueriesARB;
  if (!glGetActiveAttrib) glGetActiveAttrib = glGetActiveAttribARB;
  if (!glGetActiveUniform) glGetActiveUniform = glGetActiveUniformARB;
  if (!glGetAttachedShaders) glGetAttachedShaders = glGetAttachedObjectsARB;
  if (!glGetAttribLocation) glGetAttribLocation = glGetAttribLocationARB;
  if (!glGetBufferParameteriv) glGetBufferParameteriv = glGetBufferParameterivARB;
  if (!glGetBufferPointerv) glGetBufferPointerv = glGetBufferPointervARB;
  if (!glGetBufferSubData) glGetBufferSubData = glGetBufferSubDataARB;
  if (!glGetCompressedTexImage) glGetCompressedTexImage = glGetCompressedTexImageARB;
  // if (!glGetDebugMessageLog) glGetDebugMessageLog = glGetDebugMessageLogARB;
  // if (!glGetHandle) glGetHandle = glGetHandleARB;
  if (!glGetShaderInfoLog) glGetShaderInfoLog = glGetInfoLogARB;
  if (!glGetProgramInfoLog) glGetProgramInfoLog = glGetInfoLogARB;
  // if (!glGetNamedString) glGetNamedString = glGetNamedStringARB;
  // if (!glGetNamedStringiv) glGetNamedStringiv = glGetNamedStringivARB;
  // if (!glGetObjectParameterfv) glGetObjectParameterfv = glGetObjectParameterfvARB;
  if (!glGetShaderiv) glGetShaderiv = glGetObjectParameterivARB;
  if (!glGetProgramiv) glGetProgramiv = glGetObjectParameterivARB;
  // if (!glGetProgramEnvParameterdv) glGetProgramEnvParameterdv = glGetProgramEnvParameterdvARB;
  // if (!glGetProgramEnvParameterfv) glGetProgramEnvParameterfv = glGetProgramEnvParameterfvARB;
  // if (!glGetProgramLocalParameterdv) glGetProgramLocalParameterdv = glGetProgramLocalParameterdvARB;
  // if (!glGetProgramLocalParameterfv) glGetProgramLocalParameterfv = glGetProgramLocalParameterfvARB;
  // if (!glGetProgramString) glGetProgramString = glGetProgramStringARB;
  if (!glGetProgramiv) glGetProgramiv = glGetProgramivARB;
  if (!glGetQueryObjectiv) glGetQueryObjectiv = glGetQueryObjectivARB;
  if (!glGetQueryObjectuiv) glGetQueryObjectuiv = glGetQueryObjectuivARB;
  if (!glGetQueryiv) glGetQueryiv = glGetQueryivARB;
  if (!glGetShaderSource) glGetShaderSource = glGetShaderSourceARB;
  if (!glGetUniformLocation) glGetUniformLocation = glGetUniformLocationARB;
  if (!glGetUniformfv) glGetUniformfv = glGetUniformfvARB;
  if (!glGetUniformiv) glGetUniformiv = glGetUniformivARB;
  if (!glGetVertexAttribPointerv) glGetVertexAttribPointerv = glGetVertexAttribPointervARB;
  if (!glGetVertexAttribdv) glGetVertexAttribdv = glGetVertexAttribdvARB;
  if (!glGetVertexAttribfv) glGetVertexAttribfv = glGetVertexAttribfvARB;
  if (!glGetVertexAttribiv) glGetVertexAttribiv = glGetVertexAttribivARB;
  // if (!glGetnColorTable) glGetnColorTable = glGetnColorTableARB;
  // if (!glGetnCompressedTexImage) glGetnCompressedTexImage = glGetnCompressedTexImageARB;
  // if (!glGetnConvolutionFilter) glGetnConvolutionFilter = glGetnConvolutionFilterARB;
  // if (!glGetnHistogram) glGetnHistogram = glGetnHistogramARB;
  // if (!glGetnMapdv) glGetnMapdv = glGetnMapdvARB;
  // if (!glGetnMapfv) glGetnMapfv = glGetnMapfvARB;
  // if (!glGetnMapiv) glGetnMapiv = glGetnMapivARB;
  // if (!glGetnMinmax) glGetnMinmax = glGetnMinmaxARB;
  // if (!glGetnPixelMapfv) glGetnPixelMapfv = glGetnPixelMapfvARB;
  // if (!glGetnPixelMapuiv) glGetnPixelMapuiv = glGetnPixelMapuivARB;
  // if (!glGetnPixelMapusv) glGetnPixelMapusv = glGetnPixelMapusvARB;
  // if (!glGetnPolygonStipple) glGetnPolygonStipple = glGetnPolygonStippleARB;
  // if (!glGetnSeparableFilter) glGetnSeparableFilter = glGetnSeparableFilterARB;
  // if (!glGetnTexImage) glGetnTexImage = glGetnTexImageARB;
  // if (!glGetnUniformdv) glGetnUniformdv = glGetnUniformdvARB;
  // if (!glGetnUniformfv) glGetnUniformfv = glGetnUniformfvARB;
  // if (!glGetnUniformiv) glGetnUniformiv = glGetnUniformivARB;
  // if (!glGetnUniformuiv) glGetnUniformuiv = glGetnUniformuivARB;
  if (!glIsBuffer) glIsBuffer = glIsBufferARB;
  // if (!glIsNamedString) glIsNamedString = glIsNamedStringARB;
  if (!glIsProgram) glIsProgram = glIsProgramARB;
  if (!glIsQuery) glIsQuery = glIsQueryARB;
  if (!glLinkProgram) glLinkProgram = glLinkProgramARB;
  if (!glLoadTransposeMatrixd) glLoadTransposeMatrixd = glLoadTransposeMatrixdARB;
  if (!glLoadTransposeMatrixf) glLoadTransposeMatrixf = glLoadTransposeMatrixfARB;
  if (!glMapBuffer) glMapBuffer = glMapBufferARB;
  // if (!glMatrixIndexPointer) glMatrixIndexPointer = glMatrixIndexPointerARB;
  // if (!glMatrixIndexubv) glMatrixIndexubv = glMatrixIndexubvARB;
  // if (!glMatrixIndexuiv) glMatrixIndexuiv = glMatrixIndexuivARB;
  // if (!glMatrixIndexusv) glMatrixIndexusv = glMatrixIndexusvARB;
  if (!glMinSampleShading) glMinSampleShading = glMinSampleShadingARB;
  if (!glMultTransposeMatrixd) glMultTransposeMatrixd = glMultTransposeMatrixdARB;
  if (!glMultTransposeMatrixf) glMultTransposeMatrixf = glMultTransposeMatrixfARB;
  if (!glMultiTexCoord1d) glMultiTexCoord1d = glMultiTexCoord1dARB;
  if (!glMultiTexCoord1dv) glMultiTexCoord1dv = glMultiTexCoord1dvARB;
  if (!glMultiTexCoord1f) glMultiTexCoord1f = glMultiTexCoord1fARB;
  if (!glMultiTexCoord1fv) glMultiTexCoord1fv = glMultiTexCoord1fvARB;
  if (!glMultiTexCoord1i) glMultiTexCoord1i = glMultiTexCoord1iARB;
  if (!glMultiTexCoord1iv) glMultiTexCoord1iv = glMultiTexCoord1ivARB;
  if (!glMultiTexCoord1s) glMultiTexCoord1s = glMultiTexCoord1sARB;
  if (!glMultiTexCoord1sv) glMultiTexCoord1sv = glMultiTexCoord1svARB;
  if (!glMultiTexCoord2d) glMultiTexCoord2d = glMultiTexCoord2dARB;
  if (!glMultiTexCoord2dv) glMultiTexCoord2dv = glMultiTexCoord2dvARB;
  if (!glMultiTexCoord2f) glMultiTexCoord2f = glMultiTexCoord2fARB;
  if (!glMultiTexCoord2fv) glMultiTexCoord2fv = glMultiTexCoord2fvARB;
  if (!glMultiTexCoord2i) glMultiTexCoord2i = glMultiTexCoord2iARB;
  if (!glMultiTexCoord2iv) glMultiTexCoord2iv = glMultiTexCoord2ivARB;
  if (!glMultiTexCoord2s) glMultiTexCoord2s = glMultiTexCoord2sARB;
  if (!glMultiTexCoord2sv) glMultiTexCoord2sv = glMultiTexCoord2svARB;
  if (!glMultiTexCoord3d) glMultiTexCoord3d = glMultiTexCoord3dARB;
  if (!glMultiTexCoord3dv) glMultiTexCoord3dv = glMultiTexCoord3dvARB;
  if (!glMultiTexCoord3f) glMultiTexCoord3f = glMultiTexCoord3fARB;
  if (!glMultiTexCoord3fv) glMultiTexCoord3fv = glMultiTexCoord3fvARB;
  if (!glMultiTexCoord3i) glMultiTexCoord3i = glMultiTexCoord3iARB;
  if (!glMultiTexCoord3iv) glMultiTexCoord3iv = glMultiTexCoord3ivARB;
  if (!glMultiTexCoord3s) glMultiTexCoord3s = glMultiTexCoord3sARB;
  if (!glMultiTexCoord3sv) glMultiTexCoord3sv = glMultiTexCoord3svARB;
  if (!glMultiTexCoord4d) glMultiTexCoord4d = glMultiTexCoord4dARB;
  if (!glMultiTexCoord4dv) glMultiTexCoord4dv = glMultiTexCoord4dvARB;
  if (!glMultiTexCoord4f) glMultiTexCoord4f = glMultiTexCoord4fARB;
  if (!glMultiTexCoord4fv) glMultiTexCoord4fv = glMultiTexCoord4fvARB;
  if (!glMultiTexCoord4i) glMultiTexCoord4i = glMultiTexCoord4iARB;
  if (!glMultiTexCoord4iv) glMultiTexCoord4iv = glMultiTexCoord4ivARB;
  if (!glMultiTexCoord4s) glMultiTexCoord4s = glMultiTexCoord4sARB;
  if (!glMultiTexCoord4sv) glMultiTexCoord4sv = glMultiTexCoord4svARB;
  // if (!glNamedString) glNamedString = glNamedStringARB;
  if (!glPointParameterf) glPointParameterf = glPointParameterfARB;
  if (!glPointParameterfv) glPointParameterfv = glPointParameterfvARB;
  // if (!glProgramEnvParameter4d) glProgramEnvParameter4d = glProgramEnvParameter4dARB;
  // if (!glProgramEnvParameter4dv) glProgramEnvParameter4dv = glProgramEnvParameter4dvARB;
  // if (!glProgramEnvParameter4f) glProgramEnvParameter4f = glProgramEnvParameter4fARB;
  // if (!glProgramEnvParameter4fv) glProgramEnvParameter4fv = glProgramEnvParameter4fvARB;
  // if (!glProgramLocalParameter4d) glProgramLocalParameter4d = glProgramLocalParameter4dARB;
  // if (!glProgramLocalParameter4dv) glProgramLocalParameter4dv = glProgramLocalParameter4dvARB;
  // if (!glProgramLocalParameter4f) glProgramLocalParameter4f = glProgramLocalParameter4fARB;
  // if (!glProgramLocalParameter4fv) glProgramLocalParameter4fv = glProgramLocalParameter4fvARB;
  if (!glProgramParameteri) glProgramParameteri = glProgramParameteriARB;
  // if (!glProgramString) glProgramString = glProgramStringARB;
  // if (!glReadnPixels) glReadnPixels = glReadnPixelsARB;
  if (!glSampleCoverage) glSampleCoverage = glSampleCoverageARB;
  if (!glShaderSource) glShaderSource = glShaderSourceARB;
  if (!glTexBuffer) glTexBuffer = glTexBufferARB;
  if (!glUniform1f) glUniform1f = glUniform1fARB;
  if (!glUniform1fv) glUniform1fv = glUniform1fvARB;
  if (!glUniform1i) glUniform1i = glUniform1iARB;
  if (!glUniform1iv) glUniform1iv = glUniform1ivARB;
  if (!glUniform2f) glUniform2f = glUniform2fARB;
  if (!glUniform2fv) glUniform2fv = glUniform2fvARB;
  if (!glUniform2i) glUniform2i = glUniform2iARB;
  if (!glUniform2iv) glUniform2iv = glUniform2ivARB;
  if (!glUniform3f) glUniform3f = glUniform3fARB;
  if (!glUniform3fv) glUniform3fv = glUniform3fvARB;
  if (!glUniform3i) glUniform3i = glUniform3iARB;
  if (!glUniform3iv) glUniform3iv = glUniform3ivARB;
  if (!glUniform4f) glUniform4f = glUniform4fARB;
  if (!glUniform4fv) glUniform4fv = glUniform4fvARB;
  if (!glUniform4i) glUniform4i = glUniform4iARB;
  if (!glUniform4iv) glUniform4iv = glUniform4ivARB;
  if (!glUniformMatrix2fv) glUniformMatrix2fv = glUniformMatrix2fvARB;
  if (!glUniformMatrix3fv) glUniformMatrix3fv = glUniformMatrix3fvARB;
  if (!glUniformMatrix4fv) glUniformMatrix4fv = glUniformMatrix4fvARB;
  if (!glUnmapBuffer) glUnmapBuffer = glUnmapBufferARB;
  if (!glUseProgram) glUseProgram = glUseProgramObjectARB;
  if (!glValidateProgram) glValidateProgram = glValidateProgramARB;
  if (!glVertexAttrib1d) glVertexAttrib1d = glVertexAttrib1dARB;
  if (!glVertexAttrib1dv) glVertexAttrib1dv = glVertexAttrib1dvARB;
  if (!glVertexAttrib1f) glVertexAttrib1f = glVertexAttrib1fARB;
  if (!glVertexAttrib1fv) glVertexAttrib1fv = glVertexAttrib1fvARB;
  if (!glVertexAttrib1s) glVertexAttrib1s = glVertexAttrib1sARB;
  if (!glVertexAttrib1sv) glVertexAttrib1sv = glVertexAttrib1svARB;
  if (!glVertexAttrib2d) glVertexAttrib2d = glVertexAttrib2dARB;
  if (!glVertexAttrib2dv) glVertexAttrib2dv = glVertexAttrib2dvARB;
  if (!glVertexAttrib2f) glVertexAttrib2f = glVertexAttrib2fARB;
  if (!glVertexAttrib2fv) glVertexAttrib2fv = glVertexAttrib2fvARB;
  if (!glVertexAttrib2s) glVertexAttrib2s = glVertexAttrib2sARB;
  if (!glVertexAttrib2sv) glVertexAttrib2sv = glVertexAttrib2svARB;
  if (!glVertexAttrib3d) glVertexAttrib3d = glVertexAttrib3dARB;
  if (!glVertexAttrib3dv) glVertexAttrib3dv = glVertexAttrib3dvARB;
  if (!glVertexAttrib3f) glVertexAttrib3f = glVertexAttrib3fARB;
  if (!glVertexAttrib3fv) glVertexAttrib3fv = glVertexAttrib3fvARB;
  if (!glVertexAttrib3s) glVertexAttrib3s = glVertexAttrib3sARB;
  if (!glVertexAttrib3sv) glVertexAttrib3sv = glVertexAttrib3svARB;
  if (!glVertexAttrib4Nbv) glVertexAttrib4Nbv = glVertexAttrib4NbvARB;
  if (!glVertexAttrib4Niv) glVertexAttrib4Niv = glVertexAttrib4NivARB;
  if (!glVertexAttrib4Nsv) glVertexAttrib4Nsv = glVertexAttrib4NsvARB;
  if (!glVertexAttrib4Nub) glVertexAttrib4Nub = glVertexAttrib4NubARB;
  if (!glVertexAttrib4Nubv) glVertexAttrib4Nubv = glVertexAttrib4NubvARB;
  if (!glVertexAttrib4Nuiv) glVertexAttrib4Nuiv = glVertexAttrib4NuivARB;
  if (!glVertexAttrib4Nusv) glVertexAttrib4Nusv = glVertexAttrib4NusvARB;
  if (!glVertexAttrib4bv) glVertexAttrib4bv = glVertexAttrib4bvARB;
  if (!glVertexAttrib4d) glVertexAttrib4d = glVertexAttrib4dARB;
  if (!glVertexAttrib4dv) glVertexAttrib4dv = glVertexAttrib4dvARB;
  if (!glVertexAttrib4f) glVertexAttrib4f = glVertexAttrib4fARB;
  if (!glVertexAttrib4fv) glVertexAttrib4fv = glVertexAttrib4fvARB;
  if (!glVertexAttrib4iv) glVertexAttrib4iv = glVertexAttrib4ivARB;
  if (!glVertexAttrib4s) glVertexAttrib4s = glVertexAttrib4sARB;
  if (!glVertexAttrib4sv) glVertexAttrib4sv = glVertexAttrib4svARB;
  if (!glVertexAttrib4ubv) glVertexAttrib4ubv = glVertexAttrib4ubvARB;
  if (!glVertexAttrib4uiv) glVertexAttrib4uiv = glVertexAttrib4uivARB;
  if (!glVertexAttrib4usv) glVertexAttrib4usv = glVertexAttrib4usvARB;
  if (!glVertexAttribDivisor) glVertexAttribDivisor = glVertexAttribDivisorARB;
  if (!glVertexAttribPointer) glVertexAttribPointer = glVertexAttribPointerARB;
  // if (!glVertexBlend) glVertexBlend = glVertexBlendARB;
  // if (!glWeightPointer) glWeightPointer = glWeightPointerARB;
  // if (!glWeightbv) glWeightbv = glWeightbvARB;
  // if (!glWeightdv) glWeightdv = glWeightdvARB;
  // if (!glWeightfv) glWeightfv = glWeightfvARB;
  // if (!glWeightiv) glWeightiv = glWeightivARB;
  // if (!glWeightsv) glWeightsv = glWeightsvARB;
  // if (!glWeightubv) glWeightubv = glWeightubvARB;
  // if (!glWeightuiv) glWeightuiv = glWeightuivARB;
  // if (!glWeightusv) glWeightusv = glWeightusvARB;
  if (!glWindowPos2d) glWindowPos2d = glWindowPos2dARB;
  if (!glWindowPos2dv) glWindowPos2dv = glWindowPos2dvARB;
  if (!glWindowPos2f) glWindowPos2f = glWindowPos2fARB;
  if (!glWindowPos2fv) glWindowPos2fv = glWindowPos2fvARB;
  if (!glWindowPos2i) glWindowPos2i = glWindowPos2iARB;
  if (!glWindowPos2iv) glWindowPos2iv = glWindowPos2ivARB;
  if (!glWindowPos2s) glWindowPos2s = glWindowPos2sARB;
  if (!glWindowPos2sv) glWindowPos2sv = glWindowPos2svARB;
  if (!glWindowPos3d) glWindowPos3d = glWindowPos3dARB;
  if (!glWindowPos3dv) glWindowPos3dv = glWindowPos3dvARB;
  if (!glWindowPos3f) glWindowPos3f = glWindowPos3fARB;
  if (!glWindowPos3fv) glWindowPos3fv = glWindowPos3fvARB;
  if (!glWindowPos3i) glWindowPos3i = glWindowPos3iARB;
  if (!glWindowPos3iv) glWindowPos3iv = glWindowPos3ivARB;
  if (!glWindowPos3s) glWindowPos3s = glWindowPos3sARB;
  if (!glWindowPos3sv) glWindowPos3sv = glWindowPos3svARB;

/*
  http://developer.download.nvidia.com/opengl/specs/GL_EXT_separate_shader_objects.txt
  This extension redefines the operation of glUseProgram(GLenum program)
  to be equivalent to:

        glUseShaderProgramEXT(GL_VERTEX_SHADER, program);
        glUseShaderProgramEXT(GL_GEOMETRY_SHADER_EXT, program);
        glUseShaderProgramEXT(GL_FRAGMENT_SHADER, program);
        glActiveProgramEXT(program);

*/
  // if (!glActiveProgram) glActiveProgram = glActiveProgramEXT;
  // if (!glActiveStencilFace) glActiveStencilFace = glActiveStencilFaceEXT;
  // if (!glApplyTexture) glApplyTexture = glApplyTextureEXT;
  // if (!glAreTexturesResident) glAreTexturesResident = glAreTexturesResidentEXT;
  // if (!glArrayElement) glArrayElement = glArrayElementEXT;
  // if (!glBeginScene) glBeginScene = glBeginSceneEXT;
  if (!glBeginTransformFeedback) glBeginTransformFeedback = glBeginTransformFeedbackEXT;
  // if (!glBeginVertexShader) glBeginVertexShader = glBeginVertexShaderEXT;
  if (!glBindBufferBase) glBindBufferBase = glBindBufferBaseEXT;
  // if (!glBindBufferOffset) glBindBufferOffset = glBindBufferOffsetEXT;
  if (!glBindBufferRange) glBindBufferRange = glBindBufferRangeEXT;
  if (!glBindFragDataLocation) glBindFragDataLocation = glBindFragDataLocationEXT;
  if (!glBindFramebuffer) glBindFramebuffer = glBindFramebufferEXT;
  // if (!glBindImageTexture) glBindImageTexture = glBindImageTextureEXT;
  // if (!glBindLightParameter) glBindLightParameter = glBindLightParameterEXT;
  // if (!glBindMaterialParameter) glBindMaterialParameter = glBindMaterialParameterEXT;
  // if (!glBindMultiTexture) glBindMultiTexture = glBindMultiTextureEXT;
  // if (!glBindParameter) glBindParameter = glBindParameterEXT;
  if (!glBindRenderbuffer) glBindRenderbuffer = glBindRenderbufferEXT;
  // if (!glBindTexGenParameter) glBindTexGenParameter = glBindTexGenParameterEXT;
  // if (!glBindTexture) glBindTexture = glBindTextureEXT;
  // if (!glBindTextureUnitParameter) glBindTextureUnitParameter = glBindTextureUnitParameterEXT;
  // if (!glBindVertexShader) glBindVertexShader = glBindVertexShaderEXT;
  // if (!glBinormalPointer) glBinormalPointer = glBinormalPointerEXT;
  if (!glBlendColor) glBlendColor = glBlendColorEXT;
  if (!glBlendEquation) glBlendEquation = glBlendEquationEXT;
  if (!glBlendEquationSeparate) glBlendEquationSeparate = glBlendEquationSeparateEXT;
  if (!glBlendFuncSeparate) glBlendFuncSeparate = glBlendFuncSeparateEXT;
  if (!glBlitFramebuffer) glBlitFramebuffer = glBlitFramebufferEXT;
  // if (!glBufferRegionEnabled) glBufferRegionEnabled = glBufferRegionEnabledEXT;
  if (!glCheckFramebufferStatus) glCheckFramebufferStatus = glCheckFramebufferStatusEXT;
  // if (!glCheckNamedFramebufferStatus) glCheckNamedFramebufferStatus = glCheckNamedFramebufferStatusEXT;
  // if (!glClearColorIi) glClearColorIi = glClearColorIiEXT;
  // if (!glClearColorIui) glClearColorIui = glClearColorIuiEXT;
  // if (!glClientAttribDefault) glClientAttribDefault = glClientAttribDefaultEXT;
  // if (!glColorMaskIndexed) glColorMaskIndexed = glColorMaskIndexedEXT;
  // if (!glColorPointer) glColorPointer = glColorPointerEXT;
  if (!glColorSubTable) glColorSubTable = glColorSubTableEXT;
  if (!glColorTable) glColorTable = glColorTableEXT;
  // if (!glCompressedMultiTexImage1D) glCompressedMultiTexImage1D = glCompressedMultiTexImage1DEXT;
  // if (!glCompressedMultiTexImage2D) glCompressedMultiTexImage2D = glCompressedMultiTexImage2DEXT;
  // if (!glCompressedMultiTexImage3D) glCompressedMultiTexImage3D = glCompressedMultiTexImage3DEXT;
  // if (!glCompressedMultiTexSubImage1D) glCompressedMultiTexSubImage1D = glCompressedMultiTexSubImage1DEXT;
  // if (!glCompressedMultiTexSubImage2D) glCompressedMultiTexSubImage2D = glCompressedMultiTexSubImage2DEXT;
  // if (!glCompressedMultiTexSubImage3D) glCompressedMultiTexSubImage3D = glCompressedMultiTexSubImage3DEXT;
  // if (!glCompressedTextureImage1D) glCompressedTextureImage1D = glCompressedTextureImage1DEXT;
  // if (!glCompressedTextureImage2D) glCompressedTextureImage2D = glCompressedTextureImage2DEXT;
  // if (!glCompressedTextureImage3D) glCompressedTextureImage3D = glCompressedTextureImage3DEXT;
  // if (!glCompressedTextureSubImage1D) glCompressedTextureSubImage1D = glCompressedTextureSubImage1DEXT;
  // if (!glCompressedTextureSubImage2D) glCompressedTextureSubImage2D = glCompressedTextureSubImage2DEXT;
  // if (!glCompressedTextureSubImage3D) glCompressedTextureSubImage3D = glCompressedTextureSubImage3DEXT;
  if (!glConvolutionFilter1D) glConvolutionFilter1D = glConvolutionFilter1DEXT;
  if (!glConvolutionFilter2D) glConvolutionFilter2D = glConvolutionFilter2DEXT;
  if (!glConvolutionParameterf) glConvolutionParameterf = glConvolutionParameterfEXT;
  if (!glConvolutionParameterfv) glConvolutionParameterfv = glConvolutionParameterfvEXT;
  if (!glConvolutionParameteri) glConvolutionParameteri = glConvolutionParameteriEXT;
  if (!glConvolutionParameteriv) glConvolutionParameteriv = glConvolutionParameterivEXT;
  if (!glCopyColorSubTable) glCopyColorSubTable = glCopyColorSubTableEXT;
  if (!glCopyConvolutionFilter1D) glCopyConvolutionFilter1D = glCopyConvolutionFilter1DEXT;
  if (!glCopyConvolutionFilter2D) glCopyConvolutionFilter2D = glCopyConvolutionFilter2DEXT;
  // if (!glCopyMultiTexImage1D) glCopyMultiTexImage1D = glCopyMultiTexImage1DEXT;
  // if (!glCopyMultiTexImage2D) glCopyMultiTexImage2D = glCopyMultiTexImage2DEXT;
  // if (!glCopyMultiTexSubImage1D) glCopyMultiTexSubImage1D = glCopyMultiTexSubImage1DEXT;
  // if (!glCopyMultiTexSubImage2D) glCopyMultiTexSubImage2D = glCopyMultiTexSubImage2DEXT;
  // if (!glCopyMultiTexSubImage3D) glCopyMultiTexSubImage3D = glCopyMultiTexSubImage3DEXT;
  // if (!glCopyTexImage1D) glCopyTexImage1D = glCopyTexImage1DEXT;
  // if (!glCopyTexImage2D) glCopyTexImage2D = glCopyTexImage2DEXT;
  // if (!glCopyTexSubImage1D) glCopyTexSubImage1D = glCopyTexSubImage1DEXT;
  // if (!glCopyTexSubImage2D) glCopyTexSubImage2D = glCopyTexSubImage2DEXT;
  if (!glCopyTexSubImage3D) glCopyTexSubImage3D = glCopyTexSubImage3DEXT;
  // if (!glCopyTextureImage1D) glCopyTextureImage1D = glCopyTextureImage1DEXT;
  // if (!glCopyTextureImage2D) glCopyTextureImage2D = glCopyTextureImage2DEXT;
  // if (!glCopyTextureSubImage1D) glCopyTextureSubImage1D = glCopyTextureSubImage1DEXT;
  // if (!glCopyTextureSubImage2D) glCopyTextureSubImage2D = glCopyTextureSubImage2DEXT;
  // if (!glCopyTextureSubImage3D) glCopyTextureSubImage3D = glCopyTextureSubImage3DEXT;
  // if (!glCreateShaderProgram) glCreateShaderProgram = glCreateShaderProgramEXT;
  // if (!glCullParameterdv) glCullParameterdv = glCullParameterdvEXT;
  // if (!glCullParameterfv) glCullParameterfv = glCullParameterfvEXT;
  // if (!glDeleteBufferRegion) glDeleteBufferRegion = glDeleteBufferRegionEXT;
  if (!glDeleteFramebuffers) glDeleteFramebuffers = glDeleteFramebuffersEXT;
  if (!glDeleteRenderbuffers) glDeleteRenderbuffers = glDeleteRenderbuffersEXT;
  // if (!glDeleteTextures) glDeleteTextures = glDeleteTexturesEXT;
  // if (!glDeleteVertexShader) glDeleteVertexShader = glDeleteVertexShaderEXT;
  // if (!glDepthBounds) glDepthBounds = glDepthBoundsEXT;
  // if (!glDisableClientStateIndexed) glDisableClientStateIndexed = glDisableClientStateIndexedEXT;
  // if (!glDisableClientStatei) glDisableClientStatei = glDisableClientStateiEXT;
  // if (!glDisableIndexed) glDisableIndexed = glDisableIndexedEXT;
  // if (!glDisableVariantClientState) glDisableVariantClientState = glDisableVariantClientStateEXT;
  // if (!glDisableVertexArrayAttrib) glDisableVertexArrayAttrib = glDisableVertexArrayAttribEXT;
  // if (!glDisableVertexArray) glDisableVertexArray = glDisableVertexArrayEXT;
  // if (!glDrawArrays) glDrawArrays = glDrawArraysEXT;
  if (!glDrawArraysInstanced) glDrawArraysInstanced = glDrawArraysInstancedEXT;
  // if (!glDrawBufferRegion) glDrawBufferRegion = glDrawBufferRegionEXT;
  if (!glDrawElementsInstanced) glDrawElementsInstanced = glDrawElementsInstancedEXT;
  if (!glDrawRangeElements) glDrawRangeElements = glDrawRangeElementsEXT;
  // if (!glEdgeFlagPointer) glEdgeFlagPointer = glEdgeFlagPointerEXT;
  // if (!glEnableClientStateIndexed) glEnableClientStateIndexed = glEnableClientStateIndexedEXT;
  // if (!glEnableClientStatei) glEnableClientStatei = glEnableClientStateiEXT;
  // if (!glEnableIndexed) glEnableIndexed = glEnableIndexedEXT;
  // if (!glEnableVariantClientState) glEnableVariantClientState = glEnableVariantClientStateEXT;
  // if (!glEnableVertexArrayAttrib) glEnableVertexArrayAttrib = glEnableVertexArrayAttribEXT;
  // if (!glEnableVertexArray) glEnableVertexArray = glEnableVertexArrayEXT;
  // if (!glEndScene) glEndScene = glEndSceneEXT;
  if (!glEndTransformFeedback) glEndTransformFeedback = glEndTransformFeedbackEXT;
  // if (!glEndVertexShader) glEndVertexShader = glEndVertexShaderEXT;
  // if (!glExtractComponent) glExtractComponent = glExtractComponentEXT;
  // if (!glFlushMappedNamedBufferRange) glFlushMappedNamedBufferRange = glFlushMappedNamedBufferRangeEXT;
  if (!glFogCoordPointer) glFogCoordPointer = glFogCoordPointerEXT;
  if (!glFogCoordd) glFogCoordd = glFogCoorddEXT;
  if (!glFogCoorddv) glFogCoorddv = glFogCoorddvEXT;
  if (!glFogCoordf) glFogCoordf = glFogCoordfEXT;
  if (!glFogCoordfv) glFogCoordfv = glFogCoordfvEXT;
  // if (!glFragmentColorMaterial) glFragmentColorMaterial = glFragmentColorMaterialEXT;
  // if (!glFragmentLightModelf) glFragmentLightModelf = glFragmentLightModelfEXT;
  // if (!glFragmentLightModelfv) glFragmentLightModelfv = glFragmentLightModelfvEXT;
  // if (!glFragmentLightModeli) glFragmentLightModeli = glFragmentLightModeliEXT;
  // if (!glFragmentLightModeliv) glFragmentLightModeliv = glFragmentLightModelivEXT;
  // if (!glFragmentLightf) glFragmentLightf = glFragmentLightfEXT;
  // if (!glFragmentLightfv) glFragmentLightfv = glFragmentLightfvEXT;
  // if (!glFragmentLighti) glFragmentLighti = glFragmentLightiEXT;
  // if (!glFragmentLightiv) glFragmentLightiv = glFragmentLightivEXT;
  // if (!glFragmentMaterialf) glFragmentMaterialf = glFragmentMaterialfEXT;
  // if (!glFragmentMaterialfv) glFragmentMaterialfv = glFragmentMaterialfvEXT;
  // if (!glFragmentMateriali) glFragmentMateriali = glFragmentMaterialiEXT;
  // if (!glFragmentMaterialiv) glFragmentMaterialiv = glFragmentMaterialivEXT;
  // if (!glFramebufferDrawBuffer) glFramebufferDrawBuffer = glFramebufferDrawBufferEXT;
  // if (!glFramebufferDrawBuffers) glFramebufferDrawBuffers = glFramebufferDrawBuffersEXT;
  // if (!glFramebufferReadBuffer) glFramebufferReadBuffer = glFramebufferReadBufferEXT;
  if (!glFramebufferRenderbuffer) glFramebufferRenderbuffer = glFramebufferRenderbufferEXT;
  if (!glFramebufferTexture1D) glFramebufferTexture1D = glFramebufferTexture1DEXT;
  if (!glFramebufferTexture2D) glFramebufferTexture2D = glFramebufferTexture2DEXT;
  if (!glFramebufferTexture3D) glFramebufferTexture3D = glFramebufferTexture3DEXT;
  if (!glFramebufferTexture) glFramebufferTexture = glFramebufferTextureEXT;
  // if (!glFramebufferTextureFace) glFramebufferTextureFace = glFramebufferTextureFaceEXT;
  if (!glFramebufferTextureLayer) glFramebufferTextureLayer = glFramebufferTextureLayerEXT;
  if (!glGenFramebuffers) glGenFramebuffers = glGenFramebuffersEXT;
  if (!glGenRenderbuffers) glGenRenderbuffers = glGenRenderbuffersEXT;
  // if (!glGenSymbols) glGenSymbols = glGenSymbolsEXT;
  // if (!glGenTextures) glGenTextures = glGenTexturesEXT;
  // if (!glGenVertexShaders) glGenVertexShaders = glGenVertexShadersEXT;
  if (!glGenerateMipmap) glGenerateMipmap = glGenerateMipmapEXT;
  // if (!glGenerateMultiTexMipmap) glGenerateMultiTexMipmap = glGenerateMultiTexMipmapEXT;
  // if (!glGenerateTextureMipmap) glGenerateTextureMipmap = glGenerateTextureMipmapEXT;
  // if (!glGetBooleanIndexedv) glGetBooleanIndexedv = glGetBooleanIndexedvEXT;
  if (!glGetColorTable) glGetColorTable = glGetColorTableEXT;
  if (!glGetColorTableParameterfv) glGetColorTableParameterfv = glGetColorTableParameterfvEXT;
  if (!glGetColorTableParameteriv) glGetColorTableParameteriv = glGetColorTableParameterivEXT;
  // if (!glGetCompressedMultiTexImage) glGetCompressedMultiTexImage = glGetCompressedMultiTexImageEXT;
  // if (!glGetCompressedTextureImage) glGetCompressedTextureImage = glGetCompressedTextureImageEXT;
  if (!glGetConvolutionFilter) glGetConvolutionFilter = glGetConvolutionFilterEXT;
  if (!glGetConvolutionParameterfv) glGetConvolutionParameterfv = glGetConvolutionParameterfvEXT;
  if (!glGetConvolutionParameteriv) glGetConvolutionParameteriv = glGetConvolutionParameterivEXT;
  // if (!glGetDoubleIndexedv) glGetDoubleIndexedv = glGetDoubleIndexedvEXT;
  if (!glGetDoublei_v) glGetDoublei_v = glGetDoublei_vEXT;
  // if (!glGetFloatIndexedv) glGetFloatIndexedv = glGetFloatIndexedvEXT;
  if (!glGetFloati_v) glGetFloati_v = glGetFloati_vEXT;
  if (!glGetFragDataLocation) glGetFragDataLocation = glGetFragDataLocationEXT;
  // if (!glGetFragmentLightfv) glGetFragmentLightfv = glGetFragmentLightfvEXT;
  // if (!glGetFragmentLightiv) glGetFragmentLightiv = glGetFragmentLightivEXT;
  // if (!glGetFragmentMaterialfv) glGetFragmentMaterialfv = glGetFragmentMaterialfvEXT;
  // if (!glGetFragmentMaterialiv) glGetFragmentMaterialiv = glGetFragmentMaterialivEXT;
  if (!glGetFramebufferAttachmentParameteriv) glGetFramebufferAttachmentParameteriv = glGetFramebufferAttachmentParameterivEXT;
  // if (!glGetFramebufferParameteriv) glGetFramebufferParameteriv = glGetFramebufferParameterivEXT;
  if (!glGetHistogram) glGetHistogram = glGetHistogramEXT;
  if (!glGetHistogramParameterfv) glGetHistogramParameterfv = glGetHistogramParameterfvEXT;
  if (!glGetHistogramParameteriv) glGetHistogramParameteriv = glGetHistogramParameterivEXT;
  // if (!glGetIntegerIndexedv) glGetIntegerIndexedv = glGetIntegerIndexedvEXT;
  // if (!glGetInvariantBooleanv) glGetInvariantBooleanv = glGetInvariantBooleanvEXT;
  // if (!glGetInvariantFloatv) glGetInvariantFloatv = glGetInvariantFloatvEXT;
  // if (!glGetInvariantIntegerv) glGetInvariantIntegerv = glGetInvariantIntegervEXT;
  // if (!glGetLocalConstantBooleanv) glGetLocalConstantBooleanv = glGetLocalConstantBooleanvEXT;
  // if (!glGetLocalConstantFloatv) glGetLocalConstantFloatv = glGetLocalConstantFloatvEXT;
  // if (!glGetLocalConstantIntegerv) glGetLocalConstantIntegerv = glGetLocalConstantIntegervEXT;
  if (!glGetMinmax) glGetMinmax = glGetMinmaxEXT;
  if (!glGetMinmaxParameterfv) glGetMinmaxParameterfv = glGetMinmaxParameterfvEXT;
  if (!glGetMinmaxParameteriv) glGetMinmaxParameteriv = glGetMinmaxParameterivEXT;
  // if (!glGetMultiTexEnvfv) glGetMultiTexEnvfv = glGetMultiTexEnvfvEXT;
  // if (!glGetMultiTexEnviv) glGetMultiTexEnviv = glGetMultiTexEnvivEXT;
  // if (!glGetMultiTexGendv) glGetMultiTexGendv = glGetMultiTexGendvEXT;
  // if (!glGetMultiTexGenfv) glGetMultiTexGenfv = glGetMultiTexGenfvEXT;
  // if (!glGetMultiTexGeniv) glGetMultiTexGeniv = glGetMultiTexGenivEXT;
  // if (!glGetMultiTexImage) glGetMultiTexImage = glGetMultiTexImageEXT;
  // if (!glGetMultiTexLevelParameterfv) glGetMultiTexLevelParameterfv = glGetMultiTexLevelParameterfvEXT;
  // if (!glGetMultiTexLevelParameteriv) glGetMultiTexLevelParameteriv = glGetMultiTexLevelParameterivEXT;
  // if (!glGetMultiTexParameterIiv) glGetMultiTexParameterIiv = glGetMultiTexParameterIivEXT;
  // if (!glGetMultiTexParameterIuiv) glGetMultiTexParameterIuiv = glGetMultiTexParameterIuivEXT;
  // if (!glGetMultiTexParameterfv) glGetMultiTexParameterfv = glGetMultiTexParameterfvEXT;
  // if (!glGetMultiTexParameteriv) glGetMultiTexParameteriv = glGetMultiTexParameterivEXT;
  // if (!glGetNamedBufferParameteriv) glGetNamedBufferParameteriv = glGetNamedBufferParameterivEXT;
  // if (!glGetNamedBufferPointerv) glGetNamedBufferPointerv = glGetNamedBufferPointervEXT;
  // if (!glGetNamedBufferSubData) glGetNamedBufferSubData = glGetNamedBufferSubDataEXT;
  // if (!glGetNamedFramebufferAttachmentParameteriv) glGetNamedFramebufferAttachmentParameteriv = glGetNamedFramebufferAttachmentParameterivEXT;
  // if (!glGetNamedProgramLocalParameterIiv) glGetNamedProgramLocalParameterIiv = glGetNamedProgramLocalParameterIivEXT;
  // if (!glGetNamedProgramLocalParameterIuiv) glGetNamedProgramLocalParameterIuiv = glGetNamedProgramLocalParameterIuivEXT;
  // if (!glGetNamedProgramLocalParameterdv) glGetNamedProgramLocalParameterdv = glGetNamedProgramLocalParameterdvEXT;
  // if (!glGetNamedProgramLocalParameterfv) glGetNamedProgramLocalParameterfv = glGetNamedProgramLocalParameterfvEXT;
  // if (!glGetNamedProgramString) glGetNamedProgramString = glGetNamedProgramStringEXT;
  // if (!glGetNamedProgramiv) glGetNamedProgramiv = glGetNamedProgramivEXT;
  // if (!glGetNamedRenderbufferParameteriv) glGetNamedRenderbufferParameteriv = glGetNamedRenderbufferParameterivEXT;
  // if (!glGetPixelTransformParameterfv) glGetPixelTransformParameterfv = glGetPixelTransformParameterfvEXT;
  // if (!glGetPixelTransformParameteriv) glGetPixelTransformParameteriv = glGetPixelTransformParameterivEXT;
  // if (!glGetPointerIndexedv) glGetPointerIndexedv = glGetPointerIndexedvEXT;
  // if (!glGetPointeri_v) glGetPointeri_v = glGetPointeri_vEXT;
  if (!glGetQueryObjecti64v) glGetQueryObjecti64v = glGetQueryObjecti64vEXT;
  if (!glGetQueryObjectui64v) glGetQueryObjectui64v = glGetQueryObjectui64vEXT;
  if (!glGetRenderbufferParameteriv) glGetRenderbufferParameteriv = glGetRenderbufferParameterivEXT;
  if (!glGetSeparableFilter) glGetSeparableFilter = glGetSeparableFilterEXT;
  if (!glGetTexParameterIiv) glGetTexParameterIiv = glGetTexParameterIivEXT;
  if (!glGetTexParameterIuiv) glGetTexParameterIuiv = glGetTexParameterIuivEXT;
  // if (!glGetTextureImage) glGetTextureImage = glGetTextureImageEXT;
  // if (!glGetTextureLevelParameterfv) glGetTextureLevelParameterfv = glGetTextureLevelParameterfvEXT;
  // if (!glGetTextureLevelParameteriv) glGetTextureLevelParameteriv = glGetTextureLevelParameterivEXT;
  // if (!glGetTextureParameterIiv) glGetTextureParameterIiv = glGetTextureParameterIivEXT;
  // if (!glGetTextureParameterIuiv) glGetTextureParameterIuiv = glGetTextureParameterIuivEXT;
  // if (!glGetTextureParameterfv) glGetTextureParameterfv = glGetTextureParameterfvEXT;
  // if (!glGetTextureParameteriv) glGetTextureParameteriv = glGetTextureParameterivEXT;
  if (!glGetTransformFeedbackVarying) glGetTransformFeedbackVarying = glGetTransformFeedbackVaryingEXT;
  // if (!glGetUniformBufferSize) glGetUniformBufferSize = glGetUniformBufferSizeEXT;
  // if (!glGetUniformOffset) glGetUniformOffset = glGetUniformOffsetEXT;
  if (!glGetUniformuiv) glGetUniformuiv = glGetUniformuivEXT;
  // if (!glGetVariantBooleanv) glGetVariantBooleanv = glGetVariantBooleanvEXT;
  // if (!glGetVariantFloatv) glGetVariantFloatv = glGetVariantFloatvEXT;
  // if (!glGetVariantIntegerv) glGetVariantIntegerv = glGetVariantIntegervEXT;
  // if (!glGetVariantPointerv) glGetVariantPointerv = glGetVariantPointervEXT;
  // if (!glGetVertexArrayIntegeri_v) glGetVertexArrayIntegeri_v = glGetVertexArrayIntegeri_vEXT;
  // if (!glGetVertexArrayIntegerv) glGetVertexArrayIntegerv = glGetVertexArrayIntegervEXT;
  // if (!glGetVertexArrayPointeri_v) glGetVertexArrayPointeri_v = glGetVertexArrayPointeri_vEXT;
  // if (!glGetVertexArrayPointerv) glGetVertexArrayPointerv = glGetVertexArrayPointervEXT;
  if (!glGetVertexAttribIiv) glGetVertexAttribIiv = glGetVertexAttribIivEXT;
  if (!glGetVertexAttribIuiv) glGetVertexAttribIuiv = glGetVertexAttribIuivEXT;
  if (!glGetVertexAttribLdv) glGetVertexAttribLdv = glGetVertexAttribLdvEXT;
  if (!glHistogram) glHistogram = glHistogramEXT;
  // if (!glIndexFunc) glIndexFunc = glIndexFuncEXT;
  // if (!glIndexMaterial) glIndexMaterial = glIndexMaterialEXT;
  // if (!glIndexPointer) glIndexPointer = glIndexPointerEXT;
  // if (!glInsertComponent) glInsertComponent = glInsertComponentEXT;
  // if (!glIsEnabledIndexed) glIsEnabledIndexed = glIsEnabledIndexedEXT;
  if (!glIsFramebuffer) glIsFramebuffer = glIsFramebufferEXT;
  if (!glIsRenderbuffer) glIsRenderbuffer = glIsRenderbufferEXT;
  // if (!glIsTexture) glIsTexture = glIsTextureEXT;
  // if (!glIsVariantEnabled) glIsVariantEnabled = glIsVariantEnabledEXT;
  // if (!glLightEnvi) glLightEnvi = glLightEnviEXT;
  // if (!glLockArrays) glLockArrays = glLockArraysEXT;
  // if (!glMapNamedBuffer) glMapNamedBuffer = glMapNamedBufferEXT;
  // if (!glMapNamedBufferRange) glMapNamedBufferRange = glMapNamedBufferRangeEXT;
  // if (!glMatrixFrustum) glMatrixFrustum = glMatrixFrustumEXT;
  // if (!glMatrixLoadIdentity) glMatrixLoadIdentity = glMatrixLoadIdentityEXT;
  // if (!glMatrixLoadTransposed) glMatrixLoadTransposed = glMatrixLoadTransposedEXT;
  // if (!glMatrixLoadTransposef) glMatrixLoadTransposef = glMatrixLoadTransposefEXT;
  // if (!glMatrixLoadd) glMatrixLoadd = glMatrixLoaddEXT;
  // if (!glMatrixLoadf) glMatrixLoadf = glMatrixLoadfEXT;
  // if (!glMatrixMultTransposed) glMatrixMultTransposed = glMatrixMultTransposedEXT;
  // if (!glMatrixMultTransposef) glMatrixMultTransposef = glMatrixMultTransposefEXT;
  // if (!glMatrixMultd) glMatrixMultd = glMatrixMultdEXT;
  // if (!glMatrixMultf) glMatrixMultf = glMatrixMultfEXT;
  // if (!glMatrixOrtho) glMatrixOrtho = glMatrixOrthoEXT;
  // if (!glMatrixPop) glMatrixPop = glMatrixPopEXT;
  // if (!glMatrixPush) glMatrixPush = glMatrixPushEXT;
  // if (!glMatrixRotated) glMatrixRotated = glMatrixRotatedEXT;
  // if (!glMatrixRotatef) glMatrixRotatef = glMatrixRotatefEXT;
  // if (!glMatrixScaled) glMatrixScaled = glMatrixScaledEXT;
  // if (!glMatrixScalef) glMatrixScalef = glMatrixScalefEXT;
  // if (!glMatrixTranslated) glMatrixTranslated = glMatrixTranslatedEXT;
  // if (!glMatrixTranslatef) glMatrixTranslatef = glMatrixTranslatefEXT;
  // if (!glMemoryBarrier) glMemoryBarrier = glMemoryBarrierEXT;
  if (!glMinmax) glMinmax = glMinmaxEXT;
  if (!glMultiDrawArrays) glMultiDrawArrays = glMultiDrawArraysEXT;
  if (!glMultiDrawElements) glMultiDrawElements = glMultiDrawElementsEXT;
  // if (!glMultiTexBuffer) glMultiTexBuffer = glMultiTexBufferEXT;
  // if (!glMultiTexCoordPointer) glMultiTexCoordPointer = glMultiTexCoordPointerEXT;
  // if (!glMultiTexEnvf) glMultiTexEnvf = glMultiTexEnvfEXT;
  // if (!glMultiTexEnvfv) glMultiTexEnvfv = glMultiTexEnvfvEXT;
  // if (!glMultiTexEnvi) glMultiTexEnvi = glMultiTexEnviEXT;
  // if (!glMultiTexEnviv) glMultiTexEnviv = glMultiTexEnvivEXT;
  // if (!glMultiTexGend) glMultiTexGend = glMultiTexGendEXT;
  // if (!glMultiTexGendv) glMultiTexGendv = glMultiTexGendvEXT;
  // if (!glMultiTexGenf) glMultiTexGenf = glMultiTexGenfEXT;
  // if (!glMultiTexGenfv) glMultiTexGenfv = glMultiTexGenfvEXT;
  // if (!glMultiTexGeni) glMultiTexGeni = glMultiTexGeniEXT;
  // if (!glMultiTexGeniv) glMultiTexGeniv = glMultiTexGenivEXT;
  // if (!glMultiTexImage1D) glMultiTexImage1D = glMultiTexImage1DEXT;
  // if (!glMultiTexImage2D) glMultiTexImage2D = glMultiTexImage2DEXT;
  // if (!glMultiTexImage3D) glMultiTexImage3D = glMultiTexImage3DEXT;
  // if (!glMultiTexParameterIiv) glMultiTexParameterIiv = glMultiTexParameterIivEXT;
  // if (!glMultiTexParameterIuiv) glMultiTexParameterIuiv = glMultiTexParameterIuivEXT;
  // if (!glMultiTexParameterf) glMultiTexParameterf = glMultiTexParameterfEXT;
  // if (!glMultiTexParameterfv) glMultiTexParameterfv = glMultiTexParameterfvEXT;
  // if (!glMultiTexParameteri) glMultiTexParameteri = glMultiTexParameteriEXT;
  // if (!glMultiTexParameteriv) glMultiTexParameteriv = glMultiTexParameterivEXT;
  // if (!glMultiTexRenderbuffer) glMultiTexRenderbuffer = glMultiTexRenderbufferEXT;
  // if (!glMultiTexSubImage1D) glMultiTexSubImage1D = glMultiTexSubImage1DEXT;
  // if (!glMultiTexSubImage2D) glMultiTexSubImage2D = glMultiTexSubImage2DEXT;
  // if (!glMultiTexSubImage3D) glMultiTexSubImage3D = glMultiTexSubImage3DEXT;
  // if (!glNamedBufferData) glNamedBufferData = glNamedBufferDataEXT;
  // if (!glNamedBufferSubData) glNamedBufferSubData = glNamedBufferSubDataEXT;
  // if (!glNamedCopyBufferSubData) glNamedCopyBufferSubData = glNamedCopyBufferSubDataEXT;
  // if (!glNamedFramebufferRenderbuffer) glNamedFramebufferRenderbuffer = glNamedFramebufferRenderbufferEXT;
  // if (!glNamedFramebufferTexture1D) glNamedFramebufferTexture1D = glNamedFramebufferTexture1DEXT;
  // if (!glNamedFramebufferTexture2D) glNamedFramebufferTexture2D = glNamedFramebufferTexture2DEXT;
  // if (!glNamedFramebufferTexture3D) glNamedFramebufferTexture3D = glNamedFramebufferTexture3DEXT;
  // if (!glNamedFramebufferTexture) glNamedFramebufferTexture = glNamedFramebufferTextureEXT;
  // if (!glNamedFramebufferTextureFace) glNamedFramebufferTextureFace = glNamedFramebufferTextureFaceEXT;
  // if (!glNamedFramebufferTextureLayer) glNamedFramebufferTextureLayer = glNamedFramebufferTextureLayerEXT;
  // if (!glNamedProgramLocalParameter4d) glNamedProgramLocalParameter4d = glNamedProgramLocalParameter4dEXT;
  // if (!glNamedProgramLocalParameter4dv) glNamedProgramLocalParameter4dv = glNamedProgramLocalParameter4dvEXT;
  // if (!glNamedProgramLocalParameter4f) glNamedProgramLocalParameter4f = glNamedProgramLocalParameter4fEXT;
  // if (!glNamedProgramLocalParameter4fv) glNamedProgramLocalParameter4fv = glNamedProgramLocalParameter4fvEXT;
  // if (!glNamedProgramLocalParameterI4i) glNamedProgramLocalParameterI4i = glNamedProgramLocalParameterI4iEXT;
  // if (!glNamedProgramLocalParameterI4iv) glNamedProgramLocalParameterI4iv = glNamedProgramLocalParameterI4ivEXT;
  // if (!glNamedProgramLocalParameterI4ui) glNamedProgramLocalParameterI4ui = glNamedProgramLocalParameterI4uiEXT;
  // if (!glNamedProgramLocalParameterI4uiv) glNamedProgramLocalParameterI4uiv = glNamedProgramLocalParameterI4uivEXT;
  // if (!glNamedProgramLocalParameters4fv) glNamedProgramLocalParameters4fv = glNamedProgramLocalParameters4fvEXT;
  // if (!glNamedProgramLocalParametersI4iv) glNamedProgramLocalParametersI4iv = glNamedProgramLocalParametersI4ivEXT;
  // if (!glNamedProgramLocalParametersI4uiv) glNamedProgramLocalParametersI4uiv = glNamedProgramLocalParametersI4uivEXT;
  // if (!glNamedProgramString) glNamedProgramString = glNamedProgramStringEXT;
  // if (!glNamedRenderbufferStorage) glNamedRenderbufferStorage = glNamedRenderbufferStorageEXT;
  // if (!glNamedRenderbufferStorageMultisampleCoverage) glNamedRenderbufferStorageMultisampleCoverage = glNamedRenderbufferStorageMultisampleCoverageEXT;
  // if (!glNamedRenderbufferStorageMultisample) glNamedRenderbufferStorageMultisample = glNamedRenderbufferStorageMultisampleEXT;
  // if (!glNewBufferRegion) glNewBufferRegion = glNewBufferRegionEXT;
  // if (!glNormalPointer) glNormalPointer = glNormalPointerEXT;
  // if (!glPixelTransformParameterf) glPixelTransformParameterf = glPixelTransformParameterfEXT;
  // if (!glPixelTransformParameterfv) glPixelTransformParameterfv = glPixelTransformParameterfvEXT;
  // if (!glPixelTransformParameteri) glPixelTransformParameteri = glPixelTransformParameteriEXT;
  // if (!glPixelTransformParameteriv) glPixelTransformParameteriv = glPixelTransformParameterivEXT;
  if (!glPointParameterf) glPointParameterf = glPointParameterfEXT;
  if (!glPointParameterfv) glPointParameterfv = glPointParameterfvEXT;
  // if (!glPolygonOffset) glPolygonOffset = glPolygonOffsetEXT;
  // if (!glPrioritizeTextures) glPrioritizeTextures = glPrioritizeTexturesEXT;
  // if (!glProgramEnvParameters4fv) glProgramEnvParameters4fv = glProgramEnvParameters4fvEXT;
  // if (!glProgramLocalParameters4fv) glProgramLocalParameters4fv = glProgramLocalParameters4fvEXT;
  if (!glProgramParameteri) glProgramParameteri = glProgramParameteriEXT;
  if (!glProgramUniform1d) glProgramUniform1d = glProgramUniform1dEXT;
  if (!glProgramUniform1dv) glProgramUniform1dv = glProgramUniform1dvEXT;
  if (!glProgramUniform1f) glProgramUniform1f = glProgramUniform1fEXT;
  if (!glProgramUniform1fv) glProgramUniform1fv = glProgramUniform1fvEXT;
  if (!glProgramUniform1i) glProgramUniform1i = glProgramUniform1iEXT;
  if (!glProgramUniform1iv) glProgramUniform1iv = glProgramUniform1ivEXT;
  if (!glProgramUniform1ui) glProgramUniform1ui = glProgramUniform1uiEXT;
  if (!glProgramUniform1uiv) glProgramUniform1uiv = glProgramUniform1uivEXT;
  if (!glProgramUniform2d) glProgramUniform2d = glProgramUniform2dEXT;
  if (!glProgramUniform2dv) glProgramUniform2dv = glProgramUniform2dvEXT;
  if (!glProgramUniform2f) glProgramUniform2f = glProgramUniform2fEXT;
  if (!glProgramUniform2fv) glProgramUniform2fv = glProgramUniform2fvEXT;
  if (!glProgramUniform2i) glProgramUniform2i = glProgramUniform2iEXT;
  if (!glProgramUniform2iv) glProgramUniform2iv = glProgramUniform2ivEXT;
  if (!glProgramUniform2ui) glProgramUniform2ui = glProgramUniform2uiEXT;
  if (!glProgramUniform2uiv) glProgramUniform2uiv = glProgramUniform2uivEXT;
  if (!glProgramUniform3d) glProgramUniform3d = glProgramUniform3dEXT;
  if (!glProgramUniform3dv) glProgramUniform3dv = glProgramUniform3dvEXT;
  if (!glProgramUniform3f) glProgramUniform3f = glProgramUniform3fEXT;
  if (!glProgramUniform3fv) glProgramUniform3fv = glProgramUniform3fvEXT;
  if (!glProgramUniform3i) glProgramUniform3i = glProgramUniform3iEXT;
  if (!glProgramUniform3iv) glProgramUniform3iv = glProgramUniform3ivEXT;
  if (!glProgramUniform3ui) glProgramUniform3ui = glProgramUniform3uiEXT;
  if (!glProgramUniform3uiv) glProgramUniform3uiv = glProgramUniform3uivEXT;
  if (!glProgramUniform4d) glProgramUniform4d = glProgramUniform4dEXT;
  if (!glProgramUniform4dv) glProgramUniform4dv = glProgramUniform4dvEXT;
  if (!glProgramUniform4f) glProgramUniform4f = glProgramUniform4fEXT;
  if (!glProgramUniform4fv) glProgramUniform4fv = glProgramUniform4fvEXT;
  if (!glProgramUniform4i) glProgramUniform4i = glProgramUniform4iEXT;
  if (!glProgramUniform4iv) glProgramUniform4iv = glProgramUniform4ivEXT;
  if (!glProgramUniform4ui) glProgramUniform4ui = glProgramUniform4uiEXT;
  if (!glProgramUniform4uiv) glProgramUniform4uiv = glProgramUniform4uivEXT;
  if (!glProgramUniformMatrix2dv) glProgramUniformMatrix2dv = glProgramUniformMatrix2dvEXT;
  if (!glProgramUniformMatrix2fv) glProgramUniformMatrix2fv = glProgramUniformMatrix2fvEXT;
  if (!glProgramUniformMatrix2x3dv) glProgramUniformMatrix2x3dv = glProgramUniformMatrix2x3dvEXT;
  if (!glProgramUniformMatrix2x3fv) glProgramUniformMatrix2x3fv = glProgramUniformMatrix2x3fvEXT;
  if (!glProgramUniformMatrix2x4dv) glProgramUniformMatrix2x4dv = glProgramUniformMatrix2x4dvEXT;
  if (!glProgramUniformMatrix2x4fv) glProgramUniformMatrix2x4fv = glProgramUniformMatrix2x4fvEXT;
  if (!glProgramUniformMatrix3dv) glProgramUniformMatrix3dv = glProgramUniformMatrix3dvEXT;
  if (!glProgramUniformMatrix3fv) glProgramUniformMatrix3fv = glProgramUniformMatrix3fvEXT;
  if (!glProgramUniformMatrix3x2dv) glProgramUniformMatrix3x2dv = glProgramUniformMatrix3x2dvEXT;
  if (!glProgramUniformMatrix3x2fv) glProgramUniformMatrix3x2fv = glProgramUniformMatrix3x2fvEXT;
  if (!glProgramUniformMatrix3x4dv) glProgramUniformMatrix3x4dv = glProgramUniformMatrix3x4dvEXT;
  if (!glProgramUniformMatrix3x4fv) glProgramUniformMatrix3x4fv = glProgramUniformMatrix3x4fvEXT;
  if (!glProgramUniformMatrix4dv) glProgramUniformMatrix4dv = glProgramUniformMatrix4dvEXT;
  if (!glProgramUniformMatrix4fv) glProgramUniformMatrix4fv = glProgramUniformMatrix4fvEXT;
  if (!glProgramUniformMatrix4x2dv) glProgramUniformMatrix4x2dv = glProgramUniformMatrix4x2dvEXT;
  if (!glProgramUniformMatrix4x2fv) glProgramUniformMatrix4x2fv = glProgramUniformMatrix4x2fvEXT;
  if (!glProgramUniformMatrix4x3dv) glProgramUniformMatrix4x3dv = glProgramUniformMatrix4x3dvEXT;
  if (!glProgramUniformMatrix4x3fv) glProgramUniformMatrix4x3fv = glProgramUniformMatrix4x3fvEXT;
  if (!glProvokingVertex) glProvokingVertex = glProvokingVertexEXT;
  // if (!glPushClientAttribDefault) glPushClientAttribDefault = glPushClientAttribDefaultEXT;
  // if (!glReadBufferRegion) glReadBufferRegion = glReadBufferRegionEXT;
  if (!glRenderbufferStorage) glRenderbufferStorage = glRenderbufferStorageEXT;
  if (!glRenderbufferStorageMultisample) glRenderbufferStorageMultisample = glRenderbufferStorageMultisampleEXT;
  if (!glResetHistogram) glResetHistogram = glResetHistogramEXT;
  if (!glResetMinmax) glResetMinmax = glResetMinmaxEXT;
  // if (!glSampleMask) glSampleMask = glSampleMaskEXT;
  // if (!glSamplePattern) glSamplePattern = glSamplePatternEXT;
  if (!glSecondaryColor3b) glSecondaryColor3b = glSecondaryColor3bEXT;
  if (!glSecondaryColor3bv) glSecondaryColor3bv = glSecondaryColor3bvEXT;
  if (!glSecondaryColor3d) glSecondaryColor3d = glSecondaryColor3dEXT;
  if (!glSecondaryColor3dv) glSecondaryColor3dv = glSecondaryColor3dvEXT;
  if (!glSecondaryColor3f) glSecondaryColor3f = glSecondaryColor3fEXT;
  if (!glSecondaryColor3fv) glSecondaryColor3fv = glSecondaryColor3fvEXT;
  if (!glSecondaryColor3i) glSecondaryColor3i = glSecondaryColor3iEXT;
  if (!glSecondaryColor3iv) glSecondaryColor3iv = glSecondaryColor3ivEXT;
  if (!glSecondaryColor3s) glSecondaryColor3s = glSecondaryColor3sEXT;
  if (!glSecondaryColor3sv) glSecondaryColor3sv = glSecondaryColor3svEXT;
  if (!glSecondaryColor3ub) glSecondaryColor3ub = glSecondaryColor3ubEXT;
  if (!glSecondaryColor3ubv) glSecondaryColor3ubv = glSecondaryColor3ubvEXT;
  if (!glSecondaryColor3ui) glSecondaryColor3ui = glSecondaryColor3uiEXT;
  if (!glSecondaryColor3uiv) glSecondaryColor3uiv = glSecondaryColor3uivEXT;
  if (!glSecondaryColor3us) glSecondaryColor3us = glSecondaryColor3usEXT;
  if (!glSecondaryColor3usv) glSecondaryColor3usv = glSecondaryColor3usvEXT;
  if (!glSecondaryColorPointer) glSecondaryColorPointer = glSecondaryColorPointerEXT;
  if (!glSeparableFilter2D) glSeparableFilter2D = glSeparableFilter2DEXT;
  // if (!glSetInvariant) glSetInvariant = glSetInvariantEXT;
  // if (!glSetLocalConstant) glSetLocalConstant = glSetLocalConstantEXT;
  // if (!glShaderOp1) glShaderOp1 = glShaderOp1EXT;
  // if (!glShaderOp2) glShaderOp2 = glShaderOp2EXT;
  // if (!glShaderOp3) glShaderOp3 = glShaderOp3EXT;
  // if (!glSwizzle) glSwizzle = glSwizzleEXT;
  // if (!glTangentPointer) glTangentPointer = glTangentPointerEXT;
  if (!glTexBuffer) glTexBuffer = glTexBufferEXT;
  // if (!glTexCoordPointer) glTexCoordPointer = glTexCoordPointerEXT;
  if (!glTexImage3D) glTexImage3D = glTexImage3DEXT;
  if (!glTexParameterIiv) glTexParameterIiv = glTexParameterIivEXT;
  if (!glTexParameterIuiv) glTexParameterIuiv = glTexParameterIuivEXT;
  // if (!glTexSubImage1D) glTexSubImage1D = glTexSubImage1DEXT;
  // if (!glTexSubImage2D) glTexSubImage2D = glTexSubImage2DEXT;
  if (!glTexSubImage3D) glTexSubImage3D = glTexSubImage3DEXT;
  // if (!glTextureBuffer) glTextureBuffer = glTextureBufferEXT;
  // if (!glTextureImage1D) glTextureImage1D = glTextureImage1DEXT;
  // if (!glTextureImage2D) glTextureImage2D = glTextureImage2DEXT;
  // if (!glTextureImage3D) glTextureImage3D = glTextureImage3DEXT;
  // if (!glTextureLight) glTextureLight = glTextureLightEXT;
  // if (!glTextureMaterial) glTextureMaterial = glTextureMaterialEXT;
  // if (!glTextureNormal) glTextureNormal = glTextureNormalEXT;
  // if (!glTextureParameterIiv) glTextureParameterIiv = glTextureParameterIivEXT;
  // if (!glTextureParameterIuiv) glTextureParameterIuiv = glTextureParameterIuivEXT;
  // if (!glTextureParameterf) glTextureParameterf = glTextureParameterfEXT;
  // if (!glTextureParameterfv) glTextureParameterfv = glTextureParameterfvEXT;
  // if (!glTextureParameteri) glTextureParameteri = glTextureParameteriEXT;
  // if (!glTextureParameteriv) glTextureParameteriv = glTextureParameterivEXT;
  // if (!glTextureRenderbuffer) glTextureRenderbuffer = glTextureRenderbufferEXT;
  // if (!glTextureSubImage1D) glTextureSubImage1D = glTextureSubImage1DEXT;
  // if (!glTextureSubImage2D) glTextureSubImage2D = glTextureSubImage2DEXT;
  // if (!glTextureSubImage3D) glTextureSubImage3D = glTextureSubImage3DEXT;
  if (!glTransformFeedbackVaryings) glTransformFeedbackVaryings = glTransformFeedbackVaryingsEXT;
  if (!glUniform1ui) glUniform1ui = glUniform1uiEXT;
  if (!glUniform1uiv) glUniform1uiv = glUniform1uivEXT;
  if (!glUniform2ui) glUniform2ui = glUniform2uiEXT;
  if (!glUniform2uiv) glUniform2uiv = glUniform2uivEXT;
  if (!glUniform3ui) glUniform3ui = glUniform3uiEXT;
  if (!glUniform3uiv) glUniform3uiv = glUniform3uivEXT;
  if (!glUniform4ui) glUniform4ui = glUniform4uiEXT;
  if (!glUniform4uiv) glUniform4uiv = glUniform4uivEXT;
  // if (!glUniformBuffer) glUniformBuffer = glUniformBufferEXT;
  // if (!glUnlockArrays) glUnlockArrays = glUnlockArraysEXT;
  // if (!glUnmapNamedBuffer) glUnmapNamedBuffer = glUnmapNamedBufferEXT;
  // if (!glUseShaderProgram) glUseShaderProgram = glUseShaderProgramEXT;
  // if (!glVariantPointer) glVariantPointer = glVariantPointerEXT;
  // if (!glVariantbv) glVariantbv = glVariantbvEXT;
  // if (!glVariantdv) glVariantdv = glVariantdvEXT;
  // if (!glVariantfv) glVariantfv = glVariantfvEXT;
  // if (!glVariantiv) glVariantiv = glVariantivEXT;
  // if (!glVariantsv) glVariantsv = glVariantsvEXT;
  // if (!glVariantubv) glVariantubv = glVariantubvEXT;
  // if (!glVariantuiv) glVariantuiv = glVariantuivEXT;
  // if (!glVariantusv) glVariantusv = glVariantusvEXT;
  // if (!glVertexArrayColorOffset) glVertexArrayColorOffset = glVertexArrayColorOffsetEXT;
  // if (!glVertexArrayEdgeFlagOffset) glVertexArrayEdgeFlagOffset = glVertexArrayEdgeFlagOffsetEXT;
  // if (!glVertexArrayFogCoordOffset) glVertexArrayFogCoordOffset = glVertexArrayFogCoordOffsetEXT;
  // if (!glVertexArrayIndexOffset) glVertexArrayIndexOffset = glVertexArrayIndexOffsetEXT;
  // if (!glVertexArrayMultiTexCoordOffset) glVertexArrayMultiTexCoordOffset = glVertexArrayMultiTexCoordOffsetEXT;
  // if (!glVertexArrayNormalOffset) glVertexArrayNormalOffset = glVertexArrayNormalOffsetEXT;
  // if (!glVertexArraySecondaryColorOffset) glVertexArraySecondaryColorOffset = glVertexArraySecondaryColorOffsetEXT;
  // if (!glVertexArrayTexCoordOffset) glVertexArrayTexCoordOffset = glVertexArrayTexCoordOffsetEXT;
  // if (!glVertexArrayVertexAttribIOffset) glVertexArrayVertexAttribIOffset = glVertexArrayVertexAttribIOffsetEXT;
  // if (!glVertexArrayVertexAttribLOffset) glVertexArrayVertexAttribLOffset = glVertexArrayVertexAttribLOffsetEXT;
  // if (!glVertexArrayVertexAttribOffset) glVertexArrayVertexAttribOffset = glVertexArrayVertexAttribOffsetEXT;
  // if (!glVertexArrayVertexOffset) glVertexArrayVertexOffset = glVertexArrayVertexOffsetEXT;
  if (!glVertexAttribI1i) glVertexAttribI1i = glVertexAttribI1iEXT;
  if (!glVertexAttribI1iv) glVertexAttribI1iv = glVertexAttribI1ivEXT;
  if (!glVertexAttribI1ui) glVertexAttribI1ui = glVertexAttribI1uiEXT;
  if (!glVertexAttribI1uiv) glVertexAttribI1uiv = glVertexAttribI1uivEXT;
  if (!glVertexAttribI2i) glVertexAttribI2i = glVertexAttribI2iEXT;
  if (!glVertexAttribI2iv) glVertexAttribI2iv = glVertexAttribI2ivEXT;
  if (!glVertexAttribI2ui) glVertexAttribI2ui = glVertexAttribI2uiEXT;
  if (!glVertexAttribI2uiv) glVertexAttribI2uiv = glVertexAttribI2uivEXT;
  if (!glVertexAttribI3i) glVertexAttribI3i = glVertexAttribI3iEXT;
  if (!glVertexAttribI3iv) glVertexAttribI3iv = glVertexAttribI3ivEXT;
  if (!glVertexAttribI3ui) glVertexAttribI3ui = glVertexAttribI3uiEXT;
  if (!glVertexAttribI3uiv) glVertexAttribI3uiv = glVertexAttribI3uivEXT;
  if (!glVertexAttribI4bv) glVertexAttribI4bv = glVertexAttribI4bvEXT;
  if (!glVertexAttribI4i) glVertexAttribI4i = glVertexAttribI4iEXT;
  if (!glVertexAttribI4iv) glVertexAttribI4iv = glVertexAttribI4ivEXT;
  if (!glVertexAttribI4sv) glVertexAttribI4sv = glVertexAttribI4svEXT;
  if (!glVertexAttribI4ubv) glVertexAttribI4ubv = glVertexAttribI4ubvEXT;
  if (!glVertexAttribI4ui) glVertexAttribI4ui = glVertexAttribI4uiEXT;
  if (!glVertexAttribI4uiv) glVertexAttribI4uiv = glVertexAttribI4uivEXT;
  if (!glVertexAttribI4usv) glVertexAttribI4usv = glVertexAttribI4usvEXT;
  if (!glVertexAttribIPointer) glVertexAttribIPointer = glVertexAttribIPointerEXT;
  if (!glVertexAttribL1d) glVertexAttribL1d = glVertexAttribL1dEXT;
  if (!glVertexAttribL1dv) glVertexAttribL1dv = glVertexAttribL1dvEXT;
  if (!glVertexAttribL2d) glVertexAttribL2d = glVertexAttribL2dEXT;
  if (!glVertexAttribL2dv) glVertexAttribL2dv = glVertexAttribL2dvEXT;
  if (!glVertexAttribL3d) glVertexAttribL3d = glVertexAttribL3dEXT;
  if (!glVertexAttribL3dv) glVertexAttribL3dv = glVertexAttribL3dvEXT;
  if (!glVertexAttribL4d) glVertexAttribL4d = glVertexAttribL4dEXT;
  if (!glVertexAttribL4dv) glVertexAttribL4dv = glVertexAttribL4dvEXT;
  if (!glVertexAttribLPointer) glVertexAttribLPointer = glVertexAttribLPointerEXT;
  // if (!glVertexPointer) glVertexPointer = glVertexPointerEXT;
  // if (!glVertexWeightPointer) glVertexWeightPointer = glVertexWeightPointerEXT;
  // if (!glVertexWeightf) glVertexWeightf = glVertexWeightfEXT;
  // if (!glVertexWeightfv) glVertexWeightfv = glVertexWeightfvEXT;
  // if (!glWriteMask) glWriteMask = glWriteMaskEXT;

  // how about glIsProgram and glIsShader?
}
