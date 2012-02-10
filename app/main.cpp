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

#include "gui/mainwindow.hpp"
#include "gui/welcome.hpp"
#include "core/resource_locator.hpp"
#include "gui/limbo_dialog.hpp"
#include "core/scene.hpp"
#include "version.hpp"
#include "shaderdb/shaderdb.hpp"
#include "shaderdb/metainfo.hpp"
#ifndef _WIN32
#include "gl/sandbox_compiler.hpp"
#endif
#include "gl/program.hpp"

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
#ifndef _WIN32
  if (argc == 4 && std::string(argv[1]) == "--sandbox-compiler") {
    return SandboxCompiler::run(argc, argv);
  }
#endif

  QApplication app(argc, argv);
  //App app(argc, argv);
  // glutInit(&argc, argv);
  Log log;
  Log::info("ShaderKit %s", ShaderKit::STR_HASH);

#ifndef _WIN32
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

  int ret = 0;
  {
    MainWindow window;

    MetaInfo limbo_info;
    QString filename;
    bool cont = true;
    while (cont && ShaderDB::findLimbo(limbo_info, filename)) {
      ScenePtr scene = Scene::load(filename, Scene::Limbo);
      if (!scene) continue;
      LimboDialog* limbo = new LimboDialog(filename, scene, limbo_info, cont);
      limbo->show();
      app.exec();
    }

    if (window.loadedScenes() == 0) {
      Welcome* welcome = new Welcome;
      welcome->show();
      ret = app.exec();
    }
  }
  MainWindow::cleanup();
  return ret;
}
