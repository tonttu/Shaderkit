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
