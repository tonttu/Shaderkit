#include "limbo_dialog.hpp"
#include "ui_limbo_dialog.h"

#include "shaderdb/metainfo.hpp"

#include "scene.hpp"
#include "save_project.hpp"
#include "mainwindow.hpp"

#include <QFileInfo>
#include <QDir>

LimboDialog::LimboDialog(const QString& filename, ScenePtr scene, const MetaInfo& meta, bool& cont)
  : m_filename(filename), m_meta(meta), m_done(false), m_cont(cont), m_scene(scene),
    m_ui(new Ui::LimboDialog) {

  assert(m_scene);

  m_ui->setupUi(this);
  m_ui->name->setText(QString("Project \"%1\" (%2)").arg(meta.name, filename));
  m_ui->timestamp->setText("Last modified " + QFileInfo(filename).lastModified().toString());

  setAttribute(Qt::WA_DeleteOnClose);

  if (m_scene) {
    QString path = QFileInfo(filename).absolutePath();
    foreach (QString file, m_scene->filenames()) {
      if (file.startsWith(path)) {
        m_localFiles << file;
        file = file.mid(path.size()+1);
      }
      m_ui->files->addItem(file);
    }

    m_ui->description->setText(
          QString("<ul><li>%1 Render passes</li><li>%2 Textures</li>"
                  "<li>%3 Cameras</li><li>%4 Lights</li><li>"
                  "%5 Objects</li><li>%6 Materials</li><li>%7 Shaders</li></ul>").arg(
            m_scene->renderPasses().size()).arg(m_scene->textures().size()).arg(
            m_scene->cameras().size()).arg(m_scene->lights().size()).arg(
            m_scene->objects().size()).arg(m_scene->materials().size()).arg(
            m_scene->shaders().size()));
  } else {
    m_ui->description->setText(QString("<i>Failed to open %1</i>").arg(filename));
    m_ui->files->addItem(filename);
  }

  m_ui->sure_label->hide();
  m_ui->no_btn->hide();
  m_ui->yes_btn->hide();

  connect(m_ui->open_btn, SIGNAL(clicked()), this, SLOT(open()));
  connect(m_ui->save_as, SIGNAL(clicked()), this, SLOT(save()));
  connect(m_ui->discard_btn, SIGNAL(clicked()), this, SLOT(maybeDiscard()));
  connect(m_ui->not_now_btn, SIGNAL(clicked()), this, SLOT(notNow()));
  connect(m_ui->yes_btn, SIGNAL(clicked()), this, SLOT(discard()));
  connect(m_ui->no_btn, SIGNAL(clicked()), this, SLOT(abortDiscard()));
}

LimboDialog::~LimboDialog() {
  delete m_ui;
}

void LimboDialog::closeEvent(QCloseEvent* event) {
  if (!m_done) notNow();
}

void LimboDialog::open() {
  m_cont = false;
  MainWindow::instance().openScene(m_scene);
  close();
}

void LimboDialog::save() {
  if (SaveProject::save(m_scene)) {
    QSet<QString> newFiles = m_scene->filenames();
    foreach (const QString& file, m_localFiles - newFiles)
      QFile::remove(file);
    QDir().rmdir(QFileInfo(m_filename).absolutePath());
    close();
  }
}

void LimboDialog::maybeDiscard() {
  m_ui->open_btn->setDisabled(true);
  m_ui->save_as->setDisabled(true);
  m_ui->discard_btn->setDisabled(true);
  m_ui->not_now_btn->setDisabled(true);

  m_ui->sure_label->show();
  m_ui->no_btn->show();
  m_ui->yes_btn->show();
}

void LimboDialog::notNow() {
  m_cont = false;
  close();
}

void LimboDialog::discard() {
  foreach (const QString& file, m_localFiles)
    QFile::remove(file);
  QDir().rmdir(QFileInfo(m_filename).absolutePath());
  close();
}

void LimboDialog::abortDiscard() {
  m_ui->sure_label->hide();
  m_ui->no_btn->hide();
  m_ui->yes_btn->hide();

  m_ui->open_btn->setEnabled(true);
  m_ui->save_as->setEnabled(true);
  m_ui->discard_btn->setEnabled(true);
  m_ui->not_now_btn->setEnabled(true);
}
