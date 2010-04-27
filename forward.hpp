#ifndef FORWARD_HPP
#define FORWARD_HPP

#include <boost/shared_ptr.hpp>

class Watchable;
typedef boost::shared_ptr<Watchable> WatchablePtr;
class Watcher;

class ShaderLexer;

class Highlighter;

class Light;
typedef boost::shared_ptr<Light> LightPtr;

class State;

struct Value;

class QColor;
class QVector3D;
class QVector4D;

#endif
