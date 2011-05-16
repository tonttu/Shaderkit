#include "glwidget.hpp"
#include "texture_browser.hpp"
#include "ui_texture_browser.h"
#include "mainwindow.hpp"
#include "scene.hpp"
#include "texture.hpp"

#include <QDebug>

#include <cassert>
#include <cmath>

namespace {
  TextureBrowser* s_browser = 0;
}

TextureWidgetGL::TextureWidgetGL(QWidget* parent, const QGLWidget* shared, TexturePtr tex)
  : QGLWidget(parent, shared), m_tex(tex) {
}

void TextureWidgetGL::setTexture(TexturePtr tex) {
  m_tex = tex;
}

void TextureWidgetGL::initializeGL() {
  glEnable(GL_TEXTURE_2D);
}

void TextureWidgetGL::paintGL() {
  glCheck("TextureWidgetGL #1");
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  m_tex->bind();
  glBegin(GL_QUADS);

  glTexCoord2f(0.0f, 0.0f);
  glVertex2f(0.0f, 0.0f);

  glTexCoord2f(1.0f, 0.0f);
  glVertex2f(1.0f, 0.0f);

  glTexCoord2f(1.0f, 1.0f);
  glVertex2f(1.0f, 1.0f);

  glTexCoord2f(0.0f, 1.0f);
  glVertex2f(0.0f, 1.0f);

  glEnd();
  glCheck("TextureWidgetGL #2");
}

void TextureWidgetGL::resizeGL(int w, int h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, 1, 0, 1, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

TextureWidget::TextureWidget(QWidget* parent, TexturePtr tex)
 : QWidget(parent), m_gl(0), m_tex(tex), m_frame(new QFrame(this)) {
  setLayout(new QVBoxLayout);
  layout()->setMargin(0);
  m_frame->setLayout(new QVBoxLayout);
  m_frame->layout()->setMargin(3);
  layout()->addWidget(m_frame);
}

void TextureWidget::setSelected(bool s) {
  if (s) {
    setBackgroundRole(QPalette::Highlight);
    setAutoFillBackground(true);
  } else {
    setBackgroundRole(QPalette::Background);
    setAutoFillBackground(false);
  }
}

void TextureWidget::setTexture(TexturePtr tex) {
  if (m_tex == tex) return;
  m_tex = tex;
  if (m_gl) m_gl->setTexture(tex);
}

void TextureWidget::paintEvent(QPaintEvent* ev) {
  QWidget::paintEvent(ev);
  if (!m_gl) {
    // create the qglwidget lazily, sharing the same context as the main gl preview has
    GLWidget* s = MainWindow::instance().glwidget();
    if (!s) return;
    m_gl = new TextureWidgetGL(m_frame, s, m_tex);
    m_frame->layout()->addWidget(m_gl);
  }
}

void TextureWidget::mouseReleaseEvent(QMouseEvent* ev) {
  QWidget::mouseReleaseEvent(ev);
  emit select(this);
}

TextureBrowser::TextureBrowser(QWidget *parent)
  : QDialog(parent),
    m_ui(new Ui::TextureBrowser),
    m_timer(new QTimer(this)),
    m_selected(0) {
  m_ui->setupUi(this);
  m_ui->viewport->setLayout(new FlowLayout(m_ui->viewport));

  connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
  m_timer->start(100);
}

TextureBrowser::~TextureBrowser() {
  delete m_ui;
  assert(s_browser == this);
  s_browser = 0;
}

TextureBrowser& TextureBrowser::instance() {
  if (!s_browser)
    s_browser = new TextureBrowser;
  return *s_browser;
}

void TextureBrowser::showEvent(QShowEvent* ev) {
  QDialog::showEvent(ev);
  if (!MainWindow::instance().scene()) return;
  QMap<QString, TexturePtr> textures = MainWindow::instance().scene()->textures();

  foreach (QString name, m_textures.keys().toSet() - textures.keys().toSet()) {
    m_textures[name]->deleteLater();
    m_textures.remove(name);
  }

  foreach (QString name, textures.keys().toSet() - m_textures.keys().toSet()) {
    TextureWidget* widget = new TextureWidget(m_ui->viewport, textures[name]);
    connect(widget, SIGNAL(select(TextureWidget*)), this, SLOT(selected(TextureWidget*)));
    m_ui->viewport->layout()->addWidget(widget);
    m_textures[name] = widget;
  }

  foreach (QString name, textures.keys().toSet() & m_textures.keys().toSet()) {
    m_textures[name]->setTexture(textures[name]);
  }

  QMargins m = m_ui->viewport->contentsMargins();
  QSize s = m_ui->viewport->layout()->minimumSize();
  s.setWidth(s.width() + m.left() + m.right() +
             m_ui->scrollArea->verticalScrollBar()->width() +
             m_ui->scrollArea->frameWidth()*2);
  s.setHeight(s.height() + m.top() + m.bottom());
  m_ui->scrollArea->setMinimumWidth(s.width());
}

void TextureBrowser::paintEvent(QPaintEvent* ev) {
  foreach (TextureWidget* w, m_textures) {
    bool visible = !w->visibleRegion().isEmpty();
    if (visible && w->gl()) w->gl()->updateGL();
  }
  QDialog::paintEvent(ev);
}

void TextureBrowser::selected(TextureWidget* t) {
  if (m_selected == t) return;
  if (m_selected) m_selected->setSelected(false);
  t->setSelected(true);
  m_selected = t;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FlowLayout::FlowLayout(QWidget* parent) : QLayout(parent)
{
  m_itemsize = QSizeF(192.0f, 108.0f);
  m_minHeight = m_itemsize.height()*2 + spacing()*3;
}

FlowLayout::~FlowLayout()
{
  QLayoutItem *item;
  while ((item = takeAt(0)))
    delete item;
}

QSize FlowLayout::minimumSize() const
{
  return QSize(m_itemsize.width()*2 + spacing()*3,
               m_minHeight);
}

QSize FlowLayout::sizeHint() const
{
  return minimumSize();
}

void FlowLayout::addItem(QLayoutItem* item)
{
  m_items.push_back(item);
}

QLayoutItem* FlowLayout::itemAt(int index) const
{
  return index < 0 || index >= m_items.size() ? 0 : m_items.at(index);
}

QLayoutItem* FlowLayout::takeAt(int index)
{
  return m_items.takeAt(index);
}

int FlowLayout::count() const
{
  return m_items.size();
}

void FlowLayout::setGeometry(const QRect &r)
{
  QLayout::setGeometry(r);

  if (m_items.isEmpty())
    return;

  // m_itemsize.width() * n + spacing() * (n + 1) <= r.width();
  float itemw = m_itemsize.width(), itemh = m_itemsize.height();
  float spx = spacing(), spy = spacing();
  float width = r.width();
  int n = std::max<float>(1, (width - spx) / (itemw + spx));

  spx = (width - n*itemw) / (n+1.0f);

  for (int i = 0; i < m_items.size(); ++i) {
    int x = i % n, y = i / n;
    QLayoutItem *o = m_items.at(i);
    QRect geom(r.x() + x * (spx + itemw) + spx,
               r.y() + y * (spy + itemh) + spy,
               itemw, itemh);
    o->setGeometry(geom);
  }

  int rows = std::ceil(m_items.size() / float(n));
  m_minHeight = rows*itemh + (rows+1) * spy;
}
