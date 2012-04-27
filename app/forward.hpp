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

#ifndef FORWARD_HPP
#define FORWARD_HPP

#include "core/log.hpp"

/// @todo move these two away from here
#include <limits>
#include <QHash>

#include <memory>

namespace Shaderkit {

class Watchable;
class Watcher;

class ShaderLexer;
class ShaderError;
class ShaderCompilerOutputParser;
class UniformVar;
class AttributeVar;
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
struct RenderOptions;

class State;
class FBOImage;
typedef std::shared_ptr<FBOImage> FBOImagePtr;
class Texture;
typedef std::shared_ptr<Texture> TexturePtr;
class TextureFile;
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
struct Node;
typedef std::shared_ptr<Node> NodePtr;

class MaterialProperties;
class GLSLEditor;
class MultiEditor;
class EditorMargin;
class Highlighter;
class AfterIdleOperation;

class GLSLParser;
class GLSLpp;

class Gizmo;
typedef std::shared_ptr<Gizmo> GizmoPtr;
class FocusGrabber;
typedef std::shared_ptr<FocusGrabber> FocusGrabberPtr;

namespace Ui {
  class MainWindow;
}
class MainWindow;
class IconBtn;
class GLWidget;
class TextureWidgetGL;
class Viewport;

class MetaInfo;

class MeshManager;
class BufferObject;
class BufferObject2;

/// Hash for QSet etc
template <typename T>
uint qHash(std::shared_ptr<T> t) {
  return uint((long)t.get() & std::numeric_limits<uint>::max());
}
} // namespace Shaderkit

namespace Eigen {
  template<typename S, int R, int C, int O, int MR, int MC> class Matrix;

  typedef Matrix<float, 2, 1, 0, 2, 1> Vector2f;
  typedef Matrix<float, 3, 1, 0, 3, 1> Vector3f;
  typedef Matrix<float, 4, 1, 0, 4, 1> Vector4f;
}


#endif
