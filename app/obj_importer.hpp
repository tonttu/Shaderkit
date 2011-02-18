#ifndef OBJ_IMPORTER_HPP
#define OBJ_IMPORTER_HPP

#include "forward.hpp"

#include "assimp.hpp"

#include <QString>
#include <QMap>
#include <QSet>
#include <QVector>

class aiNode;
class aiAnimation;
class aiCamera;
class aiLight;
class aiMaterial;

class ObjImporter {
public:
  typedef QList<QPair<QString, QString> > InfoList;
  typedef QMap<QString, bool> Options;

  struct SceneInfo {
    QMap<QString, QString> objects;
    QMap<QString, QString> animations;
    QMap<QString, QString> cameras;
    QMap<QString, QString> lights;
    QMap<QString, QString> materials;
    QMap<QString, QString> textures;
  };
  struct Filter {
    QSet<QString> objects;
    QSet<QString> models;
    QSet<QString> animations;
    QSet<QString> cameras;
    QSet<QString> lights;
    QSet<QString> materials;
    QSet<QString> textures;
  };
  struct Scene {
    QMap<QString, ObjectPtr> objects;
    QMap<QString, ModelPtr> models;
    QMap<QString, AnimationPtr> animations;
    QMap<QString, CameraPtr> cameras;
    QMap<QString, LightPtr> lights;
    QMap<QString, MaterialPtr> materials;
    QMap<QString, TexturePtr> textures;
    NodePtr node;
  };

  ObjImporter();
  ~ObjImporter();

  bool readFile(QString file, Options options);
  /// @param s If given, marks all names used in the scene as already reserved
  SceneInfo analyze(ScenePtr s = ScenePtr());
  Scene load(Filter filter);

  /// Returns the list of all extension supported
  /// format: "*.3ds *.obj *.dae".
  QString extensionList() const;

private:
  MaterialPtr loadMaterial(int idx);
  MeshPtr loadMesh(ObjectPtr obj, int idx);
  void load(ObjectPtr obj, NodePtr dest, const aiNode& src);
  ObjectPtr load(const aiNode& node);
  AnimationPtr load(const aiAnimation& anim);
  CameraPtr load(const aiCamera& cam);
  LightPtr load(const aiLight& light);

  QString getName(QMap<QString, int>& map, int index) const;

  /// Often in models there is a texture defined to be something like
  /// "C:\Users\Me\SharedTextureData\funny monkeys\douroucoulis.png"
  /// even though the actual texture might really be named
  /// "./Funny Monkeys/Douroucoulis.PNG". Maybe the texture file was
  /// just copied later next to the model or maybe there was just a
  /// broken exporter. This method does it's best in finding the
  /// correct file. It it fails, it just returns the original
  /// parameter.
  QString findFile(QString file);

  Assimp::Importer* m_importer;
  const aiScene* m_aiscene;
  QMap<QString, QList<NodePtr> > m_nodeIndex;
  QVector<MeshPtr> m_meshIndex;
  QVector<MaterialPtr> m_materialIndex;
  Filter m_filter;

  struct Names {
    QMap<QString, int> objects;
    QMap<QString, int> animations;
    QMap<QString, int> cameras;
    QMap<QString, int> lights;
    QMap<QString, int> materials;
    QMap<QString, int> textures;
    QList<QString> meshes;
  } m_names;

  QString m_file;
  bool m_analyzed;
};

#endif // OBJ_IMPORTER_HPP
