#include "forward.hpp"
#include "json_exporter.hpp"

#include <QColor>

namespace Shaderkit
{
  class JSONExporter::Private
  {
  public:
    Private();

    const QByteArray& output() const { return m_output; }
    void serialize(const QVariantMap& map, const QByteArray& indent);
    void serialize(const QString& str);
    void serialize(const QStringList& lst);
    void serialize(const QList<double>& lst);
    void serialize(const QVariantList& lst, const QByteArray& indent);
    void serialize(const QVariant& v, const QByteArray& indent);

    QString convert(const QString& str) const;

  private:
    QByteArray m_indent;
    QByteArray m_output;
  };

  JSONExporter::Private::Private()
    : m_indent("  ")
  {}

  void JSONExporter::Private::serialize(const QVariantMap& map, const QByteArray& indent)
  {
    m_output += "{\n";
    const QByteArray ind = indent + m_indent;

    QList<QString> keys;
    int len = 0;
    foreach (const QString& key, map.keys()) {
      const QString& key_serialized = convert(key);
      len = std::max(len, key_serialized.size());
      keys << key_serialized;
    }

    int i = 0;
    for (auto it = map.begin(); it != map.end(); ++it) {
      m_output += ind;
      const QString& key = keys[i++];
      m_output += key.toUtf8();
      m_output += QByteArray(len-key.size(), ' ');
      m_output += " : ";

      const QVariant& v = it.value();
      serialize(v, ind);

      auto next = it;
      ++next;
      m_output += next == map.end() ? "\n" : ",\n";
    }
    m_output += indent + "}";
    if (indent.isEmpty())
      m_output += "\n";
  }

  void JSONExporter::Private::serialize(const QString& str)
  {
    m_output += convert(str).toUtf8();
  }

  void JSONExporter::Private::serialize(const QStringList& lst)
  {
    m_output += "[";
    for (int i = 0; i < lst.size(); ++i) {
      serialize(lst[i]);
      if (i < lst.size()-1)
        m_output += ", ";
    }
    m_output += "]";
  }

  void JSONExporter::Private::serialize(const QList<double>& lst)
  {
    m_output += "[";
    for (int i = 0; i < lst.size(); ++i) {
      m_output += QString::number(lst[i]).toUtf8();
      if (i < lst.size()-1)
        m_output += ", ";
    }
    m_output += "]";
  }

  void JSONExporter::Private::serialize(const QVariantList& lst, const QByteArray& indent)
  {
    if (lst.size() == 0) {
      m_output += "[]";
      return;
    }

    QList<double> dlst;
    QStringList slst;
    for (int i = 0; i < lst.size(); ++i) {
      QVariant::Type type = lst[i].type();

      bool dlst_ok = dlst.size() == i;
      bool slst_ok = slst.size() == i;
      if (!dlst_ok && !slst_ok) break;

      if (dlst_ok) {
        if (type == QVariant::Double || type == QVariant::Int ||
            type == QVariant::LongLong	|| type == QVariant::UInt	||
            type == QVariant::ULongLong)
          dlst << lst[i].toDouble();
        else if (type == QMetaType::Float)
          dlst << lst[i].value<float>();
      }
      if (slst_ok) {
        if (type == QVariant::String)
          slst << lst[i].toString();
      }
    }

    if (dlst.size() == lst.size()) {
      serialize(dlst);
      return;
    }

    if (slst.size() == lst.size()) {
      serialize(slst);
      return;
    }

    m_output += "[\n";
    const QByteArray ind = indent + m_indent;
    for (int i = 0; i < lst.size(); ++i) {
      m_output += ind;
      const QVariant& v = lst[i];
      serialize(v, ind);

      m_output += i + 1 == lst.size() ? "\n" : ",\n";
    }
    m_output += indent + "]";
  }

  void JSONExporter::Private::serialize(const QVariant& v, const QByteArray& indent)
  {
    auto type = v.type();
    if (type == QVariant::Bool) {
      const bool b = v.toBool();
      m_output += b ? "true" : "false";
    } else if (type == QVariant::Color) {
      QColor color = v.value<QColor>();
      QList<double> lst;
      lst << color.redF() << color.greenF() << color.blueF() << color.alphaF();
      serialize(lst);
    } else if (type == QVariant::Double || type == QVariant::Int ||
               type == QVariant::LongLong || type == QVariant::UInt ||
               type == QVariant::ULongLong) {
      m_output += v.toString();
    } else if (type == QVariant::Hash) {
      QVariantMap tmp;
      const QVariantHash h = v.toHash();
      for (auto it2 = h.begin(); it2 != h.end(); ++it2)
        tmp[it2.key()] = it2.value();
      serialize(tmp, indent);
    } else if (type == QVariant::List) {
      serialize(v.toList(), indent);
    } else if (type == QVariant::Map) {
      serialize(v.toMap(), indent);
    } else if (type == QVariant::StringList) {
      serialize(v.toStringList());
    } else if (type == QMetaType::Float) {
      m_output += QString::number(v.value<float>()).toUtf8();
    } else if (v.canConvert(QVariant::String)) {
      serialize(v.toString());
    } else if (type == QVariant::Invalid) {
      m_output += "null";
    } else {
      Log::error("Can't serialize value of type: %d", type);
      m_output += "null";
    }
  }

  QString JSONExporter::Private::convert(const QString &str) const
  {
    QString output;
    output += "\"";
    for (int i = 0; i < str.size(); ++i) {
      const QChar chr = str[i];
      if (chr == '"') output += "\"";
      else if (chr == '\\') output += "\\\\";
      else if (chr == '\b') output += "\\b";
      else if (chr == '\f') output += "\\f";
      else if (chr == '\n') output += "\\n";
      else if (chr == '\r') output += "\\r";
      else if (chr == '\t') output += "\\t";
      else output += chr;
    }
    output += "\"";
    return output;
  }

  JSONExporter::JSONExporter()
    : m_p(new Private)
  {
  }

  JSONExporter::~JSONExporter()
  {
    delete m_p;
  }

  QByteArray JSONExporter::serialize(const QVariantMap& map)
  {
    m_p->serialize(map, "");
    return m_p->output();
  }

} // namespace Shaderkit
