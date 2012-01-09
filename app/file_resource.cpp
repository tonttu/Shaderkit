#include "file_resource.hpp"

#include <QFileInfo>

FileResource::FileResource() {
}

FileResource::~FileResource() {
}

void FileResource::setFilename(const QString& filename) {
  m_rawFilename = filename;
  /// @todo solve this right
  QFileInfo fi(filename);
  m_filenameAbsolute = fi.canonicalFilePath();
  if (m_filenameAbsolute.isEmpty())
    m_filenameAbsolute = fi.absoluteFilePath();
}

const QString& FileResource::filename() const {
  return m_filenameAbsolute;
}

const QString& FileResource::rawFilename() const {
  return m_rawFilename;
}
