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

#include "gui/importer_wizard.hpp"
#include "ui_importer_wizard.h"

#include "core/object3d.hpp"
#include "core/scene.hpp"
#include "core/model.hpp"

#include "glm/gtx/transform.hpp"

#include <QFileInfo>
#include <QSettings>
#include <QFileDialog>

namespace
{
  QTreeWidgetItem* newItem(QTreeWidgetItem* p, const QMap<QString, QString>::const_iterator& it)
  {
    QTreeWidgetItem* item = new QTreeWidgetItem(p, QStringList() << it.key() << it.value());
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(0, Qt::Checked);
    return item;
  }
}

namespace Shaderkit
{

  ImporterWizard::ImporterWizard(ScenePtr scene, QWidget* parent)
    : QWizard(parent),
      m_first(true),
      m_ui(new Ui::ImporterWizard),
      m_scene(scene),
      m_objects(0),
      m_models(0),
      m_animations(0),
      m_cameras(0),
      m_lights(0),
      m_materials(0),
      m_textures(0)
  {
    m_ui->setupUi(this);
    setButtonText(NextButton, tr("Load file >"));
    button(NextButton)->setDisabled(true);

    setAttribute(Qt::WA_DeleteOnClose);

    connect(m_ui->browse, SIGNAL(clicked()), this, SLOT(browse()));
    connect(m_ui->filename, SIGNAL(textChanged(QString)), this, SLOT(fileChanged(QString)));
    connect(this, SIGNAL(currentIdChanged(int)), SLOT(changed(int)));

    connect(m_ui->manualDrag, SIGNAL(toggled(bool)), this, SLOT(manualDragToggled(bool)));

    QSettings settings("Shaderkit", "Shaderkit");
    m_ui->autoScale->setChecked(settings.value("import/auto_scale", true).toBool());
    m_ui->manualDrag->setChecked(settings.value("import/manual_drag", false).toBool());
  }

  ImporterWizard::~ImporterWizard()
  {
    delete m_ui;
  }

  void ImporterWizard::setVisible(bool visible)
  {
    QWizard::setVisible(visible);
    if (visible && m_first) {
      m_first = false;
      if (m_ui->filename->text().isEmpty())
        browse();
    }
  }

  void ImporterWizard::import(const QString& filename)
  {
    m_ui->filename->setText(filename);
    show();
    setWindowState(Qt::WindowActive);
  }

  void ImporterWizard::browse()
  {
    QSettings settings("Shaderkit", "Shaderkit");
    QString dir = settings.value("history/last_import_dir",
                                 settings.value("history/last_dir",
                                     QVariant(QDir::currentPath()))).toString();
    QString filter = tr("Assets (%1)").arg(m_importer.extensionList());
    QString file = QFileDialog::getOpenFileName(this, tr("Import an asset"), dir, filter);
    if (!file.isEmpty()) {
      QFileInfo fi(file);
      settings.setValue("history/last_import_dir", fi.absolutePath());
      m_ui->filename->setText(file);
    }
  }

  void ImporterWizard::changed(int page)
  {
    if (page == 1) {
      m_import.options["optimize cache locality"] = m_ui->cache_locality->isChecked();
      m_import.options["merge materials"] = m_ui->merge_materials->isChecked();
      m_import.options["find degenerates"] = m_ui->find_degenerates->isChecked();
      m_import.options["fix normals"] = m_ui->fix_normals->isChecked();
      m_import.options["optimize"] = m_ui->optimize->isChecked();
      m_import.setFilename(m_ui->filename->text());
      if (m_importer.readFile(m_import.filename(), m_import.options)) {
        ObjImporter::SceneInfo si = m_importer.analyze();
        load(si);
      } else {
        /// @todo show error message somewhere
      }
    }
  }

  void ImporterWizard::fileChanged(QString name)
  {
    bool ok = false;
    if (!name.isEmpty()) {
      QFileInfo fi(name);
      ok = fi.isReadable();
    }
    button(NextButton)->setEnabled(ok);
  }

  void ImporterWizard::manualDragToggled(bool s)
  {
    if (s)
      m_ui->autoScale->setChecked(false);
    m_ui->autoScale->setEnabled(!s);
  }

  void ImporterWizard::load(ObjImporter::SceneInfo& si)
  {
    QTreeWidget* t = m_ui->tree;
    t->clear();

    QList<QTreeWidgetItem*> items;
    m_objects = new QTreeWidgetItem(t, QStringList() << "Objects");
    m_models = new QTreeWidgetItem(t, QStringList() << "Models");
    m_animations = new QTreeWidgetItem(t, QStringList() << "Animations");
    m_cameras = new QTreeWidgetItem(t, QStringList() << "Cameras");
    m_lights = new QTreeWidgetItem(t, QStringList() << "Lights");
    m_materials = new QTreeWidgetItem(t, QStringList() << "Materials");
    m_textures = new QTreeWidgetItem(t, QStringList() << "Textures");

    items << m_objects << m_models << m_animations << m_cameras << m_lights << m_materials << m_textures;
    t->addTopLevelItems(items);

    QMap<QString, QString>::const_iterator it;

    for (it = si.objects.begin(); it != si.objects.end(); ++it) {
      newItem(m_objects, it);
      newItem(m_models, it);
    }
    for (it = si.animations.begin(); it != si.animations.end(); ++it) {
      QTreeWidgetItem* x = newItem(m_animations, it);
      x->setCheckState(0, Qt::Unchecked);
      x->setDisabled(true);
    }
    for (it = si.cameras.begin(); it != si.cameras.end(); ++it) {
      QTreeWidgetItem* x = newItem(m_animations, it);
      x->setCheckState(0, Qt::Unchecked);
      x->setDisabled(true);
    }
    for (it = si.lights.begin(); it != si.lights.end(); ++it) {
      QTreeWidgetItem* x = newItem(m_lights, it);
      x->setCheckState(0, Qt::Unchecked);
      x->setDisabled(true);
    }
    for (it = si.materials.begin(); it != si.materials.end(); ++it) {
      newItem(m_materials, it);
    }
    for (it = si.textures.begin(); it != si.textures.end(); ++it) {
      newItem(m_textures, it);
    }

    foreach (QTreeWidgetItem* i, items) {
      i->setFirstColumnSpanned(true);
      i->setExpanded(true);
      if (i->childCount() == 0) i->setDisabled(true);
    }

    t->resizeColumnToContents(0);
    t->resizeColumnToContents(1);
  }

  void ImporterWizard::done(int result)
  {
    QWizard::done(result);

    if (!result || !m_objects) return;

    QSettings settings("Shaderkit", "Shaderkit");
    settings.setValue("import/auto_scale", m_ui->autoScale->isChecked());
    settings.setValue("import/manual_drag", m_ui->manualDrag->isChecked());

    QList<QTreeWidgetItem*> items;
    QList<QSet<QString>*> filters;

    items << m_objects << m_models << m_animations << m_cameras << m_lights
          << m_materials << m_textures;
    filters << &m_import.filter.objects << &m_import.filter.models
            << &m_import.filter.animations << &m_import.filter.cameras
            << &m_import.filter.lights << &m_import.filter.materials
            << &m_import.filter.textures;

    for (int i = 0; i < items.size(); ++i) {
      int c = items[i]->childCount();
      for (int j = 0; j < c; ++j) {
        QTreeWidgetItem* item = items[i]->child(j);
        if (item->checkState(0) == Qt::Checked) {
          (*filters[i]) << item->text(0);
        }
      }
    }

    ObjImporter::Scene scene = m_importer.load(m_import.filter);
    if (m_ui->autoScale->isChecked()) {
      const float target_size = 50.0f;

      const BBox3& bbox = scene.node->bbox();
      auto vec3 = bbox.size();
      float m = std::max(std::max(vec3.x, vec3.y), vec3.z);
      if (m > std::numeric_limits<float>::epsilon()) {
        float scale = target_size/m;
        /// @todo should serialize nodes
        /// scene.node->transform = Eigen::Scaling(scale) * scene.node->transform;
        foreach (ObjectPtr obj, scene.objects)
          obj->setTransform(glm::scale(glm::vec3(scale)) * obj->transform());
      }
    }

    m_scene->merge(m_import, scene);
  }

} // namespace Shaderkit
