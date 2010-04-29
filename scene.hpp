#ifndef SCENE_HPP
#define SCENE_HPP

#include "forward.hpp"

#include <QString>

#include <boost/enable_shared_from_this.hpp>

#include <map>
#include <vector>

/**
 * Scene is a data structure that stores all objects by name, and controls
 * the rendering. Scene also knows the actual viewport size and handles
 * the JSON loading. Currently the whole JSON project-file only contains
 * stuff that are actually only in Scene.
 */
class Scene : public boost::enable_shared_from_this<Scene> {
public:
  Scene();

  /// Set the viewport size
  void resize(int width, int height);

  /// Renders everything in this scene
  void render();

  /// Current viewport width
  /// @todo QRect?
  int width() const { return m_width; }
  /// Current viewport height
  int height() const { return m_height; }

  ProgramPtr shader(QString name) { return m_shaders[name]; }
  ObjectPtr object(QString name) { return m_objects[name]; }
  LightPtr light(QString name) { return m_lights[name]; }
  CameraPtr camera(QString name) { return m_cameras[name]; }

  /// Load the scene from JSON Value
  void load(const Value& value);

protected:
  typedef std::vector<RenderPassPtr> RenderPasses;
  /**
   * RenderPasses is defined as an ordered list of passes, while actually the
   * render passes form a directed dependency graph (without loops).
   * @todo The JSON structure should be treated as a set of Render Passes, and
   *       the actual order should be calculated when loading it.
   */
  RenderPasses m_render_passes;

  std::map<QString, ObjectPtr> m_objects;
  std::map<QString, LightPtr> m_lights;
  std::map<QString, CameraPtr> m_cameras;
  std::map<QString, ProgramPtr> m_shaders;
  
  int m_width, m_height;
};

#endif // SCENE_HPP
