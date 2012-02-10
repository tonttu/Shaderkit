#include "core/mesh_manager.hpp"
#include "gl/buffer_object.hpp"

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
