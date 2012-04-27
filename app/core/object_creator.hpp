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

#ifndef OBJECT_CREATOR_HPP
#define OBJECT_CREATOR_HPP

#include "forward.hpp"

#include "gl/buffer_object.hpp"

#include "Eigen/Geometry"

class QString;
class QMouseEvent;

namespace Shaderkit {

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

  ProgramPtr m_prog;
  ProgramPtr m_prog2;

  Eigen::Projective3f m_window_to_obj;

  Eigen::Vector3f hit(const Eigen::Vector2f& c);
  void renderBox(State& state, const RenderOptions& render_opts);
  void renderSphere(State& state, const RenderOptions& render_opts);
};

} // namespace Shaderkit

#endif // OBJECT_CREATOR_HPP
