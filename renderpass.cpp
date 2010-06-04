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
#include "scene.hpp"
#include "camera.hpp"
#include "light.hpp"
#include "shader/program.hpp"
#include "object3d.hpp"
#include "fbo.hpp"
#include "texture.hpp"
#include <iostream>

RenderPass::RenderPass(ScenePtr scene) : m_scene(scene), m_clear(0) {}

int RenderPass::width() const {
  return m_width > 0 ? m_width : m_scene->width();
}

int RenderPass::height() const {
  return m_height > 0 ? m_height : m_scene->height();
}

void RenderPass::render(State& state) {
  beginFBO();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glShadeModel(GL_SMOOTH);
  glEnable(GL_MULTISAMPLE);

  if (m_clear) glClear(m_clear);

  if (m_shader) m_shader->bind();

  m_viewport->prepare(width(), height());

  /// @todo remove this, only for testing
  static float f = 0.0f;
  f += 0.2f;
  glRotatef(f, 0, 1, 0);

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

  endFBO();
}

void RenderPass::beginFBO() {
  if (!m_depth && !m_color)
    return;

  if (!m_fbo) {
    m_fbo.reset(new FrameBufferObject);
    if (m_depth) m_fbo->set(GL_DEPTH_ATTACHMENT, m_depth);
    if (m_color) m_fbo->set(GL_COLOR_ATTACHMENT0 + 0, m_color);
  }

  m_fbo->resize(width(), height());
  m_fbo->bind();
}

void RenderPass::endFBO() {
  if (m_fbo) m_fbo->unbind();
}

void RenderPass::load(QVariantMap map) {
  if (map.contains("shader")) m_shader = m_scene->shader(map["shader"].toString());

  foreach (QString name, map["objects"].toStringList())
    m_objects.insert(m_scene->object(name));

  foreach (QString name, map["lights"].toStringList())
    m_lights.insert(m_scene->light(name));

  QStringList tmp = map["viewport"].toStringList();
  if (tmp.size() == 2 && tmp[0] == "camera") m_viewport = m_scene->camera(tmp[1]);

  foreach (QString name, map["clear"].toStringList()) {
    if (name == "color") m_clear |= GL_COLOR_BUFFER_BIT;
    else if (name == "depth") m_clear |= GL_DEPTH_BUFFER_BIT;
    else if (name == "stencil") m_clear |= GL_STENCIL_BUFFER_BIT;
  }

  QVariantMap in = map["in"].toMap();
  QVariantMap out = map["out"].toMap();

  m_width = out["width"].toInt();
  m_height = out["height"].toInt();

  tmp = out["depth"].toStringList();
  if (tmp.size() == 2 && tmp[0] == "texture")
    m_depth = m_scene->texture(tmp[1]);

  tmp = out["color0"].toStringList();
  if (tmp.size() == 2 && tmp[0] == "texture")
    m_color = m_scene->texture(tmp[1]);
}
