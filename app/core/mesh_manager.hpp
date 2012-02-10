#ifndef MESH_MANAGER_HPP
#define MESH_MANAGER_HPP

#include "forward.hpp"

#include <QByteArray>
#include <QMap>

class MeshManager {
public:
  BufferObject2 & get(const QByteArray& key);

  static MeshManager& instance();

  static BufferObject2 & fetch(const QByteArray& n) {
    return instance().get(n);
  }

  template <typename T1>
  static BufferObject2 & fetch(const QByteArray& n, const T1& t1) {
    QByteArray ba = n;
    QDataStream stream(&ba, QIODevice::WriteOnly);
    stream << t1;
    return instance().get(ba);
  }
  template <typename T1, typename T2>
  static BufferObject2 & fetch(const QByteArray& n, const T1& t1, const T2& t2) {
    QByteArray ba = n;
    QDataStream stream(&ba, QIODevice::WriteOnly);
    stream << t1 << t2;
    return instance().get(ba);
  }
  template <typename T1, typename T2, typename T3>
  static BufferObject2 & fetch(const QByteArray& n, const T1& t1, const T2& t2, const T3& t3) {
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

#endif // MESH_MANAGER_HPP
