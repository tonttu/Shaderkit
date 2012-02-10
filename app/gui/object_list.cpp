#include "gui/object_list.hpp"
#include "ui_object_list.h"

#include "core/renderpass.hpp"
#include "gui/mainwindow.hpp"
#include "core/scene.hpp"
#include "core/object3d.hpp"
#include "gui/viewport.hpp"

ObjectList::ObjectList(QWidget* parent, RenderPassPtr render_pass)
  : QDialog(parent),
    m_ui(new Ui::ObjectList),
    m_render_pass(render_pass) {
  m_ui->setupUi(this);

  setAttribute(Qt::WA_DeleteOnClose);

  Viewport* view = MainWindow::instance().mainViewport();
  if (view) {
    /// @todo should somehow force this to use this render pass
    m_ui->toolbar->addAction(view->actionNewObj());
  } else {
    m_ui->toolbar->addAction(QIcon(":/icons/import.png"), "Import...",
                             &MainWindow::instance(), SLOT(import()));
  }
  m_ui->toolbar->addSeparator();
  m_duplicate = m_ui->toolbar->addAction(QIcon(":/icons/duplicate.png"),
                                         "Duplicate object", this, SLOT(duplicate()));
  m_duplicate->setEnabled(false);
  m_ui->toolbar->addSeparator();
  m_destroy = m_ui->toolbar->addAction(QIcon(":/icons/delete.png"),
                                       "Delete", this, SLOT(remove()));
  m_destroy->setEnabled(false);

  updated(m_render_pass);
  if (m_render_pass) {
    setWindowTitle(QString("Objects for render pass %1").arg(m_render_pass->name()));
    connect(m_render_pass.get(), SIGNAL(changed(RenderPassPtr)),
            this, SLOT(updated(RenderPassPtr)));
    connect(m_ui->list, SIGNAL(itemChanged(QListWidgetItem*)),
            this, SLOT(itemChanged(QListWidgetItem*)));
  } else {
    setWindowTitle("Object browser");
  }
  connect(m_ui->list, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
}

ObjectList::~ObjectList() {
  delete m_ui;
}

void ObjectList::duplicate() {
  /// @todo implement
}

void ObjectList::remove() {
  /// @todo implement
}

void ObjectList::updated(RenderPassPtr pass) {
  assert(m_render_pass == pass);

  QSet<ObjectPtr> my_objects;
  if (pass) my_objects = pass->objects();

  if (pass)
    disconnect(pass.get(), SIGNAL(changed(RenderPassPtr)),
              this, SLOT(updated(RenderPassPtr)));

  m_ui->list->clear();
  foreach (ObjectPtr obj, MainWindow::scene()->objects()) {
    /// @todo icon
    QListWidgetItem* item = new QListWidgetItem(obj->name(), m_ui->list);
    Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    if (pass) flags |= Qt::ItemIsUserCheckable;
    item->setFlags(flags);
    if (pass) {
      if (my_objects.contains(obj))
        item->setCheckState(Qt::Checked);
      else
        item->setCheckState(Qt::Unchecked);
    }
  }
  if (pass)
    connect(pass.get(), SIGNAL(changed(RenderPassPtr)),
            this, SLOT(updated(RenderPassPtr)));
}

void ObjectList::selectionChanged() {
  int c = m_ui->list->selectedItems().size();
  m_destroy->setEnabled(c >= 1);
  m_duplicate->setEnabled(c == 1);
}

void ObjectList::itemChanged(QListWidgetItem* item) {
  ObjectPtr obj = MainWindow::scene()->object(item->text());
  if (obj) {
    if (item->checkState() != Qt::Unchecked)
      m_render_pass->add(obj);
    else
      m_render_pass->remove(obj);
  }
}
