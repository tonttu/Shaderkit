/**
 * Copyright 2010,2011 Riku Palomäki.
 * This file is part of GLSL Lab.
 *
 * GLSL Lab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3 as
 * published by the Free Software Foundation.
 *
 * GLSL Lab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GLSL Lab.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "project.hpp"
#include "mainwindow.hpp"
#include "editor.hpp"
#include "shader/program.hpp"
#include "shader/shader.hpp"
#include "scene.hpp"
#include "properties.hpp"
#include "render_pass_properties.hpp"

#include <parser.h>
#include <serializer.h>

#include <set>

Project::Project(MainWindow& main_window, QString filename)
  : m_main_window(main_window), m_filename(filename) {}

Project::~Project() {
}

ScenePtr Project::load(const QString& filename) {
  ScenePtr scene;

  QDir dir(filename);
  QString root;
  if (dir.cdUp())
    root = dir.canonicalPath();
  if (root.isEmpty())
    return scene;

  QJson::Parser parser;
  bool ok;
  QFile file(filename);
  QVariant data = parser.parse(&file, &ok);
  if (ok) {
    scene.reset(new Scene(filename));
    scene->setRoot(root);
    scene->load(data.toMap());
  }
  return scene;
}

bool Project::save(const QString& filename) {
  if (!m_active_scene)
    return false;
  QJson::Serializer serializer;
  QFile file(filename);
  // serializer.serialize(QVariant, QIODevice* io, bool* ok ) uses QDataStream
  // that isn't what we want.
  const QByteArray str = serializer.serialize(m_active_scene->save());
  if (!str.isNull() && file.open(QIODevice::WriteOnly)) {
    file.write(str);
    return true;
  }
  return false;
}

void Project::codeChanged(Editor& editor) {
  if (editor.sync()) {
    QList<ShaderPtr> lst = m_active_scene->shadersByFilename(editor.filename());
    if (lst.empty()) return;
    QString plain = editor.toPlainText();
    foreach (ShaderPtr s, lst)
      s->loadSrc(plain);
  }
}

/// @todo is this needed?
/*void Project::openShader(ShaderPtr shader) {
  Editor* editor = findEditor(shader);
  if (!editor) {
    editor = m_main_window.createEditor(shader);
    connect(editor, SIGNAL(codeChanged(Editor&)), this, SLOT(codeChanged(Editor&)));
  } else {
    m_main_window.activateEditor(editor);
  }
}*/

void Project::addShader(ShaderPtr shader) {
  Watcher::instance().add(this, shader->filename());
/*  FileList::instance().update(shader);
  connect(&FileList::instance(), SIGNAL(openFile(ShaderPtr)),
          this, SLOT(openShader(ShaderPtr)));*/
}

Editor* Project::findEditor(ShaderPtr shader) {
  QList<MultiEditor*> editors = m_main_window.editors();
  for (int i = 0; i < editors.size(); ++i) {
    Editor* e = editors[i]->editor(shader);
    if (e) return e;
  }
  return 0;
}

Editor* Project::findEditor(const QString& filename) {
  /*QList<Editor*> editors = m_main_window.editors();
  for (int i = 0; i < editors.size(); ++i) {
    if (editors[i]->shader()->filename() == filename) return editors[i];
  }*/
  return 0;
}

void Project::shaderCompiled(ShaderPtr shader, ShaderError::List errors) {
  Editor* editor = findEditor(shader);

  if (editor) {
    editor->clearErrors();
    for (int i = 0; i < errors.size(); ++i) {
      editor->compileError(errors[i]);
    }
  }

  m_main_window.shaderCompiled(shader, errors);
}

void Project::linked(ProgramPtr, ShaderError::List errors) {
  m_main_window.shaderCompiled(ShaderPtr(), errors);
}

void Project::setScene(ScenePtr scene) {
  if (m_active_scene) {
    foreach (ProgramPtr prog, m_active_scene->programs()) {
      disconnect(prog.get(), SIGNAL(shaderCompiled(ShaderPtr, ShaderError::List)),
                 this, SLOT(shaderCompiled(ShaderPtr, ShaderError::List)));
      disconnect(prog.get(), SIGNAL(linked(ProgramPtr, ShaderError::List)),
                 this, SLOT(linked(ProgramPtr, ShaderError::List)));

      /*foreach (ShaderPtr shader, prog->shaders()) {
        FileList::instance().remove(shader);
      }*/
    }

    foreach (RenderPassPtr p, m_active_scene->renderPasses()) {
      RenderPassProperties::instance().remove(p);
    }
  }

  m_active_scene = scene;

  foreach (ProgramPtr prog, scene->programs()) {
    connect(prog.get(), SIGNAL(shaderCompiled(ShaderPtr, ShaderError::List)),
            this, SLOT(shaderCompiled(ShaderPtr, ShaderError::List)));
    connect(prog.get(), SIGNAL(linked(ProgramPtr, ShaderError::List)),
            this, SLOT(linked(ProgramPtr, ShaderError::List)));

    foreach (ShaderPtr shader, prog->shaders()) {
      addShader(shader);
    }
  }

  emit sceneChanged(scene);
}

void Project::fileUpdated(const QString& filename) {
  Editor* editor = findEditor(filename);
  // If there is an editor, it is a job for it to handle the reloading
  if (editor) {
    editor->fileUpdated(filename);
  } else {
    QList<ShaderPtr> lst = m_active_scene->shadersByFilename(filename);
    foreach (ShaderPtr s, lst)
      s->loadFile(filename);
  }
}

QString Project::filename() const {
  return m_filename;
}
