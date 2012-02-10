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

  QVector3D toVector(QVariant in) {
    QVariantList lst = in.toList();
    if (lst.size() == 3) {
      return QVector3D(lst[0].toFloat(), lst[1].toFloat(), lst[2].toFloat());
    }
    return QVector3D();
  }

  Eigen::Vector3f toVector3(QVariant in) {
    QVariantList lst = in.toList();
    if (lst.size() == 3) {
      return Eigen::Vector3f(lst[0].toFloat(), lst[1].toFloat(), lst[2].toFloat());
    }
    return Eigen::Vector3f(0, 0, 0);
  }

  QColor toColor(QVariant in) {
    QVariantList lst = in.toList();
    if (lst.size() == 4) {
      return QColor::fromRgbF(lst[0].toFloat(), lst[1].toFloat(), lst[2].toFloat(), lst[3].toFloat());
    }
    return QColor();
  }

  QVariantList toList(QVector3D in) {
    QVariantList ret;
    ret << in.x() << in.y() << in.z();
    return ret;
  }

  QVariantList toList(Eigen::Vector3f in) {
    QVariantList ret;
    ret << in[0] << in[1] << in[2];
    return ret;
  }

  QVariantList toList(QColor in) {
    QVariantList ret;
    ret << in.redF() << in.greenF() << in.blueF() << in.alphaF();
    return ret;
  }

  void getColor(const QColor& color, float *tmp) {
    // color.getRgbF(tmp+0, tmp+1, tmp+2, tmp+3);
    tmp[0] = color.redF();
    tmp[1] = color.greenF();
    tmp[2] = color.blueF();
    tmp[3] = color.alphaF();
  }
}
