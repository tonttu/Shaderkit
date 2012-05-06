#ifndef SHADERKIT_JSON_EXPORTER_HPP
#define SHADERKIT_JSON_EXPORTER_HPP

#include <QByteArray>
#include <QVariantMap>

namespace Shaderkit
{
  class JSONExporter
  {
  public:
    JSONExporter();
    ~JSONExporter();

    JSONExporter(const JSONExporter&) = delete;
    JSONExporter(JSONExporter&&) = delete;
    JSONExporter& operator=(const JSONExporter&) = delete;
    JSONExporter& operator=(JSONExporter&&) = delete;

    QByteArray serialize(const QVariantMap& map);

  private:
    class Private;
    Private* m_p;
  };

} // namespace Shaderkit

#endif // SHADERKIT_JSON_EXPORTER_HPP
