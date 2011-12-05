#include "light_list.hpp"
#include "ui_light_list.h"

#include "renderpass.hpp"
#include "mainwindow.hpp"
#include "scene.hpp"
#include "light.hpp"
#include "viewport.hpp"

LightList::LightList(QWidget* parent, RenderPassPtr render_pass)
  : QDialog(parent),
    m_ui(new Ui::LightList),
    m_render_pass(render_pass) {
  m_ui->setupUi(this);

  setAttribute(Qt::WA_DeleteOnClose);

  Viewport* view = MainWindow::instance().mainViewport();
  if (view) {
    /// @todo should somehow force this to use this render pass
    m_ui->toolbar->addAction(view->actionNewLight());
  }
  m_ui->toolbar->addSeparator();
  m_duplicate = m_ui->toolbar->addAction(QIcon(":/icons/duplicate.png"),
                                         "Duplicate light", this, SLOT(duplicate()));
  m_duplicate->setEnabled(false);
  m_ui->toolbar->addSeparator();
  m_destroy = m_ui->toolbar->addAction(QIcon(":/icons/delete.png"),
                                       "Delete", this, SLOT(remove()));
  m_destroy->setEnabled(false);

  updated(m_render_pass);
  if (m_render_pass) {
    setWindowTitle(QString("Lights for render pass %1").arg(m_render_pass->name()));
    connect(m_render_pass.get(), SIGNAL(changed(RenderPassPtr)),
            this, SLOT(updated(RenderPassPtr)));
    connect(m_ui->list, SIGNAL(itemChanged(QListWidgetItem*)),
            this, SLOT(itemChanged(QListWidgetItem*)));
  } else {
    setWindowTitle("Light browser");
  }
  connect(m_ui->list, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
}

LightList::~LightList() {
  delete m_ui;
}

void LightList::duplicate() {
  /// @todo implement
}

void LightList::remove() {
  /// @todo implement
}

void LightList::updated(RenderPassPtr pass) {
  assert(m_render_pass == pass);

  QSet<LightPtr> my_lights;
  if (pass) my_lights = pass->lights();

  if (pass)
    disconnect(pass.get(), SIGNAL(changed(RenderPassPtr)),
              this, SLOT(updated(RenderPassPtr)));

  m_ui->list->clear();
  foreach (LightPtr light, MainWindow::scene()->lights()) {
    /// @todo icon
    QListWidgetItem* item = new QListWidgetItem(light->name(), m_ui->list);
    Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    if (pass) flags |= Qt::ItemIsUserCheckable;
    item->setFlags(flags);
    if (pass) {
      if (my_lights.contains(light))
        item->setCheckState(Qt::Checked);
      else
        item->setCheckState(Qt::Unchecked);
    }
  }
  if (pass)
    connect(pass.get(), SIGNAL(changed(RenderPassPtr)),
            this, SLOT(updated(RenderPassPtr)));
}

void LightList::selectionChanged() {
  int c = m_ui->list->selectedItems().size();
  m_destroy->setEnabled(c >= 1);
  m_duplicate->setEnabled(c == 1);
}

void LightList::itemChanged(QListWidgetItem* item) {
  LightPtr light = MainWindow::scene()->light(item->text());
  if (light) {
    if (item->checkState() != Qt::Unchecked)
      m_render_pass->add(light);
    else
      m_render_pass->remove(light);
  }
}
