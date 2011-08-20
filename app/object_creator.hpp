#ifndef OBJECT_CREATOR_HPP
#define OBJECT_CREATOR_HPP

#include "forward.hpp"

#include "buffer_object.hpp"

#include "Eigen/Geometry"

class QString;
class QMouseEvent;

class FocusGrabber {
public:
  virtual bool move(QMouseEvent* ev) = 0;
  virtual bool btn(QMouseEvent* ev) = 0;
  virtual void render(State& state, const RenderOptions& render_opts) = 0;
  virtual bool done() const = 0;
};

class ObjectCreator : public FocusGrabber {
public:
  ObjectCreator(ScenePtr scene, const QString& name);
  virtual bool move(QMouseEvent* ev);
  virtual bool btn(QMouseEvent* ev);
  virtual void render(State& state, const RenderOptions& render_opts);
  virtual bool done() const;

private:
  int m_state;
  ScenePtr m_scene;
  QString m_name;
  Eigen::Vector3f m_points[5];
  Eigen::Vector3f m_hover;

  VertexAttrib m_attrib;
  ProgramPtr m_prog;
  ProgramPtr m_prog2;

  Eigen::Projective3f m_window_to_obj;

  Eigen::Vector3f hit(const Eigen::Vector2f& c);
  void renderBox(State& state, const RenderOptions& render_opts);
  void renderSphere(State& state, const RenderOptions& render_opts);
};

#endif // OBJECT_CREATOR_HPP
