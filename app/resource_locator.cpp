#include "resource_locator.hpp"

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
