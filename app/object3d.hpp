/**
 * Copyright 2010 Riku Palomäki.
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

#ifndef OBJECT3D_HPP
#define OBJECT3D_HPP

#include "forward.hpp"

#include <QObject>

/**
 * 3D Model class.
 *
 * Every renderable object should inherit from this, including all built-in
 * objects and models loaded from files.
 *
 * An object might have texture coordinates, but actual textures, shaders,
 * vertex colors etc. come from other classes, this is strictly 3D trimesh
 * or similar renderable object. Shaders etc should be ready before calling
 * the render() method.
 */
class Object3D : public QObject {
  Q_OBJECT

public:
  Object3D();
  virtual ~Object3D();

  /// Returns the object (model) name
  virtual QString name() const = 0;
  /// Renders the object with given state
  virtual void render(State& state) = 0;
};

/// Built-in Teapot object, that is a tuned version of GLUT teapot.
class Teapot : public Object3D {
public:
  virtual QString name() const { return "Teapot"; }
  virtual void render(State& state);
};

/// Built-in dummy Box object
class Box : public Object3D {
public:
  virtual QString name() const { return "Box"; }
  virtual void render(State& state);
};

#endif // OBJECT3D_HPP
