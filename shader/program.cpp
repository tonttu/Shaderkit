#include "shader/program.hpp"
#include "shader/shader.hpp"

GLProgram::GLProgram(const QString& name)
    : m_name(name), m_prog(0), m_compiled(false) {
/*  connect(this, SIGNAL(linked(ProgramPtr)),
          &Properties::instance(), SLOT(update(ProgramPtr)));*/
}

void GLProgram::bind() {
  if (!m_compiled) {
    if (!m_prog) {
      m_prog = new QGLShaderProgram(this);
      /*m_prog->setGeometryInputType(GL_TRIANGLES);
      m_prog->setGeometryOutputType(GL_TRIANGLE_STRIP);
      m_prog->setGeometryOutputVertexCount(1024);*/
    }

    for (Shaders::iterator it = m_shaders.begin(); it != m_shaders.end(); ++it) {
      ShaderError::List errors;
      Shader::CompileStatus status = (*it)->compile(errors);
      if (status != Shader::NONE) emit shaderCompiled(*it, errors);
      if (status == Shader::OK || status == Shader::WARNINGS)
        m_prog->addShader(**it);
    }

    m_compiled = true;
    /// @todo should we link only when there was a successful compiling?
    link();
  }
  if (m_prog->isLinked())
    m_prog->bind();
}

void GLProgram::unbind() {
  if (m_prog) m_prog->release();
}

void GLProgram::setUniform(UniformVar::List list, bool relocate) {
  for (UniformVar::List::iterator it = list.begin(); it != list.end(); ++it) {
    it->set(*m_prog, relocate);
  }
}

ShaderPtr GLProgram::addShader(const QString& filename, QGLShader::ShaderTypeBit type) {
  ShaderPtr shader(new Shader(shared_from_this(), type));
  shader->loadFile(filename);
  m_shaders.insert(shader);

  return shader;
}

void GLProgram::link() {
  GLint prog = 0;
  glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
  if (m_prog->isLinked()) {
    m_prog->bind(); /// @todo Do we need this?
    m_uniformList = getUniformList();
  }

  bool ok = m_prog->link();

  if (ok) {
    m_prog->bind(); /// @todo Do we need this?
    setUniform(m_uniformList);
    emit linked(shared_from_this());
  }

  glUseProgram(prog);
}

UniformVar::List GLProgram::getUniformList() {
  UniformVar::List list;

  GLint num = 0, buffer_size = 0;
  glGetProgramiv(m_prog->programId(), GL_ACTIVE_UNIFORMS, &num);
  glGetProgramiv(m_prog->programId(), GL_ACTIVE_UNIFORM_MAX_LENGTH, &buffer_size);

  for (GLint i = 0; i < num; i++) {
    GLchar name[buffer_size];
    GLsizei length;
    GLint size;
    GLenum type;
    glGetActiveUniform(m_prog->programId(), i, buffer_size,
                       &length, &size, &type, name);

    list.push_back(UniformVar(*m_prog, name, type));
  }
  return list;
}
