#ifndef COLOR_HPP
#define COLOR_HPP

#include "glm/vec4.hpp"

#include <cstdint>

#include <QRgb>
#include <QColor>

namespace Shaderkit
{

  class Color : public glm::vec4
  {
  public:
    Color(float r = 0, float g = 0, float b = 0, float a = 1.f);
    Color(const QColor& color);

    QVariant toQVariant() const;
    QRgb clampToQRgb() const;

    QString name() const;

    uint8_t rb() const;
    uint8_t gb() const;
    uint8_t bb() const;
    uint8_t ab() const;

    const float* data() const;

  public:
    static Color fromQVariant(const QVariant& in);
    static Color fromBytes(int r = 0, int g = 0, int b = 0, int a = 255);
    static Color fromName(const QString& name);
  };

} // namespace Shaderkit

#endif // COLOR_HPP
