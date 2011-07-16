#ifndef GIZMOS_HPP
#define GIZMOS_HPP

#include "forward.hpp"

#include "buffer_object.hpp"
#include "transform_feedback.hpp"

#include "Eigen/Geometry"

#include <QSize>
#include <QVector>
#include <QRectF>

class LineSegment {
public:
  LineSegment(const Eigen::Vector2f& a = Eigen::Vector2f(0, 0),
              const Eigen::Vector2f& b = Eigen::Vector2f(0, 0));

  float hit(const Eigen::Vector2f& p, float threshold2) const;

  const Eigen::Vector2f& point() const { return m_point; }
  const Eigen::Vector2f& point2() const { return m_point2; }

private:
  Eigen::Vector2f m_point, m_point2;

  Eigen::Vector2f m_unit;
  float m_length;
};

class Gizmo {
public:
  Gizmo();
  virtual ~Gizmo();

  void setObject(ObjectPtr obj);

  virtual void render(QSize size, State& state, const RenderOptions& render_opts) = 0;

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

  Eigen::Projective3f m_window_projection;
  float m_scale;
  Eigen::Vector2f m_start_cursor, m_current_cursor;
  Eigen::Affine3f m_object_orig_transform;

  bool m_update_ray_projection;
  Eigen::Projective3f m_ray_projection;

  const HitShape* pick(const Eigen::Vector2f& point) const;
  virtual bool makeActive(Constraint type);
};

class TranslateGizmo : public Gizmo {
public:
  TranslateGizmo();
  virtual void render(QSize size, State& state, const RenderOptions& render_opts);
  virtual void input(const Eigen::Vector2f& diff);

private:
  virtual bool makeActive(Constraint type);

  Eigen::ParametrizedLine<float, 3> m_line;
  Eigen::Hyperplane<float, 3> m_plane;

  bool m_update_line;

  BufferObject m_verts, m_colors;
};

class RotateGizmo : public Gizmo {
public:
  virtual void render(QSize size, State& state, const RenderOptions& render_opts);
};

class ScaleGizmo : public Gizmo {
public:
  virtual void render(QSize size, State& state, const RenderOptions& render_opts);
};

#endif // GIZMOS_HPP
