#include "viewport.hpp"
#include "mainwindow.hpp"
#include "glwidget.hpp"
#include "object_creator.hpp"
#include "scene.hpp"

#include <QToolBar>
#include <QVBoxLayout>

Viewport::Viewport(QWidget* parent) : QWidget(parent) {
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
  tb->addSeparator();
  m_frame_selection = tb->addAction(QIcon(":/icons/frame.png"), "Frame selection (F)", this, SLOT(frameSelection()));
  m_frame_all = tb->addAction(QIcon(":/icons/frame_all.png"), "Frame all (A)", this, SLOT(frameAll()));
  tb->addSeparator();
  m_scene_mode = tb->addAction(QIcon(":/icons/blueprint.png"), "Blueprint mode (B)", this, SLOT(sceneMode()));
  m_scene_mode->setChecked(true);
  m_render_mode = tb->addAction(QIcon(":/icons/shader.png"), "Render mode (G)", this, SLOT(renderMode()));
  tb->addSeparator();
  m_delete_object = tb->addAction(QIcon(":/icons/delete.png"), "Delete object", this, SLOT(deleteObject()));

  translateGizmo();
}

void Viewport::newObject() {
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

void Viewport::newLight() {
}

void Viewport::translateGizmo() {
  m_gl_widget->renderOptions().focus_grabber.reset();
  bool c = m_translate->isChecked();
  m_gl_widget->renderOptions().gizmo_type = c ?
        RenderOptions::TRANSLATE : RenderOptions::NONE;
  if (c) {
    m_rotate->setChecked(false);
    m_scale->setChecked(false);
  }
}

void Viewport::rotateGizmo() {
  m_gl_widget->renderOptions().focus_grabber.reset();
  bool c = m_rotate->isChecked();
  m_gl_widget->renderOptions().gizmo_type = c ?
        RenderOptions::ROTATE : RenderOptions::NONE;
  if (c) {
    m_translate->setChecked(false);
    m_scale->setChecked(false);
  }
}

void Viewport::scaleGizmo() {
  m_gl_widget->renderOptions().focus_grabber.reset();
  bool c = m_scale->isChecked();
  m_gl_widget->renderOptions().gizmo_type = c ?
        RenderOptions::SCALE : RenderOptions::NONE;
  if (c) {
    m_translate->setChecked(false);
    m_rotate->setChecked(false);
  }
}

void Viewport::frameSelection() {
}

void Viewport::frameAll() {
}

void Viewport::sceneMode() {
}

void Viewport::renderMode() {
}

void Viewport::deleteObject() {
  ScenePtr scene = m_gl_widget->scene();
  if (!scene) return;

  foreach (ObjectPtr obj, scene->selection())
    scene->remove(obj);
}
