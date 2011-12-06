#ifndef CAMERA_EDITOR_HPP
#define CAMERA_EDITOR_HPP

#include <QDialog>

#include "forward.hpp"
#include <QDialog>

namespace Ui {
class CameraEditor;
}

class QListWidgetItem;
class CameraEditor : public QDialog {
  Q_OBJECT

public:
  explicit CameraEditor(QWidget *parent, RenderPassPtr render_pass);
  ~CameraEditor();

private slots:
  void duplicate();
  void remove();
  void selectionChanged();
  void itemChanged(QListWidgetItem*);
  void updated(RenderPassPtr pass);

  void nameChanged();
  void typeChanged();
  void locChanged();
  void targetChanged();
  void fovChanged();
  void nearChanged();
  void farChanged();

private:
  void updatePanel();

  Ui::CameraEditor* m_ui;
  RenderPassPtr m_render_pass;

  CameraPtr m_camera;

  QAction* m_duplicate;
  QAction* m_destroy;
};

#endif // CAMERA_EDITOR_HPP
