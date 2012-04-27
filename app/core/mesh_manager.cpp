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

#include "core/mesh_manager.hpp"
#include "gl/buffer_object.hpp"

namespace Shaderkit {

MeshManager::MeshManager()
{
}

BufferObject2 & MeshManager::get(const QByteArray& key) {
  BufferObject2 *& bo = m_map[key];
  if (!bo) bo = new BufferObject2(GL_ARRAY_BUFFER);
  /// @todo bo.updateUsageTime();
  return *bo;
}

MeshManager& MeshManager::instance() {
  static MeshManager m;
  return m;
}

} // namespace Shaderkit
