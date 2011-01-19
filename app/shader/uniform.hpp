/**
 * Copyright 2010,2011 Riku Palomäki.
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

#ifndef SHADER_UNIFORM_HPP
#define SHADER_UNIFORM_HPP

#include "app/forward.hpp"
#include "app/opengl.hpp"

#include <QVector>

#ifdef _WIN32
#undef GLAPIENTRY
#define GLAPIENTRY __stdcall
#endif

/**
 * Uniform variable type info class, makes easier to implement more generic
 * code that does something with uniform variables.
 */
struct ShaderTypeInfo {
  typedef void (GLAPIENTRY *FloatSetter)(GLint, GLsizei, const GLfloat*);
  typedef void (GLAPIENTRY *IntSetter)(GLint, GLsizei, const GLint*);
  typedef void (GLAPIENTRY *MatrixSetter)(GLint, GLsizei, GLboolean, const GLfloat*);

  /**
   * Constructor that also stores all the type information of given OpenGL type.
   *
   * All types are either float or int types, like bool/sampler types are also ints.
   *
   * @param flags Bit field of type information:
   *              0x01 bit on == sampler type
   */
  ShaderTypeInfo(int size = 0,
      FloatSetter float_setter = 0, IntSetter int_setter = 0, MatrixSetter matrix_setter = 0,
      GLenum type = 0, GLenum base_type = 0, int flags = 0);

  /// Data type of the uniform variable, for example GL_FLOAT_MAT2x3
  /// - see the whole list from glGetActiveUniform documentation.
  GLenum type;
  /// Data type of one element in vector/matrix
  /// for example if type is GL_FLOAT_MAT2x3, this would be GL_FLOAT
  GLenum base_type;
  /// Number of elements in base_type, Different from 1 only for vector and matrix types.
  int size;

  /// If this some kind of sampler type.
  bool is_sampler;

  /// For float types, this is the correct setter function, like glUniform3fv.
  FloatSetter float_setter;
  /// For integer types, this is the correct setter function, like glUniform3iv.
  IntSetter int_setter;
  /// For matrix types, this is the correct setter function, like glUniformMatrix2x3fv.
  MatrixSetter matrix_setter;

  /// Returns the correct QVariant type for single_type.
  QVariant::Type variant() const;

  /// Return a TypeInfo object for given type
  /// @param type Data type of the uniform variable, for example GL_FLOAT_MAT2x3
  //              - see the whole list from glGetActiveUniform documentation
  static const ShaderTypeInfo& typeInfo(GLenum type);
};

/**
 * Any OpenGL uniform variable can be stored as UniformVar.
 *
 * Constructor reads the current value from given shader, and set() method
 * restores the variable state to (another) shader.
 */
class UniformVar {
public:
  typedef QVector<UniformVar> List;

  /// Constructs an invalid variable, exists only for QMap etc.
  UniformVar();

  /**
   * Constructs a new variable, and reads the current value from given shader.
   *
   * @param name The name of the active uniform variable in prog, can't be
   *             built-in uniform (anything that starts with "gl_").
   * @param type Uniform variable type, see ShaderTypeInfo::type
   */
  UniformVar(ProgramPtr prog, QString name, GLenum type);

  /**
   * Sets this variable value to the prog shader program.
   *
   * @param prog The shader program where the uniform value is stored.
   *             - Doesn't need to be same shader where this value was created.
   *             If not given, the original shader program is used.
   * @param relocate Do we use the old locations, or get new locations with
   *                 parameter names. This can only be false if this is the same
   *                 shader that was given in constructor, and it isn't relinked
   *                 since. True is always a safe choice.
   */
  void set(ProgramPtr prog = ProgramPtr(), bool relocate = true);

  /**
   * Get the uniform variable value as a float.
   * If there was a "vec4 foo[3]" then "foo[2][3]" is get(2, 3)
   * @param array_idx Array element index (zero works if this is not an array)
   * @param vector_idx Vector/Matrix element index (zero works if this is just a plain value)
   */
  float get(size_t array_idx = 0, size_t vector_idx = 0);

  /// @todo implement
  float getDefault(size_t array_idx = 0, size_t vector_idx = 0) { (void)array_idx; (void)vector_idx; return 0; }

  /**
   * Sets the uniform variable value. If no shader given, then the value is set
   * to the same shader the uniform was originally read from, if any.
   * @see get()
   * @return true if success
   */
  bool set(float value, size_t array_idx = 0, size_t vector_idx = 0,
      ProgramPtr prog = ProgramPtr(), bool relocate = true);

   /**
    * The size of the array, if this is an array. For example "vec3 foo" is not
    * an array (size == 1), it's only a vector, when "vec3 foo[2]" or
    * "float bar[2]" both are arrays of size two.
    */
  size_t arraySize() const { return m_size; }

  /// The name of the uniform variable
  QString name() const { return m_name; }

  /// Return the correct type info object for this variable.
  const ShaderTypeInfo& typeinfo() const;

  bool operator==(const UniformVar& other) const;

  /// recommended lower limit to the values in this uniform
  /// @todo implement
  float min() const { return 0.0f; }
  /// recommended upper limit to the values in this uniform
  /// @todo implement
  float max() const { return 1.0f; }

private:
  /// The name of the uniform variable
  QString m_name;

  /// Uniform variable type, see ShaderTypeInfo::type
  GLenum m_type;

  /// @see arraySize()
  size_t m_size;

  /// The shader program that was given in constructor.
  ProgramPtr m_prog;

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

  /// Name of the uniform block where this variable belongs to.
  /// Empty string means the default block.
  QString m_uniform_block;

  /// Is this a built-in "gl_*" uniform. Currently the class doesn't know
  /// how to handle those correctly.
  bool m_builtin;
};

#undef GLAPIENTRY
#define GLAPIENTRY

#endif
