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

#ifndef GIZMOS_HPP
#define GIZMOS_HPP

#include "forward.hpp"

#include "gl/buffer_object.hpp"
#include "gl/transform_feedback.hpp"

#include "Eigen/Geometry"

#include <QSize>
#include <QVector>
#include <QRectF>

namespace Shaderkit {

class LineSegment {
public:
  LineSegment(const Eigen::Vector3f& a = Eigen::Vector3f(0, 0, 0),
              const Eigen::Vector3f& b = Eigen::Vector3f(0, 0, 0));

  float hit(const Eigen::Vector2f& p, float threshold2) const;
  bool hit(const Eigen::Vector2f& p, float threshold2, float& depth) const;

  const Eigen::Vector2f& point() const { return m_point; }
  const Eigen::Vector2f& point2() const { return m_point2; }

private:
  Eigen::Vector2f m_point, m_point2;
  float m_depth, m_depth2;

  Eigen::Vector2f m_unit;
  float m_length;
};

class Gizmo {
public:
  Gizmo();
  virtual ~Gizmo();

  void setObject(ObjectPtr obj);

  void render(QSize size, State& state, const RenderOptions& render_opts);

  void hover(const Eigen::Vector2f& point);
  bool buttonDown(const Eigen::Vector2f& point);
  virtual void input(const Eigen::Vector2f& diff);
  virtual void buttonUp();

  float size() const;

  bool active() const { return m_active; }

  enum Constraint {
    X = 0,
    Y,
    Z,
    XY,
    XZ,
    YZ,
    VIEW,

    NONE
  };

protected:
  Constraint m_hover;
  Constraint m_selected;

  ObjectPtr m_object;

  bool m_active;

  enum HitMode {
    FRONT,
    NEAREST
  } m_hit_mode;

  struct HitShape {
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    HitShape(const QVector<Eigen::Vector3f>& points_ = QVector<Eigen::Vector3f>(),
             Constraint group_ = NONE)
      : points(points_), group(group_) {}

    void transform(const Eigen::Projective3f& window_projection);

    QVector<Eigen::Vector3f> points;
    QVector<LineSegment> transformed;
    Eigen::AlignedBox<float, 2> bbox;
    Constraint group;
  };

  std::vector<HitShape, Eigen::aligned_allocator<HitShape>> m_hit_shapes;

  ProgramPtr m_prog;

  // gizmo to window
  Eigen::Projective3f m_window_projection;
  float m_scale, m_scale_factor;
  Eigen::Vector2f m_start_cursor, m_current_cursor;
  Eigen::Affine3f m_object_orig_transform;

  // Transform from gizmo to object coordinates
  Eigen::Affine3f m_gizmo_to_obj;

  bool m_update_inv_projection;
  // inverse of m_window_projection, only updated when starting interaction
  Eigen::Projective3f m_window_to_gizmo;

  BufferObject m_verts, m_colors;

  const HitShape* pick(const Eigen::Vector2f& point) const;
  virtual bool makeActive(Constraint type);
  virtual void renderImpl(State& state) = 0;
};

class TranslateGizmo : public Gizmo {
public:
  TranslateGizmo();
  virtual void input(const Eigen::Vector2f& diff);

private:
  virtual void renderImpl(State& state);
  virtual bool makeActive(Constraint type);

  Eigen::ParametrizedLine<float, 3> m_line;
  Eigen::Hyperplane<float, 3> m_plane;

  bool m_update_line;
};

class RotateGizmo : public Gizmo {
public:
  RotateGizmo();
  virtual void input(const Eigen::Vector2f& diff);

protected:
  virtual void renderImpl(State& state);
  virtual bool makeActive(Constraint type);

  bool m_update_center;
  bool m_reversed;
  float m_angle, m_start_angle;
  Eigen::Vector2f m_center;
};

class ScaleGizmo : public Gizmo {
public:
protected:
  virtual void renderImpl(State& state);
};

} // namespace Shaderkit

#endif // GIZMOS_HPP
