/**
 * Copyright 2010-2012 Riku Palomäki.
 * This file is part of Shaderkit, http://www.shaderkit.org/.
 *
 * Shaderkit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3 as
 * published by the Free Software Foundation.
 *
 * Shaderkit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Shaderkit.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OBJ_IMPORTER_HPP
#define OBJ_IMPORTER_HPP

#include "forward.hpp"

#include "assimp/Importer.hpp"

#include <QString>
#include <QMap>
#include <QSet>
#include <QVector>

struct aiNode;

namespace Shaderkit {

class ObjImporter {
public:
  typedef QList<QPair<QString, QString>> InfoList;
  typedef QMap<QString, bool> Options;

  /// Name => Description
  struct SceneInfo {
    QMap<QString, QString> objects;
    QMap<QString, QString> animations;
    QMap<QString, QString> cameras;
    QMap<QString, QString> lights;
    QMap<QString, QString> materials;
    QMap<QString, QString> textures;
  };

  /// Set of all entities that are requested to be loaded.
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

    /// The scene graph of the whole imported scene
    NodePtr node;
  };

  ObjImporter();
  ~ObjImporter();

  /// The first step when importing a file is read it in with options
  /// In this step we open the file and process it through assimp
  bool readFile(QString file, Options options);

  /// Second step, this is optional, since if it's not run manually, it will be
  /// run automatically by load(). This will give list of all entities in the
  /// file along with description texts. It will also generate all the unique names
  /// for different entities.
  SceneInfo analyze();

  /// Third step. Actually create all entities that are mentioned in the filter
  /// or are strict dependencies for some mentioned objects.
  Scene load(Filter filter);

  /// Returns the list of all extension supported
  /// format: "*.3ds *.obj *.dae".
  QString extensionList() const;

  bool ping(const QString& file);

private:
  MaterialPtr loadMaterial(int idx);
  MeshPtr loadMesh(ObjectPtr obj, int idx);
  void loadNode(ObjectPtr obj, NodePtr dest, const aiNode& src);
  ObjectPtr loadObject(const QString& name);
  AnimationPtr loadAnim(const QString& name);
  CameraPtr loadCamera(const QString& name);
  LightPtr loadLight(const QString& name);

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
  QMap<QString, QList<NodePtr>> m_nodeIndex;

  /// Cache generated meshes, location in the vector corresponds to the index in m_names
  QVector<MeshPtr> m_meshIndex;
  /// Cache generated materials
  QVector<MaterialPtr> m_materialIndex;
  /// This is valid only during load() stage
  Filter m_filter;

  /// Maps names to indices, for example "Camera 3" could map to s.m_Cameras[8]
  /// Some objects might have duplicate or empty names (can they really?) in assimp,
  /// so we have a deterministic system to generate same unique names every time
  struct Names {
    QMap<QString, int> objects;
    QMap<QString, int> animations;
    QMap<QString, int> cameras;
    QMap<QString, int> lights;
    QMap<QString, int> materials;
    /// textures are not in a single array, so the index is a more complicated key
    QMap<QString, int> textures;
    /// Mesh names are generated only at the load stage, since it requires processing
    /// the whole object tree to actually generate these. Also it makes no sense to
    /// includes meshes to filter, since these are always dependent of objects
    QList<QString> meshes;
  } m_names;

  QString m_file;
  bool m_analyzed;
};

} // namespace Shaderkit

#endif // OBJ_IMPORTER_HPP
