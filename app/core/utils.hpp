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

#ifndef UTILS_HPP
#define UTILS_HPP

#include <QString>
#include <QList>
#include <QVariantList>

#include "Eigen/Geometry"

namespace Utils
{
  QString uniqueName(QString name, const QList<QString>& lst, QString def = "Untitled");

  /// Convert array of three doubles to vector
  Eigen::Vector3f toVector3(QVariant in);
  Eigen::Vector4f toVector34(const QVariant& in);
  QVariantList toList(const Eigen::Vector3f& in);
  QVariantList toList(const Eigen::Vector4f& in);

  /// helper method to convert QColor to 4 element GLfloat array
  void getColor(const QColor& color, float* tmp);
}

#endif // UTILS_HPP
