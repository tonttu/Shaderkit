#ifndef SHADERDB_HPP
#define SHADERDB_HPP

#include <QStringList>
#include <QSet>

class ShaderDB {
public:
  ShaderDB();
  virtual ~ShaderDB();

  void addPath(QString path);
  QStringList localProjects();

  static ShaderDB & instance();

protected:
  QSet<QString> scanDir(QString path, bool subdirs = true);
  QSet<QString> m_paths;

  static ShaderDB * s_instance;
};

#endif // SHADERDB_HPP
