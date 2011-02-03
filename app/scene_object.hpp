#ifndef SCENE_OBJECT_HPP
#define SCENE_OBJECT_HPP

#include <QVariantMap>
#include <QString>
#include <QStringList>

class SceneObject {
public:
  SceneObject(const QString& name);
  virtual ~SceneObject();

  QString name() const { return m_name; }
  void setName(QString name);

  virtual QVariantMap save() const;
  virtual void load(QVariantMap map);

  void setRef(QString import, QStringList iname);
  QString ref();
  QStringList refName();

protected:
  QString m_name;
  QString m_ref;
  QStringList m_ref_name;
};

#endif // SCENE_OBJECT_HPP
