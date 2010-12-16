#include "render_pass_properties.hpp"
#include "renderpass.hpp"
#include "scene.hpp"
#include "shader/program.hpp"
#include "object3d.hpp"
#include "light.hpp"
#include "camera.hpp"
#include "texture.hpp"

#include <cassert>

RenderPassProperties* RenderPassProperties::s_instance = 0;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ShaderEditor::ShaderEditor(RenderPassPtr pass) : m_pass(pass) {
  QHBoxLayout* layout = new QHBoxLayout(this);

  layout->setContentsMargins(2, 2, 2, 2);
  layout->setSpacing(0);

  setAutoFillBackground(true);

  m_shaderlist = new QComboBox(this);

  /// @todo why this or something similar won't work?
  /// @todo do not use our custom view or model
  // m_shaderlist->model()->setData(m_shaderlist->model()->index(0, 0), font, Qt::FontRole);
  {
    QListWidget* view = new QListWidget(this);

    QListWidgetItem* item = new QListWidgetItem("None");
    QFont font = item->font();
    font.setBold(true);
    item->setFont(font);
    view->addItem(item);

    /// @todo just make this a new button
    item = new QListWidgetItem("Add new...");
    item->setFont(font);
    view->addItem(item);

    // m_shaderlist->insertSeparator(2);
    item = new QListWidgetItem("");
    item->setFlags(Qt::NoItemFlags);
    font.setPixelSize(2);
    item->setFont(font);
    view->addItem(item);

    m_shaderlist->setModel(view->model());
    m_shaderlist->setView(view);
  }

  updateShaderList();

  layout->addWidget(m_shaderlist, 1);

  QPushButton* edit = new QPushButton("edit", this);
  // edit->setStyleSheet("QPushButton { padding: 3px 5px 3px 5px; }");
  layout->addWidget(edit);

  layout->addStretch(2);

  connect(m_shaderlist, SIGNAL(activated(int)), this, SLOT(listActivated(int)));
  connect(pass->scene().get(), SIGNAL(shaderListUpdated()), this, SLOT(updateShaderList()));
}

void ShaderEditor::updateShaderList() {
  while (m_shaderlist->count() > 3)
    m_shaderlist->removeItem(3);

  ProgramPtr selected = m_pass->shader();
  bool found = false;

  QMap<QString, ProgramPtr> lst = m_pass->scene()->shaders();
  for (QMap<QString, ProgramPtr>::iterator it = lst.begin(); it != lst.end(); ++it) {
    m_shaderlist->addItem((*it)->name(), it.key());
    if (!found && selected == *it) {
      m_shaderlist->setCurrentIndex(m_shaderlist->count()-1);
      found = true;
    }
  }

  if (!found)
    m_shaderlist->setCurrentIndex(0);
}

void ShaderEditor::listActivated(int index) {
  if (index == 0) {
    m_pass->setShader(ProgramPtr());
  } else if (index == 1) {
    /// @todo edit empty shader
  } else if (index > 2) {
    m_pass->setShader(m_pass->scene()->shader(m_shaderlist->itemData(index).toString()));
  }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

SizeEditor::SizeEditor(RenderPassPtr pass) : m_pass(pass) {
  QHBoxLayout* layout = new QHBoxLayout(this);

  layout->setContentsMargins(2, 2, 2, 2);
  layout->setSpacing(0);

  setAutoFillBackground(true);

  m_size = new QLineEdit(this);
  m_size->setValidator(new QRegExpValidator(QRegExp("\\d+x\\d+"), this));

  layout->addWidget(m_size, 1);

  m_autobtn = new QPushButton("auto", this);
  m_autobtn->setCheckable(true);
  m_autobtn->setChecked(false);
  // m_autobtn->setStyleSheet("QPushButton { padding: 3px 5px 3px 5px; }");
  layout->addWidget(m_autobtn);
  layout->addStretch(2);

  connect(m_size, SIGNAL(editingFinished()), this, SLOT(sizeChanged()));
  connect(pass.get(), SIGNAL(changed(RenderPassPtr)), this, SLOT(updateSize(RenderPassPtr)));
  connect(m_autobtn, SIGNAL(toggled(bool)), this, SLOT(btnToggled(bool)));

  updateSize(m_pass);
}

void SizeEditor::updateSize(RenderPassPtr pass) {
  assert(pass == m_pass);
  m_size->setText(QString("%1x%2").arg(m_pass->width()).arg(m_pass->height()));
  m_autobtn->setChecked(m_pass->autosize());
}

void SizeEditor::sizeChanged() {
  QRegExp r("^(\\d+)x(\\d+)$");
  if (r.exactMatch(m_size->text())) {
    int w = r.cap(1).toInt(), h = r.cap(2).toInt();
    m_pass->resize(w, h);
  }
}

void SizeEditor::btnToggled(bool state) {
  if (state) {
    m_size->setDisabled(true);
    m_pass->setAutosize(true);
  } else {
    m_size->setEnabled(true);
    m_pass->setAutosize(false);
    sizeChanged();
  }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ObjectInserter::ObjectInserter(RenderPassPtr pass) : m_pass(pass) {
  QHBoxLayout* layout = new QHBoxLayout(this);

  layout->setContentsMargins(2, 2, 2, 2);
  layout->setSpacing(0);

  setAutoFillBackground(true);

  m_availableObjects = new QComboBox(this);

  /// @todo make just two new buttons
  {
    QListWidget* view = new QListWidget(this);

    QListWidgetItem* item = new QListWidgetItem("Load from file...");
    QFont font = item->font();
    font.setBold(true);
    item->setFont(font);
    view->addItem(item);

    item = new QListWidgetItem("Built-in object...");
    item->setFont(font);
    view->addItem(item);

    // m_shaderlist->insertSeparator(2);
    item = new QListWidgetItem("");
    item->setFlags(Qt::NoItemFlags);
    font.setPixelSize(2);
    item->setFont(font);
    view->addItem(item);

    m_availableObjects->setModel(view->model());
    m_availableObjects->setView(view);
  }

  updateObjectList();

  layout->addWidget(m_availableObjects);
  layout->addStretch(2);

  connect(m_availableObjects, SIGNAL(activated(int)), this, SLOT(listActivated(int)));
  connect(pass->scene().get(), SIGNAL(objectListUpdated()), this, SLOT(updateObjectList()));
  connect(pass.get(), SIGNAL(changed(RenderPassPtr)), this, SLOT(updated(RenderPassPtr)));
}

void ObjectInserter::updateObjectList() {
  while (m_availableObjects->count() > 3)
    m_availableObjects->removeItem(3);

  QMap<QString, ObjectPtr> lst = m_pass->scene()->objects();
  RenderPass::Objects objs = m_pass->objects();

  for (QMap<QString, ObjectPtr>::iterator it = lst.begin(); it != lst.end(); ++it)
    if (!objs.contains(*it))
      m_availableObjects->addItem((*it)->name(), it.key());
}

void ObjectInserter::listActivated(int index) {
  if (index == 0) {
    /// @todo load from file
  } else if (index == 1) {
    /// @todo built-in object
  } else if (index > 2) {
    ObjectPtr o = m_pass->scene()->object(m_availableObjects->itemData(index).toString());
    if (o) {
      RenderPass::Objects objs = m_pass->objects();
      objs.insert(o);
      m_pass->setObjects(objs);
    }
  }
}

void ObjectInserter::updated(RenderPassPtr) {
  updateObjectList();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ObjectEditor::ObjectEditor(RenderPassPtr pass, ObjectPtr obj)
  : m_pass(pass), m_obj(obj) {
  QHBoxLayout* layout = new QHBoxLayout(this);

  layout->setContentsMargins(2, 2, 2, 2);
  layout->setSpacing(0);

  setAutoFillBackground(true);

  QPushButton* edit = new QPushButton("edit", this);
  // edit->setStyleSheet("QPushButton { padding: 3px 5px 3px 5px; }");
  layout->addWidget(edit);

  QPushButton* del = new QPushButton("delete", this);
  // del->setStyleSheet("QPushButton { padding: 3px 5px 3px 5px; }");
  layout->addWidget(del);
  layout->addStretch(2);

  connect(edit, SIGNAL(clicked()), this, SLOT(editClicked()));
  connect(del, SIGNAL(clicked()), this, SLOT(deleteClicked()));
}

void ObjectEditor::editClicked() {
  /// @todo
}

void ObjectEditor::deleteClicked() {
  RenderPass::Objects objs = m_pass->objects();
  objs.remove(m_obj);
  m_pass->setObjects(objs);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ObjectsEditor::ObjectsEditor(QTreeWidgetItem* parent, RenderPassPtr pass)
  : QTreeWidgetItem(parent), m_pass(pass) {
  setText(0, "Objects");
  updated(pass);

  QTreeWidgetItem* item = new QTreeWidgetItem(this);
  treeWidget()->setItemWidget(item, 1, new ObjectInserter(pass));

  connect(pass.get(), SIGNAL(changed(RenderPassPtr)), this, SLOT(updated(RenderPassPtr)));
}

void ObjectsEditor::updated(RenderPassPtr pass) {
  assert(pass == m_pass);

  RenderPass::Objects objs = pass->objects();
  int count = objs.size();
  setText(1, count == 1 ? QString("1 object") : QString("%1 objects").arg(count));

  RenderPass::Objects current = m_objs.keys().toSet();

  // deleted objects
  foreach (ObjectPtr o, current - objs) {
    QPair<QTreeWidgetItem*, ObjectEditor*> tmp = m_objs[o];
    removeChild(tmp.first);
    delete tmp.first;
    tmp.second->deleteLater();
    m_objs.remove(o);
  }

  // new objects
  foreach (ObjectPtr o, objs - current) {
    QTreeWidgetItem* item = new QTreeWidgetItem;
    insertChild(0, item);
    ObjectEditor* editor = new ObjectEditor(pass, o);
    item->setText(0, o->name());
    treeWidget()->setItemWidget(item, 1, editor);
    m_objs[o] = qMakePair(item, editor);
  }

  // update objects
  foreach (ObjectPtr o, objs & current) {
    QTreeWidgetItem* item = m_objs[o].first;
    item->setText(0, o->name());
  }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

LightEditor::LightEditor(RenderPassPtr pass, LightPtr light)
  : m_pass(pass), m_light(light) {
  QHBoxLayout* layout = new QHBoxLayout(this);

  layout->setContentsMargins(2, 2, 2, 2);
  layout->setSpacing(0);

  setAutoFillBackground(true);

  m_enabled = new QPushButton("enabled", this);
  m_enabled->setCheckable(true);
  // m_enabled->setStyleSheet("QPushButton { padding: 3px 5px 3px 5px; }");
  layout->addWidget(m_enabled);

  QPushButton* edit = new QPushButton("edit", this);
  // edit->setStyleSheet("QPushButton { padding: 3px 5px 3px 5px; }");
  layout->addWidget(edit);
  layout->addStretch(2);

  connect(m_enabled, SIGNAL(toggled(bool)), this, SLOT(toggled(bool)));
  connect(edit, SIGNAL(clicked()), this, SLOT(editClicked()));
}

void LightEditor::setStatus(bool v) {
  m_enabled->setChecked(v);
}

void LightEditor::editClicked() {
  /// @todo
}

void LightEditor::toggled(bool v) {
  RenderPass::Lights lights = m_pass->lights();
  if (v)
    lights.insert(m_light);
  else
    lights.remove(m_light);
  m_pass->setLights(lights);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

LightsEditor::LightsEditor(QTreeWidgetItem* parent, RenderPassPtr pass)
  : QTreeWidgetItem(parent), m_pass(pass) {
  setText(0, "Lights");
  updated(pass);

  connect(pass->scene().get(), SIGNAL(lightListUpdated()), this, SLOT(updateLightList()));
  connect(pass.get(), SIGNAL(changed(RenderPassPtr)), this, SLOT(updated(RenderPassPtr)));
}

void LightsEditor::updateLightList() {
  updated(m_pass);
}

void LightsEditor::updated(RenderPassPtr pass) {
  assert(pass == m_pass);

  RenderPass::Lights all_lights = m_pass->scene()->lights().values().toSet();
  RenderPass::Lights pass_lights = pass->lights();

  setText(1, QString("%1 (%2) lights").arg(pass_lights.size()).arg(all_lights.size()));

  RenderPass::Lights current = m_lights.keys().toSet();

  // deleted lights
  foreach (LightPtr l, current - all_lights) {
    QPair<QTreeWidgetItem*, LightEditor*> tmp = m_lights[l];
    removeChild(tmp.first);
    delete tmp.first;
    tmp.second->deleteLater();
    m_lights.remove(l);
  }

  // new lights
  foreach (LightPtr l, all_lights - current) {
    QTreeWidgetItem* item = new QTreeWidgetItem(this);
    LightEditor* editor = new LightEditor(pass, l);
    editor->setStatus(pass_lights.contains(l));
    item->setText(0, l->name());
    treeWidget()->setItemWidget(item, 1, editor);
    m_lights[l] = qMakePair(item, editor);
  }

  // update lights
  foreach (LightPtr l, pass_lights & all_lights) {
    QPair<QTreeWidgetItem*, LightEditor*> tmp = m_lights[l];
    tmp.first->setText(0, l->name());
    tmp.second->setStatus(pass_lights.contains(l));
  }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

CameraEditor::CameraEditor(RenderPassPtr pass) : m_pass(pass) {
  QHBoxLayout* layout = new QHBoxLayout(this);

  layout->setContentsMargins(2, 2, 2, 2);
  layout->setSpacing(0);

  setAutoFillBackground(true);

  m_list = new QComboBox(this);

  /// @todo do not use our custom view or model
  {
    QListWidget* view = new QListWidget(this);

    QListWidgetItem* item = new QListWidgetItem("Post-process");
    QFont font = item->font();
    font.setBold(true);
    item->setFont(font);
    view->addItem(item);

    item = new QListWidgetItem("");
    item->setFlags(Qt::NoItemFlags);
    font.setPixelSize(2);
    item->setFont(font);
    view->addItem(item);

    m_list->setModel(view->model());
    m_list->setView(view);
  }

  updateList();

  layout->addWidget(m_list, 1);

  QPushButton* add = new QPushButton("new", this);
  // add->setStyleSheet("QPushButton { padding: 3px 5px 3px 5px; }");
  layout->addWidget(add);

  QPushButton* edit = new QPushButton("edit", this);
  // edit->setStyleSheet("QPushButton { padding: 3px 5px 3px 5px; }");
  layout->addWidget(edit);
  layout->addStretch(2);

  connect(m_list, SIGNAL(activated(int)), this, SLOT(listActivated(int)));
  connect(add, SIGNAL(clicked()), this, SLOT(newClicked()));
  connect(edit, SIGNAL(clicked()), this, SLOT(editClicked()));
  connect(pass->scene().get(), SIGNAL(cameraListUpdated()), this, SLOT(updateList()));
  connect(pass.get(), SIGNAL(changed(RenderPassPtr)), this, SLOT(updated(RenderPassPtr)));
}

void CameraEditor::updateList() {
  /// @todo just update changed data
  while (m_list->count() > 2)
    m_list->removeItem(2);

  CameraPtr selected = m_pass->viewport();
  bool found = false;

  QMap<QString, CameraPtr> lst = m_pass->scene()->cameras();
  for (QMap<QString, CameraPtr>::iterator it = lst.begin(); it != lst.end(); ++it) {
    m_list->addItem((*it)->name(), it.key());
    if (!found && selected == *it) {
      m_list->setCurrentIndex(m_list->count()-1);
      found = true;
    }
  }

  if (!found && m_pass->type() == RenderPass::PostProc)
    m_list->setCurrentIndex(0);
}

void CameraEditor::updated(RenderPassPtr pass) {
  assert(m_pass == pass);

  updateList();
}

void CameraEditor::listActivated(int index) {
  if (index == 0) {
    m_pass->setType(RenderPass::PostProc);
  } else {
    m_pass->setType(RenderPass::Normal);
    m_pass->setViewport(m_pass->scene()->camera(m_list->itemData(index).toString()));
  }
}

void CameraEditor::newClicked() {
  /// @todo
}

void CameraEditor::editClicked() {
  /// @todo
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ClearEditor::ClearEditor(RenderPassPtr pass) : m_pass(pass) {
  QHBoxLayout* layout = new QHBoxLayout(this);

  layout->setContentsMargins(2, 2, 2, 2);
  layout->setSpacing(0);

  setAutoFillBackground(true);

  m_color = new QPushButton("Color", this);
  m_depth = new QPushButton("Depth", this);
  m_stencil = new QPushButton("Stencil", this);

  m_color->setCheckable(true);
  /// @todo use a real external style sheet
  // m_color->setStyleSheet("QPushButton { padding: 3px 5px 3px 5px; }");
  m_depth->setCheckable(true);
  // m_depth->setStyleSheet("QPushButton { padding: 3px 5px 3px 5px; }");
  m_stencil->setCheckable(true);
  // m_stencil->setStyleSheet("QPushButton { padding: 3px 5px 3px 5px; }");

  layout->addWidget(m_color);
  layout->addWidget(m_depth);
  layout->addWidget(m_stencil);
  layout->addStretch(2);

  updated(m_pass);

  connect(m_color, SIGNAL(clicked()), this, SLOT(clicked()));
  connect(m_depth, SIGNAL(clicked()), this, SLOT(clicked()));
  connect(m_stencil, SIGNAL(clicked()), this, SLOT(clicked()));
  connect(pass.get(), SIGNAL(changed(RenderPassPtr)), this, SLOT(updated(RenderPassPtr)));
}

void ClearEditor::updated(RenderPassPtr pass) {
  assert(m_pass == pass);

  GLbitfield value = pass->clearBits();

  m_color->setChecked(value & GL_COLOR_BUFFER_BIT);
  m_depth->setChecked(value & GL_DEPTH_BUFFER_BIT);
  m_stencil->setChecked(value & GL_STENCIL_BUFFER_BIT);

  setEnabled(pass->type() == RenderPass::Normal);
}

void ClearEditor::clicked() {
  GLbitfield value = 0;
  if (m_color->isChecked()) value |= GL_COLOR_BUFFER_BIT;
  if (m_depth->isChecked()) value |= GL_DEPTH_BUFFER_BIT;
  if (m_stencil->isChecked()) value |= GL_STENCIL_BUFFER_BIT;

  m_pass->setClearBits(value);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TextureEditor::TextureEditor(RenderPassPtr pass) : m_pass(pass) {
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

void TextureEditor::setTex(TexturePtr tex) {
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

void TextureEditor::listActivated(int index) {

}

void TextureEditor::updateList() {
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

void TextureEditor::newClicked() {
  /// @todo
}

void TextureEditor::editClicked() {
  /// @todo
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TexturesEditor::TexturesEditor(QTreeWidgetItem* parent, RenderPassPtr pass)
  : QTreeWidgetItem(parent), m_pass(pass) {
  setText(0, "Textures");
  updated(pass);

  connect(pass.get(), SIGNAL(changed(RenderPassPtr)), this, SLOT(updated(RenderPassPtr)));
}

void TexturesEditor::updated(RenderPassPtr pass) {
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
  }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

RenderPassProperties &RenderPassProperties::instance() {
  if (s_instance) return *s_instance;
  return *(new RenderPassProperties);
}

RenderPassProperties::RenderPassProperties(QWidget* parent)
  : Properties(parent) {
  if (!s_instance) s_instance = this;
  //connect(m_manager, SIGNAL(valueChanged(QtProperty*, const QVariant&)),
  //        this, SLOT(valueChanged(QtProperty*, const QVariant&)));

  QStringList labels;
  labels << "Property" << "Value";
  setHeaderLabels(labels);
  //setColumnCount(2);

  setVerticalScrollMode(ScrollPerPixel);
  setHorizontalScrollMode(ScrollPerPixel);
}

RenderPassProperties::~RenderPassProperties() {
  if (s_instance == this) s_instance = 0;
}

void RenderPassProperties::init(Sub& sub, RenderPassPtr pass) {
  /*sub.obj = m_manager->addProperty(QtVariantPropertyManager::groupTypeId(), pass->name());

  sub.clear = m_manager->addProperty(QtVariantPropertyManager::flagTypeId(), "Clear");
  QStringList clearFlags;
  clearFlags << "Color buffer" << "Depth buffer" << "Stencil buffer";
  sub.clear->setAttribute(QLatin1String("flagNames"), clearFlags);
  sub.obj->addSubProperty(sub.clear);
  m_properties[sub.clear] = qMakePair(Sub::Clear, pass);

  addProperty(sub.obj);*/

  sub.item = new QTreeWidgetItem(this);
  sub.item->setText(0, "Render");
  sub.item->setText(1, "pass");

  QTreeWidgetItem* item = new QTreeWidgetItem(sub.item);
  item->setText(0, "Shader");
  setItemWidget(item, 1, new ShaderEditor(pass));

  item = new QTreeWidgetItem(sub.item);
  item->setText(0, "Size");
  setItemWidget(item, 1, new SizeEditor(pass));

  item = new ObjectsEditor(sub.item, pass);
  item = new LightsEditor(sub.item, pass);

  item = new QTreeWidgetItem(sub.item);
  item->setText(0, "Viewport");
  setItemWidget(item, 1, new CameraEditor(pass));

  item = new QTreeWidgetItem(sub.item);
  item->setText(0, "Clear");
  setItemWidget(item, 1, new ClearEditor(pass));

  item = new TexturesEditor(sub.item, pass);

  /// @todo group and hide/show items by render pass type
}

void RenderPassProperties::update(RenderPassPtr pass) {
  Sub& sub = m_renderpasses[pass];

  // Ensure the existence of the Sub instance of this render pass
  if (!sub.item)
    init(sub, pass);
/*
  sub.obj->setPropertyName(pass->name());

  int value = pass->clearBits() & GL_COLOR_BUFFER_BIT ? 1 : 0;
  if (pass->clearBits() & GL_DEPTH_BUFFER_BIT) value |= 1 << 1;
  if (pass->clearBits() & GL_STENCIL_BUFFER_BIT) value |= 1 << 2;
  sub.clear->setValue(value);

  bool normal = pass->type() == RenderPass::Normal;
  sub.clear->setEnabled(normal);
*/
  /*
  QtVariantProperty* item = m_manager->addProperty(QVariant::Int, "width");
  item->setValue(pass->width());
  obj->addSubProperty(item);

  item = m_manager->addProperty(QVariant::Int, "height");
  item->setValue(pass->height());
  obj->addSubProperty(item);

  QtVariantProperty* in = m_manager->addProperty(QtVariantPropertyManager::groupTypeId(), "in");
  obj->addSubProperty(in);
  foreach (QString name, pass->in()) {
    item = m_manager->addProperty(QVariant::String, name);
    item->setValue(pass->in(name)->name());
    in->addSubProperty(item);
  }

  QtVariantProperty* out = m_manager->addProperty(QtVariantPropertyManager::groupTypeId(), "out");
  obj->addSubProperty(out);
  foreach (QString name, pass->out()) {
    item = m_manager->addProperty(QVariant::String, name);
    item->setValue(pass->out(name)->name());
    in->addSubProperty(item);
  }*/
}
/*
void RenderPassProperties::valueChanged(QtProperty* property, const QVariant& variant) {
  if (!m_properties.contains(property))
    return;

  QPair<Sub::Type, RenderPassPtr> & pair = m_properties[property];

  if (pair.first == Sub::Clear) {
    int value = variant.toInt();
    GLbitfield b = 0;
    if (value & (1 << 0)) b |= GL_COLOR_BUFFER_BIT;
    if (value & (1 << 1)) b |= GL_DEPTH_BUFFER_BIT;
    if (value & (1 << 2)) b |= GL_STENCIL_BUFFER_BIT;
    pair.second->setClearBits(b);
  }
*/
//  QSet<QtProperty*> p = property->parents();
/*  PropertyMap::iterator it = m_properties.find(property);
  if (it == m_properties.end()) return;
  it->set(variant);*/
//}

void RenderPassProperties::remove(RenderPassPtr pass) {
  /*if (!m_renderpasses.contains(pass))
    return;

  Sub& sub = m_renderpasses[pass];

  /// @todo Remove all old subproperties and clean m_properties
  removeProperty(sub.obj);
  m_renderpasses.remove(pass);*/
}
