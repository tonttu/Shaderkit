/**
 * Copyright 2010 Riku Palomäki.
 * This file is part of GLSL Lab.
 *
 * GLSL Lab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GLSL Lab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GLSL Lab.  If not, see <http://www.gnu.org/licenses/>.
 */

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
