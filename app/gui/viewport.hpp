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

#ifndef VIEWPORT_HPP
#define VIEWPORT_HPP

#include "forward.hpp"

#include <QWidget>

namespace Shaderkit
{

  class Viewport : public QWidget
  {
    Q_OBJECT

  public:
    Viewport(QWidget* parent = 0);

    QAction* actionNewObj() const { return m_new_object; }
    QAction* actionNewLight() const { return m_new_light; }

  signals:

  private slots:
    void newObject();
    void newLight();

    void translateGizmo();
    void rotateGizmo();
    void scaleGizmo();

    void frameSelection();
    void frameAll();

    void sceneMode();
    void renderMode();

    void deleteObject();

  protected:
    GLWidget* m_gl_widget;

    QAction* m_new_object;
    QAction* m_new_light;
    QAction* m_translate;
    QAction* m_rotate;
    QAction* m_scale;
    QAction* m_delete_object;
    QAction* m_frame_selection;
    QAction* m_frame_all;
    QAction* m_scene_mode;
    QAction* m_render_mode;
  };

} // namespace Shaderkit

#endif // VIEWPORT_HPP
