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

private:
  QMap<QString, QString> m_paths;
};

#endif // RESOURCE_LOCATOR_HPP
