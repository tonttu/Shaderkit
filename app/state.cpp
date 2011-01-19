/**
 * Copyright 2010,2011 Riku Palomäki.
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

#include <iostream>

State::State() {
  m_data.push_back(Data());
}

int State::nextFreeLight() const {
  return nextFree(m_data.front().m_lights);
}

void State::setLight(int light_id, bool in_use) {
  if (in_use) {
    m_data.front().m_lights.insert(light_id);
    enable(GL_LIGHTING);
    enable(GL_LIGHT0 + light_id);
  } else {
    m_data.front().m_lights.remove(light_id);
    disable(GL_LIGHT0 + light_id);
  }
}

void State::enable(GLenum cap) {
  glEnable(cap);
}

void State::disable(GLenum cap) {
  glDisable(cap);
}

int State::reserveTexUnit() {
  int unit = nextFree(m_data.front().m_texunits);
  m_data.front().m_texunits << unit;
  return unit;
}

void State::push() {
  m_data.push_back(m_data.front());
}

void State::pop() {
  if (m_data.size() <= 1) {
    std::cerr << "State push/pop mismatch" << std::endl;
  }
}

int State::nextFree(const QSet<int>& lst, int id) const {
  while (lst.contains(id)) ++id;
  return id;
}
