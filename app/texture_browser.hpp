#ifndef TEXTURE_BROWSER_HPP
#define TEXTURE_BROWSER_HPP

#include "forward.hpp"
#include "buffer_object.hpp"

#include <QDialog>
#include <QMap>
#include <QGLWidget>
#include <QLayout>

namespace Ui {
class TextureBrowser;
}
class QFrame;
class QLabel;

class TextureWidgetGL : public QGLWidget {
  Q_OBJECT

public:
  TextureWidgetGL(QWidget* parent, const QGLWidget* shared, TexturePtr tex);
  void setTexture(TexturePtr tex);
  TexturePtr tex() const { return m_tex; }

  int heightForWidth(int w) const;

  void enterEvent(QEvent* e);
  void leaveEvent(QEvent* e);
  void mouseDoubleClickEvent(QMouseEvent* e);

  TextureWidgetGL* preview(Qt::WindowFlags f = 0, QSize size = QSize(192, 192)) const;

signals:
  void hoverBegin();
  void hoverEnd();

protected:
  void initializeGL();
  void paintGL();
  void resizeGL(int w, int h);

private:
  /// @todo this could be shared between all texture widgets..
  BufferObject m_vertices, m_uv0;
  TexturePtr m_tex;
};

class TextureWidget : public QWidget {
  Q_OBJECT

public:
  TextureWidget(QWidget* parent, TexturePtr tex);

  TextureWidgetGL* gl() const { return m_gl; }
  void setSelected(bool s);
  void setTexture(TexturePtr tex);
  TexturePtr tex() const { return m_tex; }

signals:
  void select(TextureWidget*);

protected:
  void paintEvent(QPaintEvent*);
  void mouseReleaseEvent(QMouseEvent*);

private:
  TextureWidgetGL* m_gl;
  TexturePtr m_tex;
  QFrame* m_frame;
};

class TextureBrowser : public QWidget {
  Q_OBJECT

public:
  ~TextureBrowser();

  static TextureBrowser& instance();

  void show();

public slots:
  void selected(TextureWidget*, bool force = false);
  void updateContent(ScenePtr s = ScenePtr());

private slots:
  void paramChanged(QString = "");
  void newParam(QString);
  void newScene(ScenePtr);
  void renderPassesChanged();

  void create();
  void duplicate();
  void remove();
  void load();

  void filenameChanged();
  void browse();

  void changeRenderBuffer();
  void updateAttachmentIcons(FBOPtr fbo);

private:
  explicit TextureBrowser(QWidget* parent = 0);

  QString selectFile(QString tip);

  Ui::TextureBrowser *m_ui;
  TextureWidget* m_selected;
  TexturePtr m_select;

  QAction *m_create, *m_duplicate, *m_destroy, *m_open;

  QMap<QString, TextureWidget*> m_textures;
};

class FlowLayout : public QLayout
{
  Q_OBJECT

public:
  FlowLayout(QWidget* parent = 0);
  virtual ~FlowLayout();

  virtual QSize minimumSize() const;
  virtual QSize sizeHint() const;
  virtual void addItem(QLayoutItem*);
  virtual QLayoutItem* itemAt(int index) const;
  virtual QLayoutItem* takeAt(int index);
  virtual int count() const;
  void setGeometry(const QRect& r);

private:
  QSizeF m_itemsize;
  int m_minHeight;
  QList<QLayoutItem*> m_items;
};

#endif // TEXTURE_BROWSER_HPP
