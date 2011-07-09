/**
 * Copyright 2010,2011 Riku Palomäki.
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

#include "obj_importer.hpp"
#include "object3d.hpp"
#include "model.hpp"
#include "material.hpp"
#include "texture.hpp"
#include "opengl.hpp"
#include "utils.hpp"
#include "scene.hpp"

#include "aiScene.h"
#include "aiPostProcess.h"
#include "LogStream.h"
#include "DefaultLogger.h"

#include <cassert>

namespace {
  QString str(const aiString& aistr) {
    return QString::fromUtf8(aistr.data, aistr.length);
  }

  /// Reads color from material src to target.
  /// Example: getColor(target, material, AI_MATKEY_COLOR_DIFFUSE);
  void getColor(QVector3D& target, aiMaterial& src, const char * pKey,
                unsigned int type, unsigned int idx) {
    aiColor3D color;
    if (src.Get(pKey, type, idx, color) == AI_SUCCESS)
      target = QVector3D(color.r, color.g, color.b);
  }
  void getBool(bool& target, aiMaterial& src, const char * pKey,
               unsigned int type, unsigned int idx) {
    int value;
    if (src.Get(pKey, type, idx, value) == AI_SUCCESS)
      target = value != 0;
  }
  QMap<aiTextureType, QString> textureRoles() {
    QMap<aiTextureType, QString> roles;
    roles[aiTextureType_DIFFUSE] = "diffuse";
    roles[aiTextureType_SPECULAR] = "specular";
    roles[aiTextureType_AMBIENT] = "ambient";
    roles[aiTextureType_EMISSIVE] = "emissive";
    roles[aiTextureType_HEIGHT] = "heightmap";
    roles[aiTextureType_NORMALS] = "normalmap";
    roles[aiTextureType_SHININESS] = "shininess";
    roles[aiTextureType_OPACITY] = "opacity";
    roles[aiTextureType_DISPLACEMENT] = "displacement";
    roles[aiTextureType_LIGHTMAP] = "lightmap";
    roles[aiTextureType_REFLECTION] = "reflection";
    roles[aiTextureType_UNKNOWN] = "";
    return roles;
  }
}

class LogStream : public Assimp::LogStream {
public:
  LogStream(Log::Level level) : m_level(level) {}
  void write(const char* message) {
    Log::log(m_level, "assimp: %s", message);
  }
private:
  Log::Level m_level;
};


ObjImporter::ObjImporter() : m_analyzed(false) {
  Assimp::Logger* l = Assimp::DefaultLogger::create(0, Assimp::Logger::VERBOSE, 0);
  assert(l);
  l->attachStream(new LogStream(Log::LOG_ERROR), Assimp::Logger::Err);
  l->attachStream(new LogStream(Log::LOG_WARNING), Assimp::Logger::Warn);
  l->attachStream(new LogStream(Log::LOG_INFO), Assimp::Logger::Info);
  l->attachStream(new LogStream(Log::LOG_DEBUG), Assimp::Logger::Debugging);
  m_importer = new Assimp::Importer;
}

ObjImporter::~ObjImporter() {
  delete m_importer;
  Assimp::DefaultLogger::kill();
}

bool ObjImporter::readFile(QString file, QMap<QString, bool> options) {
  unsigned int flags =
      aiProcess_CalcTangentSpace |
      aiProcess_JoinIdenticalVertices |
      aiProcess_Triangulate | /// @todo support quads and add a flag
      aiProcess_GenSmoothNormals |
      aiProcess_SplitLargeMeshes |
      aiProcess_SortByPType |
      aiProcess_FindInvalidData |
      aiProcess_GenUVCoords |
      aiProcess_FindInstances;

  m_analyzed = false;
  m_names = Names();
  m_file = file;

  /// @todo use this information somehow
  flags |= aiProcess_ValidateDataStructure;
  //if (options["validate"]) flags |= aiProcess_ValidateDataStructure;
  if (options["optimize cache locality"]) flags |= aiProcess_ImproveCacheLocality;
  if (options["merge materials"]) flags |= aiProcess_RemoveRedundantMaterials;
  if (options["find degenerates"]) flags |= aiProcess_FindDegenerates;
  if (options["fix normals"]) flags |= aiProcess_FixInfacingNormals;
  if (options["optimize"]) flags |= aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph;

  // skinning on hw: aiProcess_LimitBoneWeights
  // maybe? aiProcess_TransformUVCoords

  const aiScene* scene = m_importer->ReadFile(file.toUtf8().data(), flags);

  if (!scene) {
    /// @todo need to log this to gui as well
    Log::error("Failed to import %s: %s", file.toUtf8().data(), m_importer->GetErrorString());
    return false;
  }

  return true;
}

ObjImporter::SceneInfo ObjImporter::analyze() {
  SceneInfo out;

  if (!m_importer->GetScene()) return out;
  const aiScene& s = *m_importer->GetScene();

  if (s.mRootNode) {
    /// even if we have only one object, use uniquename just to make sure that the name isn't empty
    QString name = Utils::uniqueName(str(s.mRootNode->mName), out.objects.keys(), "Object");
    m_names.objects[name] = 0;
    out.objects[name] = QString("%1 meshes").arg(s.mNumMeshes);
    Log::debug("Object: %s", name.toUtf8().data());
  }

  for (unsigned int i = 0; i < s.mNumAnimations; ++i) {
    aiAnimation& a = *s.mAnimations[i];
    QString name = Utils::uniqueName(str(a.mName), out.animations.keys(), "Animation");
    m_names.animations[name] = i;
    out.animations[name] = QString("%1 channels").arg(a.mNumChannels);
    Log::debug("Animation: %s", name.toUtf8().data());
  }

  for (unsigned int i = 0; i < s.mNumCameras; ++i) {
    aiCamera& c = *s.mCameras[i];
    QString name = Utils::uniqueName(str(c.mName), out.cameras.keys(), "Camera");
    m_names.cameras[name] = i;
    out.cameras[name] = "";
    Log::debug("Camera: %s", name.toUtf8().data());
  }

  for (unsigned int i = 0; i < s.mNumLights; ++i) {
    aiLight& l = *s.mLights[i];
    const char* type;
    switch(l.mType) {
    case aiLightSource_DIRECTIONAL:
      type = "Directional light";
      break;
    case aiLightSource_POINT:
      type = "Point light";
      break;
    case aiLightSource_SPOT:
      type = "Spot light";
      break;
    case aiLightSource_UNDEFINED:
    default:
      type = "Undefined type";
      break;
    }
    QString name = Utils::uniqueName(str(l.mName), out.lights.keys(), "Light");
    m_names.lights[name] = i;
    out.lights[name] = type;
    Log::debug("Light: %s", name.toUtf8().data());
  }

  for (unsigned int i = 0; i < s.mNumMaterials; ++i) {
    aiMaterial& m = *s.mMaterials[i];
    aiString name;
    aiGetMaterialString(&m, AI_MATKEY_NAME, &name);

    int num = i << 8;
    QMap<aiTextureType, QString> roles = textureRoles();
    foreach (aiTextureType type, roles.keys()) {
      unsigned int count = m.GetTextureCount(type);
      for (unsigned int j = 0; j < count; ++j) {
        aiString path;
        aiGetMaterialString(&m, AI_MATKEY_TEXTURE(type, j), &path);

        QFileInfo fi(str(path));
        QString texname = roles[type] + "-" + fi.baseName();

        texname = Utils::uniqueName(texname, out.textures.keys(), "Texture");
        out.textures[texname] = "";
        m_names.textures[texname] = num++;
        Log::debug("Texture: %s", texname.toUtf8().data());
      }
    }

    QString name2 = Utils::uniqueName(str(name), out.materials.keys(), "Material");
    m_names.materials[name2] = i;
    out.materials[name2] = QString("%1 textures").arg(num);
    Log::debug("Material: %s", name2.toUtf8().data());
  }

  m_analyzed = true;

  return out;
}

ObjImporter::Scene ObjImporter::load(Filter filter) {
  m_aiscene = m_importer->GetScene();
  if (!m_aiscene) return Scene();
  if (!m_analyzed) analyze();

  m_meshIndex.resize(m_aiscene->mNumMeshes);
  m_materialIndex.resize(m_aiscene->mNumMaterials);
  m_filter = filter;

  Scene scene;

  foreach (QString name, filter.objects & m_names.objects.keys().toSet()) {
    ObjectPtr o = loadObject(name);
    scene.objects[name] = o;
    scene.node = o->model()->node();
  }

  foreach (QString name, filter.models & m_names.objects.keys().toSet()) {
    ObjectPtr o = scene.objects.value(name);
    if (!o) o = loadObject(name);
    scene.models[name] = o->model();
    if (!scene.node)
      scene.node = o->model()->node();
  }

  foreach (QString name, filter.animations & m_names.animations.keys().toSet()) {
    scene.animations[name] = loadAnim(name);
  }

  foreach (QString name, filter.cameras & m_names.cameras.keys().toSet()) {
    scene.cameras[name] = loadCamera(name);
  }

  foreach (QString name, filter.lights & m_names.lights.keys().toSet()) {
    scene.lights[name] = loadLight(name);
  }

  foreach (QString name, filter.materials & m_names.materials.keys().toSet()) {
    scene.materials[name] = loadMaterial(m_names.materials[name]);
  }

  foreach (QString name, m_names.materials.keys()) {
    MaterialPtr m = loadMaterial(m_names.materials[name]);
    foreach (QString texname, m->textureNames()) {
      if (filter.textures.contains(texname))
        scene.textures[texname] = m->texture(texname);
    }
  }

  m_aiscene = 0;
  m_filter = Filter();
  m_nodeIndex.clear();
  m_meshIndex.clear();
  m_materialIndex.clear();

  return scene;
}

QString ObjImporter::extensionList() const {
  aiString lst;
  m_importer->GetExtensionList(lst);
  return str(lst).replace(';', ' ');
}

MaterialPtr ObjImporter::loadMaterial(int idx) {
  if (m_materialIndex[idx]) return m_materialIndex[idx];
  if (!m_aiscene->mMaterials[idx]) return MaterialPtr();

  aiMaterial& src = *m_aiscene->mMaterials[idx];
  QMap<aiTextureType, QString> roles = textureRoles();

  MaterialPtr m(new Material(m_names.materials.key(idx)));

  getColor(m->colors.diffuse, src, AI_MATKEY_COLOR_DIFFUSE);
  getColor(m->colors.specular, src, AI_MATKEY_COLOR_SPECULAR);
  getColor(m->colors.ambient, src, AI_MATKEY_COLOR_AMBIENT);
  getColor(m->colors.emissive, src, AI_MATKEY_COLOR_EMISSIVE);
  getColor(m->colors.transparent, src, AI_MATKEY_COLOR_TRANSPARENT);

  getBool(m->style.wireframe, src, AI_MATKEY_ENABLE_WIREFRAME);
  getBool(m->style.twosided, src, AI_MATKEY_TWOSIDED);

  aiShadingMode shading_model;
  if (src.Get(AI_MATKEY_SHADING_MODEL, shading_model) == AI_SUCCESS) {
    switch (shading_model) {
    case aiShadingMode_Gouraud:
      m->style.shading_model = "gouraud";
      break;
    case aiShadingMode_Phong:
      m->style.shading_model = "phong";
      break;
    case aiShadingMode_Blinn:
      m->style.shading_model = "blinn";
      break;
    case aiShadingMode_Toon:
      m->style.shading_model = "toon";
      break;
    case aiShadingMode_OrenNayar:
      m->style.shading_model = "oren-nayar";
      break;
    case aiShadingMode_Minnaert:
      m->style.shading_model = "minnaert";
      break;
    case aiShadingMode_CookTorrance:
      m->style.shading_model = "cook-torrance";
      break;
    case aiShadingMode_NoShading:
      m->style.shading_model = "constant";
      break;
    case aiShadingMode_Fresnel:
      m->style.shading_model = "fresnel";
      break;
    case aiShadingMode_Flat:
      m->style.shading_model = "flat";
      break;
    default:
      break;
    }
  }

  aiBlendMode blend_func = aiBlendMode_Default;
  if (src.Get(AI_MATKEY_BLEND_FUNC, blend_func) == AI_SUCCESS) {
    m->style.blend_mode = blend_func == aiBlendMode_Additive ? "additive" : "default";
  }

  src.Get(AI_MATKEY_OPACITY, m->style.opacity);
  src.Get(AI_MATKEY_SHININESS, m->style.shininess);
  src.Get(AI_MATKEY_SHININESS_STRENGTH, m->style.shininess_strength);
  src.Get(AI_MATKEY_REFRACTI, m->style.refracti);

  int num = idx << 8;
  foreach (aiTextureType type, roles.keys()) {
    unsigned int count = src.GetTextureCount(type);
    for (unsigned int i = 0; i < count; ++i) {
      QString name = m_names.textures.key(num++);
      QString role = roles[type];
      TextureFile* t = new TextureFile(name);
      t->setRole(role);

      aiString path;
      aiTextureMapping mapping = aiTextureMapping_UV;
      unsigned int uvindex = 0;
      float blend = 1.0f;
      aiTextureOp op = aiTextureOp_Multiply; /// @todo is this good default value?
      aiTextureMapMode mapmode[3] = { aiTextureMapMode_Wrap, aiTextureMapMode_Wrap,
                                      aiTextureMapMode_Wrap };
      unsigned int flags = 0;

      /// @todo GetTexture is missing flags-argument for some reason
      //src.GetTexture(type, i, &path, &mapping, &uvindex, &blend, &op, mapmode);
      aiGetMaterialTexture(&src, type, i, &path, &mapping, &uvindex, &blend, &op, mapmode, &flags);
      for (int j = 0; j < 2 /** @todo 3 */; ++j) {
        if (mapmode[j] == aiTextureMapMode_Decal) {
          Log::warn("Decal texture mapping mode is not supported");
          mapmode[j] = aiTextureMapMode_Clamp;
        }
        int mode = GL_REPEAT;
        if (mapmode[j] == aiTextureMapMode_Mirror) mode = GL_MIRRORED_REPEAT;
        if (mapmode[j] == aiTextureMapMode_Clamp) mode = GL_CLAMP_TO_EDGE;

        t->setParam(j == 0 ? GL_TEXTURE_WRAP_S : j == 1 ? GL_TEXTURE_WRAP_T : GL_TEXTURE_WRAP_R, mode);
      }

      if (mapping != aiTextureMapping_UV) {
        Log::warn("Only UV texture mapping is supported");
        mapping = aiTextureMapping_UV;
      }

      t->setFile(findFile(str(path)));
      t->setBlend(blend);
      t->setUV(uvindex);
      /// @todo use aiTextureOp op somehow

      m->addTexture(name, TexturePtr(t));
    }
  }

  return m;
}

MeshPtr ObjImporter::loadMesh(ObjectPtr obj, int idx) {
  /// @todo this won't work when we are able to load multiple objects from the same file
  ///       - obj.materials list might not have this mesh
  if (m_meshIndex[idx]) return m_meshIndex[idx];
  if (!m_aiscene->mMeshes[idx]) return MeshPtr();

  aiMesh& src = *m_aiscene->mMeshes[idx];

  // Because we have aiProcess_SortByPType and aiProcess_Triangulate
  // enabled, there are limitations what kind of meshes we have here.
  // So the following assert actually checks for a bug.

  if (src.mPrimitiveTypes == aiPrimitiveType_POINT) {
    Log::warn("Point primitives aren't supported yet");
    return MeshPtr();
  } else if (src.mPrimitiveTypes == aiPrimitiveType_LINE) {
    Log::warn("Line primitives aren't supported yet");
    return MeshPtr();
  }
  /// @todo add support for aiPrimitiveType_POLYGON and add possibility
  ///       to remove aiProcess_Triangulate
  assert(src.mPrimitiveTypes == aiPrimitiveType_TRIANGLE);
  assert(src.mVertices);

  TriMesh* mesh = new TriMesh;

  assert((char*)&src.mVertices[1].x - (char*)&src.mVertices[0].x == sizeof(float)*3);
  mesh->vertices.resize(src.mNumVertices*3);
  std::copy(&src.mVertices->x, &(src.mVertices + src.mNumVertices)->x, &mesh->vertices[0]);

  if (src.mNormals) {
    mesh->normals.resize(src.mNumVertices*3);
    std::copy(&src.mNormals->x, &(src.mNormals + src.mNumVertices)->x, &mesh->normals[0]);
  }

  if (src.mTangents) {
    mesh->tangents.resize(src.mNumVertices*3);
    std::copy(&src.mTangents->x, &(src.mTangents + src.mNumVertices)->x, &mesh->tangents[0]);
  }

  if (src.mBitangents) {
    mesh->bitangents.resize(src.mNumVertices*3);
    std::copy(&src.mBitangents->x, &(src.mBitangents + src.mNumVertices)->x, &mesh->bitangents[0]);
  }

  unsigned int num = src.GetNumColorChannels();
  if (num > 0) {
    mesh->colors.resize(num);
    for (unsigned int i = 0; i < num; ++i) {
      assert((char*)&src.mColors[i][1].r - (char*)&src.mColors[i][0].r == sizeof(float)*4);
      mesh->colors[i].resize(src.mNumVertices*4);
      std::copy(&src.mColors[i]->r, &(src.mColors[i] + src.mNumVertices)->r, &mesh->colors[i][0]);
    }
  }

  num = src.GetNumUVChannels();
  if (num > 0) {
    mesh->uvs.resize(num);
    mesh->uv_sizes.resize(num);
    for (unsigned int i = 0; i < num; ++i) {
      assert((char*)&src.mTextureCoords[i][1].x - (char*)&src.mTextureCoords[i][0].x == sizeof(float)*3);
      mesh->uv_sizes[i] = src.mNumUVComponents[i];
      mesh->uvs[i].resize(src.mNumVertices*mesh->uv_sizes[i]);

      if (mesh->uv_sizes[i] == 3) {
        std::copy(&src.mTextureCoords[i]->x, &(src.mTextureCoords[i] + src.mNumVertices)->x, &mesh->uvs[i][0]);
      } else {
        float* b = &src.mTextureCoords[i]->x;
        float* e = b + src.mNumVertices * 3;
        float* o = &mesh->uvs[i][0];
        if (mesh->uv_sizes[i] == 1) {
          while(b < e) {
            *o++ = *b;
            b += 3;
          }
        } else {
          assert(mesh->uv_sizes[i] == 2);
          while(b < e) {
            *o++ = *b++;
            *o++ = *b;
            b += 2;
          }
        }
      }
    }
  }

  // mPrimitiveTypes assures that every face is the same size
  mesh->indices.resize(src.mNumFaces * 3);
  for (unsigned int i = 0; i < src.mNumFaces; ++i) {
    assert(src.mFaces[i].mNumIndices == 3);
    std::copy(src.mFaces[i].mIndices, src.mFaces[i].mIndices + 3, &mesh->indices[0] + i * 3);
  }

  /// @todo bones
  /// @todo anim meshes

  mesh->name = Utils::uniqueName(str(src.mName), m_names.meshes, "Mesh");
  m_names.meshes << mesh->name;
  QString matname = m_names.materials.key(src.mMaterialIndex);
  Log::debug("Mesh - Material: %s, %s", mesh->name.toUtf8().data(), matname.toUtf8().data());
  if (m_filter.materials.contains(matname)) {
    obj->setMaterialForMesh(mesh->name, loadMaterial(src.mMaterialIndex));
  }

  return MeshPtr(mesh);
}

void ObjImporter::loadNode(ObjectPtr obj, NodePtr dest, const aiNode& src) {
  /// This probably should be unique, since we use m_nodeIndex
  dest->name = str(src.mName);
  m_nodeIndex[dest->name] << dest;
  {
    /// assimp keeps the matrix in row-major format, we use column-major format. transpose.
    /// @todo the transform should be affine? We should maybe create a matrix and
    ///       assign that to transform?
    float* d = dest->transform.data(); const float* s = &src.mTransformation.a1;
    d[0] = s[0];  d[4] = s[1];   d[8] = s[2];  d[12] = s[3];
    d[1] = s[4];  d[5] = s[5];   d[9] = s[6];  d[13] = s[7];
    d[2] = s[8];  d[6] = s[9];  d[10] = s[10]; d[14] = s[11];
    d[3] = s[12]; d[7] = s[13]; d[11] = s[14]; d[15] = s[15];
  }

  for (unsigned int i = 0; i < src.mNumMeshes; ++i) {
    MeshPtr m = loadMesh(obj, src.mMeshes[i]);
    if (m) dest->meshes << m;
  }

  for (unsigned int i = 0; i < src.mNumChildren; ++i) {
    dest->children << NodePtr(new Node);
    loadNode(obj, dest->children[i], *src.mChildren[i]);
  }

  /// @todo check for cameras and lights
}

ObjectPtr ObjImporter::loadObject(const QString& name) {
  ModelPtr model(new Model(name));
  ObjectPtr obj(new Object3D(name, model));
  loadNode(obj, model->node(), *m_aiscene->mRootNode);
  return obj;
}

AnimationPtr ObjImporter::loadAnim(const QString& name) {
  // const aiAnimation& anim = *m_aiscene->mAnimations[m_names.animations[name]];
  /// @todo implement
  return AnimationPtr();
}

CameraPtr ObjImporter::loadCamera(const QString& name) {
  // const aiCamera& cam = *m_aiscene->mCameras[m_names.cameras[name]];
  /// @todo implement
  return CameraPtr();
}

LightPtr ObjImporter::loadLight(const QString& name) {
  //const aiLight& light = *m_aiscene->mLights[m_names.lights[name]];
  /// @todo implement
  return LightPtr();
}

QString ObjImporter::findFile(QString file) {
  QString filename = QFileInfo(file).fileName();

  QString cwd = QDir::currentPath();
  QDir::setCurrent(QFileInfo(m_file).absolutePath());

  QString ret = QDir(file).canonicalPath();
  if (ret.isEmpty()) ret = QDir(filename).canonicalPath();

  QDir::setCurrent(cwd);

  /// @todo implement something funny and elegant
  return ret;
}
