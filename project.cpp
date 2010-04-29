#include "project.hpp"
#include "scene.hpp"
#include "json_value.hpp"

ScenePtr Project::load(const QString& filename) {
  ScenePtr scene;
  Value v;
  if (v.load(filename)) {
    scene.reset(new Scene);
    scene->load(v);
  }
  return scene;
}
