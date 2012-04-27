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

#include "core/animation.hpp"

namespace Shaderkit {

Animation::Animation(QString name) : SceneObject(name) {
}

QVariantMap Animation::toMap() const {
  QVariantMap map = SceneObject::toMap();
  return map;
}

void Animation::load(QVariantMap map) {
  SceneObject::load(map);
}

} // namespace Shaderkit
