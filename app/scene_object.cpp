#include "scene_object.hpp"

SceneObject::SceneObject(const QString& name) : m_name(name) {}
SceneObject::~SceneObject() {}

void SceneObject::setName(QString name) {
  m_name = name;
}

QVariantMap SceneObject::save() const {
  QVariantMap map;
  if (!m_ref.isEmpty()) {
    QStringList tmp;
    tmp << m_ref;
    tmp += m_ref_name;
    map["ref"] = tmp;
  }
  return map;
}

void SceneObject::load(QVariantMap map) {
  QStringList tmp = map["ref"].toStringList();
  if (tmp.size() > 0) {
    m_ref = tmp[0];
    m_ref_name = tmp.mid(1);
  }
}

void SceneObject::setRef(QString import, QStringList iname) {
  m_ref = import;
  m_ref_name = iname;
}

QString SceneObject::ref() {
  return m_ref;
}

QStringList SceneObject::refName() {
  return m_ref_name;
}
