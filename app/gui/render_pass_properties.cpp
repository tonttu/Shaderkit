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

#include "gui/render_pass_properties.hpp"
#include "core/renderpass.hpp"
#include "core/scene.hpp"
#include "gl/program.hpp"
#include "core/object3d.hpp"
#include "gl/light.hpp"
#include "gl/camera.hpp"
#include "gl/texture.hpp"
#include "core/material.hpp"
#include "core/utils.hpp"
#include "gui/object_list.hpp"
#include "gui/light_list.hpp"
#include "gui/camera_editor.hpp"
#include "gui/render_pass_list.hpp"

#include "gui/mainwindow.hpp"

#include <cassert>

#include <QHeaderView>
#include <QToolBar>
#include <QAction>
#include <QListWidget>
#include <QColorDialog>

namespace Shaderkit
{

  RenderPassProperties* RenderPassProperties::s_instance = 0;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if 0

  TextureEditor::TextureEditor(RenderPassPtr pass) : m_pass(pass)
  {
    QHBoxLayout* layout = new QHBoxLayout(this);

    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(0);

    setAutoFillBackground(true);

    m_target = new QLineEdit(this);
    m_list = new QComboBox(this);
    m_view = new QListWidget(this);
    m_list->setModel(m_view->model());
    m_list->setView(m_view);

    updateList();

    layout->addWidget(m_list, 1);

    QPushButton* edit = new QPushButton("edit", this);
    layout->addWidget(edit);

    QPushButton* insert = new QPushButton("new", this);
    layout->addWidget(insert);

    layout->addStretch(2);

    connect(m_list, SIGNAL(activated(int)), this, SLOT(listActivated(int)));
    connect(pass->scene().get(), SIGNAL(textureListUpdated()), this, SLOT(updateList()));

    connect(insert, SIGNAL(clicked()), this, SLOT(newClicked()));
    connect(edit, SIGNAL(clicked()), this, SLOT(editClicked()));
  }

  void TextureEditor::setTex(TexturePtr tex)
  {
    m_tex = tex;

    for (int i = 0; i < m_view->count(); ++i) {
      QListWidgetItem* item = m_view->item(i);
      Texture* t = reinterpret_cast<Texture*>(item->data(Qt::UserRole).toLongLong());
      if (t == tex.get()) {
        m_list->setCurrentIndex(i);
        return;
      }
    }
  }

  void TextureEditor::listActivated(int index)
  {

  }

  void TextureEditor::updateList()
  {
    QMap<QString, TexturePtr> textures = m_pass->scene()->textures();

    for (int i = 0; i < m_view->count();) {
      QListWidgetItem* item = m_view->item(i);
      Texture* tex = reinterpret_cast<Texture*>(item->data(Qt::UserRole).toLongLong());
      bool found = false;
      foreach (QString key, textures.keys()) {
        if (textures[key].get() == tex) {
          /// @todo update information
          textures.remove(key);
          found = true;
          break;
        }
      }

      if (!found) {
        m_view->takeItem(i);
        delete item;
      } else ++i;
    }

    foreach (TexturePtr t, textures) {
      QListWidgetItem* item = new QListWidgetItem(t->name());
      item->setData(Qt::UserRole, reinterpret_cast<qlonglong>(t.get()));
//    QFont font = item->font();
//    item->setFont(font);
      m_view->addItem(item);
    }

    setTex(m_tex);
  }

  void TextureEditor::newClicked()
  {
    /// @todo
  }

  void TextureEditor::editClicked()
  {
    /// @todo
  }

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

  TexturesEditor::TexturesEditor(QTreeWidgetItem* parent, RenderPassPtr pass)
    : QTreeWidgetItem(parent), m_pass(pass)
  {
    setIcon(0, QIcon(":/icons/texture.png"));
    setText(0, "Textures");
    QFont f = font(0);
    f.setBold(true);
    setFont(0, f);

    updated(pass);

    connect(pass.get(), SIGNAL(changed(RenderPassPtr)), this, SLOT(updated(RenderPassPtr)));
  }

  void TexturesEditor::updated(RenderPassPtr pass)
  {
    /// @todo
    /*
    assert(pass == m_pass);

    m_pass->scene()->textures();

    QMap<QString, TexturePtr> in = m_pass->in();

    setText(1, QString(in.size() == 1 ? "%1 texture" : "%1 textures").arg(in.size()));

    // index of the adder ("empty" editor)
    int adder = -1;

    for (int i = 0; i < m_editors.size();) {
      QTreeWidgetItem* item = m_editors[i].first;
      TextureEditor* editor = m_editors[i].second;
      QString name = editor->target()->text();
      bool deletethis = false;

      if (!editor->tex() || name.isEmpty()) {
        if (adder == -1) {
          adder = i;
        } else {
          deletethis = true;
        }
      } else {
        if (in.contains(name)) {
          editor->setTex(in[name]);
          in.remove(name);
        } else {
          deletethis = true;
        }
      }

      if (deletethis) {
        removeChild(item);
        delete item;
        editor->deleteLater();
        m_editors.removeAt(i);
      } else ++i;
    }

    foreach (QString name, in.keys()) {
      QTreeWidgetItem* item = new QTreeWidgetItem(this);
      TextureEditor* editor = new TextureEditor(pass);
      editor->target()->setText(name);
      editor->setTex(in[name]);
      treeWidget()->setItemWidget(item, 0, editor->target());
      treeWidget()->setItemWidget(item, 1, editor);
      m_editors << qMakePair(item, editor);
    }

    if (adder == -1) {
      QTreeWidgetItem* item = new QTreeWidgetItem(this);
      TextureEditor* editor = new TextureEditor(pass);
      treeWidget()->setItemWidget(item, 0, editor->target());
      treeWidget()->setItemWidget(item, 1, editor);
      m_editors << qMakePair(item, editor);
    } else if (adder+1 != m_editors.size()) {
      QPair<QTreeWidgetItem*, TextureEditor*> p = m_editors[adder];
      removeChild(p.first);
      addChild(p.first);
      m_editors.removeAt(adder);
      m_editors << p;
    }*/
  }
#endif
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

  RenderPassProperties& RenderPassProperties::instance()
  {
    if (s_instance) return *s_instance;
    return *(new RenderPassProperties);
  }

  RenderPassProperties::RenderPassProperties(QWidget* parent)
    : QTableWidget(parent),
      m_data(new PropertyLayoutData(3, 1)),
      m_create(0), m_reorder(0), m_duplicate(0), m_destroy(0), m_firstRender(true)
  {
    if (!s_instance) s_instance = this;

    m_data->margins(0).setLeft(5);

    setColumnCount(2);

    verticalHeader()->hide();
    horizontalHeader()->hide();

    //horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
    horizontalHeader()->setStretchLastSection(true);

    setHorizontalScrollMode(ScrollPerPixel);
    setVerticalScrollMode(ScrollPerPixel);

    setFrameShape(NoFrame);

    //setDragDropMode(DragOnly);
    setSelectionMode(SingleSelection);
    setSelectionBehavior(SelectRows);

    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  }

  RenderPassProperties::~RenderPassProperties()
  {
    if (s_instance == this) s_instance = 0;
  }

  void RenderPassProperties::init()
  {
    QToolBar* tb = parentWidget()->findChild<QToolBar*>("rp_prop_menu");
    assert(tb);

    tb->layout()->setMargin(0);
    m_create = tb->addAction(QIcon(":/icons/new2.png"), "New render pass");
    tb->addSeparator();
    m_reorder = tb->addAction(QIcon(":/icons/reorder.png"), "Reorder render passes");
    tb->addSeparator();
    m_duplicate = tb->addAction(QIcon(":/icons/duplicate.png"), "Duplicate render pass");
    tb->addSeparator();
    m_destroy = tb->addAction(QIcon(":/icons/delete.png"), "Delete");

    connect(m_create, SIGNAL(triggered()), this, SLOT(create()));
    connect(m_reorder, SIGNAL(triggered()), this, SLOT(reorder()));
    connect(m_duplicate, SIGNAL(triggered()), this, SLOT(duplicate()));
    connect(m_destroy, SIGNAL(triggered()), this, SLOT(remove()));

    selectionChanged();
  }

  void RenderPassProperties::paintEvent(QPaintEvent* e)
  {
    QTableWidget::paintEvent(e);
    if (m_firstRender) {
      horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
      m_firstRender = false;
    }
  }

#if 0
  void RenderPassProperties::dropEvent(QDropEvent* event)
  {
    Properties::dropEvent(event);

    if (event->isAccepted()) {
      MainWindow::scene()->setRenderPasses(list());
      recalcLayout();
    }
  }

  void RenderPassProperties::contextMenuEvent(QContextMenuEvent* e)
  {
    e->accept();
    QMenu* menu = new QMenu;
    QTreeWidgetItem* item = itemAt(e->pos());
    RenderPassPtr m = get(item);

    if (m) {
      menu->addAction(m_duplicate);
      menu->addSeparator();
      menu->addAction(m_destroy);
      menu->addSeparator();
      setCurrentItem(item);
      selectionChanged();
    }

    menu->addAction(m_create);
    menu->exec(e->globalPos());
  }
#endif

  QWidget* RenderPassProperties::insertItem(int row, const QString& name, PropertyLayout*& pl)
  {
    QWidget* label_container = new QWidget;
    QHBoxLayout* l = new QHBoxLayout(label_container);
    l->setMargin(0);
    QLabel* label = new QLabel(name, label_container);
    l->addWidget(label);
    l->addSpacerItem(new QSpacerItem(10, 1));
    label->setIndent(10);

    insertRow(row);
    setRowHeight(row, 24);
    QPersistentModelIndex index = model()->index(row, 0);
    setIndexWidget(index, label_container);

    QWidget* container = new QWidget;
    pl = new PropertyLayout(m_data);
    container->setLayout(pl);

    setCellWidget(row, 1, container);

    return container;
  }

  void RenderPassProperties::listUpdated(QList<RenderPassPtr> passes)
  {
    if (!passes.isEmpty() && passes[0]->scene() != m_scene) {
      if (m_scene) {
        disconnect(SLOT(materialListUpdated()));
        disconnect(SLOT(cameraListUpdated()));
      }
      m_scene = passes[0]->scene();
      if (m_scene) {
        connect(m_scene.get(), SIGNAL(materialListUpdated(ScenePtr)), this, SLOT(materialListUpdated()));
        connect(m_scene.get(), SIGNAL(cameraListUpdated()), this, SLOT(cameraListUpdated()));
      }
    } else if (m_list == passes) return;

    disconnect(SLOT(changed(RenderPassPtr)));

    m_reorder->setDisabled(passes.isEmpty());

    m_list = passes;
    m_passes.clear();
    setRowCount(0);

    foreach (RenderPassPtr pass, passes) {
      PassItem& item = m_passes[pass];

      /// @todo change icons
      item.header = new HeaderWidget(QIcon(":/icons/uniforms.png"),
                                     QIcon(":/icons/uniforms_off.png"));

      int row = rowCount();
      setRowCount(row+1);

      item.header_index = model()->index(row, 0);
      setIndexWidget(item.header_index, item.header);

      setSpan(row, 0, 1, 2);
      setRowHeight(row, 24);

      PropertyLayout* layout = 0;

      {
        QWidget* container = insertItem(++row, "Default material", layout);

        item.material_box = new QComboBox(container);
        m_senders[item.material_box] = pass;
        connect(item.material_box, SIGNAL(activated(int)), this, SLOT(materialActivated(int)));
        layout->setWidget(0, 1, item.material_box);

        /// @todo why this or something similar won't work?
        // m_shaderlist->model()->setData(m_shaderlist->model()->index(0, 0), font, Qt::FontRole);
        {
          QListWidget* view = new QListWidget(item.material_box);

          QListWidgetItem* tmp = new QListWidgetItem("None");
          QFont font = tmp->font();
          font.setBold(true);
          tmp->setFont(font);
          view->addItem(tmp);

          tmp = new QListWidgetItem("");
          tmp->setFlags(Qt::NoItemFlags);
          font.setPixelSize(2);
          tmp->setFont(font);
          view->addItem(tmp);

          item.material_box->setModel(view->model());
          item.material_box->setView(view);
        }
      }

      {
        QWidget* container = insertItem(++row, "View", layout);

        item.view_box = new QComboBox(container);
        m_senders[item.view_box] = pass;
        connect(item.view_box, SIGNAL(activated(int)), this, SLOT(cameraActivated(int)));
        layout->setWidget(0, 1, item.view_box);

        QPushButton* btn = createButton(pass, container, QIcon(":/icons/view.png"),
                                        SLOT(openCameraEditor()));
        layout->setWidget(2, 1, btn);

        {
          QListWidget* view = new QListWidget(item.view_box);

          QListWidgetItem* tmp = new QListWidgetItem("Disabled");
          QFont font = tmp->font();
          font.setBold(true);
          tmp->setFont(font);
          view->addItem(tmp);

          tmp = new QListWidgetItem("Post");
          tmp->setIcon(QIcon(":/icons/texture.png"));
          tmp->setFont(font);
          view->addItem(tmp);

          tmp = new QListWidgetItem("");
          tmp->setFlags(Qt::NoItemFlags);
          font.setPixelSize(2);
          tmp->setFont(font);
          view->addItem(tmp);

          item.view_box->setModel(view->model());
          item.view_box->setView(view);
        }
      }

      {
        QWidget* container = insertItem(++row, "Size", layout);

        item.size = new QLineEdit(container);
        item.size->setValidator(new QRegExpValidator(QRegExp("\\d+x\\d+"), item.size));
        m_senders[item.size] = pass;
        connect(item.size, SIGNAL(editingFinished()), this, SLOT(sizeEdited()));

        layout->setWidget(0, 1, item.size);

        QIcon icon;
        icon.addFile(":/btns/nosync.png", QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(":/btns/sync.png", QSize(), QIcon::Normal, QIcon::On);

        item.size_btn = createButton(pass, container, icon, SLOT(toggleAutoSize()));
        item.size_btn->setCheckable(true);
        layout->setWidget(2, 1, item.size_btn);
      }

      {
        QWidget* container = insertItem(++row, "Clear", layout);

        QWidget* colors = new QWidget(container);
        QHBoxLayout* color_layout = new QHBoxLayout(colors);
        color_layout->setMargin(0);

        QString opt[][2] = {{":/icons/colorbuffer.png", "Clear color buffer"},
          {":/icons/depthbuffer.png", "Clear depth buffer"},
          {":/icons/stencil.png", "Clear stencil buffer"}
        };
        QPushButton** btns[] = {&item.clear_color, &item.clear_depth, &item.clear_stencil};

        for (int i = 0; i < 3; ++i) {
          QPushButton *& btn = *btns[i];
          btn = new QPushButton(QIcon(opt[i][0]), "");
          btn->setToolTip(opt[i][1]);
          btn->setMaximumWidth(26);
          btn->setCheckable(true);
          color_layout->addWidget(btn);

          m_senders[btn] = pass;
          connect(btn, SIGNAL(clicked()), this, SLOT(clearChanged()));
        }

        color_layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
        layout->setWidget(0, 1, colors);

        QPushButton* btn = createButton(pass, container, QIcon(":/icons/color.png"),
                                        SLOT(openClearColorPicker()));
        layout->setWidget(2, 1, btn);
      }

      {
        QWidget* container = insertItem(++row, "Objects", layout);

        item.objects_label = new QLabel(container);
        layout->setWidget(0, 1, item.objects_label);

        QPushButton* btn = createButton(pass, container, QIcon(":/icons/objects.png"),
                                        SLOT(openObjectBrowser()));
        layout->setWidget(2, 1, btn);
      }

      {
        QWidget* container = insertItem(++row, "Lights", layout);

        item.lights_label = new QLabel(container);
        layout->setWidget(0, 1, item.lights_label);

        QPushButton* btn = createButton(pass, container, QIcon(":/icons/light.png"),
                                        SLOT(openLightEditor()));
        layout->setWidget(2, 1, btn);
      }

      connect(pass.get(), SIGNAL(changed(RenderPassPtr)),
              this, SLOT(changed(RenderPassPtr)));
      changed(pass);
    }
  }

  void RenderPassProperties::changed(RenderPassPtr pass)
  {
    if (!m_passes.contains(pass)) return;
    PassItem& item = m_passes[pass];

    /// @todo render pass icon

    item.header->setText(pass->name());

    {
      /// Material
      while (item.material_box->count() > 2)
        item.material_box->removeItem(2);

      bool found = !pass->defaultMaterial();
      item.material_box->setCurrentIndex(0);

      foreach (MaterialPtr material, m_scene->materials()) {
        item.material_box->addItem(QIcon(material->prog() ? ":/icons/shader.png" : ":/icons/noshader.png"),
                                   material->name(), material->name());
        if (!found && material == pass->defaultMaterial()) {
          found = true;
          item.material_box->setCurrentIndex(item.material_box->count()-1);
        }
      }
    }

    {
      /// View
      while (item.view_box->count() > 3)
        item.view_box->removeItem(3);

      bool found = !pass->view();
      item.view_box->setCurrentIndex(0);

      if (!found && pass->view()->type() == Camera::Rect) {
        found = true;
        item.view_box->setCurrentIndex(1);
      }
      foreach (CameraPtr camera, m_scene->cameras()) {
        item.view_box->addItem(camera->icon(), camera->name(), camera->name());
        if (!found && camera == pass->view()) {
          found = true;
          item.view_box->setCurrentIndex(item.view_box->count()-1);
        }
      }
    }

    {
      /// View / Camera
      item.size->setDisabled(pass->autosize());
      item.size_btn->setChecked(pass->autosize());
      item.size->setText(QString("%1x%2").arg(pass->width()).arg(pass->height()));
    }

    {
      /// Clear
      GLbitfield value = pass->clearBits();

      item.clear_color->setChecked(value & GL_COLOR_BUFFER_BIT);
      item.clear_depth->setChecked(value & GL_DEPTH_BUFFER_BIT);
      item.clear_stencil->setChecked(value & GL_STENCIL_BUFFER_BIT);

      /// @todo something like setHidden(pass->type() != RenderPass::Normal); ?
    }

    {
      /// Objects
      RenderPass::Objects objs = pass->objects();
      int count = objs.size();
      int total_count = pass->scene()->objects().size();
      item.objects_label->setText(QString("%1 / %2 objects").arg(count).arg(total_count));
    }

    {
      /// Lights
      RenderPass::Lights all_lights = pass->scene()->lights().values().toSet();
      RenderPass::Lights pass_lights = pass->lights();
      item.lights_label->setText(QString("%1 / %2 lights").arg(pass_lights.size()).arg(all_lights.size()));
    }
  }

  void RenderPassProperties::selectionChanged()
  {
    RenderPassPtr pass = getSelected();

    if (m_selected != pass) {
      if (m_selected && m_passes.contains(m_selected))
        m_passes[m_selected].header->setSelected(false);
      if (pass && m_passes.contains(pass))
        m_passes[pass].header->setSelected(true);
      m_selected = pass;
    }

    if (!m_create) return;

    if (pass) {
      m_duplicate->setEnabled(true);
      m_destroy->setEnabled(true);

      QString name = pass->name();
      m_duplicate->setText(QString("Duplicate \"%1\"").arg(name));
      m_destroy->setText(QString("Delete \"%1\"").arg(name));
    } else {
      m_duplicate->setEnabled(false);
      m_destroy->setEnabled(false);
      m_duplicate->setText(QString("Duplicate render pass"));
      m_destroy->setText(QString("Delete render pass"));
    }
  }

  void RenderPassProperties::sizeEdited()
  {
    RenderPassPtr pass = getSender();
    if (!pass || !m_passes.contains(pass)) return;
    QRegExp r("^(\\d+)x(\\d+)$");
    if (r.exactMatch(m_passes[pass].size->text())) {
      int w = r.cap(1).toInt(), h = r.cap(2).toInt();
      pass->setManualSize(w, h);
    }
  }

  void RenderPassProperties::toggleAutoSize()
  {
    RenderPassPtr pass = getSender();
    if (!pass || !m_passes.contains(pass)) return;
    if (m_passes[pass].size_btn->isChecked())
      pass->setAutosize();
    else
      pass->setManualSize(pass->width(), pass->height());
  }

  void RenderPassProperties::clearChanged()
  {
    RenderPassPtr pass = getSender();
    if (!pass || !m_passes.contains(pass)) return;
    PassItem& item = m_passes[pass];

    GLbitfield value = 0;
    if (item.clear_color->isChecked()) value |= GL_COLOR_BUFFER_BIT;
    if (item.clear_depth->isChecked()) value |= GL_DEPTH_BUFFER_BIT;
    if (item.clear_stencil->isChecked()) value |= GL_STENCIL_BUFFER_BIT;

    pass->setClearBits(value);
  }

  void RenderPassProperties::openClearColorPicker()
  {
    RenderPassPtr pass = getSender();
    if (!pass) return;

    Color orig = pass->clearColor();

    QColorDialog dialog(this);
    dialog.setWindowTitle("Select background color for render pass " + pass->name());
    dialog.setOptions(QColorDialog::ShowAlphaChannel);
    dialog.setCurrentColor(orig.clampToQRgb());

    connect(&dialog, SIGNAL(currentColorChanged(QColor)), this, SLOT(setClearColor(QColor)));

    RenderPassPtr tmp = pass;

    std::swap(tmp, m_pass);
    dialog.exec();
    std::swap(tmp, m_pass);

    if (dialog.result() == QDialog::Rejected) {
      pass->setClearColor(orig);
    }
  }

  void RenderPassProperties::setClearColor(QColor color)
  {
    if (m_pass) m_pass->setClearColor(color);
  }

  void RenderPassProperties::materialActivated(int index)
  {
    RenderPassPtr pass = getSender();
    if (!pass) return;
    if (index == 0) {
      pass->setDefaultMaterial(MaterialPtr());
    } else if (index > 1) {
      if (m_passes.contains(pass))
        pass->setDefaultMaterial(pass->scene()->material(m_passes[pass].material_box->itemData(index).toString()));
    }
  }

  void RenderPassProperties::cameraActivated(int index)
  {
    RenderPassPtr pass = getSender();
    if (!pass) return;
    if (index == 0) {
      pass->setType(RenderPass::Disabled);
    } else if (index == 1) {
      pass->setType(RenderPass::PostProc);
    } else {
      /// @todo this isn't exactly right
      pass->setType(RenderPass::Normal);
      if (m_passes.contains(pass))
        pass->setView(pass->scene()->camera(m_passes[pass].view_box->itemData(index).toString()));
    }
  }

  RenderPassPtr RenderPassProperties::get(QModelIndex index) const
  {
    QModelIndex idx = index.column() == 0 ? index : index.sibling(index.row(), 0);
    while (idx.isValid()) {
      for (QMap<RenderPassPtr, PassItem>::const_iterator it = m_passes.begin(); it != m_passes.end(); ++it) {
        if (idx == it->header_index)
          return it.key();
      }
      idx = idx.sibling(index.row()-1, 0);
    }
    return RenderPassPtr();
  }

  RenderPassPtr RenderPassProperties::getSelected() const
  {
    auto lst = selectedIndexes();
    if (lst.size() == 1) return get(lst[0]);
    return RenderPassPtr();
  }

  RenderPassPtr RenderPassProperties::getSender() const
  {
    return m_senders.value(sender());
  }

  QItemSelectionModel::SelectionFlags RenderPassProperties::selectionCommand(
    const QModelIndex& index, const QEvent* event) const
  {
    QWidget* w = indexWidget(index);
    if (w && typeid(*w) == typeid(HeaderWidget)) {
      return QTableWidget::selectionCommand(index, event);
    }
    return QItemSelectionModel::NoUpdate;
  }

  void RenderPassProperties::create()
  {
    ScenePtr s = MainWindow::scene();
    if (!s) {
      Log::error("No active scene");
    } else {
      s->add(RenderPassPtr(new RenderPass("Untitled", s)));
    }
  }

  void RenderPassProperties::reorder()
  {
    if (m_list.isEmpty()) return;
    RenderPassList* list = new RenderPassList(m_list, this);
    list->exec();
    if (list->result() == RenderPassList::Accepted) {
      m_list[0]->scene()->setRenderPasses(list->orderedList());
    }
  }

  void RenderPassProperties::duplicate()
  {
    RenderPassPtr pass = getSelected();

    if (pass) pass->scene()->add(pass->clone());
  }

  void RenderPassProperties::remove()
  {
    RenderPassPtr pass = getSelected();
    if (pass)
      pass->scene()->remove(pass);
  }

  void RenderPassProperties::materialListUpdated()
  {
    foreach (RenderPassPtr pass, m_passes.keys())
      changed(pass);
  }

  void RenderPassProperties::cameraListUpdated()
  {
    foreach (RenderPassPtr pass, m_passes.keys())
      changed(pass);
  }

  void RenderPassProperties::openObjectBrowser()
  {
    RenderPassPtr pass = getSender();
    if (pass) {
      ObjectList* browser = new ObjectList(0, pass);
      browser->show();
    }
  }

  void RenderPassProperties::openLightEditor()
  {
    RenderPassPtr pass = getSender();
    if (pass) {
      LightList* editor = new LightList(0, pass);
      editor->show();
    }
  }

  void RenderPassProperties::openCameraEditor()
  {
    RenderPassPtr pass = getSender();
    if (pass) {
      CameraEditor* editor = new CameraEditor(0, pass);
      editor->show();
    }
  }

  QPushButton* RenderPassProperties::createButton(RenderPassPtr pass, QWidget* container,
      const QIcon& icon, const char* method)
  {
    QPushButton* btn = new QPushButton(icon, "", container);
    btn->setFlat(true);
    btn->setMinimumSize(24, 24);
    btn->setMaximumSize(24, 24);
    m_senders[btn] = pass;
    connect(btn, SIGNAL(clicked()), this, method);
    return btn;
  }

} // namespace Shaderkit
