#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include "scene_object.hpp"

class Animation : public SceneObject {
public:
  Animation(QString name);

  QVariantMap save() const;
  void load(QVariantMap map);
};

#endif // ANIMATION_HPP
