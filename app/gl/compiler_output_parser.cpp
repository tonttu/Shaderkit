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

#include "gl/compiler_output_parser.hpp"
#include "gl/shader.hpp"
#include "gl/opengl.hpp"

#include "parser/grammar.hpp"

#include <QMap>

#include <cassert>

namespace Shaderkit
{

  bool ParserImpl::hasColumnInformation() const
  {
    return false;
  }

  class SimpleParser : public ParserImpl
  {
  public:
    virtual bool parse(const QStringList& lines, ShaderErrorList& errors);

  protected:
    QList<QRegExp> m_patterns;
    QList<QRegExp> m_modes;
    QList<QRegExp> m_ignore;
    QList<QRegExp> m_generic;

    virtual bool handleLine(const QString&) { return false; }
  };

  bool SimpleParser::parse(const QStringList& lines, ShaderErrorList& errors)
  {
    bool failed = false;
    foreach (const QString& msg, lines) {
      bool found = handleLine(msg);
      if (found) continue;

      for (int i = 0; i < m_ignore.size(); ++i) {
        if (m_ignore[i].exactMatch(msg)) {
          found = true;
          break;
        }
      }
      if (found) continue;

      QString mode;
      for (int i = 0; i < m_modes.size(); ++i) {
        if (m_modes[i].exactMatch(msg)) {
          mode = m_modes[i].cap(1);
          found = true;
          if (i != 0) {
            QRegExp r = m_modes[i];
            m_modes.removeAt(i);
            m_modes.insert(0, r);
          }
          break;
        }
      }
      if (found) continue;

      QStringList list;
      for (int i = 0; i < m_patterns.size(); ++i) {
        if (m_patterns[i].exactMatch(msg)) {
          list = m_patterns[i].capturedTexts();
          if (i != 0) {
            QRegExp r = m_patterns[i];
            m_patterns.removeAt(i);
            m_patterns.insert(0, r);
          }
        }
      }

      if (list.size() == 4) {
        QString str = mode.isEmpty() ? list[3] : list[3] + " (" + mode + ")";
        errors << ShaderError(str, list[2], list[1].toInt()-1);
      } else {
        bool ok = false;
        for (int i = 0; i < m_generic.size(); ++i) {
          if (m_generic[i].exactMatch(msg)) {
            list = m_generic[i].capturedTexts();
            if (list.size() == 3)
              errors << ShaderError(list[1], list[2], -1);
            else
              errors << ShaderError(msg, "error", -1);
            ok = true;
            break;
          }
        }

        if (!ok) {
          Log::error("Failed to parse error string: '%s'", msg.toUtf8().data());
          failed = true;
          continue;
        }
      }
    }
    return !failed;
  }


  class NvidiaParser : public SimpleParser
  {
  public:
    NvidiaParser();
  };

// 0(11) : warning C7522: OpenGL requires constants to be initialized
// 0(11) : error C0000: syntax error, unexpected identifier, expecting ',' or ';' at token "foo"
//   (0) : error C0000: syntax error, unexpected $end at token "<EOF>"
  NvidiaParser::NvidiaParser()
  {
    QString pattern;

    // 1.50 NVIDIA via Cg compiler
    pattern = "\\s*\\d* \\( (\\d+) \\)" // "0(11)", shader(line number [1])
              "\\s* : \\s*"             // ":", separator
              "([^\\s]+)  \\s+"         // "warning", type [2]
              "[^\\s:]+"                // "C7522", nvidia error code
              "\\s* : \\s*"             // ":", separator
              "(.*)";                   // the actual error [3]
    pattern.remove(QChar(' '));
    m_patterns << QRegExp(pattern, Qt::CaseSensitive, QRegExp::RegExp2);

    pattern = "\\s*(\\w*)\\s+info";
    m_modes << QRegExp(pattern, Qt::CaseInsensitive, QRegExp::RegExp2);

    pattern = "\\s*-*\\s*";
    m_ignore << QRegExp(pattern, Qt::CaseSensitive, QRegExp::RegExp2);
  }

  class FglrxParser : public SimpleParser
  {
  public:
    FglrxParser();
  };

  FglrxParser::FglrxParser()
  {
    QString pattern;

    // 1.50 ATI
    // ERROR: 0:3: error(#132) Syntax error: 'preciswion' parse error

    pattern = "\\s*(?:ERROR|WARNING):\\s+"   // ERROR:
              "\\d+ : (\\d+)"                // "0:3:", shader:line [1]:
              "\\s* : \\s*"                  // ":", separator
              "([a-z]+)"                     // "error", type [2]
              "\\([^\\)]+\\)\\s*"            // "(#132)" error code
              "(.*)";                        // the actual error [3]
    pattern.remove(QChar(' '));
    m_patterns << QRegExp(pattern, Qt::CaseSensitive, QRegExp::RegExp2);

    pattern = ".* shader was successfully compiled .*";
    m_ignore << QRegExp(pattern, Qt::CaseSensitive, QRegExp::RegExp2);

    pattern = ".* shader\\(s\\) linked.*";
    m_ignore << QRegExp(pattern, Qt::CaseSensitive, QRegExp::RegExp2);

    pattern = ".* shader failed to compile with the following errors.*";
    m_ignore << QRegExp(pattern, Qt::CaseSensitive, QRegExp::RegExp2);

    // ERROR: error(#273) 1 compilation errors.  No code generated
    // ERROR: 0:? : error(#167) Only allowed at global scope uniform
    pattern = "(?:ERROR|WARNING):\\s*(?:\\d:\\?\\s*)?((error|warning)\\(#\\d+\\).*)";
    m_generic << QRegExp(pattern, Qt::CaseSensitive, QRegExp::RegExp2);
  }

  class MesaParser : public SimpleParser
  {
  public:
    MesaParser();
  };

  MesaParser::MesaParser()
  {
    QString pattern;

    // 0:8(1): error: syntax error, unexpected XOR_ASSIGN, expecting ',' or ';'
    // 0:1(15): preprocessor error: syntax error, unexpected IDENTIFIER, expecting NEWLINE

    pattern = "\\s*\\d+ : (\\d+) \\(\\d+\\)" // "0:8(1)", shader:line [1] (column)
              "\\s* : \\s*"                  // ":", separator
              "(?:[a-z]+ \\s+)?"             // "preprocessor ", optional
              "([^\\s]+)"                    // "warning", type [2]
              "\\s* : \\s*"                  // ":", separator
              "(.*)";                        // the actual error [3]
    pattern.remove(QChar(' '));
    m_patterns << QRegExp(pattern, Qt::CaseSensitive, QRegExp::RegExp2);
  }

  ShaderCompilerOutputParser::ShaderCompilerOutputParser()
  {
    /// @todo initialize the scripting context here and load parser plugins. maybe.

    /// @todo load these better
    m_parsers << std::make_shared<NvidiaParser>();
    m_parsers << std::make_shared<FglrxParser>();
    m_parsers << std::make_shared<MesaParser>();
  }

  ShaderCompilerOutputParser& ShaderCompilerOutputParser::instance()
  {
    static ShaderCompilerOutputParser s_instance;
    return s_instance;
  }

  QStringList ShaderCompilerOutputParser::split(const QString& str) const
  {
    return str.split(QRegExp("\\s*[\\r\\n]+\\s*"), QString::SkipEmptyParts);
  }

  bool ShaderCompilerOutputParser::parse(const QString& output, ShaderErrorList& errors)
  {
    QStringList lines = split(output);
    for (int i = 0, e = m_parsers.size(); i < e; ++i) {
      std::shared_ptr<ParserImpl> impl = m_parsers[i];
      assert(impl);

      ShaderErrorList tmp;
      if (impl->parse(lines, tmp)) {
        errors += tmp;

        if (i != 0) {
          /// This seems to be the best parser implementation, always prefer this
          m_parsers.removeAll(impl);
          m_parsers.insert(0, impl);
        }
        return true;
      }
    }
    return false;
  }

  bool ShaderCompilerOutputParser::parse(Shader& shader, ShaderErrorList& errors)
  {
    ShaderLexer lexer;
    QMap<int, ShaderErrorList> errorLists;
    bool parsedOk = false;
    for (int i = 0, e = m_parsers.size(); i < e; ++i) {
      std::shared_ptr<ParserImpl> impl = m_parsers[i];
      assert(impl);
      std::vector<GLchar> log;
      GLint len = 0;

      if (!impl->hasColumnInformation() && lexer.tokens() == 0) {
        // Split the source tokens to lines so that we can find the exact error location
        lexer.loadSrc(shader.src());
        const std::string& data = lexer.toLines();
        const GLchar* str = data.c_str();
        len = data.length();

        // Recompile
        /// @todo Should use sandbox compiler also here
        glRun(glShaderSource(shader.id(), 1, &str, &len));
        glRun(glCompileShader(shader.id()));
      }

      glRun(glGetShaderiv(shader.id(), GL_INFO_LOG_LENGTH, &len));

      // Usually this shouldn't happen, since this function is called only
      // when there actually are some errors in the source code.
      if (len <= 1) {
        Log::error("CompilerOutputParser, tokenized shader gives weird results");
        continue;
      }

      log.resize(len);
      GLsizei size = len;
      glRun(glGetShaderInfoLog(shader.id(), size, &size, &log[0]));

      QStringList lines = split(QString::fromUtf8(&log[0], size));

      ShaderErrorList tmp;
      if (impl->parse(lines, tmp)) {
        errorLists.clear();
        errorLists[tmp.size()] = tmp;
        if (i != 0) {
          /// This seems to be the best parser implementation, always prefer this
          m_parsers.removeAll(impl);
          m_parsers.insert(0, impl);
        }
        parsedOk = true;
        break;
      }
      if (!tmp.isEmpty())
        errorLists[tmp.size()] = tmp;
    }

    if (!errorLists.isEmpty()) {
      foreach (ShaderError e, errorLists.values().last()) {
        // instead of using the original error log, we have used lexer to split
        // the output to lines, therefore we need to map these line number to
        // original line/column numbers
        int l = lexer.tokens();
        if (l) {
          if (e.line() >= l) {
            Log::error("BUG on Shader::handleCompilerOutput, e.line: %d, l: %d, data: %s, src: %s",
                       e.line(), l, lexer.toLines().c_str(), shader.src().toUtf8().data());
          } else {
            const ShaderLexer::Token& token = lexer.transform(e.line());
            e.setLine(token.line);
            e.setColumn(token.column);
            e.setLength(token.len);
          }
        }
        errors << e;
      }
    }

    return parsedOk;
  }

} // namespace Shaderkit
