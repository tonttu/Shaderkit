/**
 * Copyright 2010 Riku Palomäki.
 * This file is part of GLSL Lab.
 *
 * GLSL Lab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GLSL Lab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GLSL Lab.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "forward.hpp"
#include "shader/error.hpp"

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QPushButton>

/// Clickable custom button used in the editor status bar inside the tab widget.
class IconBtn : public QPushButton {
  Q_OBJECT

  virtual ~IconBtn();

public slots:
  /// Custom rendering of the widget, because it is the only way to make sure
  /// the system doesn't draw some weird lines around the button when it's active.
  virtual void paintEvent(QPaintEvent*);
};

/**
 * All other widgets are part of the widget tree that the Main Window starts as a root widget.
 */
class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget* parent = 0);
  ~MainWindow();

  /// Sets the active project.
  void setProject(ProjectPtr p);

  /// Creates a new editor widget for given shader, and add the editor to a new tab.
  Editor* createEditor(ShaderPtr shader);

  QList<Editor*> editors() { return m_editors; }

public slots:
  /// Updates the error list
  void shaderCompiled(ShaderPtr shader, ShaderError::List errors);

protected:
  void keyPressEvent(QKeyEvent* event);
  void changeEvent(QEvent* e);

protected slots:
  /// Focus the correct tab and line for selected error.
  void errorItemActivated(QTableWidgetItem*);
  /// An editors modification status is changed and the star after the tab name is added/removed.
  void modificationChanged(bool b);
  /// Save the current file (the file open in the active editor)
  void save();

  void renderToggled(bool);
  void shaderPMToggled(bool);
  void renderPMToggled(bool);
  void filelistToggled(bool);
  void logToggled(bool);

private:
  /// Main layout generated from the .ui -file.
  std::auto_ptr<Ui::MainWindow> m_ui;

  /// Currently there is exactly one project for one MainWindow.
  ProjectPtr m_project;

  /// List of all editors, in the same order as they are in editor_tabs.
  QList<Editor*> m_editors;

  /// Maps one item in the error_list (column 0) to correct error
  QMap<QTableWidgetItem*, ShaderError> m_error_list_items;
};

#endif // MAINWINDOW_HPP
