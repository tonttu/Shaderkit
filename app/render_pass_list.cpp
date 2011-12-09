#include "render_pass_list.hpp"
#include "ui_render_pass_list.h"

#include "renderpass.hpp"

RenderPassList::RenderPassList(QList<RenderPassPtr> list, QWidget* parent)
  : QDialog(parent),
    m_list(list),
    m_ui(new Ui::RenderPassList) {
  m_ui->setupUi(this);
  updateList();
  /// @todo maybe close or something if the render pass list changes?
}

RenderPassList::~RenderPassList() {
  delete m_ui;
}

QList<RenderPassPtr> RenderPassList::orderedList() const {
  QList<RenderPassPtr> res;
  for (int i = 0; i < m_ui->list->count(); ++i) {
    bool ok = false;
    int idx = m_ui->list->item(i)->data(Qt::UserRole).toInt(&ok);
    if (ok && idx >= 0 && idx < m_list.size())
      res << m_list[idx];
  }
  return res;
}

void RenderPassList::updateList() {
  m_ui->list->clear();
  int i = 0;
  foreach (RenderPassPtr pass, m_list) {
    QListWidgetItem* item = new QListWidgetItem(pass->icon(), pass->name());
    item->setData(Qt::UserRole, i++);
    m_ui->list->addItem(item);
  }
}
