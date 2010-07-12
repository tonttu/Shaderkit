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

#include "project.hpp"
#include "mainwindow.hpp"
#include "editor.hpp"
#include "shader/program.hpp"
#include "shader/shader.hpp"
#include "scene.hpp"
#include "properties.hpp"

#include <qjson/parser.h>

#include <set>

Project::Project(MainWindow& main_window) : m_main_window(main_window) {}

ScenePtr Project::load(const QString& filename) {
  ScenePtr scene;
  QJson::Parser parser;
  bool ok;
  QFile file(filename);
  QVariant data = parser.parse(&file, &ok);
  if (ok) {
    scene.reset(new Scene);
    scene->load(data.toMap());
  }
  return scene;
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

void Project::addShader(ShaderPtr shader) {
  Watcher::instance().add(this, shader->filename());
  Editor* editor = m_main_window.createEditor(shader);
  connect(editor, SIGNAL(codeChanged(Editor&)), this, SLOT(codeChanged(Editor&)));
  FileList::instance().update(shader->filename());
}

Editor* Project::findEditor(ShaderPtr shader) {
  QList<Editor*> editors = m_main_window.editors();
  for (int i = 0; i < editors.size(); ++i) {
    if (editors[i]->shader() == shader) return editors[i];
  }
  return 0;
}

Editor* Project::findEditor(const QString& filename) {
  QList<Editor*> editors = m_main_window.editors();
  for (int i = 0; i < editors.size(); ++i) {
    if (editors[i]->shader()->filename() == filename) return editors[i];
  }
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

void Project::setScene(ScenePtr scene) {
  m_active_scene = scene;

  std::map<QString, ProgramPtr> prog = scene->shaders();
  for (std::map<QString, ProgramPtr>::iterator it = prog.begin(); it != prog.end(); ++it) {
    connect(it->second.get(), SIGNAL(shaderCompiled(ShaderPtr, ShaderError::List)),
            this, SLOT(shaderCompiled(ShaderPtr, ShaderError::List)));

    GLProgram::Shaders shaders = it->second->shaders();
    for (GLProgram::Shaders::iterator it2 = shaders.begin(); it2 != shaders.end(); ++it2) {
      if (!findEditor(*it2) && !findEditor((*it2)->filename())) {
        addShader(*it2);
      }
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
