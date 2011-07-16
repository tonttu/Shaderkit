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
#include "mainwindow.hpp"
#include "gizmos.hpp"
#include "viewport.hpp"

/// @todo include something less massive
#include <QtGui>

GLWidget::GLWidget(const QGLFormat& format, QWidget* parent, const QGLWidget* shareWidget)
  : QGLWidget(format, parent, shareWidget),
    m_timer(new QTimer(this)), m_view(0), m_initialized(false) {
  m_view = dynamic_cast<Viewport*>(parent);
  setAcceptDrops(true);
  connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
  m_timer->setInterval(10);

  setMouseTracking(true);
}

GLWidget::GLWidget(QGLContext* context, QWidget* parent)
  : QGLWidget(context, parent),
    m_timer(new QTimer(this)), m_initialized(false) {
  setAcceptDrops(true);
  connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
  m_timer->setInterval(10);

  setMouseTracking(true);
}


GLWidget::~GLWidget() {
  MainWindow::instance().destroyedGL(this);
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
  if (m_scene && m_initialized) {
    if (m_scene->selection().size() != 1) {
      if (m_render_options.gizmo) m_render_options.gizmo.reset();
    } else {
      if (m_render_options.gizmo_type == RenderOptions::NONE
          && m_render_options.gizmo)
        m_render_options.gizmo.reset();
      else if (m_render_options.gizmo_type == RenderOptions::TRANSLATE
               && !dynamic_cast<TranslateGizmo*>(m_render_options.gizmo.get()))
        m_render_options.gizmo.reset(new TranslateGizmo);
      else if (m_render_options.gizmo_type == RenderOptions::ROTATE
               && !dynamic_cast<RotateGizmo*>(m_render_options.gizmo.get()))
        m_render_options.gizmo.reset(new RotateGizmo);
      else if (m_render_options.gizmo_type == RenderOptions::SCALE
               && !dynamic_cast<ScaleGizmo*>(m_render_options.gizmo.get()))
        m_render_options.gizmo.reset(new ScaleGizmo);

      if (m_render_options.gizmo)
        m_render_options.gizmo->setObject(m_scene->selection().first());
    }
    m_scene->render(m_render_options);
  }
}

void GLWidget::resizeGL(int width, int height) {
  if (m_scene) m_scene->resize(width, height);
}

void GLWidget::mouseMoveEvent(QMouseEvent* event) {
  QPointF diff = event->posF() - m_lastpos;
  m_lastpos = event->posF();

  if (event->buttons() == Qt::NoButton && m_render_options.gizmo) {
    m_render_options.gizmo->hover(
          Eigen::Vector2f(m_lastpos.x(),
                          height() - m_lastpos.y()));
  } else if(event->buttons() == Qt::LeftButton && m_render_options.gizmo && m_render_options.gizmo->active()) {
    Eigen::Vector2f diff2(diff.x(), -diff.y());
    m_render_options.gizmo->input(diff2);
  }

  /*float d = width();
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
  QGLWidget::mouseMoveEvent(event);*/
}

void GLWidget::mousePressEvent(QMouseEvent* event) {
  m_button_down[event->button()] = event->posF();

  if (event->buttons() == Qt::LeftButton && m_render_options.gizmo) {
    m_render_options.gizmo->buttonDown(Eigen::Vector2f(event->posF().x(),
                                       height() - event->posF().y()));
  }

  /*
  /// @todo re-enable rotating stuff
  if (event->button() == Qt::LeftButton || event->button() == Qt::MidButton) {
    m_lastpos = event->posF();
    event->accept();
    return;
  }*/

  QGLWidget::mousePressEvent(event);
}

void GLWidget::mouseReleaseEvent(QMouseEvent* event) {
  if(m_render_options.gizmo && m_render_options.gizmo->active()) {
    m_render_options.gizmo->buttonUp();
  }

  /// @todo multi-selection
  // select objects
  if (event->modifiers() == 0 && event->button() == Qt::LeftButton) {
    event->accept();
    QPointF diff = event->posF() - m_button_down[event->button()];
    if (diff.x()*diff.x() + diff.y()*diff.y() < 3*3) {
      struct hate_too_old_gcc { static void func(ScenePtr scene, ObjectPtr obj, MeshPtr /*mesh*/) {
          QList<ObjectPtr> objects;
          objects << obj;
          scene->setSelection(objects);
      }};
      using namespace std::placeholders;
      m_scene->pick(float(event->pos().x())/width(), 1.0f-float(event->pos().y())/height(), true,
                    std::bind(&hate_too_old_gcc::func, m_scene, _1, _2));
    }
  }
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
