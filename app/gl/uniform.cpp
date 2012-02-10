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

#include "gl/uniform.hpp"
#include "gl/program.hpp"
#include "gl/opengl.hpp"

#include <stdexcept>
#include <cassert>

ShaderTypeInfo::ShaderTypeInfo(int size_,
    FloatSetter float_setter_, IntSetter int_setter_, MatrixSetter matrix_setter_,
    GLenum type_, GLenum single_type_, int flags)
  : type(type_), base_type(single_type_), size(size_),
    is_sampler(flags & 0x01),
    float_setter(float_setter_), int_setter(int_setter_), matrix_setter(matrix_setter_) {}

QVariant::Type ShaderTypeInfo::variant() const {
  if (base_type == GL_FLOAT) return QVariant::Double;
  if (base_type == GL_INT) return QVariant::Int;
  if (base_type == GL_BOOL) return QVariant::Bool;
  return QVariant::Invalid;
}

const ShaderTypeInfo& ShaderTypeInfo::typeInfo(GLenum type) {
  static QMap<GLenum, ShaderTypeInfo> types;
  typedef ShaderTypeInfo P;

  if (types.empty()) {
    // Add all known uniform types to the list
    /// @todo this is OpenGL 2.0 list, update the list with OpenGL 3.2 or ever newer.
    types[GL_FLOAT]             = P(1, glUniform1fv, 0, 0, GL_FLOAT, GL_FLOAT);
    types[GL_FLOAT_VEC2]        = P(2, glUniform2fv, 0, 0, GL_FLOAT_VEC2, GL_FLOAT);
    types[GL_FLOAT_VEC3]        = P(3, glUniform3fv, 0, 0, GL_FLOAT_VEC3, GL_FLOAT);
    types[GL_FLOAT_VEC4]        = P(4, glUniform4fv, 0, 0, GL_FLOAT_VEC4, GL_FLOAT);
    types[GL_INT]               = P(1, 0, glUniform1iv, 0, GL_INT, GL_INT);
    types[GL_INT_VEC2]          = P(2, 0, glUniform2iv, 0, GL_INT_VEC2, GL_INT);
    types[GL_INT_VEC3]          = P(3, 0, glUniform3iv, 0, GL_INT_VEC3, GL_INT);
    types[GL_INT_VEC4]          = P(4, 0, glUniform4iv, 0, GL_INT_VEC4, GL_INT);
    types[GL_BOOL]              = P(1, 0, glUniform1iv, 0, GL_BOOL, GL_BOOL);
    types[GL_BOOL_VEC2]         = P(2, 0, glUniform2iv, 0, GL_BOOL_VEC2, GL_BOOL);
    types[GL_BOOL_VEC3]         = P(3, 0, glUniform3iv, 0, GL_BOOL_VEC3, GL_BOOL);
    types[GL_BOOL_VEC4]         = P(4, 0, glUniform4iv, 0, GL_BOOL_VEC4, GL_BOOL);
    types[GL_FLOAT_MAT2]        = P(4, 0, 0, glUniformMatrix2fv, GL_FLOAT_MAT2, GL_FLOAT);
    types[GL_FLOAT_MAT3]        = P(9, 0, 0, glUniformMatrix3fv, GL_FLOAT_MAT3, GL_FLOAT);
    types[GL_FLOAT_MAT4]        = P(16, 0, 0, glUniformMatrix4fv, GL_FLOAT_MAT4, GL_FLOAT);
    types[GL_FLOAT_MAT2x3]      = P(6, 0, 0, glUniformMatrix2x3fv, GL_FLOAT_MAT2x3, GL_FLOAT);
    types[GL_FLOAT_MAT2x4]      = P(8, 0, 0, glUniformMatrix2x4fv, GL_FLOAT_MAT2x4, GL_FLOAT);
    types[GL_FLOAT_MAT3x2]      = P(6, 0, 0, glUniformMatrix3x2fv, GL_FLOAT_MAT3x2, GL_FLOAT);
    types[GL_FLOAT_MAT3x4]      = P(12, 0, 0, glUniformMatrix3x4fv, GL_FLOAT_MAT3x4, GL_FLOAT);
    types[GL_FLOAT_MAT4x2]      = P(8, 0, 0, glUniformMatrix4x2fv, GL_FLOAT_MAT4x2, GL_FLOAT);
    types[GL_FLOAT_MAT4x3]      = P(12, 0, 0, glUniformMatrix4x3fv, GL_FLOAT_MAT4x3, GL_FLOAT);
    types[GL_SAMPLER_1D]        = P(1, 0, glUniform1iv, 0, GL_SAMPLER_1D, GL_INT, 1);
    types[GL_SAMPLER_2D]        = P(1, 0, glUniform1iv, 0, GL_SAMPLER_2D, GL_INT, 1);
    types[GL_SAMPLER_3D]        = P(1, 0, glUniform1iv, 0, GL_SAMPLER_3D, GL_INT, 1);
    types[GL_SAMPLER_CUBE]      = P(1, 0, glUniform1iv, 0, GL_SAMPLER_CUBE, GL_INT, 1);
    types[GL_SAMPLER_1D_SHADOW] = P(1, 0, glUniform1iv, 0, GL_SAMPLER_1D_SHADOW, GL_INT, 1);
    types[GL_SAMPLER_2D_SHADOW] = P(1, 0, glUniform1iv, 0, GL_SAMPLER_2D_SHADOW, GL_INT, 1);

    // error value
    types[GL_FALSE]             = P();
  }

  auto it = types.find(type);
  if (it != types.end()) return *it;
  return types[GL_FALSE];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UniformVar::UniformVar() : m_type(0), m_size(0), m_builtin(false) {}

UniformVar::UniformVar(ProgramPtr prog, QString name, GLenum type)
    : m_name(name), m_type(type), m_size(1), m_prog(prog), m_builtin(name.startsWith("gl_")) {
  const ShaderTypeInfo& info = typeinfo();
  if (info.size <= 0) throw std::runtime_error("Unknown Uniform variable type");
  glCheck("UniformVar");

  // built-in var, we can't set/get it with this class.
  if (m_builtin) return;

  /// @todo don't use assert
  assert(glRun2(glIsProgram(prog->id())));

  GLint b = 0;
  glRun(glGetProgramiv(prog->id(), GL_LINK_STATUS, &b));
  /// @todo don't use assert
  assert(b);

  m_location.push_back(glRun2(glGetUniformLocation(prog->id(), name.toStdString().c_str())));

  /// @todo Do we know if the array locations are in adjacent locations?
  ///       location[n] seems to be always location[0] + n, and glUniform*v
  ///       -functions allow changing the whole array with only the location
  ///       of the first element.
  for (int i = 1;; ++i) {
    GLint l = glRun2(glGetUniformLocation(prog->id(),
        (name + '[' + QString::number(i) + ']').toStdString().c_str()));
    if (l >= 0) {
      m_location.push_back(l);
      ++m_size;
    } else break; // the end of the array, or not an array at all
  }

  if (info.base_type == GL_FLOAT) {
    // For example vec3[6] needs 6*3 floats
    m_floatdata.resize(m_size * info.size);
    for (size_t i = 0; i < m_size; ++i) {
      glRun(glGetUniformfv(prog->id(), m_location[i], &m_floatdata.front() + i * info.size));
    }
  } else {
    m_intdata.resize(m_size * info.size);
    for (size_t i = 0; i < m_size; i++) {
      glRun(glGetUniformiv(prog->id(), m_location[i], &m_intdata.front() + i * info.size));
    }
  }
}

void UniformVar::set(ProgramPtr prog, bool relocate) {
  glCheck("UniformVar::set");
  const ShaderTypeInfo& info = typeinfo();

  // built-in var, we can't set/get it with this class.
  if (m_builtin) return;

  if (!prog) prog = m_prog;
  if (!m_prog) return;

  /// @todo should we set every array element one by one?
  ///       Things wouldn't break so easy if the array size changes.
  /*
  for (int i = 0; i < m_size; ++i) {
    GLint loc = m_location[i];
    if (relocate) loc = glGetUniformLocation(prog->id(), (i == 0 ? m_name :
        m_name + '[' + QString::number(i) + ']').toStdString().c_str());

    if (loc == -1) continue;
  }
  */

  GLint loc = m_location[0];
  if (relocate) loc = glRun2(glGetUniformLocation(prog->id(), m_name.toStdString().c_str()));

  if (loc == -1) return;

  GLint oldprog = 0;
  glRun(glGetIntegerv(GL_CURRENT_PROGRAM, &oldprog));
  if (oldprog != GLint(prog->id())) prog->bind(0);

  if (info.matrix_setter) {
    glRun(info.matrix_setter(loc, m_size, 0, &m_floatdata.front()));
  } else if (info.float_setter) {
    assert(!m_floatdata.empty());
    glRun(info.float_setter(loc, m_size, &m_floatdata.front()));
  } else {
    assert(!m_intdata.empty());
    assert(info.int_setter);
    glRun(info.int_setter(loc, m_size, &m_intdata.front()));
  }

  if (oldprog != GLint(prog->id())) {
    glRun(glUseProgram(oldprog));
  }
}
/*
QVariant UniformVar::get(size_t array_idx, size_t vector_idx) {
  const ShaderTypeInfo& info = typeinfo();

  // built-in var, we can't set/get it with this class.
  if (m_builtin) return QVariant();

  QVariant qv(info.variant());

  if (info.base_type == GL_FLOAT)
    qv.setValue(m_floatdata[info.size * array_idx + vector_idx]);
  else
    qv.setValue(m_intdata[info.size * array_idx + vector_idx]);
  return qv;
}

bool UniformVar::set(QVariant value, size_t array_idx, size_t vector_idx,
    ProgramPtr prog, bool relocate) {
  const ShaderTypeInfo& info = typeinfo();

  // built-in var, we can't set/get it with this class.
  if (m_builtin) return false;

  if (info.base_type == GL_FLOAT)
    m_floatdata[info.size * array_idx + vector_idx] = value.toFloat();
  else
    m_intdata[info.size * array_idx + vector_idx] = value.toInt();

  set(prog, relocate);
  /// @todo actually check for errors
  return true;
}*/

float UniformVar::get(size_t array_idx, size_t vector_idx) {
  const ShaderTypeInfo& info = typeinfo();

  // built-in var, we can't set/get it with this class.
  if (m_builtin) return std::numeric_limits<float>::quiet_NaN();

  if (info.base_type == GL_FLOAT)
    return m_floatdata[info.size * array_idx + vector_idx];
  else
    return m_intdata[info.size * array_idx + vector_idx];
}

bool UniformVar::set(float value, size_t array_idx, size_t vector_idx,
    ProgramPtr prog, bool relocate) {
  const ShaderTypeInfo& info = typeinfo();

  // built-in var, we can't set/get it with this class.
  if (m_builtin) return false;

  if (info.base_type == GL_FLOAT)
    m_floatdata[info.size * array_idx + vector_idx] = value;
  else
    m_intdata[info.size * array_idx + vector_idx] = int(value);

  set(prog, relocate);
  /// @todo actually check for errors
  return true;
}

const ShaderTypeInfo& UniformVar::typeinfo() const {
  return ShaderTypeInfo::typeInfo(m_type);
}

bool UniformVar::operator==(const UniformVar& other) const {
  return m_name == other.m_name &&
         m_type == other.m_type &&
         m_size == other.m_size &&
         m_prog == other.m_prog &&
         m_location == other.m_location &&
         m_floatdata == other.m_floatdata &&
         m_intdata == other.m_intdata &&
         m_uniform_block == other.m_uniform_block &&
         m_builtin == other.m_builtin;
}
