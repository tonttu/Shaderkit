#ifndef SHADERKIT_PLANE_HPP
#define SHADERKIT_PLANE_HPP

namespace Shaderkit
{
  class Plane
  {
  public:
    Plane() {}
    Plane(glm::vec3 normal, glm::vec3 point) : m_origin(point), m_normal(normal) {}

    glm::vec3 origin() const { return m_origin; }
    glm::vec3 normal() const { return m_normal; }

  private:
    glm::vec3 m_origin;
    glm::vec3 m_normal;
  };

}

#endif // SHADERKIT_PLANE_HPP
