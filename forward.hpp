#ifndef FORWARD_HPP
#define FORWARD_HPP

#include <boost/shared_ptr.hpp>

class Watchable;
typedef boost::shared_ptr<Watchable> WatchablePtr;
class Watcher;

class ShaderLexer;

class Highlighter;

class State;

#endif
