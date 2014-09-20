#ifndef ATTRIBUTE_HPP
#define ATTRIBUTE_HPP

#include "gl/scene_object.hpp"

namespace Shaderkit
{
  template <typename T, typename = void>
  struct AttributeCmp
  {
    static inline bool equal(const T& a, const T& b)
    {
      return a == b;
    }
  };

  template <typename T>
  struct AttributeCmp<T, typename std::enable_if<sizeof(typename T::MatrixType)>::type>
  {
    static inline bool equal(const T& a, const T& b)
    {
      return a.matrix() == b.matrix();
    }
  };

  template <typename T>
  class Attribute
  {
  public:
    Attribute(SceneObject& parent, const T& default_value = T())
      : m_scene_object(parent)
      , m_value(default_value)
    {
    }

    Attribute<T>(const Attribute<T>&) = delete;

    Attribute<T>& operator=(const Attribute<T>& value)
    {
      return (*this = value.value());
    }

    Attribute<T>& operator=(const T& value)
    {
      if (AttributeCmp<T>::equal(m_value, value)) return *this;
      m_value = value;
      m_scene_object.attributeChanged();
      return *this;
    }

    const T& value() const { return m_value; }
    const T& operator*() const { return m_value; }
    operator const T&() const { return m_value; }

    const T* operator->() const { return &m_value; }

  private:
    SceneObject& m_scene_object;
    T m_value;
  };

#define DEFINE_COMPOUND_ATTR_OP(op)                                 \
  template <typename T, typename Y>                                 \
  Attribute<T>& operator op##= (Attribute<T>& attr, const Y& value) \
  {                                                                 \
    attr = attr.value() op value;                                   \
    return attr;                                                    \
  }

  DEFINE_COMPOUND_ATTR_OP(+)
  DEFINE_COMPOUND_ATTR_OP(-)
  DEFINE_COMPOUND_ATTR_OP(*)
  DEFINE_COMPOUND_ATTR_OP(/)
  DEFINE_COMPOUND_ATTR_OP(%)
  DEFINE_COMPOUND_ATTR_OP(&)
  DEFINE_COMPOUND_ATTR_OP(|)
  DEFINE_COMPOUND_ATTR_OP(^)
  DEFINE_COMPOUND_ATTR_OP(<<)
  DEFINE_COMPOUND_ATTR_OP(>>)

#undef DEFINE_OP
}

#endif // ATTRIBUTE_HPP
