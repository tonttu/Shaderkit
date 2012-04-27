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

#ifndef RENDER_PASS_LIST_HPP
#define RENDER_PASS_LIST_HPP

#include "forward.hpp"

#include <QDialog>

namespace Shaderkit {

namespace Ui {
class RenderPassList;
}

class RenderPassList : public QDialog {
  Q_OBJECT

public:
  RenderPassList(QList<RenderPassPtr> list, QWidget* parent = 0);
  ~RenderPassList();

  QList<RenderPassPtr> orderedList() const;

private:
  QList<RenderPassPtr> m_list;
  Ui::RenderPassList* m_ui;

  void updateList();
};

} // namespace Shaderkit

#endif // RENDER_PASS_LIST_HPP
