#include "gizmos.hpp"

const char* vertex_shader =
    "#version 150 compatibility\n"
    "\n"
    "precision highp float;\n"
    "\n"
    "out vec2 point;\n"
    "\n"
    "void main() {\n"
    "  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;\n"
    "  point = -gl_Position.xy / gl_Position.w;\n"
    "}\n";


Gizmo::Gizmo() : m_hover(NONE), m_selected(NONE), m_scale(1.0f), m_active(false) {}
Gizmo::~Gizmo() {}

void Gizmo::setObject(ObjectPtr obj) {
  if(m_active)
    buttonUp();
  m_object = obj;
}

void Gizmo::hover(QPointF point) {
  const HitShape* hit = pick(point);
  m_hover = hit ? hit->group : m_selected;
}

bool Gizmo::buttonDown(QPointF point) {
  const HitShape* hit = pick(point);
  if (hit) {
    m_selected = hit->group;
    m_active = true;
  }
  return hit != 0;
}

void Gizmo::input(QPointF diff) {
  /// @todo do something
}

void Gizmo::buttonUp() {
  /// @todo recalc scale
  m_active = false;
}

void Gizmo::setScale(float scale) {
  m_scale = scale;
}

float Gizmo::size() const {
  return 100.0f;
}

const Gizmo::HitShape* Gizmo::pick(QPointF point) const {
  const HitShape* hit = 0;
  float hit_dist2 = std::numeric_limits<float>::infinity();
  foreach (const HitShape& shape, m_hit_shapes) {
    if (!shape.bbox.contains(point)) continue;
    foreach (const QPointF& p, shape.polyline) {
      QPointF diff = p - point;
      float dist2 = diff.x()*diff.x() + diff.y()*diff.y();
      if(dist2 <= shape.distance2 && dist2 < hit_dist2) {
        hit_dist2 = dist2;
        hit = &shape;
      }
    }
  }

  return hit;
}

void TranslateGizmo::render(QSize size, State& state, const RenderOptions& render_opts) {

}

void RotateGizmo::render(QSize size, State& state, const RenderOptions& render_opts) {

}

void ScaleGizmo::render(QSize size, State& state, const RenderOptions& render_opts) {

}
