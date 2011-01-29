#ifndef UTILS_HPP
#define UTILS_HPP

#include <QString>
#include <QList>

namespace Utils
{
  QString uniqueName(QString name, const QList<QString>& lst, QString def = "Untitled");
}

#endif // UTILS_HPP
