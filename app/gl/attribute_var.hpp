#ifndef ATTRIBUTE_VAR_HPP
#define ATTRIBUTE_VAR_HPP

#include "forward.hpp"
#include "gl/opengl.hpp"

#include <QVector>

namespace Shaderkit {

class AttributeVar {
public:
  typedef QVector<AttributeVar> List;

  AttributeVar();

  AttributeVar(ProgramPtr prog, QString name, GLenum type);
  //void set(ProgramPtr prog = ProgramPtr(), bool relocate = true);

  //bool set(float value, size_t array_idx = 0, size_t vector_idx = 0,
  //    ProgramPtr prog = ProgramPtr(), bool relocate = true);

   /**
    * The size of the array, if this is an array. For example "vec3 foo" is not
    * an array (size == 1), it's only a vector, when "vec3 foo[2]" or
    * "float bar[2]" both are arrays of size two.
    */
  //size_t arraySize() const { return m_size; }

  /// The name of the uniform variable
  QString name() const { return m_name; }

  bool operator==(const AttributeVar& other) const;

private:
  QString m_name;

  GLenum m_type;

  /// @see arraySize()
//  size_t m_size;

  ProgramPtr m_prog;

//  QVector<GLint> m_location;

  /// Is this a built-in "gl_*" attribute.
  bool m_builtin;
};

} // namespace Shaderkit

#endif // ATTRIBUTE_VAR_HPP
