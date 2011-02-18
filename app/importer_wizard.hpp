#ifndef IMPORTER_WIZARD_HPP
#define IMPORTER_WIZARD_HPP

#include "forward.hpp"
#include "obj_importer.hpp"

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

  QTreeWidgetItem* m_objects;
  QTreeWidgetItem* m_models;
  QTreeWidgetItem* m_animations;
  QTreeWidgetItem* m_cameras;
  QTreeWidgetItem* m_lights;
  QTreeWidgetItem* m_materials;
  QTreeWidgetItem* m_textures;
};

#endif // IMPORTER_WIZARD_HPP
