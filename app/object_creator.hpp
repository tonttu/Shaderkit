#ifndef OBJECT_CREATOR_HPP
#define OBJECT_CREATOR_HPP

#include "forward.hpp"

#include "Eigen/Geometry"

class QString;
class QMouseEvent;

class FocusGrabber {
public:
  virtual bool move(QMouseEvent* ev) = 0;
  virtual bool btn(QMouseEvent* ev) = 0;
  virtual void render(State& state, const RenderOptions& render_opts) = 0;
};

class ObjectCreator : public FocusGrabber {
public:
  ObjectCreator(ScenePtr scene, const QString& name);
  virtual bool move(QMouseEvent* ev);
  virtual bool btn(QMouseEvent* ev);
  virtual void render(State& state, const RenderOptions& render_opts);

private:
  int m_state;
  ScenePtr m_scene;
  QString m_name;
  Eigen::Vector3f m_points[5];
  Eigen::Vector3f m_hover;

  ProgramPtr m_prog;

  Eigen::Projective3f m_window_to_obj;

  Eigen::Vector3f hit(const Eigen::Vector2f& c);
  void renderBox(State& state, const RenderOptions& render_opts);
};

#endif // OBJECT_CREATOR_HPP
