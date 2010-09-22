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

#ifndef PROJECT_HPP
#define PROJECT_HPP

#include "forward.hpp"
#include "watcher.hpp"
#include "shader/error.hpp"

#include <QString>

#include <boost/enable_shared_from_this.hpp>

/**
 * One project means a set of scenes where only one can be active at time.
 * Project is basically everything that is stored in the project file, although
 * large part of the functionality is actually implemented in Scene.
 */
class Project : public QObject, public boost::enable_shared_from_this<Project>,
    public Watchable {
  Q_OBJECT

public:
  Project(MainWindow& main_window, QString filename);

  /// Loads a scene from JSON file.
  static ScenePtr load(const QString& filename);

  /// Sets the active scene, connects to all necessary signals, creates editors etc.
  void setScene(ScenePtr scene);

  /// A file was changed on the disk.
  /// Delegates handling the event to right editor, or if no editor is open
  /// with this file, forces the shader to reload the file.
  void fileUpdated(const QString& filename);

  /// Returns the project (*.lab or *.zip) filename
  QString filename() const;

signals:
  void sceneChanged(ScenePtr);

public slots:
  /// Shader code was changed on the editor
  void codeChanged(Editor& editor);
  void shaderCompiled(ShaderPtr shader, ShaderError::List errors);
  void openShader(ShaderPtr shader);

protected:
  /// Creates a new editor for a shader
  void addShader(ShaderPtr shader);

  Editor* findEditor(ShaderPtr shader);
  Editor* findEditor(const QString& filename);

  typedef QMap<QString, ShaderPtr> Files;

  /// Maps filename to shader
  Files m_shaders;

  MainWindow& m_main_window;
  ScenePtr m_active_scene;
  QString m_filename;
};

#endif // PROJECT_HPP
