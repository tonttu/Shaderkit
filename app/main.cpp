/**
 * Copyright 2010,2011 Riku Palomäki.
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

#include "mainwindow.hpp"
#include "welcome.hpp"
#include "resource_locator.hpp"
#include "shaderdb/shaderdb.hpp"
#ifndef _WIN32
#include "shader/sandbox_compiler.hpp"
#endif
#include "shader/program.hpp"

#include <QApplication>
#include <QDir>

// #include <GL/glut.h>

/*#include <X11/Xlib.h>

class App : public QApplication {
public:
  App(int& argc, char* argv[]) : QApplication(argc, argv) {}

  bool x11EventFilter(XEvent* e) {
    if (e->type == ButtonRelease) {
      restoreOverrideCursor();
      Log::info("Restore");
    }
    return false;
  }
};*/

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  //App app(argc, argv);
  // glutInit(&argc, argv);
  Log log;

#ifndef _WIN32
  if (argc == 4 && std::string(argv[1]) == "--sandbox-compiler") {
    return SandboxCompiler::run(QString(argv[2]).toInt(), QString(argv[3]).toInt());
  }

  SandboxCompiler init(argv[0]);
#endif

  ShaderDB db;
  {
    db.addPath(QDir::currentPath());
    db.addPath(QDir::currentPath() + "/examples");
    db.addPath(QDir::currentPath() + "/..");
    db.addPath(QDir::currentPath() + "/../examples");
    QDir dir(argv[0]);
    if (dir.cdUp()) {
      db.addPath(dir.path());
      db.addPath(dir.path() + "/examples");
      db.addPath(dir.path() + "/..");
      db.addPath(dir.path() + "/../examples");
      db.addPath(dir.path() + "../share/shaderkit/examples");
    }
    db.addPath(QDir::homePath() + "/.shaderkit/shaderdb", true);
  }

  ResourceLocator rl;
  rl.setPath("db", db.defaultPath());

  ShaderManager shader_manager;

  MainWindow window;
  Welcome* welcome = new Welcome;
  welcome->show();

  return app.exec();
}
