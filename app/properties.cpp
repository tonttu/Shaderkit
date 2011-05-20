/**
 * Copyright 2010,2011 Riku Palomäki.
 * This file is part of GLSL Lab.
 *
 * GLSL Lab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3 as
 * published by the Free Software Foundation.
 *
 * GLSL Lab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GLSL Lab.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "properties.hpp"
#include "shader/shader.hpp"
#include "shader/program.hpp"
#include "renderpass.hpp"
#include "texture.hpp"
#include "material.hpp"
#include "scene.hpp"
#include "mainwindow.hpp"
#include "utils.hpp"
#include "texture_browser.hpp"
#include "glwidget.hpp"

#include <cassert>

namespace {
  class BranchItem : public QLabel {
  public:
    BranchItem() : QLabel("Foo"), m_hover(false) {
      setMouseTracking(true);
    }
//    QSize sizeHint() { return QSize(128, 128); }
    void paintEvent(QPaintEvent * e) {

      //qApp->widgetAt()
      QPainter painter(this);
      QStyleOption opt;
      opt.rect = e->rect().translated(0, 5);
      opt.state = QStyle::State_Item | QStyle::State_Children | QStyle::State_Open
          /*| QStyle::State_MouseOver*/ | QStyle::State_Enabled | QStyle::State_Active;

      if (m_hover) opt.state |= QStyle::State_MouseOver;

      style()->drawPrimitive(QStyle::PE_IndicatorBranch, &opt, &painter, 0);
    }

    void enterEvent(QEvent* event) {
      QLabel::enterEvent(event);
      m_hover = true;
    }

    void leaveEvent(QEvent* event) {
      QLabel::leaveEvent(event);
      m_hover = false;
    }

    bool m_hover;
  };
}

UEditor::UEditor(QTableWidget* w, int row, MaterialPtr mat_, UniformVar& var)
 : QTableWidgetItem(/*p*/),
   mat(mat_),
   name(var.name()) {
  setFlags(flags() & ~Qt::ItemIsSelectable);
  w->insertRow(row);
}

UniformVar* UEditor::getVar() {
  UniformVar::List& list = mat->uniformList();
  for (int i = 0; i < list.size(); ++i) {
    if (list[i].name() == name)
      return &list[i];
  }
  return 0;
}

FloatEditor::FloatEditor(QTableWidget* w, int row, MaterialPtr mat, UniformVar& var)
  : UEditor(w, row, mat, var),
    edit(new QLineEdit),
    slider(new QSlider(Qt::Horizontal)),
    min(std::numeric_limits<float>::max()), max(std::numeric_limits<float>::min()),
    m_reset_action(new QAction("Reset", slider)) {
  slider->setMinimum(0);
  slider->setMaximum(1000);
  slider->setContextMenuPolicy(Qt::ActionsContextMenu);
  slider->addAction(m_reset_action);

  edit->setFrame(false);

  w->setItem(row, 1, new QTableWidgetItem(var.name()));
  w->setCellWidget(row, 2, edit);
  w->setCellWidget(row, 3, slider);

  connect(edit, SIGNAL(editingFinished()), this, SLOT(editingFinished()));
  connect(slider, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
  connect(m_reset_action, SIGNAL(triggered()), this, SLOT(reset()));
}

FloatEditor::~FloatEditor() {
}

void FloatEditor::updateUI(UniformVar& var) {
  float value = var.get();

  QString txt = QString::number(value);
  if (edit->text() != txt)
    edit->setText(txt);

  if (slider->isSliderDown()) return;

  if (var.min() < min) min = var.min();
  if (var.max() > max) max = var.max();
  if (value < min) min = value;
  if (value > max) max = value;

  float tmp = max - min;
  value -= min;
  if (tmp > 0.000000001f) {
    int ivalue = value/tmp*1000;
    if (slider->value() != ivalue)
      slider->setValue(ivalue);
   }
}

void FloatEditor::editingFinished() {
  UniformVar* var = getVar();
  if (var)
    var->set(edit->text().toFloat());
}

void FloatEditor::valueChanged(int v) {
  float tmp = max - min;
  if (tmp > 0.000000001f) {
    tmp = min + tmp * v / 1000.0f;
  }

  UniformVar* var = getVar();
  if (var)
    var->set(tmp);
}

void FloatEditor::reset() {
  UniformVar* var = getVar();
  if (var) {
    min = var->min();
    max = var->max();
    var->set(var->getDefault());
  }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TextureEditor::TextureEditor(QTableWidget* w, int row, MaterialPtr mat, UniformVar& var)
  : UEditor(w, row, mat, var) {

  QWidget* container = new QWidget;
  m_texname = new QLabel(container);

  GLWidget* s = MainWindow::instance().glwidget();
  assert(s);
  m_icon = new TextureWidgetGL(container, s, TexturePtr());
  m_icon->setMinimumSize(24, 24);
  m_icon->setMaximumSize(24, 24);

  connect(m_icon, SIGNAL(hoverBegin()), this, SLOT(hoverBegin()));

  w->setItem(row, 1, new QTableWidgetItem(var.name()));
  w->setCellWidget(row, 2, m_icon);
  w->setCellWidget(row, 3, m_texname);
}

TextureEditor::~TextureEditor() {
}

void TextureEditor::updateUI(UniformVar& var) {
  TexturePtr tex = mat->texture(var.name());

  m_icon->setTexture(tex);
  QString n = tex ? tex->name() : "<i>texture not found</i>";
  m_texname->setText(n);
}

void TextureEditor::hoverBegin() {
  QPoint point = m_icon->mapToGlobal(m_icon->rect().center());

  QWidget* w = QApplication::widgetAt(point);
  if (w && w != m_icon) return;

  TextureWidgetGL* zoom = m_icon->preview(Qt::ToolTip | Qt::BypassGraphicsProxyWidget, QSize(128, 128));
  connect(zoom, SIGNAL(hoverEnd()), zoom, SLOT(deleteLater()));
  zoom->show();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


MaterialProperties* MaterialProperties::s_instance = 0;
//FileList* FileList::s_instance = 0;

MaterialProperties &MaterialProperties::instance() {
  if (s_instance) return *s_instance;
  return *(new MaterialProperties);
}

/*FileList &FileList::instance() {
  if (s_instance) return *s_instance;
  return *(new FileList);
}*/

MaterialProperties::Sub::~Sub() {
}

Properties::Properties(QWidget* parent)
  : QTreeWidget(parent) {
}

MaterialProperties::MaterialProperties(QWidget* parent)
  : QTableWidget(parent),
    m_only_uniforms(0), m_create(0), m_open(0), m_duplicate(0), m_edit(0), m_destroy(0),
    m_hover_row(-1) {
  if (!s_instance) s_instance = this;

  verticalHeader()->hide();
  horizontalHeader()->hide();
  horizontalHeader()->setStretchLastSection(true);

  connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  connect(this, SIGNAL(itemDoubleClicked(QTableWidgetItem*)),
          this, SLOT(itemSelected(QTableWidgetItem*)));
}

MaterialProperties::~MaterialProperties() {
  if (s_instance == this) s_instance = 0;
}

void MaterialProperties::init() {
  QToolBar* tb = parentWidget()->findChild<QToolBar*>("m_prop_menu");
  assert(tb);

  tb->layout()->setMargin(0);
  //QIcon icon(":/icons/sliders.png");
  QIcon icon(":/icons/uniforms_off.png");
  icon.addFile(":/icons/uniforms.png", QSize(), QIcon::Normal, QIcon::On);
  m_only_uniforms = tb->addAction(icon, "Show only uniform variables",
                                  this, SLOT(toggleMode()));
  m_only_uniforms->setCheckable(true);
  m_only_uniforms->setChecked(false);
  tb->addSeparator();
  m_create = tb->addAction(QIcon(":/icons/new2.png"), "New material",
                           this, SLOT(create()));
  m_open = tb->addAction(QIcon(":/icons/load_texture.png"), "New material from texture",
                         this, SLOT(load()));
  tb->addSeparator();
  m_duplicate = tb->addAction(QIcon(":/icons/duplicate.png"), "Duplicate material",
                              this, SLOT(duplicate()));
  tb->addSeparator();
  m_edit = tb->addAction(QIcon(":/icons/edit.png"), "Edit material",
                         this, SLOT(edit()));
  m_destroy = tb->addAction(QIcon(":/icons/delete.png"), "Delete material",
                            this, SLOT(remove()));

  m_edit->setDisabled(true);

  ensurePolished();

  selectionChanged();
}

void MaterialProperties::update(MaterialPtr mat) {
  UniformVar::List list = mat->uniformList();

  Sub& sub = m_materials[mat];
  if (!sub.item) {
    int r = rowCount();
    setRowCount(r+1);

    setCellWidget(r, 0, new BranchItem);

    sub.item = new QTableWidgetItem;
    setItem(r, 1, sub.item);
    setSpan(r, 1, 1, 3);
    sub.item->setFlags(sub.item->flags() | Qt::ItemIsDragEnabled);
    /// @todo change icon when the type changes
    // sub.item->setIcon(0, mat->icon());
/*    expandItem(sub.item);
    sub.item->setFirstColumnSpanned(true);
    sub.item->setBackgroundColor(0, palette().color(QPalette::Dark));*/
    QFont font = sub.item->font();
    font.setBold(true);
    sub.item->setFont(font);

    QFontMetrics m(font);
    sub.item->setSizeHint(QSize(50, m.height()+10));
    sub.item->setTextAlignment(Qt::AlignLeft | Qt::AlignBottom);

    MainWindow::instance().openMaterial(mat);
  }
  QString progname = mat->prog() ? mat->prog()->name() : "fixed pipeline";
  int t = mat->textureNames().size();
  sub.item->setText(QString(t == 1 ? "%1 - %2 - %3 texture" : "%1 - %2 - %3 textures").
                    arg(mat->name()).arg(progname).arg(t));

  horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
  horizontalHeader()->setResizeMode(1, QHeaderView::ResizeToContents);
  QFontMetrics m(font());
  setColumnWidth(2, m.width("888.888"));

  verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);


  QSet<QString> names;
  foreach (UniformVar var, list) {
    const ShaderTypeInfo& type = var.typeinfo();
    names << var.name();

    UEditor* editor = sub.editors[var.name()];
    if (!editor) {
      editor = createEditor(mat, var, type, sub.item);
      sub.editors[var.name()] = editor;
    }

    if (editor)
      editor->updateUI(var);
  }

  foreach (QString name, sub.editors.keys().toSet() - names)
    sub.editors.remove(name);
}

void MaterialProperties::itemSelected(QTableWidgetItem* item) {
  MaterialPtr m = get(item);
  if (m) MainWindow::instance().openMaterial(m);
}

void MaterialProperties::updateMaterialList(ScenePtr scene) {
  QSet<MaterialPtr> current = m_materials.keys().toSet();
  QSet<MaterialPtr> materials = scene->materials().values().toSet();
  foreach (MaterialPtr m, current - materials) {
    delete m_materials[m].item;
    m_materials.remove(m);
  }
  foreach (MaterialPtr m, materials - current)
    update(m);
}

void MaterialProperties::setActiveMaterials(QSet<MaterialPtr> materials) {
/*  QSet<MaterialPtr> current = m_materials.keys().toSet();
  foreach (MaterialPtr m, current - materials) {
    delete m_materials[m].item;
    m_materials.remove(m);
  }
  foreach (MaterialPtr m, materials - current)
    update(m);*/
}

void MaterialProperties::selectionChanged() {
  if (!m_create) return;

  MaterialPtr m;
  QList<QTableWidgetItem*> items = selectedItems();
  if (items.size() == 1) m = get(items[0]);
  if (m) {
    m_duplicate->setEnabled(true);
    /// @todo implement
    m_edit->setEnabled(false);
    m_destroy->setEnabled(true);

    QString name = m->name();
    m_duplicate->setText(QString("Duplicate \"%1\"").arg(name));
    m_edit->setText(QString("Edit \"%1\"").arg(name));
    m_destroy->setText(QString("Delete \"%1\"").arg(name));
  } else {
    m_duplicate->setEnabled(false);
    m_edit->setEnabled(false);
    m_destroy->setEnabled(false);

    m_duplicate->setText("Duplicate material");
    m_edit->setText("Edit material");
    m_destroy->setText("Delete material");
  }
}

void MaterialProperties::create() {
  ScenePtr s = MainWindow::scene();
  if (!s) return;

  MaterialPtr m(new Material(Utils::uniqueName("Untitled", s->materials().keys())));
  s->setMaterial(m->name(), m);
}

void MaterialProperties::load() {
  ScenePtr s = MainWindow::scene();
  if (!s) return;

  QStringList lst;
  foreach(QByteArray a, QImageReader::supportedImageFormats())
    lst << QString("*.%1").arg(QString::fromUtf8(a));

  QSettings settings("GLSL-Lab", "GLSL-Lab");
  QString dir = settings.value("history/last_import_dir",
                               settings.value("history/last_dir",
                               QVariant(QDir::currentPath()))).toString();
  QString filter = tr("Images (%1)").arg(lst.join(" "));
  QString file = QFileDialog::getOpenFileName(this, tr("Create a new material from an image"), dir, filter);
  if (!file.isEmpty()) {
    QFileInfo fi(file);
    settings.setValue("history/last_import_dir", fi.absolutePath());

    MaterialPtr m(new Material(Utils::uniqueName(fi.baseName(), s->materials().keys())));
    TextureFile* tex = new TextureFile(Utils::uniqueName(fi.baseName(), s->textures().keys()));
    tex->setFile(file);

    m->addTexture("diffuse", TexturePtr(tex));
    s->setMaterial(m->name(), m);
  }
}

void MaterialProperties::duplicate() {
  if (selectedItems().size() != 1) return;
  MaterialPtr orig = get(selectedItems()[0]);
  if (!orig) return;
  ScenePtr s = orig->scene();
  if (!s) return;

  MaterialPtr cloned = orig->clone(true);
  cloned->setName(Utils::uniqueName(cloned->name(), s->materials().keys()));
  s->setMaterial(cloned->name(), cloned);
}

void MaterialProperties::edit() {
  /// @todo implement
}

void MaterialProperties::remove() {
  foreach (QTableWidgetItem* item, selectedItems()) {
    MaterialPtr m = get(item);
    if (!m) return;
    ScenePtr s = m->scene();
    if (!s) return;
    s->remove(m);
  }
}

void MaterialProperties::toggleMode() {

}

UEditor* MaterialProperties::createEditor(MaterialPtr mat, UniformVar& var,
                                          const ShaderTypeInfo& type, QTableWidgetItem* p) {
  int r = row(p) + 1;
  if (var.arraySize() == 1) {
    if (type.type == GL_FLOAT) {
      return new FloatEditor(this, r, mat, var);
    } else if (type.type == GL_SAMPLER_2D) {
      return new TextureEditor(this, r, mat, var);
    } else {
      /// @todo implement
    }
  } else {
    /// @todo implement
  }
  return 0;
}

void MaterialProperties::startDrag(Qt::DropActions supportedActions) {
  MaterialPtr material;
  foreach (QTableWidgetItem* item, selectedItems()) {
    material = get(item);
    if (material) break;
  }
  if (!material) {
    Log::info("no material");
    return;
  }
  //qApp->setOverrideCursor(QCursor(QPixmap(":/icons/shader.png")));

  QMimeData* data = new QMimeData;
  data->setData("text/x-glsl-lab-material", material->name().toUtf8());

  QDrag* drag = new QDrag(this);
  drag->setMimeData(data);
  drag->setPixmap(QPixmap(":/icons/shader.png"));
  drag->setHotSpot(QPoint(8, 8));
  drag->exec(supportedActions);
 // drag->setDragCursor(QCursor(Qt::BlankCursor).pixmap(), Qt::CopyAction);
}

void MaterialProperties::contextMenuEvent(QContextMenuEvent* e) {
  e->accept();
  QMenu* menu = new QMenu;
  QTableWidgetItem* item = itemAt(e->pos());
  MaterialPtr m = get(item);

  if (m) {
    menu->addAction(m_duplicate);
    menu->addSeparator();
    menu->addAction(m_edit);
    menu->addAction(m_destroy);
    menu->addSeparator();
    setCurrentItem(item);
    selectionChanged();
  }

  menu->addAction(m_only_uniforms);
  menu->addSeparator();
  menu->addAction(m_create);
  menu->addAction(m_open);
  menu->exec(e->globalPos());
}

MaterialPtr MaterialProperties::get(QTableWidgetItem*& i) const {
  if (!i) return MaterialPtr();

  QSet<QTableWidgetItem*> set;
  int r = i->row();
  for (int c = 0; c < columnCount(); ++c)
    set << item(r, c);

  for (auto it = m_materials.begin(); it != m_materials.end(); ++it) {
    if (set.contains(it->item)) {
      i = it->item;
      return it.key();
    }
  }
  return MaterialPtr();
}

bool MaterialProperties::viewportEvent(QEvent* event) {
  if (event->type() == QEvent::MouseMove) {
    QMouseEvent* me = static_cast<QMouseEvent*>(event);
    int r = rowAt(me->pos().y());
    if (r != m_hover_row) {
      //Log::info("%d", r);
      m_hover_row = r;
    }
  }

  return QTableWidget::viewportEvent(event);
}

QItemSelectionModel::SelectionFlags MaterialProperties::selectionCommand(
    const QModelIndex& index, const QEvent* event) const {
  QTableWidgetItem* i = item(index.row(), 1);
  if (i && (i->flags() & Qt::ItemIsSelectable))
    return QTableWidget::selectionCommand(index, event);
  return QItemSelectionModel::NoUpdate;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#if 0
FileList::FileList(QWidget* parent)
  : QTreeWidget(parent),
    m_src(new QTreeWidgetItem(this)),
    m_create(0), m_open(0), m_duplicate(0), m_edit(0), m_destroy(0) {
  if (!s_instance) s_instance = this;

  m_src->setBackgroundColor(0, palette().color(QPalette::Dark));
  QFont font = m_src->font(0);
  font.setBold(true);
  m_src->setFont(0, font);
  m_src->setText(0, "Sources");
  expandItem(m_src);

  connect(this, SIGNAL(itemActivated(QTreeWidgetItem*, int)),
          this, SLOT(activateFile(QTreeWidgetItem*, int)));

  connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
}

FileList::~FileList() {
  if (s_instance == this) s_instance = 0;
}

void FileList::init() {
  QToolBar* tb = parentWidget()->findChild<QToolBar*>("filelist_menu");
  assert(tb);

  tb->layout()->setMargin(0);
  m_create = tb->addAction(QIcon(":/icons/new2.png"), "New file");
  m_open = tb->addAction(QIcon(":/icons/load_textfile.png"), "Open file");
  tb->addSeparator();
  m_duplicate = tb->addAction(QIcon(":/icons/duplicate.png"), "Duplicate file");
  tb->addSeparator();
  m_edit = tb->addAction(QIcon(":/icons/edit.png"), "Edit");
  m_destroy = tb->addAction(QIcon(":/icons/delete.png"), "Delete");

  m_create->setDisabled(true);
  m_open->setDisabled(true);
  m_duplicate->setDisabled(true);
  m_edit->setDisabled(true);
  m_destroy->setDisabled(true);
}

void FileList::update(ShaderPtr shader) {
  QTreeWidgetItem* item = m_files[shader->filename()];

  if (!item) {
    item = new QTreeWidgetItem(m_src);
    m_files[shader->filename()] = item;
    m_items[item] = shader;
    QDir file(shader->filename());
    item->setText(0, file.dirName());
    item->setIcon(0, shader->icon());
//    QTreeWidget::update();
  }
}

void FileList::activateFile(QTreeWidgetItem* item, int /*column*/) {
  ShaderPtr shader = m_items[item];
  if(shader) {
    emit openFile(shader);
  }
}

void FileList::remove(ShaderPtr shader) {
  QTreeWidgetItem* item = m_files[shader->filename()];
  if (item) {
    m_items.remove(item);
    m_files.remove(shader->filename());
    m_src->removeChild(item);
    delete item;
  }
}

void FileList::selectionChanged() {
  if (!m_create) return;

  QList<QTreeWidgetItem*> items = selectedItems();
  if (items.size() == 1) {
    /// @todo implement
    /*m_duplicate->setEnabled(true);
    m_edit->setEnabled(true);
    m_destroy->setEnabled(true);*/
  } else {
    m_duplicate->setEnabled(false);
    m_edit->setEnabled(false);
    m_destroy->setEnabled(false);
  }
}
#endif
