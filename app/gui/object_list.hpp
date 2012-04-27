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

#ifndef OBJECT_LIST_HPP
#define OBJECT_LIST_HPP

#include "forward.hpp"

#include <QDialog>

class QListWidgetItem;

namespace Shaderkit
{

  namespace Ui
  {
    class ObjectList;
  }

  class ObjectList : public QDialog
  {
    Q_OBJECT

  public:
    explicit ObjectList(QWidget* parent, RenderPassPtr render_pass);
    ~ObjectList();

  private slots:
    void duplicate();
    void remove();
    void selectionChanged();
    void itemChanged(QListWidgetItem*);
    void updated(RenderPassPtr pass);

  private:
    Ui::ObjectList* m_ui;
    RenderPassPtr m_render_pass;

    QAction* m_duplicate;
    QAction* m_destroy;
  };

} // namespace Shaderkit

#endif // OBJECT_LIST_HPP
