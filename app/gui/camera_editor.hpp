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
  void create();
  void duplicate();
  void remove();
  void selectionChanged();
  void itemChanged(QListWidgetItem*);
  void updated(RenderPassPtr pass);
  void cameraListUpdated();

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
