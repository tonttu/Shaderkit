#include "glwidget.hpp"
#include "texture_browser.hpp"
#include "ui_texture_browser.h"
#include "mainwindow.hpp"
#include "scene.hpp"
#include "texture.hpp"
#include "state.hpp"
#include "renderpass.hpp"

#include <QDebug>

#include <cassert>
#include <cmath>

namespace {
  TextureBrowser* s_browser = 0;
}

TextureWidgetGL::TextureWidgetGL(QWidget* parent, const QGLWidget* shared, TexturePtr tex)
  : QGLWidget(parent, shared), m_tex(tex) {
  m_vertices.setCache(false);
  m_uv0.setCache(false);
  if (tex) TextureChangeManager::listen(tex, this, std::bind(&TextureWidgetGL::updateGL, this));
}

void TextureWidgetGL::setTexture(TexturePtr tex) {
  if (tex == m_tex) return;
  if (tex) TextureChangeManager::listen(tex, this, std::bind(&TextureWidgetGL::updateGL, this));
  if (m_tex) TextureChangeManager::forget(m_tex, this);
  m_tex = tex;
}

void TextureWidgetGL::initializeGL() {
  glEnable(GL_TEXTURE_2D);
}

void TextureWidgetGL::paintGL() {
  glCheck("TextureWidgetGL #1");

  if (!m_tex) {
    glClear(GL_COLOR_BUFFER_BIT);
    return;
  }

  bool clip_mode = true;

  float ar = m_tex->width() / float(m_tex->height());
  float w = 1.0f, h = 1.0f;
  if (ar >= 1.0f) {
    h = w / ar;
  } else {
    w = h * ar;
  }

  if (clip_mode && std::fabs(ar - 1.0f) > 0.0001f) {
    glClear(GL_COLOR_BUFFER_BIT);
  }

  float full[] = {0,0, 1,0, 1,1, 0,1};

  float xa = (1-w)/2, xb = (1+w)/2,
        ya = (1-h)/2, yb = (1+h)/2;
  float clipped[] = {xa,ya, xb,ya, xb,yb, xa,yb};

  xa = -(1-w)/(2*w), xb = 1+(1-w)/(2*w),
  ya = -(1-h)/(2*h), yb = 1+(1-h)/(2*h);
  float wrap[] = {xa,ya, xb,ya, xb,yb, xa,yb};

  float * vertices = clip_mode ? clipped : full;
  float * uvs = clip_mode ? full : wrap;

  State state(0);
  m_vertices.enableArray(state, GL_VERTEX_ARRAY, 2, 4, vertices);
  m_uv0.enableArray(state, GL_TEXTURE_COORD_ARRAY, 2, 4, uvs);
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  m_tex->bind();
  glDrawArrays(GL_QUADS, 0, 4);
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

int TextureWidgetGL::heightForWidth(int w) const {
  //if (m_tex) return w *  m_tex->height() / m_tex->width();
  return w;
}

void TextureWidgetGL::enterEvent(QEvent *e) {
  QGLWidget::enterEvent(e);
  if (m_tex) emit hoverBegin();
}

void TextureWidgetGL::leaveEvent(QEvent *e) {
  QGLWidget::leaveEvent(e);
  if (windowFlags().testFlag(Qt::ToolTip))
    emit hoverEnd();
}

void TextureWidgetGL::mouseDoubleClickEvent(QMouseEvent* e) {
  QGLWidget::mouseDoubleClickEvent(e);
  QPoint c = mapToGlobal(rect().center());
  if (windowFlags().testFlag(Qt::ToolTip)) {
    setWindowFlags(Qt::Window);
    setGeometry(c.x()-96, c.y()-96, 192, 192);
    show();
  } else {
    TextureWidgetGL* pr = preview();
    pr->show();
  }
}

TextureWidgetGL* TextureWidgetGL::preview(Qt::WindowFlags f, QSize size) const {
  TextureWidgetGL* ret = new TextureWidgetGL(0, this, tex());
  if (f) ret->setWindowFlags(f);
  ret->setAttribute(Qt::WA_DeleteOnClose);
  if (tex())
    ret->setWindowTitle(QString("%1 - GLSL Lab").arg(tex()->name()));

  QPoint c = mapToGlobal(rect().center());
  ret->setGeometry(QRect(c - QPoint(size.width() / 2, size.height() / 2), size));
  return ret;
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
  : QWidget(parent),
    m_ui(new Ui::TextureBrowser),
    m_selected(0) {
  m_ui->setupUi(this);
  m_ui->viewport->setLayout(new FlowLayout(m_ui->viewport));

  m_ui->color->addItem("Depth buffer");
  for (int i = 0; i < 8; ++i)
    m_ui->color->addItem(QString("Color buffer %1").arg(i));

  m_ui->renderpass->hide();
  m_ui->color->hide();

  connect(&MainWindow::instance(), SIGNAL(newScene(ScenePtr)),
          this, SLOT(newScene(ScenePtr)));
  newScene(MainWindow::instance().scene());
  renderPassesChanged();

  setAttribute(Qt::WA_DeleteOnClose);

  m_ui->scrollArea->setBackgroundRole(QPalette::Base);

  m_ui->toolbar->layout()->setMargin(0);
  m_create = m_ui->toolbar->addAction(QIcon(":/icons/new2.png"),
                                      "New Texture", this, SLOT(create()));
  m_open = m_ui->toolbar->addAction(QIcon(":/icons/load_texture.png"),
                         "New Texture from file", this, SLOT(load()));
  m_ui->toolbar->addSeparator();
  m_duplicate = m_ui->toolbar->addAction(QIcon(":/icons/duplicate.png"),
                                         "Duplicate texture", this, SLOT(duplicate()));
  m_duplicate->setEnabled(false);
  m_ui->toolbar->addSeparator();
  m_destroy = m_ui->toolbar->addAction(QIcon(":/icons/delete.png"),
                                       "Delete", this, SLOT(remove()));
  m_destroy->setEnabled(false);

  connect(m_ui->filename, SIGNAL(editingFinished()), this, SLOT(filenameChanged()));
  connect(m_ui->browse, SIGNAL(clicked()), this, SLOT(browse()));

  connect(m_ui->buttonBox, SIGNAL(rejected()), this, SLOT(close()));
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

void TextureBrowser::updateContent(ScenePtr scene) {
  if (!scene) scene = MainWindow::instance().scene();
  if (!scene) return;
  QMap<QString, TexturePtr> textures = scene->textures();

  foreach (QString name, m_textures.keys().toSet() - textures.keys().toSet()) {
    if (m_selected && m_textures[name] == m_selected) {
      selected(0);
    }
    m_textures[name]->deleteLater();
    m_textures.remove(name);
  }

  foreach (QString name, textures.keys().toSet() - m_textures.keys().toSet()) {
    TextureWidget* widget = new TextureWidget(m_ui->viewport, textures[name]);
    connect(widget, SIGNAL(select(TextureWidget*)), this, SLOT(selected(TextureWidget*)));
    m_ui->viewport->layout()->addWidget(widget);
    m_textures[name] = widget;
    if (m_select && widget->tex() == m_select) {
      selected(widget);
      m_select.reset();
    }
  }

  foreach (QString name, textures.keys().toSet() & m_textures.keys().toSet()) {
    TextureWidget* widget = m_textures[name];
    widget->setTexture(textures[name]);
    if (m_select && widget->tex() == m_select) {
      selected(widget);
      m_select.reset();
    }
  }

  QMargins m = m_ui->viewport->contentsMargins();
  QSize s = m_ui->viewport->layout()->minimumSize();
  s.setWidth(s.width() + m.left() + m.right() +
             m_ui->scrollArea->verticalScrollBar()->width() +
             m_ui->scrollArea->frameWidth()*2);
  s.setHeight(s.height() + m.top() + m.bottom());
  m_ui->scrollArea->setMinimumWidth(s.width());
}

void TextureBrowser::show() {
  m_ui->buttonBox->setStandardButtons(QDialogButtonBox::Close);
  QWidget::show();
  updateContent();
}

void TextureBrowser::selected(TextureWidget* w, bool force) {
  if (m_selected == w && !force) return;
  if (m_selected != w) {
    if (m_selected) m_selected->setSelected(false);
    if (w) w->setSelected(true);
    m_selected = w;
  }

  m_ui->params->setRowCount(0);

  if (w) {
    m_duplicate->setEnabled(true);
    m_destroy->setEnabled(true);
  } else {
    m_duplicate->setEnabled(false);
    m_destroy->setEnabled(false);
    m_ui->filename->hide();
    m_ui->browse->hide();
    m_ui->name->clear();
    m_ui->sizeinfo->clear();
    m_ui->renderpass->hide();
    m_ui->color->hide();
    return;
  }

  TexturePtr t = w->tex();
  TextureFile* tf = dynamic_cast<TextureFile*>(t.get());

  m_ui->name->setText(t->name());
  m_ui->sizeinfo->setText(QString("Texture #%1 %2x%3 %4").arg(t->id()).
                          arg(t->width()).arg(t->height()).arg(t->internalFormatStr()));

  RenderPassPtr rp = MainWindow::scene()->findRenderer(t);
  if (rp) {
    m_ui->renderpass->setCurrentIndex(m_ui->renderpass->findText(rp->name()));
    FBOPtr fbo = rp->fbo();

    int buffer = fbo->buffers().key(t);
    if (buffer == GL_DEPTH_ATTACHMENT)
      m_ui->color->setCurrentIndex(0);
    else if (buffer >= GL_COLOR_ATTACHMENT0 && buffer < GL_COLOR_ATTACHMENT0 + 8)
      m_ui->color->setCurrentIndex(buffer - GL_COLOR_ATTACHMENT0);
    else
      m_ui->color->setCurrentIndex(-1);
    m_ui->renderpass->show();
    m_ui->color->show();
  } else {
    m_ui->renderpass->hide();
    m_ui->color->hide();
  }

  if (tf) {
    m_ui->filename->setText(tf->file());
    m_ui->filename->show();
    m_ui->browse->show();
  } else {
    m_ui->filename->hide();
    m_ui->browse->hide();
  }

  QMap<QString, Texture::Param> lst = t->paramStrings();
  QSet<QString> used_params;
  for (auto it = lst.begin(); it != lst.end(); ++it) {
    used_params << it.key();
    int r = m_ui->params->rowCount();
    m_ui->params->insertRow(r);
    m_ui->params->setItem(r, 0, new QTableWidgetItem(it.key()));
    m_ui->params->item(r, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    QWidget* w;
    Texture::ParamType type = Texture::paramType(it.key());
    if (type == Texture::ENUM) {
      QComboBox* tmp = new QComboBox;
      tmp->setEditable(true);
      QStringList choices = Texture::paramChoices(it.key());
      tmp->addItems(choices);
      QString value = Texture::enumToString(it.value().i);
      if (value.isEmpty()) {
        tmp->addItem(QString::number(it.value().i));
        tmp->setCurrentIndex(tmp->count()-1);
      } else {
        tmp->setCurrentIndex(choices.indexOf(value));
      }
      connect(tmp, SIGNAL(currentIndexChanged(QString)), this, SLOT(paramChanged(QString)));
      w = tmp;
    } else {
      QLineEdit* tmp = new QLineEdit(it.value().is_float ? QString::number(it.value().f)
                                                         : QString::number(it.value().i));
      connect(tmp, SIGNAL(editingFinished()), this, SLOT(paramChanged()));
      w = tmp;
    }
    m_ui->params->setCellWidget(r, 1, w);
  }

  QStringList rest = (Texture::allParams().toSet() - used_params).toList();
  if (!rest.isEmpty()) {
    int r = m_ui->params->rowCount();
    m_ui->params->insertRow(r);

    QComboBox* param = new QComboBox;
    param->addItem("New parameter...");
    param->insertSeparator(1);
    param->addItems(rest);
    param->setCurrentIndex(0);
    m_ui->params->setCellWidget(r, 0, param);
    m_ui->params->setItem(r, 1, new QTableWidgetItem());
    m_ui->params->item(r, 1)->setFlags(Qt::ItemIsEnabled);

    connect(param, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(newParam(QString)));
  }

  m_ui->params->resizeColumnToContents(0);
}

void TextureBrowser::paramChanged(QString value) {
  QWidget* w = qobject_cast<QWidget*>(sender());
  if (!w || !m_selected || !m_selected->tex()) return;
  int r = 0;
  for (; r < m_ui->params->rowCount(); ++r) {
    if (m_ui->params->cellWidget(r, 1) == w) break;
  }
  if (r >= m_ui->params->rowCount()) return;

  if (value.isEmpty()) {
    QLineEdit* tmp = qobject_cast<QLineEdit*>(w);
    if (tmp) value = tmp->text();
  }

  QString name = m_ui->params->item(r, 0)->text();
  m_selected->tex()->setParam(name, value);
  selected(m_selected, true);
}

void TextureBrowser::newParam(QString name) {
  if (name.isEmpty() || !m_selected || !m_selected->tex()) return;

  m_selected->gl()->makeCurrent();
  m_selected->tex()->setParam(name, m_selected->tex()->param(name));
  selected(m_selected, true);
}

void TextureBrowser::newScene(ScenePtr scene) {
  /// @todo what about the old signals?
  if (scene) {
    connect(scene.get(), SIGNAL(textureListUpdated()), this, SLOT(updateContent()));
    connect(scene.get(), SIGNAL(renderPassesListUpdated()), this, SLOT(renderPassesChanged()));
  }
  updateContent(scene);
}

void TextureBrowser::renderPassesChanged() {
  m_ui->renderpass->clear();
  ScenePtr s = MainWindow::scene();
  if (!s) return;
  foreach (RenderPassPtr rp, s->renderPasses())
    m_ui->renderpass->addItem(rp->icon(), rp->name());
}

void TextureBrowser::create() {
  /// @todo implement
}

void TextureBrowser::duplicate() {
  ScenePtr s = MainWindow::scene();
  if (!s || !m_selected || !m_selected->tex()) return;

  m_select = m_selected->tex()->clone();
  s->addTexture(m_select);
}

void TextureBrowser::remove() {
  ScenePtr s = MainWindow::scene();
  if (!s || !m_selected || !m_selected->tex()) return;
  s->remove(m_selected->tex());
}

void TextureBrowser::load() {
  QString file = selectFile(tr("Create a new texture from an image"));
  if (!file.isEmpty()) {
    QFileInfo fi(file);

    TextureFile* tex = new TextureFile(fi.baseName());
    tex->setFile(file);
    TexturePtr texp(tex);
    m_select = texp;
    MainWindow::scene()->addTexture(texp);
  }
}

void TextureBrowser::filenameChanged() {
  if (!m_selected) return;
  TextureFile* tf = dynamic_cast<TextureFile*>(m_selected->tex().get());
  if (!tf || tf->file() == m_ui->filename->text()) return;
  tf->setFile(m_ui->filename->text());
}

void TextureBrowser::browse() {
  QString file = selectFile(tr("Select an image"));
  if (!file.isEmpty()) {
    m_ui->filename->setText(file);
    filenameChanged();
  }
}

QString TextureBrowser::selectFile(QString tip) {
  ScenePtr s = MainWindow::scene();
  if (!s) return "";

  QStringList lst;
  foreach(QByteArray a, QImageReader::supportedImageFormats())
    lst << QString("*.%1").arg(QString::fromUtf8(a));

  QSettings settings("GLSL-Lab", "GLSL-Lab");
  QString dir = settings.value("history/last_import_dir",
                               settings.value("history/last_dir",
                               QVariant(QDir::currentPath()))).toString();
  /*if (m_selected) {
    TextureFile* tf = dynamic_cast<TextureFile*>(m_selected->tex().get());
    if (tf) {
      QFileInfo fi(tf->file());
      dir = fi.absolutePath();
    }
  }*/
  QString filter = tr("Images (%1)").arg(lst.join(" "));
  QString file = QFileDialog::getOpenFileName(this, tip, dir, filter);
  if (!file.isEmpty()) {
    QFileInfo fi(file);
    settings.setValue("history/last_import_dir", fi.absolutePath());
  }
  return file;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FlowLayout::FlowLayout(QWidget* parent) : QLayout(parent)
{
  m_itemsize = QSizeF(150.0f, 150.0f);
  m_minHeight = m_itemsize.height() + spacing()*2;
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
  return index < 0 || index >= m_items.size() ? 0 : m_items.takeAt(index);
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
