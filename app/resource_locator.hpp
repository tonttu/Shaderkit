#ifndef RESOURCE_LOCATOR_HPP
#define RESOURCE_LOCATOR_HPP

#include <QAbstractFileEngineHandler>
#include <QMap>

class ResourceLocator : public QAbstractFileEngineHandler
{
public:
  ResourceLocator();
  virtual ~ResourceLocator();

  QAbstractFileEngine* create(const QString& fileName) const;

  static void setPath(const QString& prefix, const QString& path);
  static QString ui(const QString& res);
  static QString rename(const QString& src, const QString& new_base);
  static QString unique(const QString& src);

private:
  QMap<QString, QString> m_paths;
};

#endif // RESOURCE_LOCATOR_HPP
