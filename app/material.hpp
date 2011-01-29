#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "forward.hpp"

#include <QString>

class Material {
public:
  Material(QString name);

  QString name() const { return m_name; }

private:
  QString m_name;
};

#endif // MATERIAL_HPP
