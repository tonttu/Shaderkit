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
  void input(const Eigen::Vector2f& diff);
  void buttonUp();

  float size() const;

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

    void transform(const Eigen::Projective3f& projection, QSize size);

    QVector<Eigen::Vector3f> points;
    QVector<LineSegment> transformed;
    Eigen::AlignedBox<float, 2> bbox;
    Constraint group;
  };

  std::vector<HitShape, Eigen::aligned_allocator<HitShape>> m_hit_shapes;

  ProgramPtr m_prog;

  float m_scale;

  const HitShape* pick(const Eigen::Vector2f& point) const;
};

class TranslateGizmo : public Gizmo {
public:
  virtual void render(QSize size, State& state, const RenderOptions& render_opts);

private:
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
