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

RenderPass::RenderPass(QString name, ScenePtr scene)
  : m_type(Disabled), m_name(name), m_scene(scene), m_clear(0),
    m_width(0), m_height(0), m_autosize(true) {
  /// @todo remove these, non-gui class shouldn't call gui stuff
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

void RenderPass::setName(const QString& name) {
  m_name = name;
}

void RenderPass::setType(Type type) {
  if (type == m_type) return;
  if (type == PostProc) {
    m_viewport.reset(new Camera("post"));
    m_viewport->setRect();
  } else if (type == Disabled) {
    m_viewport.reset();
  }
  m_type = type;
  emit changed(shared_from_this());
}

void RenderPass::setDefaultMaterial(MaterialPtr mat) {
  if (mat != m_defaultMaterial) {
    m_defaultMaterial = mat;
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
  if (m_type == PostProc)
    icon = ":/icons/2dpass.png";
  else if (m_type == Normal)
    icon = ":/icons/3dpass.png";
  else
    icon = ":/icons/disabled_pass.png";

  return QIcon(icon);
}

RenderPassPtr RenderPass::clone() const {
  RenderPassPtr r(new RenderPass(m_name, m_scene));
  r->m_type = m_type;
  r->m_objects = m_objects;
  r->m_lights = m_lights;
  r->m_defaultMaterial = m_defaultMaterial;
  r->m_viewport = m_viewport;
  r->m_clear = m_clear;
  r->m_width = m_width;
  r->m_height = m_height;
  r->m_autosize = m_autosize;
  r->m_depth = m_depth;
  r->m_color = m_color;
  return r;
}

void RenderPass::render(State& state, const RenderOptions& render_opts) {
  state.push();
  beginFBO();

  resize(width(), height());
  state.setCamera(m_viewport);
  m_viewport->prepare(width(), height());

  glClearColor(0.2f, 0.2f, 0.2f, 1);
  if (m_clear) glClear(m_clear);
  if (m_defaultMaterial) state.pushMaterial(m_defaultMaterial);

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
    glEnable(GL_NORMALIZE);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_MULTISAMPLE);
    //glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);

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

    if (render_opts.ui) renderUI(state, render_opts);
  }

  if (m_defaultMaterial) state.popMaterial();
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

void RenderPass::renderUI(State& state, const RenderOptions& render_opts) {
  state.push();
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_LINE_SMOOTH);
  glLineWidth(1.7f);

  if (render_opts.grid) {
    if (!m_gridVertices.size()) {
      float step = 10.0f;
      int dist = 5;
      std::vector<float> v;
      std::vector<float> c;
#define V(a,b,c) v.push_back(a), v.push_back(b), v.push_back(c)
#define C(r,g,b,a) c.push_back(r), c.push_back(g), c.push_back(b), c.push_back(a)
      for (int i = -dist; i <= dist; ++i) {
        V(-dist*step, 0, i*step); V(dist*step, 0, i*step);
        V(i*step, 0, -dist*step); V(i*step, 0, dist*step);

        if (i == 0) {
          V((dist-0.2f)*step, 0, -step*0.2f); V(dist*step, 0, 0);
          V((dist-0.2f)*step, 0, step*0.2f); V(dist*step, 0, 0);

          V(-step*0.2f, 0, (dist-0.2f)*step); V(0, 0, dist*step);
          V(step*0.2f, 0, (dist-0.2f)*step); V(0, 0, dist*step);

          for (int j = 0; j < 2; ++j) C(1.0f, 0.5, 0.5f, 0.5f);
          for (int j = 0; j < 2; ++j) C(0.5f, 1.0, 0.5f, 0.5f);
          for (int j = 0; j < 4; ++j) C(1.0f, 0.5, 0.5f, 0.5f);
          for (int j = 0; j < 4; ++j) C(0.5f, 1.0, 0.5f, 0.5f);
        } else {
          for (int j = 0; j < 4; ++j) {
            c.push_back(0.5f); c.push_back(0.5f); c.push_back(0.5f); c.push_back(0.3f);
          }
        }
      }
#undef V
#undef C
      m_gridVertices.enableArray(state, GL_VERTEX_ARRAY, 3, v);
      m_gridColors.enableArray(state, GL_COLOR_ARRAY, 4, c);
    } else {
      m_gridVertices.enableArray(state, GL_VERTEX_ARRAY, 3);
      m_gridColors.enableArray(state, GL_COLOR_ARRAY, 4);
    }
    glRun(glDrawArrays(GL_LINES, 0, m_gridVertices.size()/sizeof(float)/3));
  }
  state.pop();
}

QVariantMap RenderPass::save() const {
  QVariantMap map;

  QVariantList tmp;
  if (m_clear & GL_COLOR_BUFFER_BIT) tmp << "color";
  if (m_clear & GL_DEPTH_BUFFER_BIT) tmp << "depth";
  if (m_clear & GL_STENCIL_BUFFER_BIT) tmp << "stencil";

  if (!tmp.isEmpty()) map["clear"] = tmp;

  map["name"] = m_name;

  /// @todo material here
  ///if (m_shader) map["shader"] = m_shader->name();

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

  /** @todo to material
  foreach (QString name, m_in.keys()) {
    tmp.clear();
    tmp << "texture" << m_in[name]->name();
    in[name] = tmp;
  }

  if (!in.isEmpty()) map["in"] = in;*/
  if (!out.isEmpty()) map["out"] = out;

  return map;
}

void RenderPass::load(QVariantMap map) {
  if (map.contains("material"))
    m_defaultMaterial = m_scene->material(map["material"].toString());

  foreach (QString name, map["objects"].toStringList()) {
    ObjectPtr o = m_scene->object(name);
    if (o) m_objects.insert(o);
  }

  foreach (QString name, map["lights"].toStringList()) {
    LightPtr l = m_scene->light(name);
    if (l) m_lights.insert(l);
  }

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

  QVariantMap out = map["render"].toMap();

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

  emit changed(shared_from_this());
}
