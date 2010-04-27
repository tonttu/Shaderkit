#include "state.hpp"

int State::nextFreeLight() const {
  int light = 0;
  while (m_lights.contains(light)) ++light;
  return light;
}

void State::setLight(int light_id, bool in_use) {
  if (in_use) {
    m_lights.insert(light_id);
    enable(GL_LIGHTING);
    enable(GL_LIGHT0 + light_id);
  } else {
    m_lights.remove(light_id);
    disable(GL_LIGHT0 + light_id);
  }
}

void State::enable(GLenum cap) {
  glEnable(cap);
}

void State::disable(GLenum cap) {
  glDisable(cap);
}
