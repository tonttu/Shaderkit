#ifndef GLSL_EDITOR_HPP
#define GLSL_EDITOR_HPP

#include "forward.hpp"

#include "parser/glsl_parser.hpp"

#include <QPlainTextEdit>

namespace Shaderkit
{

  /**
   * GLSL code editor.
   *
   * @todo Split Editor to GLSLEditor and Editor, so we could make plain text editor,
   *       JSON editor and maybe even C++ editor.
   */
  class GLSLEditor : public QTextEdit
  {
    Q_OBJECT

  public:
    GLSLEditor(MultiEditor& me, QWidget* parent, ShaderPtr shader, QTextDocument* doc);
    virtual ~GLSLEditor();

    /// Renders the line numbers.
    void marginPaintEvent(QPaintEvent* event);

    /// Margin width, that is, the width of the are with line numbers.
    int marginWidth() const;

    /// Opens the editor with the contents of file.
    //void readFile(const QString& res);

    /// The file was changed on the disk.
    //void fileUpdated(const QString& res);

    /// Forgets all compile errors.
    void clearErrors();

    /// We capture all events on the viewport to be able to show smart tooltips.
    virtual bool viewportEvent(QEvent* event);

    /// Return the shader the object is editing.
    ShaderPtr shader() { return m_shader; }

    void save();

    bool checkClose();

  protected:
    /// Capture the resize event to set the margin geometry correctly.
    void resizeEvent(QResizeEvent* event);

    /// We need to handle page up/down events manually because of the weird scroll layout
    void keyPressEvent(QKeyEvent* event);

    void focusInEvent(QFocusEvent* event);
    void focusOutEvent(QFocusEvent* event);

    void scrollPage(bool up, bool moveAnchor);

  public slots:
    /// Adds a new error to list and sets the error text styles to correct place.
    void compileError(const ShaderError& e);
    /// For internal use, we wan't to separate real text changes from format changes.
    void textChangedSlot();

  private slots:
    /// Called when line number is changed
    void updateMarginWidth(int blockCount);
    /// When the text cursor moves, this is called and the line is highlighted
    void highlightCurrentLine();
    /// Called when we should use m_parser to update stuff
    void process();

  private:
    MultiEditor& m_multiEditor;
    EditorMargin* m_margin;
    /// Syntax Highlight provider
    Highlighter* m_highlighter;

    AfterIdleOperation* m_process_timeout;
    GLSLParser m_parser;

    ShaderPtr m_shader;

    /// Extra selections are used to highlight the current line and show
    /// compiler errors. When m_currentLineSelection or m_errorSelections etc
    /// are changed, this should be called to update the change to editor.
    void updateExtraSelections();

    QTextEdit::ExtraSelection m_currentLineSelection;
    QList<QTextEdit::ExtraSelection> m_errorSelections;
    QList<QTextEdit::ExtraSelection> m_warningSelections;
    QList<QTextEdit::ExtraSelection> m_commentedSelections;

    /// All lines that have an error, starting from zero
    QSet<int> m_errorLines;
    /// All lines that have a warning, starting from zero
    QSet<int> m_warningLines;

    /// All errors and their locations
    QMap<ShaderError, QTextCursor> m_errors;

    /// Currently for every keypress we are updating the whole data to this
    /// variable to be able to actually separate correct codeChange
    /// signals from all of those textChanged-signals.
    /// @todo isn't there any better way to do this?
    QString m_lastdata;

    int m_marginWidth;
  };

} // namespace Shaderkit

#endif // GLSL_EDITOR_HPP
