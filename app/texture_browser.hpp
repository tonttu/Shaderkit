#ifndef TEXTURE_BROWSER_HPP
#define TEXTURE_BROWSER_HPP

#include "forward.hpp"

#include <QDialog>
#include <QMap>
#include <QGLWidget>
#include <QLayout>

namespace Ui {
class TextureBrowser;
}
class QFrame;

class TextureWidgetGL : public QGLWidget {
  Q_OBJECT

public:
  TextureWidgetGL(QWidget* parent, const QGLWidget* shared, TexturePtr tex);

protected:
  void initializeGL();
  void paintGL();
  void resizeGL(int w, int h);

private:
  TexturePtr m_tex;
};

class TextureWidget : public QWidget {
  Q_OBJECT

public:
  TextureWidget(QWidget* parent, TexturePtr tex);

protected:
  void paintEvent(QPaintEvent*);

private:
  TextureWidgetGL* m_gl;
  TexturePtr m_tex;
  QFrame* m_frame;
};

class TextureBrowser : public QDialog {
  Q_OBJECT

public:
  ~TextureBrowser();

  static TextureBrowser& instance();

  virtual void showEvent(QShowEvent*);

private:
  explicit TextureBrowser(QWidget* parent = 0);
  Ui::TextureBrowser *m_ui;

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
  QList<QLayoutItem*> m_items;
};

#endif // TEXTURE_BROWSER_HPP
