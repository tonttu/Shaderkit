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

#include "gui/viewport.hpp"
#include "gui/mainwindow.hpp"
#include "gui/glwidget.hpp"
#include "core/object_creator.hpp"
#include "core/scene.hpp"

#include <QToolBar>
#include <QVBoxLayout>
#include <QAction>
#include <QMenu>

namespace Shaderkit
{

  Viewport::Viewport(QWidget* parent) : QWidget(parent)
  {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    m_gl_widget = MainWindow::instance().createGL<GLWidget>(this);
    layout->addWidget(m_gl_widget);

    QToolBar* tb = new QToolBar(this);
    tb->setFloatable(false);
    tb->setMovable(false);
    tb->setIconSize(QSize(16, 16));
    layout->addWidget(tb);

    m_new_object = tb->addAction(QIcon(":/icons/new_object.png"), "New object", this, SLOT(newObject()));
    m_new_light = tb->addAction(QIcon(":/icons/new_light.png"), "New light", this, SLOT(newLight()));
    m_new_light->setEnabled(false);
    tb->addSeparator();
    m_translate = tb->addAction(QIcon(":/icons/translate.png"), "Translate (W)", this, SLOT(translateGizmo()));
    m_translate->setCheckable(true);
    m_translate->setChecked(true);
    m_rotate = tb->addAction(QIcon(":/icons/rotate.png"), "Rotate (E)", this, SLOT(rotateGizmo()));
    m_rotate->setCheckable(true);
    m_rotate->setChecked(false);
    m_scale = tb->addAction(QIcon(":/icons/scale.png"), "Scale (R)", this, SLOT(scaleGizmo()));
    m_scale->setCheckable(true);
    m_scale->setChecked(false);
    m_scale->setEnabled(false);
    tb->addSeparator();
    m_frame_selection = tb->addAction(QIcon(":/icons/frame.png"), "Frame selection (F)", this, SLOT(frameSelection()));
    m_frame_selection->setEnabled(false);
    m_frame_all = tb->addAction(QIcon(":/icons/frame_all.png"), "Frame all (A)", this, SLOT(frameAll()));
    m_frame_all->setEnabled(false);
    tb->addSeparator();
    m_scene_mode = tb->addAction(QIcon(":/icons/blueprint.png"), "Blueprint mode (B)", this, SLOT(sceneMode()));
    m_scene_mode->setChecked(true);
    m_scene_mode->setEnabled(false);
    m_render_mode = tb->addAction(QIcon(":/icons/shader.png"), "Render mode (G)", this, SLOT(renderMode()));
    m_render_mode->setEnabled(false);
    tb->addSeparator();
    m_delete_object = tb->addAction(QIcon(":/icons/delete.png"), "Delete object", this, SLOT(deleteObject()));

    translateGizmo();
  }

  void Viewport::newObject()
  {
    QMenu menu("New object", this);
    menu.addAction(QIcon(":/icons/cube.png"), "Box")->setData("box");
    menu.addAction(QIcon(":/icons/sphere.png"), "Sphere")->setData("sphere");
    menu.addAction(QIcon(":/icons/tube.png"), "Tube")->setDisabled(true);// ->setData("tube");
    menu.addAction(QIcon(":/icons/torus.png"), "Torus")->setDisabled(true);// ->setData("torus");
    menu.addAction(QIcon(":/icons/cone.png"), "Cone")->setDisabled(true);//->setData("cone");
    menu.addAction(QIcon(":/icons/plane.png"), "Plane")->setDisabled(true);//->setData("plane");
    menu.addSeparator();
    menu.addAction(QIcon(":/icons/import.png"), "Import...")->setData("import");
    QAction* a = menu.exec(QCursor::pos());
    if (!a) return;

    if (a->data().toString() == "import") {
      MainWindow::instance().import();
      return;
    }

    m_gl_widget->renderOptions().gizmo_type = RenderOptions::NONE;
    m_gl_widget->renderOptions().grid_animation = 0.0f;
    m_gl_widget->renderOptions().focus_grabber.reset(
      new ObjectCreator(m_gl_widget->scene(), a->data().toString()));
  }

  void Viewport::newLight()
  {
  }

  void Viewport::translateGizmo()
  {
    m_gl_widget->renderOptions().focus_grabber.reset();
    bool c = m_translate->isChecked();
    m_gl_widget->renderOptions().gizmo_type = c ?
        RenderOptions::TRANSLATE : RenderOptions::NONE;
    if (c) {
      m_rotate->setChecked(false);
      m_scale->setChecked(false);
    }
  }

  void Viewport::rotateGizmo()
  {
    m_gl_widget->renderOptions().focus_grabber.reset();
    bool c = m_rotate->isChecked();
    m_gl_widget->renderOptions().gizmo_type = c ?
        RenderOptions::ROTATE : RenderOptions::NONE;
    if (c) {
      m_translate->setChecked(false);
      m_scale->setChecked(false);
    }
  }

  void Viewport::scaleGizmo()
  {
    m_gl_widget->renderOptions().focus_grabber.reset();
    bool c = m_scale->isChecked();
    m_gl_widget->renderOptions().gizmo_type = c ?
        RenderOptions::SCALE : RenderOptions::NONE;
    if (c) {
      m_translate->setChecked(false);
      m_rotate->setChecked(false);
    }
  }

  void Viewport::frameSelection()
  {
  }

  void Viewport::frameAll()
  {
  }

  void Viewport::sceneMode()
  {
  }

  void Viewport::renderMode()
  {
  }

  void Viewport::deleteObject()
  {
    ScenePtr scene = m_gl_widget->scene();
    if (!scene) return;

    foreach (ObjectPtr obj, scene->selection())
      scene->remove(obj);
  }

} // namespace Shaderkit
