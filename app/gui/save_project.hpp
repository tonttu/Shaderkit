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

#ifndef SAVE_PROJECT_HPP
#define SAVE_PROJECT_HPP

#include "forward.hpp"
#include <QDialog>

namespace Ui {
class SaveProject;
}

class SaveProject : public QDialog {
  Q_OBJECT

public:
  explicit SaveProject(QWidget* parent = 0);
  ~SaveProject();

  static bool save(ScenePtr scene);

protected:
  void keyPressEvent(QKeyEvent* e);

private slots:
  void filenameChanged(QString);
  void browse();

private:
  Ui::SaveProject* m_ui;
};

#endif // SAVE_PROJECT_HPP
