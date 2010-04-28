#include "shader/compiler_output_parser.hpp"

#include <cassert>

/**
 * NVIDIA:
 *  0(11) : warning C7522: OpenGL requires constants to be initialized
 *  0(11) : error C0000: syntax error, unexpected identifier, expecting ',' or ';' at token "foo"
 */
ShaderCompilerOutputParser::ShaderCompilerOutputParser(QString compiler_output) : m_pos(0) {
  QString pattern;
  if (true /* 1.50 NVIDIA via Cg compiler */) {
    pattern = "\\d+ \\(  (\\d+)  \\)"  // "0(11)", shader(line number [1])
              "\\s* : \\s*"            // ":", separator
              "([^\\s]+)  \\s+"        // "warning", type [2]
              "[^\\s:]+"               // "C7522", nvidia error code
              "\\s* : \\s*"            // ":", separator
              "(.*)";                  // the actual error [3]
  }
  pattern.remove(QChar(' '));
  m_regexp = QRegExp(pattern, Qt::CaseSensitive, QRegExp::RegExp2);
  m_lines = compiler_output.split(QRegExp("[\\r\\n]+"), QString::SkipEmptyParts);
}

bool ShaderCompilerOutputParser::left() {
  return m_pos < m_lines.size();
}

ShaderError ShaderCompilerOutputParser::next() {
  m_regexp.indexIn(m_lines[m_pos++]);
  QStringList list = m_regexp.capturedTexts();

  /// @todo assert is way too strong for this, fix this
  assert(list.size() == 4);

  return ShaderError(list[3], list[2], list[1].toInt());
}

