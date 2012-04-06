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
#include <QSettings>
#include <QTextStream>

#ifdef _WIN32
#include <QDesktopServices>
#endif

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

void findPath(ShaderDB& db, const QString& root, bool add_root = true, const QString& name = "examples") {
  QDir dir(root);
  for (int i = 0; i < 5 && dir.exists(); ++i) {
    if (i == 0 && add_root) db.addPath(root);
    if (dir.exists(name)) db.addPath(dir.path() + "/" + name);
    if (!dir.cdUp()) break;
  }
}

// If we are developing the application and compiling it with shadow building,
// it's tricky to try to determine the actual source directory. Let's see if
// we can parse relevant information from Makefile
void findSources(ShaderDB& db, const QString& root) {
  QDir dir(root);
  QRegExp r("# Project:\\s+([^\\s].+)[\\\\/][^\\\\/]+");
  for (int i = 0; i < 2 && dir.exists(); ++i) {
    if (dir.exists("Makefile")) {
      QFile file(dir.path() + "/Makefile");
      if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream in(&file);
        QString line = in.readLine();
        for (int j = 0; j < 50 && !line.isNull(); ++j) {
          if (r.exactMatch(line)) {
            if (QDir::isRelativePath(r.cap(1))) {
              findPath(db, dir.path() + "/" + r.cap(1), false);
            } else {
              findPath(db, r.cap(1), false);
            }
            // Stop at the first match
            break;
          }
          line = in.readLine();
        }
      }
      // Stop at the first Makefile
      break;
    }
    if (!dir.cdUp()) break;
  }
}

int main(int argc, char* argv[]) {
#ifndef _WIN32
  if (argc == 4 && std::string(argv[1]) == "--sandbox-compiler") {
    return SandboxCompiler::run(argc, argv);
  }
#endif

#ifdef _WIN32
  // For installer, do not (un)install running application
  CreateMutexA(0, false, "ShaderkitGlobalMutex");
#endif

  QApplication app(argc, argv);
  app.setApplicationName("Shaderkit");
  app.setApplicationVersion(ShaderKit::STR);

  //App app(argc, argv);
  // glutInit(&argc, argv);
  Log log;
  Log::info("ShaderKit %s", ShaderKit::STR_HASH);

#ifndef _WIN32
  SandboxCompiler init(argv[0]);
#endif

  ShaderDB db;
  {
    findPath(db, QDir::currentPath());
    findSources(db, QDir::currentPath());

    QDir dir(argv[0]);
    if (dir.cdUp()) {
      findPath(db, dir.path());
      findPath(db, dir.path() + "/../share/shaderkit");
      findSources(db, dir.path());
      app.addLibraryPath(dir.path() + "/../plugins");
    }

#ifdef _WIN32
    QSettings settings("HKEY_LOCAL_MACHINE\\Software\\Shaderkit",
                       QSettings::NativeFormat);
    QString root = settings.value("Root").toString();
    if (!root.isEmpty()) {
      findPath(db, root);
      app.addLibraryPath(root + "/plugins");
    }
#endif

    const char* env = getenv("SHADERKIT_EXAMPLES");
    if (env && *env)
      findPath(db, env);

#ifdef _WIN32
    db.addPath(QDesktopServices::storageLocation(QDesktopServices::DataLocation)
               + "/shaderdb", true);
#else
    // On linux QDesktopServices returns funny ~/.local/share/data -directory,
    // that seems to be nonstandard and weird
    db.addPath(QDir::homePath() + "/.local/share/shaderkit/shaderdb", true);
#endif
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
