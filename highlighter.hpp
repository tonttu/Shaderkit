#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>

/**
 * GLSL Highlighter that uses the flex lexer to tokenize the input and then
 * apply some colors to specific token groups.
 *
 * This doesn't do any fancy parsing of the file, all kind of context-aware
 * formatting is done separately on the Editor.
 *
 * @todo handle multiline c-comments. Need to use one state variable for that.
 */
class Highlighter : public QSyntaxHighlighter {
  Q_OBJECT

public:
  Highlighter(QTextDocument* parent = 0);

protected:
  /// Highlights one block (== one line)
  void highlightBlock(const QString& text);

  /// @todo this should load a config file
  void loadConfig();

  QTextCharFormat m_errorFormat, m_commentFormat, m_ppFormat, m_typeFormat,
    m_qualifierFormat, m_keywordFormat, m_constantFormat, m_operatorFormat,
    m_parenthesesFormat, m_buildinVarFormat, m_idFormat;
};

#endif // HIGHLIGHTER_H
