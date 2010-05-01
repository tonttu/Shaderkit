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

#include "mainwindow.hpp"
#include "project.hpp"

#include <QApplication>

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  MainWindow window;
  ProjectPtr project(new Project(window));
  window.setProject(project);
  project->setScene(Project::load("test.lab"));
  window.resize(window.sizeHint());
  window.show();
  return app.exec();
}
