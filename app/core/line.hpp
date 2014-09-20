#ifndef SHADERKIT_LINE_HPP
#define SHADERKIT_LINE_HPP

#include "plane.hpp"

#include "glm/gtx/intersect.hpp"

namespace Shaderkit
{
  class Line
  {
  public:
    inline static Line through(glm::vec3 p0, glm::vec3 p1);
    inline static Line originAndDirection(glm::vec3 origin, glm::vec3 unitDirection);

    inline glm::vec3 origin() const { return m_origin; }
    inline glm::vec3 direction() const { return m_direction; }

    inline glm::vec3 intersection(const Plane& plane) const;

    inline Line() {}

  private:
    inline Line(glm::vec3 origin, glm::vec3 direction);

  private:
    glm::vec3 m_origin;
    glm::vec3 m_direction;
  };

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  Line Line::through(glm::vec3 p0, glm::vec3 p1)
  {
    return Line(p0, glm::normalize(p1-p0));
  }

  Line Line::originAndDirection(glm::vec3 origin, glm::vec3 unitDirection)
  {
    return Line(origin, unitDirection);
  }

  Line::Line(glm::vec3 origin, glm::vec3 direction)
    : m_origin(origin),
      m_direction(direction)
  {}

  glm::vec3 Line::intersection(const Plane& plane) const
  {
    float dist = 0;
    glm::intersectRayPlane(m_origin, m_direction, plane.origin(), plane.normal(), dist);
    return m_origin + m_direction * dist;
  }
}

#endif // SHADERKIT_LINESEGMENT_HPP
