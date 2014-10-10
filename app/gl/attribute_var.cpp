#include "attribute_var.hpp"

namespace Shaderkit
{

  AttributeVar::AttributeVar()
    : m_type(0), m_builtin(false) {}

  AttributeVar::AttributeVar(ProgramPtr prog, QString name, GLenum type)
    : m_name(name), m_type(type), m_prog(prog), m_builtin(name.startsWith("gl_"))
  {
  }

  const ShaderTypeInfo&AttributeVar::typeinfo() const
  {
    return ShaderTypeInfo::typeInfo(m_type);
  }

  bool AttributeVar::operator==(const AttributeVar& other) const
  {
    return m_name == other.m_name &&
           m_type == other.m_type &&
           m_prog == other.m_prog &&
           m_builtin == other.m_builtin;
  }

  QList<VarGroupDescription> AttributeVar::builtInVars()
  {
    static QList<VarGroupDescription> s_vars;
    if (s_vars.isEmpty()) {
      VarGroupDescription g("mesh", "Mesh");
      g("vertex", "Vertex coordinate", "vec3")
          ("normal", "Normal vector", "vec3")
          ("tangent", "Tangent vector", "vec3")
          ("bitangent", "Bitangent vector", "vec3")
          ("color", "Vertex color", "vec3")
          ("uv", "UV coordinate", "vec1..4");
      s_vars << g;
    }
    return s_vars;
  }

  /*vec4 gl_Color
  vec4 gl_SecondaryColor
  vec3 gl_Normal
  vec4 gl_Vertex
  vec4 gl_MultiTexCoord0
  vec4 gl_MultiTexCoord1
  vec4 gl_MultiTexCoord2
  vec4 gl_MultiTexCoord3
  vec4 gl_MultiTexCoord4
  vec4 gl_MultiTexCoord5
  vec4 gl_MultiTexCoord6
  vec4 gl_MultiTexCoord7
  float gl_FogCoord*/

} // namespace Shaderkit
