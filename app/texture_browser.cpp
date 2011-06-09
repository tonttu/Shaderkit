/**
 * Copyright 2010,2011 Riku Palomäki.
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

#include "glwidget.hpp"
#include "texture_browser.hpp"
#include "ui_texture_browser.h"
#include "mainwindow.hpp"
#include "scene.hpp"
#include "texture.hpp"
#include "state.hpp"
#include "renderpass.hpp"
#include "material.hpp"

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

  State state(0);

  float ww = width(), wh = height();
  float tex_ar = m_tex->width() / float(m_tex->height());
  float win_ar = ww / wh;

  int mode = 0;
  if (mode == 0) {
    float w, h;
    if (tex_ar > win_ar) {
      w = width(), h = w / tex_ar;
    } else {
      h = height(), w = h * tex_ar;
    }
    float oy = (height() - h) * 0.5f;
    float ox = (width() - w) * 0.5f;
    float vertices[] = {ox,oy, w+ox,oy, w+ox,h+oy, ox,h+oy};
    float uvs[] = {0,0, 1,0, 1,1, 0,1};

    m_vertices.enableArray(state, GL_VERTEX_ARRAY, 2, 4, vertices);
    m_uv0.enableArray(state, GL_TEXTURE_COORD_ARRAY, 2, 4, uvs);
  } else if (mode == 1) {
    float vertices[] = {0, 0, ww,0, ww,wh, 0,wh};

    float f = 0.2f;
    float uv_w, uv_h;
    if (tex_ar < win_ar) {
      uv_h = 1.0f + 2.0f*f;
      uv_w = uv_h * win_ar;
    } else {
      uv_w = 1.0f + 2.0f*f;
      uv_h = uv_w / win_ar;
    }

    float x = (uv_w - 1.0f) * 0.5f;
    float y = (uv_h - 1.0f) * 0.5f;
    float uvs[] = {-x,-y, x+1,-y, x+1,y+1, -x,y+1};

    m_vertices.enableArray(state, GL_VERTEX_ARRAY, 2, 4, vertices);
    m_uv0.enableArray(state, GL_TEXTURE_COORD_ARRAY, 2, 4, uvs);
  }

  if (mode == 0 && std::fabs(tex_ar - win_ar) > 0.0001f) {
    glClear(GL_COLOR_BUFFER_BIT);
  }

  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  m_tex->bind();
  glDrawArrays(GL_QUADS, 0, 4);
  glCheck("TextureWidgetGL #2");
}

void TextureWidgetGL::resizeGL(int w, int h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w, 0, h, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

int TextureWidgetGL::heightForWidth(int w) const {
  //if (m_tex) return w *  m_tex->height() / m_tex->width();
  return w;
}

void TextureWidgetGL::mouseReleaseEvent(QMouseEvent* e) {
  QGLWidget::mouseReleaseEvent(e);
  if (m_tex && !windowFlags().testFlag(Qt::ToolTip))
    emit previewBegin();
}


/*void TextureWidgetGL::enterEvent(QEvent *e) {
  QGLWidget::enterEvent(e);
  if (m_tex) emit hoverBegin();
}*/

void TextureWidgetGL::leaveEvent(QEvent *e) {
  QGLWidget::leaveEvent(e);
  if (windowFlags().testFlag(Qt::ToolTip))
    emit previewEnd();
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
    ret->setWindowTitle(QString("%1 - Shaderkit").arg(tex()->name()));

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
    m_selected(0),
    m_attachment(0) {
  m_ui->setupUi(this);
  m_ui->viewport->setLayout(new FlowLayout(m_ui->viewport));

  m_ui->color->addItem("Depth buffer", GL_DEPTH_ATTACHMENT);
  for (int i = 0; i < 16; ++i)
    m_ui->color->addItem(QString("Color buffer %1").arg(i), GL_COLOR_ATTACHMENT0 + i);

  m_ui->panel->setDisabled(true);

  connect(&MainWindow::instance(), SIGNAL(newScene(ScenePtr)),
          this, SLOT(newScene(ScenePtr)));
  newScene(MainWindow::instance().scene());
  renderPassesChanged();

  setAttribute(Qt::WA_DeleteOnClose);

  QTableWidget* tw = new QTableWidget(this);
  tw->verticalHeader()->hide();
  tw->horizontalHeader()->hide();
  tw->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  tw->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  tw->setGridStyle(Qt::DotLine);
  tw->setFrameShape(QFrame::NoFrame);
  tw->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  tw->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

  m_ui->internal_format->setModel(tw->model());
  m_ui->internal_format->setView(tw);
  connect(m_ui->internal_format, SIGNAL(activated(QString)),
          this, SLOT(setInternalFormat(QString)));

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

  connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(apply()));
  connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(close()));
  connect(m_ui->buttonBox, SIGNAL(rejected()), this, SLOT(close()));
  connect(m_ui->renderpass, SIGNAL(activated(int)), this, SLOT(changeRenderBuffer()));
  connect(m_ui->color, SIGNAL(activated(int)), this, SLOT(changeRenderBuffer()));
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
  selected(0);
  m_ui->task_description->setText("");
  m_target.first.reset();
  m_ui->buttonBox->setStandardButtons(QDialogButtonBox::Close);
  QWidget::show();
  updateContent();
}

void TextureBrowser::show(MaterialPtr material, QString uniformName, TexturePtr current)
{
  selected(0);
  m_select = current;
  m_target = qMakePair(material, uniformName);
  m_ui->task_description->setText(QString("Select a texture for variable <b>%1</b> in material <b>%2</b>.").
                                  arg(uniformName).arg(material->name()));
  m_ui->buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Apply |
                                      QDialogButtonBox::Close);
  m_ui->buttonBox->button(QDialogButtonBox::Apply)->setDisabled(true);
  connect(m_ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()),
          this, SLOT(apply()));
  QWidget::show();
  updateContent();
}

void TextureBrowser::selected(TextureWidget* w, bool force) {
  if (m_selected == w && !force) return;
  if (m_selected != w) {
    if (m_selected) {
      m_selected->setSelected(false);
      if (m_selected->tex())
        TextureChangeManager::forget(m_selected->tex(), this);
    }
    if (w) {
      w->setSelected(true);
      if (w->tex()) {
        TextureChangeManager::listen(w->tex(), this, std::bind(
            &TextureBrowser::updateInternalFormat, this), false);
      }
    }
    m_selected = w;
  }

  m_ui->params->setRowCount(0);
  updateInternalFormat();

  if (m_target.first) {
    TexturePtr target = m_target.first->texture(m_target.second);
    bool changed = w ? target != w->tex() : (bool)target;
    m_ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(changed);
  }

  if (w) {
    m_duplicate->setEnabled(true);
    m_destroy->setEnabled(true);
    m_ui->panel->setDisabled(false);
  } else {
    m_ui->panel->setDisabled(true);
    m_duplicate->setEnabled(false);
    m_destroy->setEnabled(false);
    m_ui->filename->clear();
    m_ui->filename->show();
    m_ui->filename_label->show();
    m_ui->browse->show();
    m_ui->name->setText("Texture");
    m_ui->sizeinfo->clear();
    m_ui->renderpass->setCurrentIndex(0);
    m_ui->renderpass->show();
    m_ui->renderpass_label->show();
    for (int i = 0; i < m_ui->color->count(); ++i)
      m_ui->color->setItemIcon(0, QIcon());
    m_ui->color->setCurrentIndex(1);
    m_ui->color->show();
    return;
  }

  TexturePtr t = w->tex();
  TextureFile* tf = dynamic_cast<TextureFile*>(t.get());

  m_ui->name->setText(t->name());

  RenderPassPtr rp = MainWindow::scene()->findRenderer(t);
  int buffer = 0;
  if (rp) {
    m_ui->renderpass->setCurrentIndex(m_ui->renderpass->findText(rp->name()));
    FBOPtr fbo = rp->fbo();
    updateAttachmentIcons(fbo);

    buffer = fbo->buffers().key(t);
    if (buffer == GL_DEPTH_ATTACHMENT)
      m_ui->color->setCurrentIndex(0);
    else if (buffer >= GL_COLOR_ATTACHMENT0 && buffer < GL_COLOR_ATTACHMENT0 + 16)
      m_ui->color->setCurrentIndex(buffer - GL_COLOR_ATTACHMENT0 + 1);
    else
      m_ui->color->setCurrentIndex(-1);
  } else {
    m_ui->renderpass->setCurrentIndex(-1);
    m_ui->color->setCurrentIndex(-1);
    for (int i = 0; i < m_ui->color->count(); ++i)
      m_ui->color->setItemIcon(0, QIcon());
  }

  if (tf) {
    m_ui->filename->setText(tf->file());
    m_ui->filename->show();
    m_ui->filename_label->show();
    m_ui->browse->show();

    m_ui->renderpass->hide();
    m_ui->renderpass_label->hide();
    m_ui->color->hide();
  } else {
    m_ui->filename->hide();
    m_ui->filename_label->hide();
    m_ui->browse->hide();

    m_ui->renderpass->show();
    m_ui->renderpass_label->show();
    m_ui->color->show();
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

void TextureBrowser::updateInternalFormat() {
  TexturePtr t = m_selected ? m_selected->tex() : TexturePtr();
  if (!t) {
    m_ui->internal_format->setCurrentIndex(-1);
    return;
  }

  m_ui->sizeinfo->setText(QString("Texture #%1 %2x%3 %4").arg(t->id()).
                          arg(t->width()).arg(t->height()).arg(t->internalFormatStr()));

  int buffer = t->attachment();
  QString current = t->internalFormatStr();

  if (buffer == m_attachment &&
      m_ui->internal_format->currentText() == current) return;

  QTableWidget* tw = dynamic_cast<QTableWidget*>(m_ui->internal_format->view());
  assert(tw);
  if (buffer != m_attachment || tw->rowCount() == 0) {
    m_attachment = buffer;
    int columns = 1;
    if (buffer == GL_DEPTH_ATTACHMENT || buffer == GL_STENCIL_ATTACHMENT) {
      QList<QString> all = buffer == GL_DEPTH_ATTACHMENT
          ? Texture::depthRenderableInternalFormatsStr().toList()
          : Texture::stencilRenderableInternalFormatsStr().toList();
      qSort(all);

      tw->setColumnCount(1);
      tw->setRowCount(all.size());
      m_ui->internal_format->setModelColumn(0);
      m_ui->internal_format->setCurrentIndex(-1);

      for (int i = 0; i < all.size(); ++i) {
        tw->setItem(i, 0, new QTableWidgetItem(all[i]));
        if (all[i] == current) m_ui->internal_format->setCurrentIndex(i);
      }
    } else {
      bool color = buffer >= GL_COLOR_ATTACHMENT0 && buffer < GL_COLOR_ATTACHMENT0 + 16;
      const QList<QPair<QString, QStringList>>& formats = Texture::internalFormats(color);

      columns = color ? 3 : 4;
      int elements = formats.size();
      for (auto it = formats.begin(); it != formats.end(); ++it) {
        elements += it->second.size();
      }
      int rows = std::ceil(float(elements) / columns);

      int selected_row = -1, selected_column = 0;

      tw->setColumnCount(columns);
      tw->setRowCount(rows);

      int row = -1, column = 0;
      for (auto it = formats.begin(); it != formats.end(); ++it) {
        if (++row == rows) row = 0, ++column;
        QTableWidgetItem* item = new QTableWidgetItem(it->first);
        item->setFlags(Qt::ItemIsEnabled);

        QFont font = item->font();
        font.setBold(true);
        item->setFont(font);
        tw->setItem(row, column, item);

        foreach (QString n, it->second) {
          if (++row == rows) row = 0, ++column;
          tw->setItem(row, column, new QTableWidgetItem(n));
          if (n == current && selected_row == -1)
            selected_row = row, selected_column = column;
        }
      }

      m_ui->internal_format->setModelColumn(selected_column);
      m_ui->internal_format->setCurrentIndex(selected_row);
    }

    tw->horizontalHeader()->setStretchLastSection(columns == 1);
    tw->ensurePolished();
    int width = 0;
    for (int i = 0; i < columns; ++i) width += tw->columnWidth(i);
    tw->setMinimumWidth(columns == 1 ? std::max(width, m_ui->internal_format->width()): width);
  } else {
    bool found = false;
    QAbstractItemModel* m = m_ui->internal_format->model();
    for (int i = 0; i < tw->columnCount(); ++i) {
      QModelIndexList lst = m->match(m->index(0, i), Qt::DisplayRole, current, 1, Qt::MatchExactly);
      if (!lst.isEmpty()) {
        m_ui->internal_format->setModelColumn(lst[0].column());
        m_ui->internal_format->setCurrentIndex(lst[0].row());
        found = true;
        break;
      }
    }
    if (!found) {
      m_ui->internal_format->setModelColumn(0);
      m_ui->internal_format->setCurrentIndex(-1);
    }
  }
}

void TextureBrowser::setInternalFormat(QString value) {
  TexturePtr t = m_selected ? m_selected->tex() : TexturePtr();
  if (t) {
    t->setInternalFormat(value);
    updateInternalFormat();
  }
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

  /// @todo "None", change this to use different view/model
  /// @todo what happens to the current active selection?
  m_ui->renderpass->addItem("");
  foreach (RenderPassPtr rp, s->renderPasses())
    m_ui->renderpass->addItem(rp->icon(), rp->name(), rp->name());
}

void TextureBrowser::create() {
  TexturePtr tex(new Texture("Untitled"));
  m_select = tex;
  MainWindow::scene()->addTexture(tex);
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

void TextureBrowser::apply() {
  if (m_target.first) {
    m_target.first->setTexture(m_target.second, m_selected ? m_selected->tex() : TexturePtr());
  }
  m_ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
}

void TextureBrowser::changeRenderBuffer() {
  ScenePtr s = MainWindow::scene();
  if (m_ui->renderpass->currentIndex() < 0 ||
      m_ui->color->currentIndex() < 0 ||
      !s || !m_selected->tex()) return;

  QString rpname = m_ui->renderpass->itemData(m_ui->renderpass->currentIndex()).toString();
  int target = m_ui->color->itemData(m_ui->color->currentIndex()).toInt();

  if (rpname.isEmpty()) {
    m_selected->tex()->setFBO(FBOPtr());
    return;
  }

  foreach (RenderPassPtr rp, s->renderPasses()) {
    if (rp->name() == rpname) {
      if (rp->fbo()->buffers().value(target) != m_selected->tex()) {
        rp->fbo()->set(target, m_selected->tex());
        updateAttachmentIcons(rp->fbo());
      }
      break;
    }
  }
}

void TextureBrowser::updateAttachmentIcons(FBOPtr fbo) {
  FrameBufferObject::Buffers buffers = fbo->buffers();

  for (int i = 0; i < m_ui->color->count(); ++i) {
    QIcon icon;
    FBOImagePtr img = buffers.value(i == 0 ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0 + i - 1);
    if (dynamic_cast<Texture*>(img.get())) {
      icon = QIcon(":/icons/texture.png");
    } else if (dynamic_cast<RenderBuffer*>(img.get())) {
      icon = QIcon(":/icons/renderbuffer.png");
    }
    m_ui->color->setItemIcon(i, icon);
  }
}


QString TextureBrowser::selectFile(QString tip) {
  ScenePtr s = MainWindow::scene();
  if (!s) return "";

  QStringList lst;
  foreach(QByteArray a, QImageReader::supportedImageFormats())
    lst << QString("*.%1").arg(QString::fromUtf8(a));

  QSettings settings("Shaderkit", "Shaderkit");
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
