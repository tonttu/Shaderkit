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

#include "glwidget.hpp"
#include "scene.hpp"
#include "wrap_glext.h"

/// @todo include something less massive
#include <QtGui>

GLWidget::GLWidget(QWidget *parent)
  : QGLWidget(defaultFormat(), parent),
    m_timer(new QTimer(this)), m_initialized(false) {
  connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
  m_timer->setInterval(10);
}

GLWidget::~GLWidget() {}

QGLFormat GLWidget::defaultFormat() {
  QGLFormat format(QGL::DoubleBuffer | QGL::DepthBuffer | QGL::Rgba |
                   QGL::AlphaChannel | QGL::DirectRendering | QGL::SampleBuffers);
  //format.setVersion(3, 2);
  //format.setProfile(QGLFormat::CompatibilityProfile);
  return format;
}

QSize GLWidget::minimumSizeHint() const {
  return QSize(64, 36);
}

QSize GLWidget::sizeHint() const {
  return QSize(448, 252);
}

void GLWidget::initializeGL() {
  static GLenum s_glew_status = GLEW_OK + 1;
  if (s_glew_status != GLEW_OK) {
    s_glew_status = glewInit();
    if (s_glew_status == GLEW_OK) {
      Log::info("GLEW %s initialized", glewGetString(GLEW_VERSION));
      wrap_gl_extensions();
      m_initialized = true;
    } else {
      Log::error("Failed to initialize GLEW: %s", glewGetErrorString(s_glew_status));
    }
  }

  /*int major = 0, minor = 0, profile = 0;
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MINOR_VERSION, &minor);
  glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);

  qDebug() << "OpenGL" << (QString::number(major) + "." + QString::number(minor)).toStdString().c_str() <<
      (profile == GL_CONTEXT_CORE_PROFILE_BIT ? "Core" :
       profile == GL_CONTEXT_COMPATIBILITY_PROFILE_BIT ? "Compatibility" :
      (major > 3 || (major >= 3 && minor >= 2)) ? "Core (default)" : "Unknown") << "profile initialized";*/

  /// @todo this should be in RenderPass
  qglClearColor(QColor(0,0,0));
  m_timer->start();
}

void GLWidget::paintGL() {
  if (m_scene && m_initialized) m_scene->render();
}

void GLWidget::resizeGL(int width, int height) {
  if (m_scene) m_scene->resize(width, height);
}

void GLWidget::sceneChange(ScenePtr s) {
  m_scene = s;
  if (s) s->resize(size().width(), size().height());
}
