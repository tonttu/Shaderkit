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

#include "gui/properties.hpp"
#include "gl/shader.hpp"
#include "gl/program.hpp"
#include "core/renderpass.hpp"
#include "gl/texture.hpp"
#include "core/material.hpp"
#include "core/scene.hpp"
#include "gui/mainwindow.hpp"
#include "core/utils.hpp"
#include "gui/texture_browser.hpp"
#include "gui/glwidget.hpp"

#include <cassert>

#include <QApplication>
#include <QPaintEvent>
#include <QAction>
#include <QHeaderView>
#include <QToolBar>
#include <QImageReader>
#include <QSettings>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenu>

class PropertyItem : public QWidgetItem {
public:
  PropertyItem(QWidget* w, int colspan = 1) : QWidgetItem(w), m_colspan(colspan) {}
  int colspan() const { return m_colspan; }

private:
  int m_colspan;
};

namespace {
  template <typename T> T& insert(QList<T>& l) {
    l.push_back(T());
    return l.back();
  }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

PropertyLayoutData::PropertyLayoutData(int c, int s) : columns(c), stretch_column(s) {
  padding.resize(columns);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

PropertyLayout::PropertyLayout(std::shared_ptr<PropertyLayoutData> data)
  : m_data(data), m_row(insert(data->items)) {
  m_row.resize(m_data->columns);
}

PropertyLayout::~PropertyLayout() {
  qDeleteAll(m_row);
  for (int i = 0; i < m_data->items.size(); ++i) {
    if (&m_data->items[i] == &m_row) {
      m_data->items.removeAt(i);
      break;
    }
  }
}

QSize PropertyLayout::minimumSize() const {
  return m_data->min_size;
}

QSize PropertyLayout::sizeHint() const {
  return m_data->min_size;
}

void PropertyLayout::addItem(QLayoutItem*) {
  Log::fatal("PropertyLayout::addItem shouldn't be called directly");
}

QLayoutItem* PropertyLayout::itemAt(int index) const {
  foreach (PropertyItem* item, m_row) {
    if (!item) continue;
    if (index-- == 0) return item;
  }
  return 0;
}

QLayoutItem* PropertyLayout::takeAt(int index) {
  for (int c = 0; c < m_data->columns; ++c) {
    PropertyItem* ret = m_row[c];
    if (!ret) continue;
    if (index-- == 0) {
      m_row[c] = 0;
      return ret;
    }
  }
  return 0;
}

int PropertyLayout::count() const {
  int c = 0;
  foreach (PropertyItem* item, m_row)
    if (item) ++c;
  return c;
}

void PropertyLayout::setWidget(int column, int colspan, QWidget* widget) {
  assert(!m_row[column]);
  m_row[column] = new PropertyItem(widget, colspan);
  addChildWidget(widget);
}

void PropertyLayout::setGeometry(const QRect& r) {
  PropertyLayoutData& data = *m_data;
  /// (start column (inclusive), end column (inclusive)) => min width
  QMap<QPair<int, int>, int> spans;

  int height = 0;
  for (auto it = data.items.begin(); it != data.items.end(); ++it) {
    for (int c = 0; c < data.columns; ++c) {
      PropertyItem* item = (*it)[c];
      if (!item) continue;

      int c2 = c+item->colspan()-1;
      QSize size = item->sizeHint();

      int & w = spans[qMakePair(c, c2)];
      w = qMax(w, size.width() +
               data.padding[c].left() + data.padding[c2].right());

      if (&m_row == &*it) {
        int padding_height = 0;
        for (int i = c; i <= c2; ++i) {
          int tmp = data.padding[i].top() + data.padding[i].bottom();
          padding_height = qMax(padding_height, tmp);
        }
        height = qMax(height, size.height() + padding_height);
      }
    }
  }

  QVector<int> pos(data.columns + 1);
  for (int c = 0; c < data.columns; ++c) {
    int & p = pos[c+1];
    p = pos[c];
    for (auto it = spans.begin(); it != spans.end(); ++it) {
      if (it.key().second == c)
        p = qMax(p, pos[it.key().first] + *it);
    }
  }
  data.min_size = QSize(pos.last(), height);

  if (data.stretch_column < data.columns - 1) {
    int tst = data.stretch_column + 1;
    int w = 0;
    for (auto it = spans.begin(); it != spans.end(); ++it)
      if (it.key().first == tst && it.key().second == tst)
        w = qMax(w, *it);
    pos[tst] = pos[tst+1] - w;
  }

  int stretch_amount = qMax(0, r.width() - pos.last());
  for (int c = data.stretch_column + 1; c <= data.columns; ++c)
    pos[c] += stretch_amount;

  for (int c = 0; c < data.columns; ++c) {
    PropertyItem* item = m_row[c];
    if (!item) continue;

    int c2 = c + item->colspan() - 1;
    int x = pos[c];
    int w = pos[c2+1] - pos[c];

    QMargins& m = data.padding[c];
    QMargins& m2 = data.padding[c2];

    item->setGeometry(QRect(x + m.left(), r.top() + m.top(),
                            w - m.left() - m2.right(), r.height() - m.top() - m.bottom()));
  }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

LineEdit::LineEdit(QWidget* parent) : QLineEdit(parent) {
  connect(this, SIGNAL(textChanged(QString)), this, SLOT(updateSizes()));
}

QSize LineEdit::minimumSizeHint() const {
  const int vertical_margin = 1;
  const int horizontal_margin = 2;

  ensurePolished();
  QFontMetrics fm = fontMetrics();
  QMargins margins = contentsMargins();
  QMargins text_margins = textMargins();

  int w = qMax(fm.width(text()), fm.width("888.88")) +
      2 * horizontal_margin +
      margins.left() + margins.right() +
      text_margins.left() + text_margins.right();
  int h = fm.height() + qMax(2*vertical_margin, fm.leading()) +
      margins.top() + margins.bottom() +
      text_margins.top() + text_margins.bottom();

  QStyleOptionFrameV2 opt;
  initStyleOption(&opt);
  return style()->sizeFromContents(QStyle::CT_LineEdit, &opt, QSize(w, h).
                                   expandedTo(QApplication::globalStrut()), this);
}

QSize LineEdit::sizeHint() const {
  return minimumSizeHint();
}

void LineEdit::updateSizes() {
  updateGeometry();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

HeaderWidget::HeaderWidget(QIcon active, QIcon inactive)
  : m_selected(false), m_icon(&m_inactive), m_active(active), m_inactive(inactive) {
  setAutoFillBackground(true);
}

void HeaderWidget::setSelected(bool selected) {
  if (m_selected != selected) {
    m_selected = selected;
    m_icon = selected ? &m_active : &m_inactive;
    update();
  }
}

void HeaderWidget::paintEvent(QPaintEvent* ev) {
  QPainter painter(this);
  painter.setClipRect(ev->rect());

  QStyleOptionHeader opt;
  opt.state = QStyle::State_Raised | QStyle::State_Horizontal | QStyle::State_Enabled;
  //opt.state |= QStyle::State_MouseOver;

  if (m_selected) opt.state |= QStyle::State_On;

  opt.textAlignment = Qt::AlignLeft | Qt::AlignVCenter;
  opt.orientation = Qt::Horizontal;

  opt.text = m_text;
  opt.icon = *m_icon;

  /// clip borders
  QRect r = ev->rect();
  r.setRight(r.right() + 3);
  r.setBottom(r.bottom() + 1);
  opt.rect = r;

  style()->drawControl(QStyle::CE_Header, &opt, &painter, this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

VarEditor::VarEditor(MaterialProperties& prop, int row, MaterialPtr mat, const QString& name)
 : m_mat(mat), m_name(name) {
  QWidget* container = new QWidget;
  QHBoxLayout* l = new QHBoxLayout(container);
  l->setMargin(0);
  m_label = new QLabel(m_name, container);
  l->addWidget(m_label);
  l->addSpacerItem(new QSpacerItem(10, 1));
  m_label->setIndent(10);

  prop.insertRow(row);
  prop.setRowHeight(row, 24);
  m_index = prop.model()->index(row, 0);
  prop.setIndexWidget(m_index, container);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

AttributeEditor::AttributeEditor(MaterialProperties& prop, int row,
                                 MaterialPtr mat, AttributeVar& var)
  : VarEditor(prop, row, mat, var.name())
  , m_combo(new QComboBox()) {
  prop.setCellWidget(row, 1, m_combo);

  clear();
  m_combo->setEditable(true);

  connect(m_combo, SIGNAL(currentIndexChanged(QString)),
          this, SLOT(editingFinished()));
}

void AttributeEditor::updateUI(AttributeVar&) {
  QString selection = "";
  if (m_mat) {
    const QMap<QString, MappableValue>& attrs = m_mat->attributeMap();
    if (attrs.contains(m_name))
      selection = attrs.value(m_name).toString();
  }
  if (selection != m_combo->currentText()) {
    for (int i = 0; i < m_combo->count(); ++i) {
      if (m_combo->itemText(i) == selection) {
        m_combo->setCurrentIndex(i);
        return;
      }
    }
    m_combo->addItem(selection);
    m_combo->setCurrentIndex(m_combo->count()-1);
  }
}

void AttributeEditor::editingFinished() {
  if (m_mat)
    m_mat->setAttributeMapping(m_name, m_combo->currentText());
}

void AttributeEditor::clear() {
  /// @todo better way to define these, with some type information
  QStringList choices;
  choices << "" << "mesh.vertex" << "mesh.normal" << "mesh.tangent" <<
             "mesh.bitangent" << "mesh.color" << "mesh.uv";
  m_combo->clear();
  m_combo->addItems(choices);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UniformEditor::UniformEditor(MaterialProperties& prop, int row, MaterialPtr mat, UniformVar& var)
  : VarEditor(prop, row, mat, var.name()) {
}

UniformVar* UniformEditor::getVar() {
  UniformVar::List& list = m_mat->uniformList();
  for (int i = 0; i < list.size(); ++i) {
    if (list[i].name() == m_name)
      return &list[i];
  }
  return 0;
}

FloatEditor::FloatEditor(MaterialProperties& prop, int row, MaterialPtr mat, UniformVar& var)
  : UniformEditor(prop, row, mat, var),
    m_min(std::numeric_limits<float>::max()),
    m_max(std::numeric_limits<float>::min()) {

  QWidget* container = new QWidget;
  PropertyLayout* l = new PropertyLayout(prop.layoutData());
  container->setLayout(l);

  m_edit = new LineEdit(container);
  m_edit->setFrame(false);
  l->setWidget(0, 1, m_edit);

  m_slider = new QSlider(Qt::Horizontal, container);
  l->setWidget(1, 2, m_slider);

  prop.setCellWidget(row, 1, container);

  m_reset_action = new QAction("Reset", m_slider);

  m_slider->setMinimum(0);
  m_slider->setMaximum(1000);
  m_slider->setContextMenuPolicy(Qt::ActionsContextMenu);
  m_slider->addAction(m_reset_action);

  connect(m_edit, SIGNAL(editingFinished()), this, SLOT(editingFinished()));
  connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
  connect(m_reset_action, SIGNAL(triggered()), this, SLOT(reset()));
}

FloatEditor::~FloatEditor() {
}

void FloatEditor::updateUI(UniformVar& var) {
  float value = var.get();

  QString txt = QString::number(value);
  if (m_edit->text() != txt)
    m_edit->setText(txt);

  if (m_slider->isSliderDown()) return;

  if (var.min() < m_min) m_min = var.min();
  if (var.max() > m_max) m_max = var.max();
  if (value < m_min) m_min = value;
  if (value > m_max) m_max = value;

  float tmp = m_max - m_min;
  value -= m_min;
  if (tmp > 0.000000001f) {
    int ivalue = value/tmp*1000;
    if (m_slider->value() != ivalue)
      m_slider->setValue(ivalue);
   }
}

void FloatEditor::editingFinished() {
  UniformVar* var = getVar();
  if (var)
    var->set(m_edit->text().toFloat());
}

void FloatEditor::valueChanged(int v) {
  float tmp = m_max - m_min;
  if (tmp > 0.000000001f) {
    tmp = m_min + tmp * v / 1000.0f;
  }

  UniformVar* var = getVar();
  if (var)
    var->set(tmp);
}

void FloatEditor::reset() {
  UniformVar* var = getVar();
  if (var) {
    m_min = var->min();
    m_max = var->max();
    var->set(var->getDefault());
  }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TextureEditor::TextureEditor(MaterialProperties& prop, int row, MaterialPtr mat, UniformVar& var)
  : UniformEditor(prop, row, mat, var) {

  QWidget* container = new QWidget;
  PropertyLayout* l = new PropertyLayout(prop.layoutData());
  container->setLayout(l);

  m_texname = new QLabel(container);
  l->setWidget(1, 1, m_texname);

  m_icon = MainWindow::instance().createGL<TextureWidgetGL>(container);
  m_icon->setMinimumSize(24, 24);
  m_icon->setMaximumSize(24, 24);
  l->setWidget(0, 1, m_icon);

  QPushButton* icon = new QPushButton(QIcon(":/icons/texture_browser.png"), "", container);
  icon->setFlat(true);
  icon->setMinimumSize(24, 24);
  icon->setMaximumSize(24, 24);
  l->setWidget(2, 1, icon);
  connect(icon, SIGNAL(clicked()), this, SLOT(browse()));

  prop.setCellWidget(row, 1, container);

  connect(m_icon, SIGNAL(previewBegin()), this, SLOT(showPreview()));
}

TextureEditor::~TextureEditor() {
}

void TextureEditor::updateUI(UniformVar& var) {
  TexturePtr tex = m_mat->texture(var.name());

  m_icon->setTexture(tex);
  QString n = tex ? tex->name() : "<i>texture not found</i>";
  m_texname->setText(n);
}

void TextureEditor::showPreview() {
  QPoint point = m_icon->mapToGlobal(m_icon->rect().center());

  QWidget* w = QApplication::widgetAt(point);
  if (w && w != m_icon) return;

  TextureWidgetGL* zoom = m_icon->preview(Qt::ToolTip | Qt::BypassGraphicsProxyWidget, QSize(128, 128));
  connect(zoom, SIGNAL(previewEnd()), zoom, SLOT(deleteLater()));
  zoom->show();
}

void TextureEditor::browse() {
  TextureBrowser::instance().show(m_mat, m_name, m_icon->tex());
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

/*MaterialProperties::Sub::~Sub() {
}*/

MaterialProperties::MaterialProperties(QWidget* parent)
  : QTableWidget(parent),
    m_only_uniforms(0), m_create(0), m_open(0), m_duplicate(0), m_edit(0), m_destroy(0),
    /*m_hover_row(-1),*/ m_data(new PropertyLayoutData(3, 1)) {
  if (!s_instance) s_instance = this;

  //m_data->padding[0].setLeft(5);
  m_data->padding[2].setRight(5);

  setColumnCount(2);

  verticalHeader()->hide();
  horizontalHeader()->hide();

  horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
  horizontalHeader()->setStretchLastSection(true);

  setHorizontalScrollMode(ScrollPerPixel);
  setVerticalScrollMode(ScrollPerPixel);

  setFrameShape(NoFrame);

  // mouse tracking?

  setDragDropMode(DragOnly);
  setSelectionMode(SingleSelection);
  setSelectionBehavior(SelectRows);

  connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  connect(this, SIGNAL(cellDoubleClicked(int,int)),
          this, SLOT(itemSelected(int,int)));
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

  selectionChanged();
}

void MaterialProperties::update(MaterialPtr mat) {
  MaterialItem& item = m_materials[mat];
  if (!item.material) {
    item.material = mat;
    item.header = new HeaderWidget(QIcon(":/icons/uniforms.png"),
                                   QIcon(":/icons/uniforms_off.png"));

    int r = rowCount();
    setRowCount(r+1);

    item.header_index = model()->index(r, 0);
    item.last_index = item.header_index;
    setIndexWidget(item.header_index, item.header);
    model()->setData(item.header_index, mat->name(), Qt::UserRole);

    setSpan(r, 0, 1, 2);
    setRowHeight(r, 24);

//    sub.item->setFlags(sub.item->flags() | Qt::ItemIsDragEnabled);
    /// @todo change icon when the type changes
    // sub.item->setIcon(0, mat->icon());

    connect(mat.get(), SIGNAL(changed(MaterialPtr)),
            this, SLOT(materialChanged(MaterialPtr)));

    MainWindow::instance().openMaterial(mat);
  }

  item.header->setText(mat->name());

  updateUniforms(item, mat);
  updateAttributes(item, mat);
}

void MaterialProperties::updateUniforms(MaterialItem& item, MaterialPtr mat) {
  QSet<QString> names;
  UniformVar::List list = mat->uniformList();
  foreach (UniformVar var, list) {
    const ShaderTypeInfo& type = var.typeinfo();
    names << var.name();

    UniformEditor* editor = item.uniform_editors.value(var.name());
    if (!editor) {
      editor = createEditor(mat, var, type, item.last_index.row() + 1);
      if (editor) {
        item.last_index = editor->index();
        model()->setData(item.last_index, mat->name(), Qt::UserRole);
        item.uniform_editors[var.name()] = editor;
      }
    }

    if (editor)
      editor->updateUI(var);
  }

  foreach (QString name, item.uniform_editors.keys().toSet() - names) {
    UniformEditor* editor = item.uniform_editors[name];
    if (item.last_index.row() == editor->index().row()) {
      item.last_index = item.last_index.sibling(item.last_index.row()-1, 0);
    }
    removeRow(editor->index().row());
    editor->deleteLater();
    item.uniform_editors.remove(name);
  }
}

void MaterialProperties::updateAttributes(MaterialItem& item, MaterialPtr mat) {
  QSet<QString> names;
  AttributeVar::List list = mat->attributeList();
  foreach (AttributeVar var, list) {
    //const ShaderTypeInfo& type = var.typeinfo();
    names << var.name();
    AttributeEditor* editor = item.attribute_mappers.value(var.name());
    if (!editor) {
      editor = new AttributeEditor(*this, item.last_index.row() + 1, mat, var);
      item.last_index = editor->index();
      model()->setData(item.last_index, mat->name(), Qt::UserRole);
      item.attribute_mappers[var.name()] = editor;
    }

    if (editor)
      editor->updateUI(var);
  }
  foreach (QString name, item.attribute_mappers.keys().toSet() - names) {
    AttributeEditor* editor = item.attribute_mappers[name];
    if (item.last_index.row() == editor->index().row()) {
      item.last_index = item.last_index.sibling(item.last_index.row()-1, 0);
    }
    removeRow(editor->index().row());
    editor->deleteLater();
    item.attribute_mappers.remove(name);
  }
}

void MaterialProperties::updateMaterialList(ScenePtr scene) {
  QSet<MaterialPtr> current = m_materials.keys().toSet();
  QSet<MaterialPtr> materials = scene->materials().values().toSet();
  foreach (MaterialPtr m, current - materials) {
    MaterialItem& item = m_materials[m];
    int r1 = item.header_index.row(), r2 = item.last_index.row();
    for (int r = r2; r >= r1; --r)
      removeRow(r);
    foreach (UniformEditor* e, item.uniform_editors)
      e->deleteLater();
    m_materials.remove(m);
    disconnect(m.get(), SIGNAL(changed(MaterialPtr)),
               this, SLOT(materialChanged(MaterialPtr)));

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
  MaterialPtr m = getSelected();

  if (m_selected != m) {
    if (m_selected && m_materials.contains(m_selected))
      m_materials[m_selected].header->setSelected(false);
    if (m && m_materials.contains(m))
      m_materials[m].header->setSelected(true);
    m_selected = m;
  }

  if (!m_create) return;

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

void MaterialProperties::itemSelected(int row, int column) {
  MaterialPtr m = get(model()->index(row, column));
  if (m) MainWindow::instance().openMaterial(m);
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

  QSettings settings("Shaderkit", "Shaderkit");
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
    tex->setFilename(file);

    m->addTexture("diffuse", TexturePtr(tex));
    s->setMaterial(m->name(), m);
  }
}

void MaterialProperties::duplicate() {
  MaterialPtr orig = getSelected();
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
  MaterialPtr m = getSelected();
  if (!m) return;
  ScenePtr s = m->scene();
  if (!s) return;
  s->remove(m);
}

void MaterialProperties::toggleMode() {

}

void MaterialProperties::materialChanged(MaterialPtr mat) {
  update(mat);
}

UniformEditor* MaterialProperties::createEditor(MaterialPtr mat, UniformVar& var,
                                                const ShaderTypeInfo& type, int row) {
  if (var.arraySize() == 1) {
    if (type.type == GL_FLOAT) {
      return new FloatEditor(*this, row, mat, var);
    } else if (type.type == GL_SAMPLER_2D) {
      return new TextureEditor(*this, row, mat, var);
    } else {
      /// @todo implement
    }
  } else {
    /// @todo implement
  }
  return 0;
}

void MaterialProperties::startDrag(Qt::DropActions supportedActions) {
  MaterialPtr material = getSelected();
  if (!material) {
    Log::info("no material");
    return;
  }
  //qApp->setOverrideCursor(QCursor(QPixmap(":/icons/shader.png")));

  QMimeData* data = new QMimeData;
  data->setData("text/x-shaderkit-material", material->name().toUtf8());

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
  MaterialPtr m = get(indexAt(e->pos()));

  if (m && m_materials.contains(m)) {
    menu->addAction(m_duplicate);
    menu->addSeparator();
    menu->addAction(m_edit);
    menu->addAction(m_destroy);
    menu->addSeparator();
    setCurrentIndex(m_materials[m].header_index);
    selectionChanged();
  }

  menu->addAction(m_only_uniforms);
  menu->addSeparator();
  menu->addAction(m_create);
  menu->addAction(m_open);
  menu->exec(e->globalPos());
}

MaterialPtr MaterialProperties::get(QModelIndex index) const {
  QModelIndex idx = index.column() == 0 ? index : index.sibling(index.row(), 0);
  return MainWindow::scene()->material(idx.data(Qt::UserRole).toString());
}

MaterialPtr MaterialProperties::getSelected() const {
  auto lst = selectedIndexes();
  if (lst.size() == 1) return get(lst[0]);
  return MaterialPtr();
}
#if 0

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
#endif

QItemSelectionModel::SelectionFlags MaterialProperties::selectionCommand(
    const QModelIndex& index, const QEvent* event) const {
  QWidget* w = indexWidget(index);
  if (w && typeid(*w) == typeid(HeaderWidget)) {
    return QTableWidget::selectionCommand(index, event);
  }
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
