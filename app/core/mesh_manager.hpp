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

#ifndef MESH_MANAGER_HPP
#define MESH_MANAGER_HPP

#include "forward.hpp"

#include <QByteArray>
#include <QMap>

namespace Shaderkit
{

  class MeshManager
  {
  public:
    BufferObject2& get(const QByteArray& key);

    static MeshManager& instance();

    static BufferObject2& fetch(const QByteArray& n) {
      return instance().get(n);
    }

    template <typename T1>
    static BufferObject2& fetch(const QByteArray& n, const T1& t1) {
      QByteArray ba = n;
      QDataStream stream(&ba, QIODevice::WriteOnly);
      stream << t1;
      return instance().get(ba);
    }
    template <typename T1, typename T2>
    static BufferObject2& fetch(const QByteArray& n, const T1& t1, const T2& t2) {
      QByteArray ba = n;
      QDataStream stream(&ba, QIODevice::WriteOnly);
      stream << t1 << t2;
      return instance().get(ba);
    }
    template <typename T1, typename T2, typename T3>
    static BufferObject2& fetch(const QByteArray& n, const T1& t1, const T2& t2, const T3& t3) {
      QByteArray ba = n;
      QDataStream stream(&ba, QIODevice::WriteOnly);
      stream << t1 << t2 << t3;
      return instance().get(ba);
    }

  private:
    MeshManager();

    /// @todo ref<BufferObject2>, garbage collection
    QMap<QByteArray, BufferObject2*> m_map;
  };

} // namespace Shaderkit

#endif // MESH_MANAGER_HPP
