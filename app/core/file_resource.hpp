#ifndef FILE_RESOURCE_HPP
#define FILE_RESOURCE_HPP

#include <QString>

class FileResource {
public:
  FileResource();
  virtual ~FileResource();

  virtual void setFilename(const QString& filename);
  /// @returns Absolute filename
  const QString& filename() const;
  const QString& rawFilename() const;

private:
  QString m_filenameAbsolute;
  QString m_rawFilename;
};

#endif // FILE_RESOURCE_HPP
