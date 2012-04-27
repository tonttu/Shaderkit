#ifndef COLOR_HPP
#define COLOR_HPP

#include <Eigen/Geometry>

#include <cstdint>

#include <QRgb>

namespace Shaderkit
{

  class Color : public Eigen::Vector4f
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

  public:
    static Color fromQVariant(const QVariant& in);
    static Color fromBytes(int r = 0, int g = 0, int b = 0, int a = 255);
    static Color fromName(const QString& name);
  };

} // namespace Shaderkit

#endif // COLOR_HPP
