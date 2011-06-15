#include "viewport.hpp"
#include "mainwindow.hpp"
#include "glwidget.hpp"

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
  m_translate = tb->addAction(QIcon(":/icons/rotate.png"), "Translate (W)", this, SLOT(translateGizmo()));
  m_rotate = tb->addAction(QIcon(":/icons/rotate.png"), "Rotate (E)", this, SLOT(rotateGizmo()));
  m_scale = tb->addAction(QIcon(":/icons/scale.png"), "Scale (R)", this, SLOT(scaleGizmo()));
  tb->addSeparator();
  m_frame_selection = tb->addAction(QIcon(":/icons/frame.png"), "Frame selection (F)", this, SLOT(frameSelection()));
  m_frame_all = tb->addAction(QIcon(":/icons/frame_all.png"), "Frame all (A)", this, SLOT(frameAll()));
  tb->addSeparator();
  m_scene_mode = tb->addAction(QIcon(":/icons/blueprint.png"), "Blueprint mode (B)", this, SLOT(sceneMode()));
  m_render_mode = tb->addAction(QIcon(":/icons/shader.png"), "Render mode (G)", this, SLOT(renderMode()));
  tb->addSeparator();
  m_delete_object = tb->addAction(QIcon(":/icons/delete.png"), "Delete object", this, SLOT(deleteObject()));
}

void Viewport::newObject() {
  QMenu menu("New object", this);
  menu.addAction(QIcon(":/icons/cube.png"), "Cube")->setData("cube");
  menu.addAction(QIcon(":/icons/sphere.png"), "Sphere")->setData("sphere");
  menu.addAction(QIcon(":/icons/tube.png"), "Tube")->setData("tube");
  menu.addAction(QIcon(":/icons/torus.png"), "Torus")->setData("torus");
  menu.addAction(QIcon(":/icons/cone.png"), "Cone")->setData("cone");
  menu.addAction(QIcon(":/icons/plane.png"), "Plane")->setData("plane");
  menu.addSeparator();
  menu.addAction(QIcon(":/icons/import.png"), "Import...")->setData("import");
  QAction* a = menu.exec(QCursor::pos());
  if (!a) return;
}

void Viewport::newLight() {
}

void Viewport::translateGizmo() {
}

void Viewport::rotateGizmo() {
}

void Viewport::scaleGizmo() {
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
}
