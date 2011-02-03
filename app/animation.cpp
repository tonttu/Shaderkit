#include "animation.hpp"

Animation::Animation(QString name) : SceneObject(name) {
}

QVariantMap Animation::save() const {
  QVariantMap map = SceneObject::save();
  return map;
}

void Animation::load(QVariantMap map) {
  SceneObject::load(map);
}
