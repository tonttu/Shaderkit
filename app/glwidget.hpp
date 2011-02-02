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

#ifndef GLWIDGET_HPP
#define GLWIDGET_HPP

#include "forward.hpp"
#include "opengl.hpp"

#include <QGLWidget>

/**
 * OpenGL Widget. This will create the OpenGL context and the widget will
 * work as the final viewport.
 *
 * Widget has exactly one Scene that is connected to it.
 */
class GLWidget : public QGLWidget {
  Q_OBJECT

public:
  GLWidget(QWidget* parent = 0);
  ~GLWidget();

  QSize minimumSizeHint() const;
  QSize sizeHint() const;

  static QGLFormat defaultFormat();

public slots:
  void sceneChange(ScenePtr);

protected:
  /// After the initialization is done, invalidate the screen every 10 milliseconds
  void initializeGL();

  /// Render the scene
  void paintGL();

  /// Update the viewport size to scene
  void resizeGL(int width, int height);

  void mouseMoveEvent(QMouseEvent* event);
  void mousePressEvent(QMouseEvent* event);
  void wheelEvent(QWheelEvent* event);

  ScenePtr m_scene;
  QTimer* m_timer;
  QPointF m_lastpos;

  bool m_initialized;
};

#endif // GLWIDGET_HPP
