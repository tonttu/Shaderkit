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

#include "renderpass.hpp"
// #include "scene.hpp"
#include "camera.hpp"
#include "light.hpp"
#include "shader/program.hpp"
#include "object3d.hpp"

RenderPass::RenderPass(ScenePtr scene) : m_scene(scene), m_clear(0) {}

void RenderPass::render(State& state) {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glShadeModel(GL_SMOOTH);
  glEnable(GL_MULTISAMPLE);

  if (m_clear) glClear(m_clear);

  if (m_shader) m_shader->bind();

//  m_viewport->prepare(m_scene->width(), m_scene->height());

  for (Lights::iterator it = m_lights.begin(); it != m_lights.end(); ++it) {
    (*it)->activate(state);
  }

  for (Objects::iterator it = m_objects.begin(); it != m_objects.end(); ++it) {
    glPushMatrix();
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    (*it)->render(state);
    glPopAttrib();
    glPopMatrix();
  }

  for (Lights::iterator it = m_lights.begin(); it != m_lights.end(); ++it) {
    (*it)->deactivate(state);
  }

  if (m_shader) m_shader->unbind();
}
