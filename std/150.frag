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
