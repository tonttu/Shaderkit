/**
 * Copyright 2010-2012 Riku Palomäki.
 * This file is part of Shaderkit, http://www.shaderkit.org/.
 *
 * Shaderkit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3 as
 * published by the Free Software Foundation.
 *
 * Shaderkit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Shaderkit.  If not, see <http://www.gnu.org/licenses/>.
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
