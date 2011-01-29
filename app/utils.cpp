#include "utils.hpp"

#include <QSet>

namespace Utils {
  QString uniqueName(QString name, const QList<QString>& lst, QString def) {
    if (name.isEmpty()) name = def;

    QSet<QString> set;
    foreach (QString str, lst) set << str.toLower();

    QString base = name;
    int i = 1;
    while (set.contains(name.toLower()))
      name = base + "." + QString::number(i++);
    return name;
  }
}
