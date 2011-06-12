/**
 * Copyright 2010,2011 Riku Palomäki.
 * This file is part of Shaderkit, http://www.shaderkit.org/.
 *
 * Shaderkit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3 as
 * published by the Free Software Foundation.
 *
 * Shaderkit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Shaderkit.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "glwidget.hpp"
#include "scene.hpp"
#include "wrap_glext.h"
#include "camera.hpp"
#include "object3d.hpp"
#include "model.hpp"

/// @todo include something less massive
#include <QtGui>

GLWidget::GLWidget(QWidget *parent)
  : QGLWidget(defaultFormat(), parent),
    m_timer(new QTimer(this)), m_initialized(false) {
  setAcceptDrops(true);
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

  m_timer->start();
}

void GLWidget::paintGL() {
  if (m_scene && m_initialized) m_scene->render();
}

void GLWidget::resizeGL(int width, int height) {
  if (m_scene) m_scene->resize(width, height);
}

void GLWidget::mouseMoveEvent(QMouseEvent* event) {
  QPointF diff = event->posF() - m_lastpos;
  m_lastpos = event->posF();
  float d = width();
  if (height() < d) d = height();
  diff.setX(diff.x() / d);
  diff.setY(diff.y() / d);
  CameraPtr cam;
  if (m_scene && (cam = m_scene->camera())) {
    if (event->buttons() & Qt::LeftButton) {
      cam->rotate(diff);
    } else if (event->buttons() & Qt::MidButton) {
      cam->translate(diff);
    } else {
      QGLWidget::mouseMoveEvent(event);
      return;
    }
    event->accept();
    return;
  }
  QGLWidget::mouseMoveEvent(event);
}

void GLWidget::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton || event->button() == Qt::MidButton) {
    m_lastpos = event->posF();
    event->accept();
    return;
  }
  QGLWidget::mousePressEvent(event);
}

void GLWidget::wheelEvent(QWheelEvent* event) {
  CameraPtr cam;
  if (m_scene && (cam = m_scene->camera())) {
    cam->zoom(event->delta()*-0.1f);
    event->accept();
  } else QGLWidget::wheelEvent(event);
}

void GLWidget::dragEnterEvent(QDragEnterEvent* event) {
  if (m_scene && event->mimeData()->hasFormat("text/x-shaderkit-material")) {
    event->acceptProposedAction();
  }
}

void GLWidget::dragLeaveEvent(QDragLeaveEvent* event) {
  m_scene->pick(-1, -1);
}

void GLWidget::dragMoveEvent(QDragMoveEvent* event) {
  /*m_scene->setPickDisplay(float(event->pos().x())/width(),
                          1.0f-float(event->pos().y())/height());*/
  /*QCursor* cursor = QApplication::overrideCursor();
  if (cursor) {
    cursor->setShape(Qt::BusyCursor);
    QApplication::changeOverrideCursor(QCursor(*cursor));
  }
  event->acceptProposedAction();*/
}

void GLWidget::dropEvent(QDropEvent* event) {
  MaterialPtr material = m_scene->material(event->mimeData()->data("text/x-shaderkit-material"));
  if(material) {
    /* m_scene->pick(float(event->pos().x())/width(), 1.0f-float(event->pos().y())/height(), true,
                  [=](ObjectPtr obj, MeshPtr mesh){ obj->setMaterial(mesh->name, material); }); */
    struct hate_too_old_gcc { static void func(MaterialPtr material, ObjectPtr obj, MeshPtr mesh) {
        obj->setMaterialForMesh(mesh->name, material);
    }};
    using namespace std::placeholders;
    m_scene->pick(float(event->pos().x())/width(), 1.0f-float(event->pos().y())/height(), true,
                  std::bind(&hate_too_old_gcc::func, material, _1, _2));
  } else {
    m_scene->pick(-1, -1);
  }
}


void GLWidget::sceneChange(ScenePtr s) {
  m_scene = s;
  if (s) s->resize(size().width(), size().height());
}
