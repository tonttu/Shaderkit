#include "camera_editor.hpp"
#include "ui_camera_editor.h"

#include "renderpass.hpp"
#include "mainwindow.hpp"
#include "scene.hpp"
#include "camera.hpp"
#include "viewport.hpp"

CameraEditor::CameraEditor(QWidget* parent, RenderPassPtr render_pass)
  : QDialog(parent),
    m_ui(new Ui::CameraEditor),
    m_render_pass(render_pass) {
  m_ui->setupUi(this);

  setAttribute(Qt::WA_DeleteOnClose);

  Viewport* view = MainWindow::instance().mainViewport();
  if (view) {
    /// @todo should somehow force this to use this render pass
    //m_ui->toolbar->addAction(view->actionNewCamera());
  }
  m_ui->toolbar->addSeparator();
  m_duplicate = m_ui->toolbar->addAction(QIcon(":/icons/duplicate.png"),
                                         "Duplicate camera", this, SLOT(duplicate()));
  m_duplicate->setEnabled(false);
  m_ui->toolbar->addSeparator();
  m_destroy = m_ui->toolbar->addAction(QIcon(":/icons/delete.png"),
                                       "Delete camera", this, SLOT(remove()));
  m_destroy->setEnabled(false);

  updated(m_render_pass);
  if (m_render_pass) {
    setWindowTitle(QString("Cameras for render pass %1").arg(m_render_pass->name()));
    connect(m_render_pass.get(), SIGNAL(changed(RenderPassPtr)),
            this, SLOT(updated(RenderPassPtr)));
    connect(m_ui->list, SIGNAL(itemChanged(QListWidgetItem*)),
            this, SLOT(itemChanged(QListWidgetItem*)));
  } else {
    setWindowTitle("Camera browser");
  }
  connect(m_ui->list, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));

  connect(m_ui->name, SIGNAL(editingFinished()), this, SLOT(nameChanged()));
  connect(m_ui->type, SIGNAL(currentIndexChanged(int)), this, SLOT(typeChanged()));
  connect(m_ui->locx, SIGNAL(editingFinished()), this, SLOT(locChanged()));
  connect(m_ui->locy, SIGNAL(editingFinished()), this, SLOT(locChanged()));
  connect(m_ui->locz, SIGNAL(editingFinished()), this, SLOT(locChanged()));
  connect(m_ui->targetx, SIGNAL(editingFinished()), this, SLOT(targetChanged()));
  connect(m_ui->targety, SIGNAL(editingFinished()), this, SLOT(targetChanged()));
  connect(m_ui->targetx, SIGNAL(editingFinished()), this, SLOT(targetChanged()));
  connect(m_ui->fov, SIGNAL(editingFinished()), this, SLOT(fovChanged()));
  connect(m_ui->near_plane, SIGNAL(editingFinished()), this, SLOT(nearChanged()));
  connect(m_ui->auto_near, SIGNAL(toggled(bool)), this, SLOT(nearChanged()));
  connect(m_ui->far_plane, SIGNAL(editingFinished()), this, SLOT(farChanged()));
  connect(m_ui->auto_far, SIGNAL(toggled(bool)), this, SLOT(farChanged()));

  m_ui->locx->setValidator(new QDoubleValidator(this));
  m_ui->locy->setValidator(new QDoubleValidator(this));
  m_ui->locz->setValidator(new QDoubleValidator(this));
  m_ui->targetx->setValidator(new QDoubleValidator(this));
  m_ui->targety->setValidator(new QDoubleValidator(this));
  m_ui->targetz->setValidator(new QDoubleValidator(this));
  m_ui->fov->setValidator(new QDoubleValidator(this));

  updatePanel();
}

CameraEditor::~CameraEditor() {
  delete m_ui;
}

void CameraEditor::duplicate() {
  /// @todo implement
}

void CameraEditor::remove() {
  /// @todo implement
}

void CameraEditor::updated(RenderPassPtr pass) {
  assert(m_render_pass == pass);

  CameraPtr my_camera = pass->view();

  if (pass)
    disconnect(pass.get(), SIGNAL(changed(RenderPassPtr)),
              this, SLOT(updated(RenderPassPtr)));

  m_ui->list->clear();
  foreach (CameraPtr camera, MainWindow::scene()->cameras()) {
    /// @todo icon
    QListWidgetItem* item = new QListWidgetItem(camera->name(), m_ui->list);
    Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    if (pass) flags |= Qt::ItemIsUserCheckable;
    item->setFlags(flags);
    if (pass) {
      if (my_camera == camera)
        item->setCheckState(Qt::Checked);
      else
        item->setCheckState(Qt::Unchecked);
    }
  }
  if (pass)
    connect(pass.get(), SIGNAL(changed(RenderPassPtr)),
            this, SLOT(updated(RenderPassPtr)));
}

void CameraEditor::selectionChanged() {
  int c = m_ui->list->selectedItems().size();
  m_destroy->setEnabled(c >= 1);
  m_duplicate->setEnabled(c == 1);

  if (m_ui->list->selectedItems().size() == 1)
    m_camera = MainWindow::scene()->camera(m_ui->list->selectedItems()[0]->text());
  else
    m_camera.reset();
  updatePanel();
}

void CameraEditor::updatePanel() {
  if (m_camera) {
    m_ui->name->setText(m_camera->name());
    int idx = m_camera->type() == Camera::Perspective ?
          0 : m_camera->type() == Camera::Ortho ?
          1 : 2;
    m_ui->type->setCurrentIndex(idx);
    if (idx == 0) {
      m_ui->locx->setText(QString::number(m_camera->location().x()));
      m_ui->locy->setText(QString::number(m_camera->location().y()));
      m_ui->locz->setText(QString::number(m_camera->location().z()));
      m_ui->targetx->setText(QString::number(m_camera->target().x()));
      m_ui->targety->setText(QString::number(m_camera->target().y()));
      m_ui->targetz->setText(QString::number(m_camera->target().z()));
      m_ui->fov->setText(QString::number(m_camera->fov()));
      m_ui->near_plane->setText(QString::number(m_camera->near()));
      m_ui->far_plane->setText(QString::number(m_camera->far()));
      m_ui->clip_group->show();
      m_ui->perspective_group->show();
    } else if (idx == 1) {
      /// @todo not implemented
      m_ui->near_plane->setText(QString::number(m_camera->near()));
      m_ui->far_plane->setText(QString::number(m_camera->far()));
      m_ui->clip_group->show();
      m_ui->perspective_group->hide();
    } else if (idx == 2) {
      m_ui->clip_group->hide();
      m_ui->perspective_group->hide();
    }
    m_ui->panel->setEnabled(true);
  } else {
    m_ui->panel->setEnabled(false);
    m_ui->name->setText("");
    m_ui->type->setCurrentIndex(0);
    m_ui->locx->setText("0.0");
    m_ui->locy->setText("0.0");
    m_ui->locz->setText("0.0");
    m_ui->targetx->setText("0.0");
    m_ui->targety->setText("0.0");
    m_ui->targetz->setText("0.0");
    m_ui->fov->setText("0.0");
    m_ui->near_plane->setText("0.0");
    m_ui->far_plane->setText("0.0");
    m_ui->clip_group->hide();
    m_ui->perspective_group->hide();
  }
}

void CameraEditor::itemChanged(QListWidgetItem* item) {
  CameraPtr camera = MainWindow::scene()->camera(item->text());
  if (camera) {
    if (item->checkState() != Qt::Unchecked)
      m_render_pass->setView(camera);
    else if (m_render_pass->view() == camera)
      m_render_pass->setView(CameraPtr());
  }
}

void CameraEditor::nameChanged() {
  /// @todo implement
}

void CameraEditor::typeChanged() {
  if (!m_camera) return;

  Camera::Type t = m_ui->type->currentIndex() == 0 ?
        Camera::Perspective : m_ui->type->currentIndex() == 1 ?
          Camera::Ortho : Camera::Rect;
  m_camera->setType(t);

  updatePanel();
}

void CameraEditor::locChanged() {
  if (!m_camera) return;
  Eigen::Vector3f loc(m_ui->locx->text().toFloat(),
                      m_ui->locy->text().toFloat(),
                      m_ui->locz->text().toFloat());
  m_camera->setLocation(loc);
}

void CameraEditor::targetChanged() {
  if (!m_camera) return;
  Eigen::Vector3f target(m_ui->targetx->text().toFloat(),
                         m_ui->targety->text().toFloat(),
                         m_ui->targetz->text().toFloat());
  m_camera->setTarget(target);
}

void CameraEditor::fovChanged() {
  if (!m_camera) return;
  m_camera->setFov(m_ui->fov->text().toFloat());
}

void CameraEditor::nearChanged() {
  if (!m_camera) return;
  m_camera->setNear(m_ui->near_plane->text().toFloat());
}

void CameraEditor::farChanged() {
  if (!m_camera) return;
  m_camera->setFar(m_ui->far_plane->text().toFloat());
}
