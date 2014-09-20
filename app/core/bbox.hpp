#ifndef SHADERKIT_BBOX_HPP
#define SHADERKIT_BBOX_HPP

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

#include <algorithm>
#include <limits>

namespace Shaderkit
{
  template <typename T>
  class BBoxT
  {
  public:
    inline BBoxT();

    inline void extend(T v);
    inline void clear();

    inline T min() const { return m_min; }
    inline T max() const { return m_max; }
    inline T size() const { return m_max - m_min; }
    inline T center() const { return typename T::value_type(0.5) * (m_min + m_max); }
    inline bool contains(T v) const;

  private:
    T m_min, m_max;
  };

  class BBox3 : public BBoxT<glm::vec3>
  {
  public:
    using BBoxT<glm::vec3>::extend;
    inline void extend(glm::vec4);
  };

  typedef BBoxT<glm::vec2> BBox2;

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  template <typename T>
  BBoxT<T>::BBoxT()
    : m_min(std::numeric_limits<typename T::value_type>::infinity()),
      m_max(-std::numeric_limits<typename T::value_type>::infinity())
  {}

  template <typename T>
  void BBoxT<T>::extend(T v)
  {
    for (int i = 0; i < v.length(); ++i) {
      m_min[i] = std::min(m_min[i], v[i]);
      m_max[i] = std::max(m_max[i], v[i]);
    }
  }

  template <typename T>
  void BBoxT<T>::clear()
  {
    m_min = T(std::numeric_limits<typename T::value_type>::infinity());
    m_max = T(-std::numeric_limits<typename T::value_type>::infinity());
  }

  template <typename T>
  bool BBoxT<T>::contains(T v) const
  {
    for (int i = 0; i < v.length(); ++i)
      if (m_min[i] > v[i] || m_max[i] < v[i])
        return false;

    return true;
  }

  void BBox3::extend(glm::vec4 v)
  {
    extend(glm::vec3(v.x / v.w, v.y / v.w, v.z / v.w));
  }

} // namespace Shaderkit

#endif // SHADERKIT_BBOX_HPP
