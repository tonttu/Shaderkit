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

#include "gui/render_pass_list.hpp"
#include "ui_render_pass_list.h"

#include "core/renderpass.hpp"

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
