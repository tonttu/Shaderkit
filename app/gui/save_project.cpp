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

#include "gui/save_project.hpp"
#include "ui_save_project.h"
#include "core/scene.hpp"
#include "gl/program.hpp"
#include "gl/texture.hpp"

#include <QDir>
#include <QKeyEvent>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>

namespace
{
  bool dirHasFiles(const QString& path)
  {
    QDir dir(path);
    if (!dir.exists()) return false;
    return !dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot).isEmpty();
  }
}

namespace Shaderkit
{

  SaveProject::SaveProject(QWidget* parent)
    : QDialog(parent),
      m_ui(new Ui::SaveProject)
  {
    m_ui->setupUi(this);
    filenameChanged("");
    connect(m_ui->filename, SIGNAL(textChanged(QString)), this, SLOT(filenameChanged(QString)));
    connect(m_ui->browse, SIGNAL(clicked()), this, SLOT(browse()));
  }

  SaveProject::~SaveProject()
  {
    delete m_ui;
  }

  void SaveProject::keyPressEvent(QKeyEvent* e)
  {
    if (m_ui->filename->hasFocus() &&
        (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)) {

      QList<QPushButton*> list = qFindChildren<QPushButton*>(this);
      for (int i = 0; i < list.size(); ++i) {
        QPushButton* pb = list.at(i);
        if (pb->isDefault()) {
          pb->setFocus();
          break;
        }
      }
      e->accept();
    } else {
      QDialog::keyPressEvent(e);
    }
  }

  bool SaveProject::save(ScenePtr scene)
  {
    if (!scene) return false;

    // type, name, path (full)
    QList<QPair<QIcon, QStringList>> files;
    QSet<QString> included;

    QFileInfo fi;
    foreach (ProgramPtr p, scene->materialPrograms()) {
      foreach (ShaderPtr s, p->shaders()) {
        fi.setFile(s->filename());
        if (included.contains(s->filename()))
          continue;
        included << s->filename();
        files << qMakePair(s->icon(), QStringList() << "Shader" << fi.fileName()
                           << s->filename() << "shader");
      }
    }

    auto imp = scene->imports();
    for (auto it = imp.begin(); it != imp.end(); ++it) {
      fi.setFile(it->filename());
      if (included.contains(it->filename()))
        continue;
      included << it->filename();
      /// @todo replace "Imported file" with "Imported .3ds" etc
      files << qMakePair(it->icon(), QStringList() << "Imported file" << fi.fileName()
                         << it->filename() << "import");
    }

    foreach (auto t, scene->textures()) {
      TextureFile* tf = dynamic_cast<TextureFile*>(t.get());
      if (!tf) continue;
      fi.setFile(tf->filename());
      if (included.contains(tf->filename()))
        continue;
      included << tf->filename();
      files << qMakePair(tf->icon(), QStringList() << "Texture" << QString("%1 (%2)").
                         arg(tf->name(), fi.fileName()) << tf->filename() << "texture");
    }

    SaveProject save;
    save.m_ui->list->setRowCount(files.size());
    int r = 0;
    foreach (auto f, files) {
      QTableWidgetItem* item = new QTableWidgetItem(f.second[0]);
      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
      item->setIcon(f.first);
      item->setCheckState(Qt::Checked);
      item->setData(Qt::UserRole, f.second[2]);
      save.m_ui->list->setItem(r, 0, item);

      item = new QTableWidgetItem(f.second[1]);
      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
      item->setToolTip(f.second[2]);
      item->setData(Qt::UserRole, f.second[3]);
      save.m_ui->list->setItem(r, 1, item);
      ++r;
    }

    save.m_ui->list->resizeColumnsToContents();

    QSettings settings("Shaderkit", "Shaderkit");
    QString dir = settings.value("history/last_projects_dir", QVariant(QDir::currentPath())).toString();
    while (dir.endsWith("/")) dir.chop(1);
    QString n = scene->metainfo().name;
    n.replace("/", "-");
    save.m_ui->filename->setText(QString("%1/%2/%2.shaderkit").arg(dir, n));

    if (save.exec() == Accepted) {
      QString file = save.m_ui->filename->text();
      QString dir = QFileInfo(file).path();
      if (!QDir().exists(dir) && !QDir().mkpath(dir)) {
        Log::error("Failed to create directory %s", dir.toUtf8().data());
        return false;
      }

      for (int r = 0; r < save.m_ui->list->rowCount(); ++r) {
        QTableWidgetItem* item = save.m_ui->list->item(r, 0);
        if (item->checkState() != Qt::Checked) continue;
        QString full = item->data(Qt::UserRole).toString();
        QString full2 = dir + "/" + QFileInfo(full).fileName();
        QString type = save.m_ui->list->item(r, 1)->data(Qt::UserRole).toString();

        if (type == "shader") {
          if (!scene->renameShaderFile(full, full2, true))
            return false;
        } else if (type == "texture") {
          if (!scene->renameTextureFile(full, full2, true))
            return false;
        } else if (type == "import") {
          if (!scene->renameImportFile(full, full2, true))
            return false;
        } else {
          assert(false);
        }
      }

      settings.setValue("history/last_projects_dir", QFileInfo(dir).path());
      QString err;
      bool ok = scene->save(file, err);
      if (!ok) QMessageBox::warning(&save, "Failed to save project", err);
      return ok;
    }
    return false;
  }

  void SaveProject::filenameChanged(QString name)
  {
    bool nowarn = true;
    bool cansave = true;
    if (!name.isEmpty()) {
      nowarn = false;
      QFileInfo fi(name);
      if (fi.exists()) {
        m_ui->warning->setText("Warning: File already exists");
      } else if (dirHasFiles(fi.path())) {
        m_ui->warning->setText(QString("Warning: Project directory %1 already exists and is not empty").
                               arg(fi.path()));
      } else if (fi.fileName().isEmpty()) {
        m_ui->warning->setText("Error: Filename is empty");
        cansave = false;
      } else if (!fi.fileName().endsWith(".shaderkit")) {
        m_ui->warning->setText(QString("Warning: Filename %1 doesn't use file extension \".shaderkit\"").
                               arg(fi.fileName()));
      } else nowarn = true;
    }
    m_ui->warning->setHidden(nowarn);
    m_ui->warning_icon->setHidden(nowarn);
    m_ui->buttonBox->button(QDialogButtonBox::Save)->setDisabled(!cansave || name.isEmpty());
  }

  void SaveProject::browse()
  {
    QFileInfo fi(m_ui->filename->text());
    QFileDialog dialog(this, tr("Save project file as"), fi.path());
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("shaderkit");
    if (dialog.exec() == Accepted && dialog.selectedFiles().size() == 1) {
      m_ui->filename->setText(dialog.selectedFiles()[0]);
    }
  }

} // namespace Shaderkit
