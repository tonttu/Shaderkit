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
