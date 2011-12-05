#ifndef VIEWPORT_HPP
#define VIEWPORT_HPP

#include "forward.hpp"

#include <QWidget>

class Viewport : public QWidget {
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

#endif // VIEWPORT_HPP
