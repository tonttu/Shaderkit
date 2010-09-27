/**
 * Copyright 2010 Riku Palomäki.
 * This file is part of GLSL Lab.
 *
 * GLSL Lab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3 as
 * published by the Free Software Foundation.
 *
 * GLSL Lab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GLSL Lab.  If not, see <http://www.gnu.org/licenses/>.
 */

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
