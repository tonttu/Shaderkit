/**
 * Copyright 2010-2012 Riku Palomäki.
 * This file is part of Shaderkit, http://www.shaderkit.org/.
 *
 * Shaderkit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3 as
 * published by the Free Software Foundation.
 *
 * Shaderkit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Shaderkit.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "core/utils.hpp"

#include <QSet>
#include <QColor>

namespace Utils {
  QString uniqueName(QString name, const QList<QString>& lst, QString def) {
    if (name.isEmpty()) name = def;

    QSet<QString> set;
    foreach (QString str, lst) set << str.toLower();

    QString base = name;
    int i = 1;
    while (set.contains(name.toLower()))
      name = base + "." + QString::number(i++);
    return name;
  }

  Eigen::Vector3f toVector3(QVariant in) {
    QVariantList lst = in.toList();
    if (lst.size() == 3) {
      return Eigen::Vector3f(lst[0].toFloat(), lst[1].toFloat(), lst[2].toFloat());
    }
    return Eigen::Vector3f(0, 0, 0);
  }

  QVariantList toList(const Eigen::Vector3f& in) {
    QVariantList ret;
    ret << in[0] << in[1] << in[2];
    return ret;
  }

  QVariantList toList(const Eigen::Vector4f& in) {
    QVariantList ret;
    ret << in[0] << in[1] << in[2] << in[3];
    return ret;
  }
}
