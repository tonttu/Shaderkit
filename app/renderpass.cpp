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

#include "renderpass.hpp"
#include "scene.hpp"
#include "camera.hpp"
#include "light.hpp"
#include "shader/program.hpp"
#include "object3d.hpp"
#include "fbo.hpp"
#include "texture.hpp"
#include "properties.hpp"
#include "render_pass_properties.hpp"
#include "mainwindow.hpp"
#include "state.hpp"
#include <iostream>

RenderPass::RenderPass(QString name, ScenePtr scene)
  : m_type(Normal), m_name(name), m_scene(scene), m_clear(0),
    m_width(0), m_height(0), m_autosize(true) {
  connect(this, SIGNAL(changed(RenderPassPtr)),
          &RenderPassProperties::instance(), SLOT(update(RenderPassPtr)));
  connect(this, SIGNAL(changed(RenderPassPtr)),
          &MainWindow::instance(), SLOT(changed(RenderPassPtr)));
}

int RenderPass::width() const {
  return m_width > 0 && !m_autosize ? m_width : m_scene->width();
}

int RenderPass::height() const {
  return m_height > 0 && !m_autosize ? m_height : m_scene->height();
}

bool RenderPass::autosize() const {
  return m_autosize;
}

void RenderPass::setAutosize(bool v) {
  m_autosize = v;
}

void RenderPass::resize(int w, int h) {
  if (m_width == w && m_height == h)
    return;

  m_width = w;
  m_height = h;
  emit changed(shared_from_this());
}

void RenderPass::setClearBits(GLbitfield bits) {
  if (m_clear != bits) {
    m_clear = bits;
    emit changed(shared_from_this());
  }
}

QStringList RenderPass::out() const {
  /// @todo put these in a map, and make sure the names are unique
  QStringList tmp;
  if (m_color) tmp.append(m_color->name());
  if (m_depth) tmp.append(m_depth->name());
  return tmp;
}

FBOImagePtr RenderPass::out(const QString& name) const {
  if (m_color->name() == name) return m_color;
  if (m_depth->name() == name) return m_depth;
  return FBOImagePtr();
}


QString RenderPass::name() const {
  return m_name;
}

void RenderPass::setType(Type type) {
  if (type == PostProc) {
    m_viewport.reset(new Camera("post"));
    m_viewport->setRect();
  }
  m_type = type;
}

void RenderPass::setShader(ProgramPtr shader) {
  if (shader != m_shader) {
    m_shader = shader;
    emit changed(shared_from_this());
  }
}

void RenderPass::setObjects(Objects objs) {
  if (objs != m_objects) {
    m_objects = objs;
    emit changed(shared_from_this());
  }
}

void RenderPass::setLights(Lights lights) {
  if (lights != m_lights) {
    m_lights = lights;
    emit changed(shared_from_this());
  }
}

void RenderPass::setViewport(CameraPtr camera) {
  if (m_viewport != camera) {
    m_viewport = camera;
    emit changed(shared_from_this());
  }
}

QIcon RenderPass::icon() {
  const char* icon = 0;
  if(m_type == PostProc)
    icon = ":/icons/2dpass.png";
  else
    icon = ":/icons/3dpass.png";

  return QIcon(icon);
}

void RenderPass::render(State& state) {
  state.push();
  beginFBO();

  if (m_clear) glClear(m_clear);
  bool shader = false;
  if (m_shader) shader = m_shader->bind();

  if (shader) {
    m_shader->setUniform(m_uniform_list);
  }

  resize(width(), height());

  m_viewport->prepare(width(), height());

  foreach (QString name, m_in.keys()) {
    int unit = state.reserveTexUnit();
    m_in[name]->bind(unit);
    if (shader)
      m_shader->setUniform(name, unit);
  }

  if (m_type == PostProc) {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(0.0f, 0.0f);

    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(width(), 0.0f);

    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(width(), height());

    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(0.0f, height());

    glEnd();
  } else {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_MULTISAMPLE);

    /// @todo remove this, only for testing
    static float f = 0.0f;
    f += 0.2f;
    glRotatef(f, 0, 1, 0);

    foreach (LightPtr light, m_lights)
      light->activate(state);

    foreach (ObjectPtr obj, m_objects) {
      glPushMatrix();
      glPushAttrib(GL_ALL_ATTRIB_BITS);
      obj->render(state);
      glPopAttrib();
      glPopMatrix();
    }

    foreach (LightPtr light, m_lights)
      light->deactivate(state);
  }

  foreach (TexturePtr tex, m_in)
    tex->unbind();

  if (shader) {
    UniformVar::List list = m_shader->getUniformList();
    m_shader->unbind();

    if(list != m_uniform_list)
      m_uniform_list = list;

    if(list != m_uniform_list_prev) {
      std::cout << "UPDATE" << std::endl;
      m_uniform_list_prev = list;
      ShaderProperties::instance().update(shared_from_this());
    }
  }

  endFBO();
  state.pop();
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

QVariantMap RenderPass::save() const {
  QVariantMap map;

  QVariantList tmp;
  if (m_clear & GL_COLOR_BUFFER_BIT) tmp << "color";
  if (m_clear & GL_DEPTH_BUFFER_BIT) tmp << "depth";
  if (m_clear & GL_STENCIL_BUFFER_BIT) tmp << "stencil";

  if (!tmp.isEmpty()) map["clear"] = tmp;

  if (m_shader) map["shader"] = m_shader->name();

  tmp.clear();
  foreach (ObjectPtr obj, m_objects)
    tmp << obj->name();
  if(!tmp.isEmpty()) map["objects"] = tmp;

  tmp.clear();
  foreach (LightPtr obj, m_lights)
    tmp << obj->name();
  if(!tmp.isEmpty()) map["lights"] = tmp;

  if (m_type == Normal) {
    tmp.clear();
    tmp << "camera" << m_viewport->name();
    map["viewport"] = tmp;
  } else if (m_type == PostProc) {
    map["viewport"] = "post";
  }

  QVariantMap in, out;

  if (m_width > 0 && !m_autosize) out["width"] = m_width;
  if (m_height > 0 && !m_autosize) out["height"] = m_height;

  if (m_depth) {
    tmp.clear();
    tmp << m_depth->imageClass();
    if (!m_depth->name().isEmpty())
      tmp << m_depth->name();
    out["depth"] = tmp;
  }

  if (m_color) {
    tmp.clear();
    tmp << m_color->imageClass();
    if (!m_color->name().isEmpty())
      tmp << m_color->name();
    out["color0"] = tmp;
  }

  foreach (QString name, m_in.keys()) {
    tmp.clear();
    tmp << "texture" << m_in[name]->name();
    in[name] = tmp;
  }

  if (!in.isEmpty()) map["in"] = in;
  if (!out.isEmpty()) map["out"] = out;

  return map;
}

void RenderPass::load(QVariantMap map) {
  if (map.contains("shader")) m_shader = m_scene->shader(map["shader"].toString());

  foreach (QString name, map["objects"].toStringList())
    m_objects.insert(m_scene->object(name));

  foreach (QString name, map["lights"].toStringList())
    m_lights.insert(m_scene->light(name));

  QStringList tmp = map["viewport"].toStringList();
  if (tmp.size() == 2 && tmp[0] == "camera") {
    m_viewport = m_scene->camera(tmp[1]);
    m_type = Normal;
  }
  if (tmp.size() == 1 && tmp[0] == "post") {
    setType(PostProc);
  }

  foreach (QString name, map["clear"].toStringList()) {
    if (name == "color") m_clear |= GL_COLOR_BUFFER_BIT;
    else if (name == "depth") m_clear |= GL_DEPTH_BUFFER_BIT;
    else if (name == "stencil") m_clear |= GL_STENCIL_BUFFER_BIT;
  }

  QVariantMap in = map["in"].toMap();
  QVariantMap out = map["out"].toMap();

  m_width = out["width"].toInt();
  m_height = out["height"].toInt();

  m_autosize = m_width <= 0 || m_height <= 0;

  tmp = out["depth"].toStringList();
  if (tmp.size() == 2 && tmp[0] == "texture")
    m_depth = m_scene->genTexture(tmp[1]);

  if (tmp.size() == 1 && tmp[0] == "renderbuffer")
    m_depth.reset(new RenderBuffer);

  if (tmp.size() == 2 && tmp[0] == "renderbuffer")
    m_depth.reset(new RenderBuffer(tmp[1]));

  tmp = out["color0"].toStringList();
  if (tmp.size() == 2 && tmp[0] == "texture")
    m_color = m_scene->genTexture(tmp[1]);

  if (tmp.size() == 1 && tmp[0] == "renderbuffer")
    m_color.reset(new RenderBuffer);

  if (tmp.size() == 2 && tmp[0] == "renderbuffer")
    m_color.reset(new RenderBuffer(tmp[1]));

  for (QVariantMap::iterator it = in.begin(); it != in.end(); ++it) {
    tmp = it->toStringList();
    if (tmp.size() == 2 && tmp[0] == "texture")
      m_in[it.key()] = m_scene->genTexture(tmp[1]);
  }
  emit changed(shared_from_this());
}
