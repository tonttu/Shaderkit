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

#ifndef IMPORTER_WIZARD_HPP
#define IMPORTER_WIZARD_HPP

#include "forward.hpp"
#include "obj_importer.hpp"
#include "scene.hpp"

#include <QWizard>

namespace Ui {
class ImporterWizard;
}
class QTreeWidgetItem;
class ImporterWizard : public QWizard {
  Q_OBJECT

public:
  explicit ImporterWizard(ScenePtr scene, QWidget *parent = 0);
  ~ImporterWizard();

  void setVisible(bool visible);

protected slots:
  void browse();
  void changed(int);
  void fileChanged(QString);

protected:
  void load(ObjImporter::SceneInfo& si);
  void done(int result);

private:
  bool m_first;
  ObjImporter m_importer;
  Ui::ImporterWizard* m_ui;
  ScenePtr m_scene;
  Scene::Import m_import;

  QTreeWidgetItem* m_objects;
  QTreeWidgetItem* m_models;
  QTreeWidgetItem* m_animations;
  QTreeWidgetItem* m_cameras;
  QTreeWidgetItem* m_lights;
  QTreeWidgetItem* m_materials;
  QTreeWidgetItem* m_textures;
};

#endif // IMPORTER_WIZARD_HPP
