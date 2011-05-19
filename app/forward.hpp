/**
 * Copyright 2010,2011 Riku Palomäki.
 * This file is part of GLSL Lab.
 *
 * GLSL Lab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3 as
 * published by the Free Software Foundation.
 *
 * GLSL Lab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GLSL Lab.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FORWARD_HPP
#define FORWARD_HPP

#include "log.hpp"

/// @todo move these two away from here
#include <limits>
#include <QHash>

#include <memory>

class Watchable;
class Watcher;

class ShaderLexer;
class ShaderError;
class ShaderCompilerOutputParser;
class UniformVar;
struct ShaderTypeInfo;
class Shader;
typedef std::shared_ptr<Shader> ShaderPtr;
class GLProgram;
typedef std::shared_ptr<GLProgram> ProgramPtr;

class Light;
typedef std::shared_ptr<Light> LightPtr;
class Camera;
typedef std::shared_ptr<Camera> CameraPtr;
class Scene;
typedef std::shared_ptr<Scene> ScenePtr;
class RenderPass;
typedef std::shared_ptr<RenderPass> RenderPassPtr;
class SceneObject;

class State;
class FBOImage;
typedef std::shared_ptr<FBOImage> FBOImagePtr;
class Texture;
typedef std::shared_ptr<Texture> TexturePtr;
class FrameBufferObject;
typedef std::shared_ptr<FrameBufferObject> FBOPtr;

class QueryShader;
class Object3D;
typedef std::shared_ptr<Object3D> ObjectPtr;
class Teapot;
class Box;
class Mesh;
class TriMesh;
typedef std::shared_ptr<Mesh> MeshPtr;
class Animation;
typedef std::shared_ptr<Animation> AnimationPtr;
class Material;
typedef std::shared_ptr<Material> MaterialPtr;
class Model;
typedef std::shared_ptr<Model> ModelPtr;
class Node;
typedef std::shared_ptr<Node> NodePtr;

class MaterialProperties;
class GLSLEditor;
class MultiEditor;
class EditorMargin;
class Highlighter;

namespace Ui {
  class MainWindow;
}
class MainWindow;
class IconBtn;
class GLWidget;
class TextureWidgetGL;

class QColor;
class QVector3D;
class QVector4D;

/// Hash for QSet etc
template <typename T>
uint qHash(std::shared_ptr<T> t) {
  return uint((long)t.get() & std::numeric_limits<uint>::max());
}

#endif
