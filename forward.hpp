#ifndef FORWARD_HPP
#define FORWARD_HPP

#include <boost/shared_ptr.hpp>

class Watchable;
typedef boost::shared_ptr<Watchable> WatchablePtr;
class Watcher;

class ShaderLexer;
class ShaderError;
class ShaderCompilerOutputParser;

class Highlighter;

class Light;
typedef boost::shared_ptr<Light> LightPtr;

class Camera;
typedef boost::shared_ptr<Camera> CameraPtr;

class State;

struct Value;

class QColor;
class QVector3D;
class QVector4D;

class Object3D;
typedef boost::shared_ptr<Object3D> ObjectPtr;
class Teapot;
class Box;

#endif
