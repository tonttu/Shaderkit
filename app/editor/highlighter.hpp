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

#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include "forward.hpp"

#include <QSyntaxHighlighter>

namespace Shaderkit
{

  /**
   * GLSL Highlighter that uses the flex lexer to tokenize the input and then
   * apply some colors to specific token groups.
   *
   * This doesn't do any fancy parsing of the file, all kind of context-aware
   * formatting is done separately on the Editor.
   */
  class Highlighter : public QSyntaxHighlighter
  {
    Q_OBJECT

  public:
    Highlighter(QTextDocument* parent, GLSLParser& parser);
    virtual ~Highlighter();

  protected:
    /// Highlights one block (== one line)
    void highlightBlock(const QString& text);

    /// @todo this should load a config file
    void loadConfig();

    QTextCharFormat m_errorFormat, m_commentFormat, m_ppFormat, m_typeFormat,
                    m_qualifierFormat, m_keywordFormat, m_constantFormat, m_operatorFormat,
                    m_parenthesesFormat, m_buildinVarFormat, m_idFormat;

  private:
    GLSLParser& m_parser;
  };

} // namespace Shaderkit

#endif // HIGHLIGHTER_H
