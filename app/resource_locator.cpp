#include "resource_locator.hpp"
#include "log.hpp"

#include <cassert>

namespace {
  ResourceLocator* s_instance = 0;
}

ResourceLocator::ResourceLocator() {
  if (!s_instance) s_instance = this;
}

ResourceLocator::~ResourceLocator() {
  if (s_instance == this) s_instance = 0;
}

void ResourceLocator::setPath(const QString& prefix, const QString& opath) {
  if (!s_instance) return;
  QString path = opath;
  while (path.endsWith('/'))
    path.truncate(path.length()-1);
  assert(!path.startsWith('$'));
  s_instance->m_paths[prefix] = path;
}

QAbstractFileEngine* ResourceLocator::create(const QString &fileName) const {
  QRegExp re("^\\$([^/]+)(/.*)$");
  if (re.exactMatch(fileName)) {
    QString path = m_paths.value(re.cap(1));
    if (!path.isEmpty()) {
      return QAbstractFileEngine::create(path + re.cap(2));
    }
  }
  return 0;
}

QString ResourceLocator::ui(const QString& res) {
  int idx = res.lastIndexOf('/');
  if (idx == -1) return "";
  return res.mid(idx+1);
}

QString ResourceLocator::rename(const QString& src, const QString& new_base) {
  QFileInfo fi(src), fi2(new_base);

  QString file = fi.path() + "/" + fi2.fileName();
  int i = 1;
  while (QFile::exists(file)) {
    file = fi.path() + "/" + fi2.baseName() + QString::number(i);
    if (!fi2.completeSuffix().isEmpty())
      file += "." + fi2.completeSuffix();
  }

  return file;
}
