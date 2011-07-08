#ifndef GIZMOS_HPP
#define GIZMOS_HPP

#include "forward.hpp"

#include <QSize>
#include <QPointF>
#include <QVector>
#include <QRectF>

class Gizmo {
public:
  Gizmo();
  virtual ~Gizmo();

  void setObject(ObjectPtr obj);

  virtual void render(QSize size, State& state, const RenderOptions& render_opts) = 0;

  void hover(QPointF point);
  bool buttonDown(QPointF point);
  void input(QPointF diff);
  void buttonUp();

  void setScale(float scale);
  float size() const;

  enum Constraint {
    NONE,
    X,
    Y,
    Z,
    XY,
    XZ,
    YZ,
    VIEW
  };

protected:
  Constraint m_hover;
  Constraint m_selected;

  ObjectPtr m_object;

  float m_scale;

  bool m_active;

  struct HitShape {
    HitShape(const QVector<QPointF>& polyline_ = QVector<QPointF>(),
             const QRectF& bbox_ = QRectF(),
             float distance2_ = 0, Constraint group_ = NONE)
      : polyline(polyline_), bbox(bbox_), distance2(distance2_), group(group_) {}

    QVector<QPointF> polyline;
    QRectF bbox;
    float distance2;
    Constraint group;
  };

  QList<HitShape> m_hit_shapes;

  const HitShape* pick(QPointF point) const;
};

class TranslateGizmo : public Gizmo {
public:
  virtual void render(QSize size, State& state, const RenderOptions& render_opts);
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
