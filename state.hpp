#ifndef STATE_HPP
#define STATE_HPP

#include <QSet>
#include <QtOpenGL>

/**
 * Represents OpenGL state.
 *
 * This class is meant to work as a replacement for plain OpenGL 2 API. It is
 * faster to keep track of all server-side GL capabilities and only
 * enable/disable minimal set of those when needed. Also some things that were
 * just some magical built-in global variables in shaders are replaced in
 * newer OpenGL versions for example with custom uniform variables, and setting
 * those correctly is a job for this class.
 * 
 * Currently the apply() method doesn't actually do anything, and for example
 * all glEnable/glDisable calls are made directly in the enable/disable methods.
 *
 * @todo Actually store the OpenGL state and do those things documented.
 */
class State {
public:
  /// Returns the next available light id, can be used like GL_LIGHT0 + id
  int nextFreeLight() const;
  /// Reserve/Release a light id
  void setLight(int light_id, bool in_use);

  /// Set GL capability
  void enable(GLenum cap);
  /// Get GL capability
  void disable(GLenum cap);

protected:
  QSet<int> m_lights;
};

#endif // STATE_HPP
