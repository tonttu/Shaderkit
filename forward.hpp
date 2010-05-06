/**
 * Copyright 2010 Riku Palomäki.
 * This file is part of GLSL Lab.
 *
 * GLSL Lab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
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

#include <boost/shared_ptr.hpp>

class Watchable;
class Watcher;

class ShaderLexer;
class ShaderError;
class ShaderCompilerOutputParser;
class UniformVar;
class ShaderTypeInfo;
class Shader;
typedef boost::shared_ptr<Shader> ShaderPtr;
class GLProgram;
typedef boost::shared_ptr<GLProgram> ProgramPtr;

class Light;
typedef boost::shared_ptr<Light> LightPtr;
class Camera;
typedef boost::shared_ptr<Camera> CameraPtr;
class Scene;
typedef boost::shared_ptr<Scene> ScenePtr;
class RenderPass;
typedef boost::shared_ptr<RenderPass> RenderPassPtr;

class State;

class Object3D;
typedef boost::shared_ptr<Object3D> ObjectPtr;
class Teapot;
class Box;

class Properties;
class Editor;
class EditorMargin;
class Highlighter;

namespace Ui {
  class MainWindow;
}
class MainWindow;
class GLWidget;

class Project;
typedef boost::shared_ptr<Project> ProjectPtr;

class QColor;
class QVector3D;
class QVector4D;

#endif
