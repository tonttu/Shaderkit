/**
 * Copyright 2010-2012 Riku Palomäki.
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

#ifndef GLWIDGET_HPP
#define GLWIDGET_HPP

#include "forward.hpp"
#include "gl/opengl.hpp"
#include "core/renderpass.hpp"

#include "Eigen/Geometry"

#include <QGLWidget>

namespace Shaderkit
{

  /**
   * OpenGL Widget. This will create the OpenGL context and the widget will
   * work as the final viewport.
   *
   * Widget has exactly one Scene that is connected to it.
   */
  class GLWidget : public QGLWidget
  {
    Q_OBJECT

  public:
    GLWidget(const QGLFormat& format, QWidget* parent = 0, const QGLWidget* shareWidget = 0);
    GLWidget(QGLContext* context, QWidget* parent = 0);
    ~GLWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    RenderOptions& renderOptions() { return m_render_options; }

    ScenePtr scene() const { return m_scene; }

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
    void mouseReleaseEvent(QMouseEvent* event);

    void wheelEvent(QWheelEvent* event);

    void dragEnterEvent(QDragEnterEvent* event);
    void dragLeaveEvent(QDragLeaveEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dropEvent(QDropEvent* event);

    ScenePtr m_scene;
    QTimer* m_timer;

    QMap<Qt::MouseButton, Eigen::Vector2f> m_button_down;

    Viewport* m_view;
    CameraPtr m_debug_camera;

    RenderOptions m_render_options;
    bool m_initialized;
  };

  extern QVector<Eigen::Vector2f> g_debug_qt;
  extern QVector<Eigen::Vector2f> g_debug_2d;

} // namespace Shaderkit

#endif // GLWIDGET_HPP
