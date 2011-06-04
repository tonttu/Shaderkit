#include "resource_locator.hpp"
#include "log.hpp"

#include <cassert>

#include <QSet>

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

QString ResourceLocator::rename(const QString& src, const QString& new_base, const QSet<QString>& lst) {
  QFileInfo fi(src), fi2(new_base);
  return unique(fi.path() + "/" + fi2.fileName(), lst);
}

QString ResourceLocator::unique(const QString& src, const QSet<QString>& lst) {
  QFileInfo fi(src);
  QString start = fi.path() + "/" + fi.baseName();
  QString suffix = fi.completeSuffix().isEmpty() ? "" : "." + fi.completeSuffix();

  QString file = src;
  QFileInfo fi2(file);
  int i = -1;
  while (QFile::exists(file) || lst.contains(fi2.absoluteFilePath())) {
    if (i == -1) {
      QRegExp re("(\\d+)$");
      int p = re.indexIn(start);
      if (p >= 0) {
        start = start.left(p);
        i = re.cap(1).toInt() + 1;
      } else {
        i = 1;
      }
    }

    file = start + QString::number(i++) + suffix;
    fi2.setFile(file);
  }

  return file;
}
