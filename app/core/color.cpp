#include "color.hpp"

#include <QVariantList>
#include <QColor>

namespace
{
  template <typename T>
  T clamp(T input, T lo, T hi)
  {
    return std::min(std::max(input, lo), hi);
  }
}

namespace Shaderkit
{

  Color::Color(float r, float g, float b, float a)
    : glm::vec4(r, g, b, a)
  {
  }

  Color::Color(const QColor& c)
    : glm::vec4(c.redF(), c.greenF(), c.blueF(), c.alphaF())
  {
  }

  Color Color::fromQVariant(const QVariant& in)
  {
    QVariantList lst = in.toList();
    if (lst.size() == 3) {
      return Color(lst[0].toFloat(), lst[1].toFloat(), lst[2].toFloat());
    } else if (lst.size() == 4) {
      return Color(lst[0].toFloat(), lst[1].toFloat(), lst[2].toFloat(), lst[3].toFloat());
    }
    return Color();
  }

  Color Color::fromBytes(int r, int g, int b, int a)
  {
    return Color(r/255.0f, g/255.0f, b/255.0f, a/255.0f);
  }

  Color Color::fromName(const QString& name)
  {
    if (name.length() == 9 && name.startsWith('#')) {
      QColor c(name.left(7));
      c.setAlpha(name.right(2).toInt(0, 16));
      return Color(c.redF(), c.greenF(), c.blueF(), c.alphaF());
    } else {
      QColor c(name);
      if (c.isValid())
        return Color(c.redF(), c.greenF(), c.blueF(), c.alphaF());
      auto lst = name.trimmed().split(QRegExp("\\s*,\\s*"));
      Color color(0, 0, 0, 1);
      if (lst.size() == 3 || lst.size() == 4) {
        for (int i = 0; i < lst.size(); ++i) {
          color[i] = lst[i].toFloat();
        }
      }
      return color;
    }
  }

  QVariant Color::toQVariant() const
  {
    return QVariantList() << (*this)[0] << (*this)[1] << (*this)[2] << (*this)[3];
  }

  QRgb Color::clampToQRgb() const
  {
    return qRgba(rb(), gb(), bb(), ab());
  }

  QString Color::name() const
  {
    bool byte_color = true;
    for (int i = 0; i < 4; ++i)
      if ((*this)[i] < 0.0f || (*this)[i] > 1.0f) byte_color = false;

    if (byte_color) {
      uint32_t c = (uint32_t(rb()) << 24) | (gb() << 16) | (bb() << 8) | ab();
      return QString("#%1").arg(c, 8, 16, QLatin1Char('0')).toUpper();
    }
    return QString("(%1,%2,%3,%4)").arg((*this)[0]).arg((*this)[1]).
           arg((*this)[2]).arg((*this)[3]);
  }

  uint8_t Color::rb() const
  {
    long tmp = std::lround((*this)[0]*255);
    return clamp(tmp, 0l, 255l);
  }

  uint8_t Color::gb() const
  {
    long tmp = std::lround((*this)[1]*255);
    return clamp(tmp, 0l, 255l);
  }

  uint8_t Color::bb() const
  {
    long tmp = std::lround((*this)[2]*255);
    return clamp(tmp, 0l, 255l);
  }

  uint8_t Color::ab() const
  {
    long tmp = std::lround((*this)[3]*255);
    return clamp(tmp, 0l, 255l);
  }

  const float* Color::data() const
  {
    return &x;
  }

} // namespace Shaderkit
