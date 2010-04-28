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

#ifndef SHADER_UNIFORM_HPP
#define SHADER_UNIFORM_HPP

#include "forward.hpp"

#include <QVector>
#include <QtOpenGL>

/**
 * Any OpenGL uniform variable can be stored as UniformVar.
 *
 * Constructor reads the current value from given shader, and set() method
 * restores the variable state to (another) shader.
 */
class UniformVar {
public:
  /// Constructs an invalid variable, exists only for QMap etc.
  UniformVar();

  /**
   * Constructs a new variable, and reads the current value from given shader.
   *
   * @param name The name of the active uniform variable in prog, can't be
   *             built-in uniform (anything that starts with "gl_").
   * @param type Uniform variable type, see ShaderTypeInfo::type
   */
  UniformVar(QGLShaderProgram& prog, QString name, GLenum type);

  /**
   * Sets this variable value to the prog shader program.
   *
   * @param prog The shader program where the uniform value is stored.
   *             - Doesn't need to be same shader where this value was created.
   * @param relocate Do we use the old locations, or get new locations with
   *                 parameter names. This can only be false if this is the same
   *                 shader that was given in constructor, and it isn't relinked
   *                 since. True is always a safe choice.
   */
  void set(QGLShaderProgram& prog, bool relocate = true);

  /**
   * Get the uniform variable value as a QVariant (of array element idx)
   */
  QVariant getValue(QVariant::Type type, int idx = 0);

private:
  /// The name of the uniform variable
  QString m_name;

  /// Uniform variable type, see ShaderTypeInfo::type
  GLenum m_type;

   /**
    * The size of the array, if this is an array. For example "vec3 foo" is not
    * an array (size == 1), it's only a vector, when "vec3 foo[2]" or
    * "float bar[2]" both are arrays of size two.
    */
  size_t m_size;

  /**
   * Uniform variable locations in the shader for every variable array element.
   * For example ivec3 (type GL_INT_VEC3) should have 1 location, but ivec3[2]
   * should have 2 different locations.
   */
  QVector<GLint> m_location;

  /// The actual uniform data, if this is a float type
  QVector<GLfloat> m_floatdata;
  /// The actual uniform data, if this is a int type
  QVector<GLint> m_intdata;
};

#endif
