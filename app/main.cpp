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

#include "version.hpp"

#include "core/resource_locator.hpp"
#include "core/scene.hpp"
#include "core/material.hpp"

#include "shaderdb/shaderdb.hpp"
#include "shaderdb/metainfo.hpp"

#include "parser/glsl_parser.hpp"

#include "gl/program.hpp"
#include "gl/texture.hpp"

#include "gui/mainwindow.hpp"
#include "gui/welcome.hpp"
#include "gui/limbo_dialog.hpp"
#include "gui/importer_wizard.hpp"

#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QTextStream>
#include <QImageReader>

#ifndef _WIN32
#include "gl/sandbox_compiler.hpp"
#endif

#ifdef _WIN32
#include <QDesktopServices>
#endif

using namespace Shaderkit;

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

struct Commands {
  Commands() : show_help(false), show_version(false) {}
  QStringList files;
  QStringList errors;
  bool show_help;
  bool show_version;
};

void showHelp()
{
  printf("Usage:   shaderkit [command]... [file]...\n\n"
         "Commands:\n"
         "  --version           Print Shaderkit version (%s)\n"
         "  --help              Show this help\n\n"
         "Files can include not more than one .shaderkit project file and any number of\n"
         "different resource files. If only resource files are given, an empty project is\n"
         "created.\n\n"
         "Resource file can be any supported 3D model, image (texture) or GLSL shader.\n\n"
         "You may optionally separate options and files with --\n", Shaderkit::STR);
}

void showVersion()
{
  printf("Shaderkit %s (%s)\n", Shaderkit::STR, Shaderkit::STR_HASH);
}

void showErrors(const QStringList& errors)
{
  foreach (const QString& err, errors) {
    fprintf(stderr, "%s\n", err.toUtf8().data());
  }
}

Commands parseCmd(int argc, char* argv[])
{
  Commands cmds;
  for (int i = 1; i < argc; ++i) {
    QString arg(argv[i]);

    if (arg == "--version") {
      cmds.show_version = true;
    } else if (arg == "--help") {
      cmds.show_help = true;
    } else if (arg == "--") {
      for (int j = i + 1; j < argc; ++j)
        cmds.files << argv[j];
      break;
    } else if (arg.startsWith("-")) {
      cmds.errors << "Unknown parameter '" + arg + "'";
    } else {
      cmds.files << arg;
    }
  }

  return cmds;
}

void findPath(QStringList& paths, const QString& root, bool add_root, const QString& name)
{
  QDir dir(root);
  for (int i = 0; i < 5 && dir.exists(); ++i) {
    if (i == 0 && add_root) paths << root;
    if (dir.exists(name)) paths << (dir.path() + "/" + name);
    if (!dir.cdUp()) break;
  }
}

// If we are developing the application and compiling it with shadow building,
// it's tricky to try to determine the actual source directory. Let's see if
// we can parse relevant information from Makefile
void findSources(QStringList& paths, const QString& root, const QString& name)
{
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
              findPath(paths, dir.path() + "/" + r.cap(1), false, name);
            } else {
              findPath(paths, r.cap(1), false, name);
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

int main(int argc, char* argv[])
{
#ifndef _WIN32
  if (argc == 4 && std::string(argv[1]) == "--sandbox-compiler") {
    return SandboxCompiler::run(argc, argv);
  }
#endif

  qRegisterMetaType<MaterialPtr>("MaterialPtr");

#ifdef _WIN32
  // For installer, do not (un)install running application
  CreateMutexA(0, false, "ShaderkitGlobalMutex");
#endif

  QApplication app(argc, argv);
  app.setApplicationName("Shaderkit");
  app.setApplicationVersion(Shaderkit::STR);

  auto cmds = parseCmd(argc, argv);

  if (cmds.show_help) {
    showHelp();
    return 0;
  } else if (cmds.show_version) {
    showVersion();
    return 0;
  } else if (!cmds.errors.isEmpty()) {
    showErrors(cmds.errors);
    return 1;
  }

  //App app(argc, argv);
  // glutInit(&argc, argv);
  Log log;
  Log::info("Shaderkit %s", Shaderkit::STR_HASH);

  QFileInfo bin(QCoreApplication::applicationFilePath());

#ifndef _WIN32
  SandboxCompiler init(bin.absoluteFilePath());
#endif

  if (QDir(bin.absolutePath()).exists()) {
    app.addLibraryPath(bin.absolutePath() + "/../plugins");
  }

  {
    QSettings settings("HKEY_LOCAL_MACHINE\\Software\\Shaderkit",
                       QSettings::NativeFormat);
    QString root = settings.value("Root").toString();
    if (!root.isEmpty())
      app.addLibraryPath(root + "/plugins");
  }

  ShaderDB db;

  QStringList paths[2];
  const char * names[] = {"examples", "template-shaders"};
  for (int i = 0; i < 2; ++i) {
    findPath(paths[i], QDir::currentPath(), true, names[i]);
    findSources(paths[i], QDir::currentPath(), names[i]);

    if (QDir(bin.absolutePath()).exists()) {
      findPath(paths[i], bin.absolutePath(), true, names[i]);
      findPath(paths[i], bin.absolutePath() + "/../share/shaderkit", true, names[i]);
      findSources(paths[i], bin.absolutePath(), names[i]);
    } else {
#ifdef INSTALL_PREFIX
#define TO_STR(s) #s
      findPath(paths[i], TO_STR(INSTALL_PREFIX)"/share/shaderkit", true, names[i]);
#endif
    }

#ifdef _WIN32
    QSettings settings("HKEY_LOCAL_MACHINE\\Software\\Shaderkit",
                       QSettings::NativeFormat);
    QString root = settings.value("Root").toString();
    if (!root.isEmpty())
      findPath(paths[i], root, true, names[i]);
#endif

#ifdef _WIN32
    QString tmp = QDesktopServices::storageLocation(QDesktopServices::DataLocation) +
        "/shaderkit/" + names[i];
#else
    // On linux QDesktopServices returns funny ~/.local/share/data -directory,
    // that seems to be nonstandard and weird
    QString tmp = QDir::homePath() + "/.config/shaderkit/" + names[i];
#endif
    if (QDir(tmp).exists())
      paths[i] << tmp;
  }

  for (const auto & path: paths[0])
    db.addPath(path);

  for (const auto & path: paths[1])
    TemplateBuilder::addDefaultPath(path);

  {
#ifdef _WIN32
    QString tmp = QDesktopServices::storageLocation(QDesktopServices::DataLocation) +
        "/shaderkit/shaderdb";
#else
    QString tmp = QDir::homePath() + "/.config/shaderkit/shaderdb";
#endif
    if (QDir(tmp).exists())
      db.addPath(tmp, true);

    const char* env = getenv("SHADERKIT_EXAMPLES");
    if (env && *env)
      db.addPath(env, true);

    env = getenv("SHADERKIT_TEMPLATE_SHADERS");
    if (env && *env)
      TemplateBuilder::addDefaultPath(env);
  }

  ResourceLocator rl;
  rl.setPath("db", db.defaultPath());

  int ret = 0;
  {
    MainWindow window;

    if (!cmds.files.isEmpty()) {
      MetaInfo scene_info;
      QList<TexturePtr> textures;
      MaterialPtr mat;
      QStringList imports;
      ScenePtr scene;

      std::shared_ptr<ObjImporter> importer;

      foreach (QString filename, cmds.files) {
        if (MetaInfo::ping(filename, scene_info)) {
          if (scene) {
            Log::error("Can open only one scene at a time, %s ignored", filename.toUtf8().data());
            continue;
          }
          scene = Scene::load(filename, Scene::Ok);
          if (scene) continue;
        }
        QFileInfo fi(filename);

        if (fi.size() > 0 && fi.size() < 100000) {
          Shader::Type glsl_type = GLSLParser::detect(filename);

          if (glsl_type != Shader::Unknown) {
            if (!mat)
              mat.reset(new Material("Material"));

            mat->prog()->addShader(filename, glsl_type);
            continue;
          }
        }

        QByteArray format = QImageReader::imageFormat(filename);
        if (!format.isEmpty()) {
          TextureFile* tf = new TextureFile(fi.baseName());
          tf->setFilename(filename);
          textures << TexturePtr(tf);
          continue;
        }

        if (!importer)
          importer.reset(new ObjImporter());

        if (importer->ping(filename)) {
          imports << filename;
          continue;
        }

        Log::error("Couldn't detect file type: %s",  filename.toUtf8().data());
      }

      // We were given some random (non-project)files at the command line,
      // start a new project and add all the files there
      if (!scene)
        scene = ShaderDB::newLocalScene(cmds.files.size() == 1
                                        ? QFileInfo(cmds.files[0]).baseName()
                                        : "Unknown project");

      foreach (auto t, textures) scene->add(t);
      if (mat) scene->add(mat);

      foreach (auto i, imports) {
        ImporterWizard* w = new ImporterWizard(scene);
        w->import(i);
      }

      MainWindow::instance().openScene(scene);
      app.exec();
    }

    if (window.loadedScenes() == 0) {
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
