#include "render_pass_properties.hpp"
#include "renderpass.hpp"
#include "scene.hpp"
#include "shader/program.hpp"

#include <typeinfo>

RenderPassProperties* RenderPassProperties::s_instance = 0;

ShaderEditor::ShaderEditor(RenderPassPtr pass) : m_pass(pass) {
  QHBoxLayout* layout = new QHBoxLayout(this);

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
  layout->addWidget(edit);

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

  QTreeWidgetItem* shader = new QTreeWidgetItem(sub.item);
  shader->setText(0, "Shader");
  setItemWidget(shader, 1, new ShaderEditor(pass));
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
